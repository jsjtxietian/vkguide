#include "vk_textures.h"
#include "vk_initializers.h"
#include <iostream>

#include "texture_asset.h"
#include "asset_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool vkutil::load_image_from_file(VulkanEngine &engine, const char *file, AllocatedImage &outImage)
{
    int texWidth, texHeight, texChannels;

    stbi_uc *pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        std::cout << "Failed to load texture file " << file << std::endl;
        return false;
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;

    // the format R8G8B8A8 matches exactly with the pixels loaded from stb_image lib
    VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

    // allocate temporary buffer for holding texture data to upload
    AllocatedBuffer stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                         VMA_MEMORY_USAGE_CPU_ONLY);

    // copy data to buffer
    void *data;
    vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);
    // we no longer need the loaded data, so we can free the pixels as they are now in the staging buffer
    stbi_image_free(pixels);

    VkExtent3D imageExtent;
    imageExtent.width = static_cast<uint32_t>(texWidth);
    imageExtent.height = static_cast<uint32_t>(texHeight);
    imageExtent.depth = 1;

    VkImageCreateInfo dimg_info = vkinit::image_create_info(
        image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

    AllocatedImage newImage;
    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    // allocate and create the image
    vmaCreateImage(engine._allocator, &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, nullptr);

    engine.immediate_submit([&](VkCommandBuffer cmd)
                            {
                                // what part of the image we will transform
                                VkImageSubresourceRange range;
                                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                range.baseMipLevel = 0;
                                range.levelCount = 1;
                                range.baseArrayLayer = 0;
                                range.layerCount = 1;

                                VkImageMemoryBarrier imageBarrier_toTransfer = {};
                                imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

                                imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                                imageBarrier_toTransfer.image = newImage._image;
                                imageBarrier_toTransfer.subresourceRange = range;

                                imageBarrier_toTransfer.srcAccessMask = 0;
                                imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                                // barrier the image into the transfer-receive layout
                                vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);
                                
                                VkBufferImageCopy copyRegion = {};
                                copyRegion.bufferOffset = 0;
                                copyRegion.bufferRowLength = 0;
                                copyRegion.bufferImageHeight = 0;
                                copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                copyRegion.imageSubresource.mipLevel = 0;
                                copyRegion.imageSubresource.baseArrayLayer = 0;
                                copyRegion.imageSubresource.layerCount = 1;
                                copyRegion.imageExtent = imageExtent;

                                // copy the buffer into the image
                                vkCmdCopyBufferToImage(cmd, stagingBuffer._buffer, newImage._image, 
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion); 
                                
                                VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

                                imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                                imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                                imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                                imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                                //barrier the image into the shader readable layout
                                vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable); });

    engine._mainDeletionQueue.push_function([=]()
                                            { vmaDestroyImage(engine._allocator, newImage._image, newImage._allocation); });

    vmaDestroyBuffer(engine._allocator, stagingBuffer._buffer, stagingBuffer._allocation);

    std::cout << "Texture loaded successfully " << file << std::endl;

    outImage = newImage;

    return true;
}

bool vkutil::load_image_from_asset(VulkanEngine &engine, const char *filename, AllocatedImage &outImage)
{
    assets::AssetFile file;
    bool loaded = assets::load_binaryfile(filename, file);

    if (!loaded)
    {
        std::cout << "Error when loading texture " << filename << std::endl;
        return false;
    }

    assets::TextureInfo textureInfo = assets::read_texture_info(&file);

    VkDeviceSize imageSize = textureInfo.textureSize;
    VkFormat image_format;
    switch (textureInfo.textureFormat)
    {
    case assets::TextureFormat::RGBA8:
        image_format = VK_FORMAT_R8G8B8A8_UNORM;
        break;
    default:
        return false;
    }

    AllocatedBufferUntyped stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_UNKNOWN, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    std::vector<MipmapInfo> mips;

    void *data;
    vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);
    size_t offset = 0;
    {

        for (int i = 0; i < textureInfo.pages.size(); i++)
        {
            ZoneScopedNC("Unpack Texture", tracy::Color::Magenta);
            MipmapInfo mip;
            mip.dataOffset = offset;
            mip.dataSize = textureInfo.pages[i].originalSize;
            mips.push_back(mip);
            assets::unpack_texture_page(&textureInfo, i, file.binaryBlob.data(), (char *)data + offset);

            offset += mip.dataSize;
        }
    }
    vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);

    outImage = upload_image_mipmapped(textureInfo.pages[0].width, textureInfo.pages[0].height, image_format, engine, stagingBuffer, mips);

    vmaDestroyBuffer(engine._allocator, stagingBuffer._buffer, stagingBuffer._allocation);

    return true;
}