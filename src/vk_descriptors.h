#pragma once

#include "vk_types.h"
#include <vector>
#include <array>
#include <unordered_map>

namespace vkutil
{

    // manages allocation of descriptor sets.
    // Will keep creating new descriptor pools once they get filled.
    // Can reset the entire thing and reuse pools.

    // https://github.com/vblanco20-1/Vulkan-Descriptor-Allocator
    class DescriptorAllocator
    {
    public:
        struct PoolSizes
        {
            // it’s a multiplier on the number of descriptor sets allocated for the pools
            // if you set VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER to 4.f in there, 
            // it means that when a pool for 1000 descriptors is allocated, 
            // the pool will have space for 4000 combined image descriptors.
            std::vector<std::pair<VkDescriptorType, float>> sizes =
                {
                    {VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f},
                    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f},
                    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f}};
        };

        // reset all the DescriptorPools held inside the system, 
        // and move them to the freePools array, where they can be reused later
        void reset_pools();
        // perform the descriptor set allocator
        bool allocate(VkDescriptorSet *set, VkDescriptorSetLayout layout);

        void init(VkDevice newDevice);

        void cleanup();

        VkDevice device;

    private:
        VkDescriptorPool grab_pool();

        VkDescriptorPool currentPool{VK_NULL_HANDLE};
        PoolSizes descriptorSizes;
        // active in the allocator, and have descriptors allocated in them
        std::vector<VkDescriptorPool> usedPools;
        // stores completely reset pools for reuse
        std::vector<VkDescriptorPool> freePools;
    };

    // caches DescriptorSetLayouts to avoid creating duplicated layouts.
    class DescriptorLayoutCache
    {
    public:
        void init(VkDevice newDevice);
        void cleanup();

        VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo *info);

        struct DescriptorLayoutInfo
        {
            // good idea to turn this into a inlined array
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            bool operator==(const DescriptorLayoutInfo &other) const;

            size_t hash() const;
        };

    private:
        struct DescriptorLayoutHash
        {

            std::size_t operator()(const DescriptorLayoutInfo &k) const
            {
                return k.hash();
            }
        };

        std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
        VkDevice device;
    };

    // Uses the 2 objects above to allocate and write a descriptor set and its layout automatically.
    class DescriptorBuilder
    {
    public:
        static DescriptorBuilder begin(DescriptorLayoutCache *layoutCache, DescriptorAllocator *allocator);

        DescriptorBuilder &bind_buffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

        DescriptorBuilder &bind_image(uint32_t binding, VkDescriptorImageInfo *imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

        bool build(VkDescriptorSet &set, VkDescriptorSetLayout &layout);
        bool build(VkDescriptorSet &set);

    private:
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        DescriptorLayoutCache *cache;
        DescriptorAllocator *alloc;
    };
}
