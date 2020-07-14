#include "CommandBuffer.h"
#ifdef __linux__
#define SURFACE_EXTENSION "VK_KHR_xlib_surface"
#include "X11/Xlib.h"
#include "vulkan/vulkan_xlib.h"
#elif _WIN32
#pragma once
#include <winapifamily.h>
#include <sdkddkver.h>

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IX86)
#define _X86_
#if !defined(_CHPE_X86_ARM64_) && defined(_M_HYBRID)
#define _CHPE_X86_ARM64_
#endif
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_AMD64)
#define _AMD64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM)
#define _ARM_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM64)
#define _ARM64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_M68K)
#define _68K_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_MPPC)
#define _MPPC_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_M_IX86) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IA64)
#if !defined(_IA64_)
#define _IA64_
#endif /* !_IA64_ */
#endif
#include <windef.h>
#include <minwinbase.h>
#include <libloaderapi.h>

#include "vulkan/vulkan_win32.h"
#include "Device.h"
#define SURFACE_EXTENSION "VK_KHR_win32_surface"
#endif


#pragma comment(lib, "vulkan-1.lib")

VkInstance			instance;
VkDevice			dev;
VkPhysicalDevice	pdev;
uint				memory_idx[2];
VkQueue				queue;

VkInstance GetInstance()
{
	return instance;
}

VkDevice GetDevice()
{
	return dev;
}

VkPhysicalDevice GetPhysicalDevice()
{
	return pdev;
}

VkQueue GetQueue()
{
	return queue;
}

VkQueue GetDeviceQueue(uint queueFamilyIndex, uint queueIndex)
{
	VkQueue queue;
	vkGetDeviceQueue(dev, queueFamilyIndex, queueIndex, &queue);
	return queue;
}

void QueueSubmit(uint count, const VkSubmitInfo* submits, VkFence fence)
{
	vkQueueSubmit(queue, count, submits, fence);
}

void QueueWaitIdle()
{
	vkQueueWaitIdle(queue);
}

void QueuePresent(const VkPresentInfoKHR* info)
{
	vkQueuePresentKHR(queue, info);
}

void DeviceWaitIdle()
{
	vkDeviceWaitIdle(dev);
}

VkDeviceMemory AllocateMemory(const VkMemoryAllocateInfo* pAllocateInfo)
{
	VkDeviceMemory memory;
	vkAllocateMemory(dev, pAllocateInfo, 0, &memory);
	return memory;
}

void FreeMemory(VkDeviceMemory memory)
{
	vkFreeMemory(dev, memory, 0);
}

char* MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
{
	char* map;
	vkMapMemory(dev, memory, offset, size, 0, (void**)&map);
	return map;
}

void UnmapMemory(VkDeviceMemory memory)
{
	vkUnmapMemory(dev, memory);
}

void FlushMappedMemoryRanges(uint memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
	vkFlushMappedMemoryRanges(dev, memoryRangeCount, pMemoryRanges);
}

void InvalidateMappedMemoryRanges(uint memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
	vkInvalidateMappedMemoryRanges(dev, memoryRangeCount, pMemoryRanges);
}

VkDeviceSize GetDeviceMemoryCommitment(VkDeviceMemory memory)
{
	VkDeviceSize size;
	vkGetDeviceMemoryCommitment(dev, memory, &size);
	return size;
}

void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	vkBindBufferMemory(dev, buffer, memory, memoryOffset);
}

void BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
	vkBindImageMemory(dev, image, memory, memoryOffset);
}

VkMemoryRequirements GetBufferMemoryRequirements(VkBuffer buffer)
{
	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(dev, buffer, &req);
	return req;
}

VkMemoryRequirements GetImageMemoryRequirements(VkImage image)
{
	VkMemoryRequirements req;
	vkGetImageMemoryRequirements(dev, image, &req);
	return req;
}

VkSparseImageMemoryRequirements GetImageSparseMemoryRequirements(VkImage image, uint32_t* pSparseMemoryRequirementCount)
{
	VkSparseImageMemoryRequirements req;
	vkGetImageSparseMemoryRequirements(dev, image, pSparseMemoryRequirementCount, &req);
	return req;
}

VkFence CreateFence(VkFenceCreateFlags flag)
{
	VkFenceCreateInfo info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, 0, flag };
	VkFence fence;
	vkCreateFence(dev, &info, 0, &fence);
	return fence;
}

void DestroyFence(VkFence fence)
{
	vkDestroyFence(dev, fence, 0);
}

void ResetFences(uint32_t fenceCount, const VkFence* pFences)
{
	vkResetFences(dev, fenceCount, pFences);
}

