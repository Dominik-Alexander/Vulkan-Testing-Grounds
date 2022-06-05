#include "vulkan_base.h"

#include <SDL.h>
#include <iostream>

bool initVulkanInstance(VulkanContext* context) {
	uint32_t layerPropertyCount = 0;
	VKA(vkEnumerateInstanceLayerProperties(&layerPropertyCount, 0));
	VkLayerProperties* layerProperties = new VkLayerProperties[layerPropertyCount];
	VKA(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties));
	for (uint32_t i = 0; i < layerPropertyCount; ++i)
	{
		std::cout << layerProperties[i].layerName << std::endl;
		std::cout << layerProperties[i].description << std::endl;
	}

	const char* enabledLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

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
	
	if (VK(vkCreateInstance(&createInfo, 0, &context->instance)) != VK_SUCCESS)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Vulkan was not created!", NULL);
		return false;
	}

	return true;
}

VulkanContext* initVulkan() {
	VulkanContext* context = new VulkanContext;

	if (!initVulkanInstance(context))
	{
		return 0;
	}

	return context;
}