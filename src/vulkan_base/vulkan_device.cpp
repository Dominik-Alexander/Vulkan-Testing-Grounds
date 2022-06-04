#include "vulkan_base.h"

#include <SDL.h>

bool initVulkanInstance(VulkanContext* context) {
	VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	applicationInfo.pApplicationName = "Vulkan-Testing-Grounds";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.pEngineName = "Hello Vulkan";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_VERSION_1_0;

	VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &applicationInfo;
	
	if (vkCreateInstance(&createInfo, 0, &context->instance) != VK_SUCCESS)
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