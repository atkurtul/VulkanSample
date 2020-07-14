#pragma once
#include "Device.h"
#include "Util.h"

struct Swapchain : AliasType<VkSwapchainKHR>
{
    VkSurfaceKHR		surface;
    void Init(VkExtent2D extent)
    {
        VkSwapchainCreateInfoKHR info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        info.surface = surface;
        info.minImageCount = NFRAMES;
        info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = GetPresentMode(surface);
        info.clipped = 1;
        handle = CreateSwapchain(&info);
    }

    Swapchain(void* hwnd, VkExtent2D extent)
    {
        surface = CreateWin32Surface(hwnd);
        if (!GetPhysicalDeviceSurfaceSupport(0, surface))
            printf("Surface isnt supported\n");
        Init(extent);
    }

    void Recreate(VkExtent2D extent)
    {
        DestroySwapchain(handle);
        Init(extent);
    }
};

struct FrameResource
{
    VkRenderPassBeginInfo	passInfo;
    VkSubmitInfo			submitInfo;
    VkPresentInfoKHR		presentInfo;
    VkSemaphore             acquireSemaphore;
    VkSemaphore             presentSemaphore;
    VkImageView             view;
    VkFramebuffer           framebuffer;

    void Init(VkImage img, uint ms,
        VkImageView depthBuffer,
        VkImageView colorBuffer,
        VkRenderPass pass,
        VkExtent2D extent,
        VkSwapchainKHR* sc,
        VkClearValue* clear,
        VkCommandBuffer* cmd,
        uint frameIdx)
    {
        acquireSemaphore = CreateSemaphore(0);
        presentSemaphore = CreateSemaphore(0);

        view = MkImageView(img, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
        //VkImageView attachments[3] = { colorBuffer, depthBuffer, view };
        VkImageView attachments[3];
        if (ms == 1)
        {
            attachments[0] = view;
            attachments[1] = depthBuffer;
            framebuffer = MkFramebuffer(pass, 2, attachments, extent.width, extent.height);
        }
        else
        {
            attachments[0] = colorBuffer;
            attachments[1] = depthBuffer;
            attachments[2] = view;
            framebuffer = MkFramebuffer(pass, 3, attachments, extent.width, extent.height);
        }

        static VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        static uint idx[NFRAMES];
        idx[frameIdx] = frameIdx;
        passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passInfo.renderPass = pass;
        passInfo.framebuffer = framebuffer;
        passInfo.renderArea.extent = extent;
        passInfo.clearValueCount = sizeof(attachments) / sizeof(VkImageView);
        passInfo.pClearValues = clear;

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.pWaitSemaphores = &acquireSemaphore;
        submitInfo.pSignalSemaphores = &presentSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = cmd;

        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &presentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = sc;
        presentInfo.pImageIndices = &idx[frameIdx];
    }

    void Free()
    {
        DestroyImageView(view);
        DestroyFramebuffer(framebuffer);
        DestroySemaphore(acquireSemaphore);
        DestroySemaphore(presentSemaphore);
    }
};