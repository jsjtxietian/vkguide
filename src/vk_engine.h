// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include "vk_mesh.h"
#include <vector>
#include <unordered_map>
#include <deque>
#include <functional>
#include <glm/glm.hpp>

// note that we store the VkPipeline and layout by value, not pointer.
// They are 64 bit handles to internal driver structures anyway so storing pointers to them isn't very useful
struct Material
{
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

struct RenderObject
{
	Mesh *mesh;
	Material *material;
	glm::mat4 transformMatrix;
};

struct GPUCameraData
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
};

struct FrameData
{
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	AllocatedBuffer cameraBuffer;
	VkDescriptorSet globalDescriptor;
};

class PipelineBuilder
{
public:
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
	VkPipelineDepthStencilStateCreateInfo _depthStencil;

	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()> &&function)
	{
		deletors.push_back(function);
	}

	void flush()
	{
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
		{
			(*it)(); // call the function
		}

		deletors.clear();
	}
};

struct MeshPushConstants
{
	glm::vec4 data;
	glm::mat4 render_matrix;
};

struct GPUSceneData
{
	glm::vec4 fogColor;		// w is for exponent
	glm::vec4 fogDistances; // x for min, y for max, zw unused.
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; // w for sun power
	glm::vec4 sunlightColor;
};
// number of frames to overlap when rendering
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine
{
public:
	bool _isInitialized{false};
	int _frameNumber{0};
	int _selectedShader{0};
	struct SDL_Window *_window{nullptr};
	DeletionQueue _mainDeletionQueue;
	VmaAllocator _allocator; // vma lib allocator

	VkExtent2D _windowExtent{800, 600};

	VkInstance _instance;					   // Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;			   // GPU chosen as the default device
	VkDevice _device;						   // Vulkan device for commands
	VkSurfaceKHR _surface;					   // Vulkan window surface

	VkPhysicalDeviceProperties _gpuProperties;

	VkSwapchainKHR _swapchain;

	// image format expected by the windowing system
	VkFormat _swapchainImageFormat;

	// array of images from the swapchain
	std::vector<VkImage> _swapchainImages;

	// array of image-views from the swapchain
	std::vector<VkImageView> _swapchainImageViews;

	VkQueue _graphicsQueue;		   // queue we will submit to
	uint32_t _graphicsQueueFamily; // family of that queue

	VkCommandPool _commandPool;			// the command pool for our commands
	VkCommandBuffer _mainCommandBuffer; // the buffer we will record into

	VkRenderPass _renderPass;

	std::vector<VkFramebuffer> _framebuffers;

	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;
	VkPipeline _redTrianglePipeline;

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;
	Mesh _triangleMesh;
	Mesh _monkeyMesh;

	VkImageView _depthImageView;
	AllocatedImage _depthImage;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorPool _descriptorPool;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _sceneParameterBuffer;

	// the format for the depth image
	VkFormat _depthFormat;

	// frame storage
	FrameData _frames[FRAME_OVERLAP];

	// getter for the frame we are rendering to right now.
	FrameData &get_current_frame();

	// default array of renderable objects
	std::vector<RenderObject> _renderables;
	std::unordered_map<std::string, Material> _materials;
	std::unordered_map<std::string, Mesh> _meshes;

	// create material and add it to the map
	Material *create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string &name);

	// returns nullptr if it can't be found
	Material *get_material(const std::string &name);

	// returns nullptr if it can't be found
	Mesh *get_mesh(const std::string &name);

	// our draw function
	void draw_objects(VkCommandBuffer cmd, RenderObject *first, int count);

	// initializes everything in the engine
	void init();

	// shuts down the engine
	void cleanup();

	// draw loop
	void draw();

	// run main loop
	void run();

private:
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_default_renderpass();
	void init_framebuffers();
	void init_sync_structures();
	void init_pipelines();
	void init_scene();
	void init_descriptors();

	// loads a shader module from a spir-v file. Returns false if it errors
	bool load_shader_module(const char *filePath, VkShaderModule *outShaderModule);

	void load_meshes();

	void upload_mesh(Mesh &mesh);

	AllocatedBuffer create_buffer(size_t allocSize,
								  VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	size_t pad_uniform_buffer_size(size_t originalSize);
};
