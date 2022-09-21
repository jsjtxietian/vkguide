// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

// we will add our main reusable types here
struct AllocatedBuffer
{
    // a handle to a GPU side Vulkan buffer
    VkBuffer _buffer;
    // holds the state that the VMA library uses, like the memory that buffer was allocated from, and its size
    VmaAllocation _allocation;
};
