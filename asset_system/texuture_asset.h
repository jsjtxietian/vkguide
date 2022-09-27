#pragma once
#include "asset_loader.h"

namespace assets
{
    enum class TextureFormat : uint32_t
    {
        Unknown = 0,
        RGBA8
    };

    struct PageInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t compressedSize;
        uint32_t originalSize;
    };

    struct TextureInfo
    {
        uint64_t textureSize;
        TextureFormat textureFormat;
        CompressionMode compressionMode;

        std::string originalFile;
        std::vector<PageInfo> pages;
    };

    // parse the metadata json in a file and convert it into the TextureInfo struct
    TextureInfo read_texture_info(AssetFile *file);
    // work with a texture info alongside the binary blob of pixel data,
    // and will decompress the texture into the destination buffer
    void unpack_texture(TextureInfo *info, const char *sourcebuffer, size_t sourceSize, char *destination);
    void unpack_texture_page(TextureInfo *info, int pageIndex, char *sourcebuffer, char *destination);
    AssetFile pack_texture(TextureInfo *info, void *pixelData);
}

//  example of how to load the data
//  AllocatedBuffer stagingBuffer = engine.create_buffer(textureInfo.textureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
//  void* data;
//  vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);
//  assets::unpack_texture(&textureInfo, file.binaryBlob.data(), file.binaryBlob.size(), (char*)data);
//  vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);