#include "vulkan_base.h"

#include <SDL.h>
#include <iostream>

bool initVulkanInstance(VulkanContext* context, uint32_t instanceExtensionCount, const char** instanceExtensions) {
	uint32_t layerPropertyCount = 0;
	VKA(vkEnumerateInstanceLayerProperties(&layerPropertyCount, 0));
	VkLayerProperties* layerProperties = new VkLayerProperties[layerPropertyCount];
	VKA(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties));
	for (uint32_t i = 0; i < layerPropertyCount; ++i)
	{
#ifdef VULKAN_INFO_OUTPUT
		std::cout << layerProperties[i].layerName << std::endl;
		std::cout << layerProperties[i].description << std::endl;
#endif // VULKAN_INFO_OUTPUT
	}
	delete[] layerProperties;

	const char* enabledLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	uint32_t availableInstanceExtensionCount = 0;
	VKA(vkEnumerateInstanceExtensionProperties(0, &availableInstanceExtensionCount, 0));
	VkExtensionProperties* instanceExtensionProperties = new VkExtensionProperties[availableInstanceExtensionCount];
	VKA(vkEnumerateInstanceExtensionProperties(0, &availableInstanceExtensionCount, instanceExtensionProperties));
	for (uint32_t i = 0; i < availableInstanceExtensionCount; ++i)
	{
#ifdef VULKAN_INFO_OUTPUT
		std::cout << instanceExtensionProperties[i].extensionName << std::endl;
#endif // VULKAN_INFO_OUTPUT
	}
	delete[] instanceExtensionProperties;

	VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	applicationInfo.pApplicationName = "Vulkan-Testing-Grounds";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.pEngineName = "Hello Vulkan";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_VERSION_1_0;

	VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledLayerCount = ARRAY_COUNT(enabledLayers);
	createInfo.ppEnabledLayerNames = enabledLayers;
	createInfo.enabledExtensionCount = instanceExtensionCount;
	createInfo.ppEnabledExtensionNames = instanceExtensions;
	
	if (VK(vkCreateInstance(&createInfo, 0, &context->instance)) != VK_SUCCESS)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Vulkan was not created!", NULL);
		return false;
	}

	return true;
}

VulkanContext* initVulkan(uint32_t instanceExtensionCount, const char** instanceExtensions) {
	VulkanContext* context = new VulkanContext;

	if (!initVulkanInstance(context, instanceExtensionCount, instanceExtensions))
	{
		return 0;
	}

	return context;
}