void GetFenceStatus(VkFence fence)
{
	vkGetFenceStatus(dev, fence);
}

void WaitForFences(uint fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64 timeout)
{
	vkWaitForFences(dev, fenceCount, pFences, waitAll, timeout);
}

VkSemaphore CreateSemaphore(VkSemaphoreCreateFlags flag)
{
	VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, 0, flag };
	VkSemaphore semaphore;
	vkCreateSemaphore(dev, &info, 0, &semaphore);
	return semaphore;
}

void DestroySemaphore(VkSemaphore semaphore)
{
	vkDestroySemaphore(dev, semaphore, 0);
}

VkEvent CreateEvent(VkEventCreateFlags flag)
{
	VkEventCreateInfo info = { VK_STRUCTURE_TYPE_EVENT_CREATE_INFO, 0, flag };
	VkEvent event;
	vkCreateEvent(dev, &info, 0, &event);
	return event;
}

void DestroyEvent(VkEvent event)
{
	vkDestroyEvent(dev, event, 0);
}

void GetEventStatus(VkEvent event)
{
	vkGetEventStatus(dev, event);
}

void VkSetEvent(VkEvent event)
{
	vkSetEvent(dev, event);
}

void VkResetEvent(VkEvent event)
{
	vkResetEvent(dev, event);
}

void CreateQueryPool(const VkQueryPoolCreateInfo* pCreateInfo, VkQueryPool* pQueryPool)
{
	vkCreateQueryPool(dev, pCreateInfo, 0, pQueryPool);
}

void DestroyQueryPool(VkQueryPool queryPool)
{
	vkDestroyQueryPool(dev, queryPool, 0);
}

void GetQueryPoolResults(VkQueryPool queryPool, uint firstQuery, uint queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
	vkGetQueryPoolResults(dev, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

VkBuffer CreateBuffer(const VkBufferCreateInfo* pCreateInfo)
{
	VkBuffer buffer;
	vkCreateBuffer(dev, pCreateInfo, 0, &buffer);
	return buffer;
}

void DestroyBuffer(VkBuffer buffer)
{
	vkDestroyBuffer(dev, buffer, 0);
}

VkBufferView CreateBufferView(const VkBufferViewCreateInfo* pCreateInfo)
{
	VkBufferView view;
	vkCreateBufferView(dev, pCreateInfo, 0, &view);
	return view;
}

void DestroyBufferView(VkBufferView bufferView)
{
	vkDestroyBufferView(dev, bufferView, 0);
}

VkImage CreateImage(const VkImageCreateInfo* pCreateInfo)
{
	VkImage img;
	vkCreateImage(dev, pCreateInfo, 0, &img);
	return img;
}

void DestroyImage(VkImage image)
{
	vkDestroyImage(dev, image, 0);
}

VkSubresourceLayout GetImageSubresourceLayout(VkImage image, const VkImageSubresource* pSubresource)
{
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(dev, image, pSubresource, &layout);
	return layout;
}

VkImageView CreateImageView(const VkImageViewCreateInfo* pCreateInfo)
{
	VkImageView view;
	vkCreateImageView(dev, pCreateInfo, 0, &view);
	return view;
}

void DestroyImageView(VkImageView imageView)
{
	vkDestroyImageView(dev, imageView, 0);
}

VkShaderModule CreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo)
{
	VkShaderModule mod;
	vkCreateShaderModule(dev, pCreateInfo, 0, &mod);
	return mod;
}

void DestroyShaderModule(VkShaderModule shaderModule)
{
	vkDestroyShaderModule(dev, shaderModule, 0);
}

VkPipelineCache CreatePipelineCache(const VkPipelineCacheCreateInfo* pCreateInfo)
{
	VkPipelineCache cache;
	vkCreatePipelineCache(dev, pCreateInfo, 0, &cache);
	return cache;
}

void DestroyPipelineCache(VkPipelineCache pipelineCache)
{
	vkDestroyPipelineCache(dev, pipelineCache, 0);
}

void GetPipelineCacheData(VkPipelineCache pipelineCache, size_t* pDataSize, void* pData)
{
	vkGetPipelineCacheData(dev, pipelineCache, pDataSize, pData);
}

void MergePipelineCaches(VkPipelineCache dstCache, uint srcCacheCount, const VkPipelineCache* pSrcCaches)
{
	vkMergePipelineCaches(dev, dstCache, srcCacheCount, pSrcCaches);
}

void CreateGraphicsPipelines(VkPipelineCache pipelineCache, uint createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, VkPipeline* pPipelines)
{
	vkCreateGraphicsPipelines(dev, pipelineCache, createInfoCount, pCreateInfos, 0, pPipelines);
}

void CreateComputePipelines(VkPipelineCache pipelineCache, uint createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, VkPipeline* pPipelines)
{
	vkCreateComputePipelines(dev, pipelineCache, createInfoCount, pCreateInfos, 0, pPipelines);
}

void DestroyPipeline(VkPipeline pipeline)
{
	vkDestroyPipeline(dev, pipeline, 0);
}

VkPipelineLayout CreatePipelineLayout(const VkPipelineLayoutCreateInfo* pCreateInfo)
{
	VkPipelineLayout layout;
	vkCreatePipelineLayout(dev, pCreateInfo, 0, &layout);
	return layout;
}

void DestroyPipelineLayout(VkPipelineLayout pipelineLayout)
{
	vkDestroyPipelineLayout(dev, pipelineLayout, 0);
}

VkSampler CreateSampler(const VkSamplerCreateInfo* pCreateInfo)
{
	VkSampler sampler;
	vkCreateSampler(dev, pCreateInfo, 0, &sampler);
	return sampler;
}

void DestroySampler(VkSampler sampler)
{
	vkDestroySampler(dev, sampler, 0);
}

VkDescriptorSetLayout CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo)
{
	VkDescriptorSetLayout layout;
	vkCreateDescriptorSetLayout(dev, pCreateInfo, 0, &layout);
	return layout;
}

