#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

#include "vulkan_base/vulkan_base.h"

VulkanContext* context;
VkSurfaceKHR surface;
VkRenderPass renderPass;
VulkanSwapchain swapchain;
std::vector<VkFramebuffer> framebuffers;
VulkanPipeline pipeline;
VkCommandPool commandPool;
VkCommandBuffer commandBuffer;
VkFence fence;

void initApplication(SDL_Window* window) {
    uint32_t instanceExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, 0);
    const char** enabledInstanceExtensions = new const char* [instanceExtensionCount];
    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, enabledInstanceExtensions);

    const char* enabledDeviceExtensions[]{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    context = initVulkan(instanceExtensionCount, enabledInstanceExtensions, ARRAY_COUNT(enabledDeviceExtensions), enabledDeviceExtensions);
    SDL_Vulkan_CreateSurface(window, context->instance, &surface);
    swapchain = createSwapchain(context, surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    renderPass = createRenderPass(context, swapchain.format);
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

    pipeline = createPipeline(context, "../shaders/triangle_vert.spv", "../shaders/triangle_frag.spv", renderPass, swapchain.width, swapchain.height);

    {
        VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        VKA(vkCreateFence(context->device, &createInfo, 0, &fence))
    }
    
    {
        VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = context->graphicsQueue.familyIndex;
        VKA(vkCreateCommandPool(context->device, &createInfo, 0, &commandPool));
    }

    {
        VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        VKA(vkAllocateCommandBuffers(context->device, &allocateInfo, &commandBuffer));
    }
}

void renderApplication() {
    static float greenChannel = 0.0f;
    greenChannel += 0.01f;
    if (greenChannel > 1.0f) greenChannel = 0.0f;
    uint32_t imageIndex = 0;
    VK(vkAcquireNextImageKHR(context->device, swapchain.swapchain, UINT64_MAX, 0, fence, &imageIndex));

    VKA(vkResetCommandPool(context->device, commandPool, 0));

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    {
        VkClearValue clearValue = { 0.0f, greenChannel, 0.25f, 1.0f };
        VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = framebuffers[imageIndex];
        beginInfo.renderArea = { {0, 0}, {swapchain.width, swapchain.height} };
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &clearValue;
        vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
    }
    VKA(vkEndCommandBuffer(commandBuffer));

    VKA(vkWaitForFences(context->device, 1, &fence, VK_TRUE, UINT64_MAX));
    VKA(vkResetFences(context->device, 1, &fence));

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VKA(vkQueueSubmit(context->graphicsQueue.queue, 1, &submitInfo, 0));

    VKA(vkDeviceWaitIdle(context->device));

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.swapchain;
    presentInfo.pImageIndices = &imageIndex;
    VK(vkQueuePresentKHR(context->graphicsQueue.queue, &presentInfo));
}

void shutdownApplication() {
    VKA(vkDeviceWaitIdle(context->device));

    VK(vkDestroyFence(context->device, fence, 0));
    VK(vkDestroyCommandPool(context->device, commandPool, 0));

    destroyPipeline(context, &pipeline);

    for (uint32_t i = 0; i < framebuffers.size(); ++i)
    {
        VK(vkDestroyFramebuffer(context->device, framebuffers[i], 0));
    }
    framebuffers.clear();
    destroyRenderpass(context, renderPass);
    destroySwapchain(context, &swapchain);
    VK(vkDestroySurfaceKHR(context->instance, surface, 0));
    exitVulkan(context);
}

int main() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);

		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Hello, Vulkan!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_VULKAN);

	if (!window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);
	}

    initApplication(window);

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
                renderApplication();
                break;
            }
        }
    }

    shutdownApplication();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}