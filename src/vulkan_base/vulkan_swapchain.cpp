#include "vulkan_base.h"

VulkanSwapchain createSwapchain(VulkanContext* context, VkSurfaceKHR surface, VkImageUsageFlags usage) {
	VulkanSwapchain result = {};

	VkBool32 supportsPresent = 0;
	VKA(vkGetPhysicalDeviceSurfaceSupportKHR(context->physicalDevice, context->graphicsQueue.familyIndex, surface, &supportsPresent));
	if (!supportsPresent)
	{
		std::cout << "Graphics queue does not support present";
		return result;
	}

	uint32_t numFormats = 0;
	VKA(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, surface, &numFormats, 0));
	VkSurfaceFormatKHR* availableFormats = new VkSurfaceFormatKHR[numFormats];
	VKA(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, surface, &numFormats, availableFormats));
	if (numFormats <= 0)
	{
		std::cout << "No surface formats available!" << std::endl;
		delete[] availableFormats;
		return result;
	}

	VkFormat format = availableFormats[0].format;
	VkColorSpaceKHR colorSpace = availableFormats[0].colorSpace;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VKA(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physicalDevice, surface, &surfaceCapabilities));
	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		surfaceCapabilities.currentExtent.width = surfaceCapabilities.minImageExtent.width;
	}
	if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF)
	{
		surfaceCapabilities.currentExtent.height = surfaceCapabilities.minImageExtent.height;
	}

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = format;
	createInfo.imageColorSpace = colorSpace;
	createInfo.imageExtent = surfaceCapabilities.currentExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = usage;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	VKA(vkCreateSwapchainKHR(context->device, &createInfo, 0, &result.swapchain));

	uint32_t numImages;
	VKA(vkGetSwapchainImagesKHR(context->device, result.swapchain, &numImages, 0));

	result.images.resize(numImages);
	VKA(vkGetSwapchainImagesKHR(context->device, result.swapchain, &numImages, result.images.data()));

	delete[] availableFormats;

	return result;
}

void destroySwapchain(VulkanContext* context, VulkanSwapchain* swapchain) {
	VK(vkDestroySwapchainKHR(context->device, swapchain->swapchain, 0));
}