void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
{
	vkDestroyDescriptorSetLayout(dev, descriptorSetLayout, 0);
}

VkDescriptorPool CreateDescriptorPool(const VkDescriptorPoolCreateInfo* pCreateInfo)
{
	VkDescriptorPool pool;
	vkCreateDescriptorPool(dev, pCreateInfo, 0, &pool);
	return pool;
}

void DestroyDescriptorPool(VkDescriptorPool descriptorPool)
{
	vkDestroyDescriptorPool(dev, descriptorPool, 0);
}

void ResetDescriptorPool(VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
	vkResetDescriptorPool(dev, descriptorPool, flags);
}

void AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets)
{
	vkAllocateDescriptorSets(dev, pAllocateInfo, pDescriptorSets);
}

void FreeDescriptorSets(VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets)
{
	vkFreeDescriptorSets(dev, descriptorPool, descriptorSetCount, pDescriptorSets);
}

void UpdateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies)
{
	vkUpdateDescriptorSets(dev, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

VkFramebuffer CreateFramebuffer(const VkFramebufferCreateInfo* pCreateInfo)
{
	VkFramebuffer fbuffer;
	vkCreateFramebuffer(dev, pCreateInfo, 0, &fbuffer);
	return fbuffer;
}

void DestroyFramebuffer(VkFramebuffer framebuffer)
{
	vkDestroyFramebuffer(dev, framebuffer, 0);
}

VkRenderPass CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo)
{
	VkRenderPass pass;
	vkCreateRenderPass(dev, pCreateInfo, 0, &pass);
	return pass;
}

void DestroyRenderPass(VkRenderPass renderPass)
{
	vkDestroyRenderPass(dev, renderPass, 0);
}

VkExtent2D GetRenderAreaGranularity(VkRenderPass renderPass)
{
	VkExtent2D granularity;
	vkGetRenderAreaGranularity(dev, renderPass, &granularity);
	return granularity;
}

VkCommandPool CreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo)
{
	VkCommandPool pool;
	vkCreateCommandPool(dev, pCreateInfo, 0, &pool);
	return pool;
}

void DestroyCommandPool(VkCommandPool commandPool)
{
	vkDestroyCommandPool(dev, commandPool, 0);
}

void ResetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
	vkResetCommandPool(dev, commandPool, flags);
}

void AllocateCommandBuffers(const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers)
{
	vkAllocateCommandBuffers(dev, pAllocateInfo, pCommandBuffers);
}

void FreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	vkFreeCommandBuffers(dev, commandPool, commandBufferCount, pCommandBuffers);
}

VkSwapchainKHR CreateSwapchain(const VkSwapchainCreateInfoKHR* pCreateInfo)
{
	VkSwapchainKHR swapchain;
	vkCreateSwapchainKHR(dev, pCreateInfo, 0, &swapchain);
	return swapchain;
}

void DestroySwapchain(VkSwapchainKHR swapchain)
{
	vkDestroySwapchainKHR(dev, swapchain, 0);
}

VkImage* GetSwapchainImages(VkSwapchainKHR swapchain, uint* count)
{
	vkGetSwapchainImagesKHR(dev, swapchain, count, 0);
	VkImage* images = new VkImage[*count];
	vkGetSwapchainImagesKHR(dev, swapchain, count, images);
	return images;
}

