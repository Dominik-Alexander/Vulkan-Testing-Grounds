#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

#include "vulkan_base/vulkan_base.h"

int main() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);

		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Hello, Vulkan!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_VULKAN);

    uint32_t instanceExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, 0);
    const char** enabledInstanceExtensions = new const char* [instanceExtensionCount];
    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, enabledInstanceExtensions);

    const char* enabledDeviceExtensions[]{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VulkanContext* context = initVulkan(instanceExtensionCount, enabledInstanceExtensions, ARRAY_COUNT(enabledDeviceExtensions), enabledDeviceExtensions);
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window, context->instance, &surface);
    VulkanSwapchain swapchain = createSwapchain(context, surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    VkRenderPass renderPass = createRenderPass(context, swapchain.format);
    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(swapchain.images.size());
    for (uint32_t i = 0; i < swapchain.images.size(); ++i)
    {
        VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &swapchain.imageViews[i];
        createInfo.width = swapchain.width;
        createInfo.height = swapchain.height;
        createInfo.layers = 1;
        VKA(vkCreateFramebuffer(context->device, &createInfo, 0, &framebuffers[i]));
    }

	if (!window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);
	}

    SDL_Event event;

    bool isRunning = true;

    while (isRunning)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
            {
                isRunning = false;
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
            }
            default:
            {
                break;
            }
        }
    }

    VKA(vkDeviceWaitIdle(context->device));
    for (uint32_t i = 0; i < framebuffers.size(); ++i)
    {
        VK(vkDestroyFramebuffer(context->device, framebuffers[i], 0));
    }
    framebuffers.clear();
    destroyRenderpass(context, renderPass);
    destroySwapchain(context, &swapchain);
    VK(vkDestroySurfaceKHR(context->instance, surface, 0));
    exitVulkan(context);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}