VkResult AcquireNextImage(VkSwapchainKHR swapchain, uint64 timeout, VkSemaphore semaphore, VkFence fence, uint* idx)
{
	return vkAcquireNextImageKHR(dev, swapchain, timeout, semaphore, fence, idx);
}

VkDeviceGroupPresentCapabilitiesKHR GetDeviceGroupPresentCapabilities()
{
	VkDeviceGroupPresentCapabilitiesKHR caps;
	vkGetDeviceGroupPresentCapabilitiesKHR(dev, &caps);
	return caps;
}

VkDeviceGroupPresentModeFlagsKHR GetDeviceGroupSurfacePresentModes(VkSurfaceKHR surface)
{
	VkDeviceGroupPresentModeFlagsKHR modes;
	vkGetDeviceGroupSurfacePresentModesKHR(dev, surface, &modes);
	return modes;
}

uint AcquireNextImage2(const VkAcquireNextImageInfoKHR* pAcquireInfo)
{
	uint idx;
	vkAcquireNextImage2KHR(dev, pAcquireInfo, &idx);
	return idx;
}

VkSurfaceKHR CreateWin32Surface(void* hwnd)
{
	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	info.hwnd = (HWND)hwnd;
	info.hinstance = GetModuleHandle(0);
	vkCreateWin32SurfaceKHR(instance, &info, 0, &surface);
	return surface;
}

void DestroySurface(VkSurfaceKHR surface)
{
	vkDestroySurfaceKHR(instance, surface, 0);
}

VkExtensionProperties* EnumerateInstanceExtensionProperties(uint* count)
{
	vkEnumerateInstanceExtensionProperties(0, count, 0);
	VkExtensionProperties* props = new VkExtensionProperties[*count];
	vkEnumerateInstanceExtensionProperties(0, count, props);
	return props;
}

VkLayerProperties* EnumerateInstanceLayerProperties(uint* count)
{
	vkEnumerateInstanceLayerProperties(count, 0);
	VkLayerProperties* props = new VkLayerProperties[*count];
	vkEnumerateInstanceLayerProperties(count, props);
	return props;
}

VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures()
{
	VkPhysicalDeviceFeatures feat;
	vkGetPhysicalDeviceFeatures(pdev, &feat);
	return feat;
}

VkFormatProperties GetPhysicalDeviceFormatProperties(VkFormat format)
{
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(pdev, format, &props);
	return props;
}

VkPhysicalDeviceProperties GetPhysicalDeviceProperties()
{
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pdev, &props);
	return props;
}

VkExtensionProperties* EnumerateDeviceExtensionProperties(uint* count)
{
	vkEnumerateDeviceExtensionProperties(pdev, 0, count, 0);
	VkExtensionProperties* props = new VkExtensionProperties[*count];
	vkEnumerateDeviceExtensionProperties(pdev, 0, count, props);
	return props;
}

VkQueueFamilyProperties* GetPhysicalDeviceQueueFamilyProperties(uint* count)
{
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, count, 0);
	VkQueueFamilyProperties* props = new VkQueueFamilyProperties[*count];
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, count, props);
	return props;
}

VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties()
{
	VkPhysicalDeviceMemoryProperties props;
	vkGetPhysicalDeviceMemoryProperties(pdev, &props);
	return props;
}

#ifdef __linux__
VkBool32 GetPresentationSupport(uint queueFamilyIndex, Display* dpy, VisualID visualID);
{
	return vkGetPhysicalDeviceXlibPresentationSupportKHR(pdev, queueFamilyIndex, dpy, visualID);
}
#endif

#ifdef _WIN32
VkBool32 GetPresentationSupport(uint queueFamilyIndex)
{
	return vkGetPhysicalDeviceWin32PresentationSupportKHR(pdev, queueFamilyIndex);
}
#endif

VkBool32 GetPhysicalDeviceSurfaceSupport(uint queueFamilyIndex, VkSurfaceKHR surface)
{
	VkBool32 supported;
	vkGetPhysicalDeviceSurfaceSupportKHR(pdev, queueFamilyIndex, surface, &supported);
	return supported;
}

VkSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilities(VkSurfaceKHR surface)
{
	VkSurfaceCapabilitiesKHR caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdev, surface, &caps);
	return caps;
}

VkSurfaceFormatKHR* GetPhysicalDeviceSurfaceFormats(VkSurfaceKHR surface, uint* count)
{
	vkGetPhysicalDeviceSurfaceFormatsKHR(pdev, surface, count, 0);
	VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[*count];
	vkGetPhysicalDeviceSurfaceFormatsKHR(pdev, surface, count, formats);
	return formats;
}

VkPresentModeKHR* GetPhysicalDeviceSurfacePresentModes(VkSurfaceKHR surface, uint* count)
{
	vkGetPhysicalDeviceSurfacePresentModesKHR(pdev, surface, count, 0);
	VkPresentModeKHR* modes = new VkPresentModeKHR[*count];
	vkGetPhysicalDeviceSurfacePresentModesKHR(pdev, surface, count, modes);
	return modes;
}

VkRect2D* GetPhysicalDevicePresentRectangles(VkSurfaceKHR surface, uint* count)
{
	vkGetPhysicalDevicePresentRectanglesKHR(pdev, surface, count, 0);
	VkRect2D* rects = new VkRect2D[*count];
	vkGetPhysicalDevicePresentRectanglesKHR(pdev, surface, count, rects);
	return rects;
}

VkPresentModeKHR GetPresentMode(VkSurfaceKHR surface)
{
	VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
	uint count;
	VkPresentModeKHR* modes = GetPhysicalDeviceSurfacePresentModes(surface, &count);
	for (uint i = 0; i < count; ++i)
	{
		if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			mode = modes[i];
			break;
		}
		if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			mode = modes[i];
	}
	delete[] modes;
	return mode;
}


void InitVulkan()
{
	const char* extensions[] = { "VK_KHR_surface",  SURFACE_EXTENSION, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };
	const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
	const char* device_ext[] = { "VK_KHR_swapchain", VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME };

	VkApplicationInfo appinfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appinfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
	appinfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
	appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

	VkInstanceCreateInfo info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	info.pApplicationInfo = &appinfo;
	info.pNext = 0;
	info.enabledLayerCount = sizeof(layers) / sizeof(char*);
	info.ppEnabledLayerNames = layers;
	info.enabledExtensionCount = sizeof(extensions) / sizeof(char*);
	info.ppEnabledExtensionNames = extensions;


	vkCreateInstance(&info, 0, &instance);
	VkPhysicalDevice devices[16];
	uint count;
	vkEnumeratePhysicalDevices(instance, &count, 0);
	vkEnumeratePhysicalDevices(instance, &count, devices);

	pdev = devices[0];
	for (uint i = 0; i < count; ++i)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(devices[i], &props);
		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			pdev = devices[i];
		}
	}

	float prio = 1;
	VkDeviceQueueCreateInfo qinfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	qinfo.queueCount = 1;
	qinfo.pQueuePriorities = &prio;

	VkPhysicalDeviceFeatures features = { 0 };
	features.fillModeNonSolid = 1;
	features.sampleRateShading = 1;
	features.vertexPipelineStoresAndAtomics = 1;
	features.samplerAnisotropy = 1;

	VkPhysicalDeviceDescriptorIndexingFeatures extFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };
	extFeatures.descriptorBindingPartiallyBound = 1;
	extFeatures.descriptorBindingVariableDescriptorCount = 1;

	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pNext = &extFeatures;
	deviceInfo.pQueueCreateInfos = &qinfo;
	deviceInfo.pEnabledFeatures = &features;
	deviceInfo.enabledExtensionCount = sizeof(device_ext) / sizeof(char*);
	deviceInfo.ppEnabledExtensionNames = device_ext;
	deviceInfo.enabledLayerCount = sizeof(layers) / sizeof(char*);
	deviceInfo.ppEnabledLayerNames = layers;


	vkCreateDevice(pdev, &deviceInfo, 0, &dev);

	queue = GetDeviceQueue(0, 0);
	VkPhysicalDeviceMemoryProperties props = GetPhysicalDeviceMemoryProperties();
	memory_idx[0] = -1;
	memory_idx[1] = -1;
	for (uint i = 0; i < props.memoryTypeCount; ++i)
	{
		VkMemoryPropertyFlags flag = props.memoryTypes[i].propertyFlags;
		if (memory_idx[0] == -1 && (flag & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) memory_idx[0] = i;
		if (memory_idx[1] == -1 && (flag & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))) memory_idx[1] = i;
	}

	VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CommandBuffer::pool = CreateCommandPool(&poolInfo);

	VkEXTFN::PushDescriptorSet = (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(dev, "vkCmdPushDescriptorSetKHR");
}

void DestroyInstance()
{
	DestroyCommandPool(CommandBuffer::pool);
	vkDestroyDevice(dev, 0);
	vkDestroyInstance(instance, 0);
}

