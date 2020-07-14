#pragma once
#include "pch.h"

inline VkAttachmentDescription MkAttachment(VkFormat format, uint ms, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout layout)
{
    VkAttachmentDescription attachment = { 0 };
    attachment.format = format;
    attachment.samples = VkSampleCountFlagBits(ms);
    attachment.loadOp = loadOp;
    attachment.storeOp = storeOp;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = layout;
    return attachment;
}

inline VkAttachmentDescription MkDepthAttachment(uint ms)
{
    return MkAttachment(VK_FORMAT_D32_SFLOAT, ms, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
}

inline VkAttachmentDescription MkColorAttachment(uint ms, uint present, VkAttachmentLoadOp load_op)
{
    return MkAttachment(VK_FORMAT_B8G8R8A8_UNORM, ms, load_op, VK_ATTACHMENT_STORE_OP_STORE, present ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

inline VkAttachmentReference MkColorRef(uint slot)
{
    VkAttachmentReference ref = { slot, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    return ref;
}

inline VkAttachmentReference MkDepthRef(uint slot)
{
    VkAttachmentReference ref = { slot, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    return ref;
}

inline VkSubpassDescription MkSubpassDesc(uint ncolor)
{
    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = ncolor;
    return subpass;
}

inline VkFramebuffer MkFramebuffer(VkRenderPass pass, uint nattachment, VkImageView* attachments, uint width, uint height)
{
	VkFramebufferCreateInfo info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	info.renderPass = pass;
	info.attachmentCount = nattachment;
	info.pAttachments = attachments;
	info.width = width;
	info.height = height;
	info.layers = 1;
	return CreateFramebuffer(&info);
}

inline VkRenderPass MkRenderPass(uint nattachments, VkAttachmentDescription* attachments, uint nsubpass, VkSubpassDescription* subpasses, uint ndep, VkSubpassDependency* dep)
{
	VkRenderPassCreateInfo info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	info.attachmentCount = nattachments;
	info.pAttachments = attachments;
	info.subpassCount = nsubpass;
	info.pSubpasses = subpasses;
	info.dependencyCount = ndep;
	info.pDependencies = dep;
	return CreateRenderPass(&info);
}

inline VkImage MkVkImage(VkFormat format, VkImageUsageFlags usage, VkExtent2D extent, uint mip, uint ms)
{
	VkImageCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	info.imageType = VK_IMAGE_TYPE_2D;
	info.extent.width = extent.width;
	info.extent.height = extent.height;
	info.extent.depth = 1;
	info.mipLevels = mip;
	info.arrayLayers = 1;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.samples = VkSampleCountFlagBits(ms);
	info.format = format;
	info.usage = usage;
	return CreateImage(&info);
}

inline VkImageView MkImageView(VkImage img, VkFormat format, VkImageAspectFlags aspect)
{
	VkImageViewCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	info.image = img;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = format;
	info.subresourceRange.aspectMask = aspect;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.layerCount = 1;
	return CreateImageView(&info);
}

inline VkRenderPass MkRenderPass(uint ms)
{
	if (ms == 1)
	{
		VkAttachmentDescription attachments[2] = {
			MkColorAttachment(1, 1, VK_ATTACHMENT_LOAD_OP_CLEAR),
			MkDepthAttachment(1),
		};
		VkAttachmentReference ref[2] = { MkColorRef(0), MkDepthRef(1) };
		VkSubpassDescription subpass = MkSubpassDesc(1);
		subpass.pColorAttachments = &ref[0];
		subpass.pDepthStencilAttachment = &ref[1];
		return MkRenderPass(sizeof(attachments) / sizeof(VkAttachmentDescription), attachments, 1, &subpass, 0, 0);
	}

	VkAttachmentDescription attachments[3] = {
	MkColorAttachment(ms, 0, VK_ATTACHMENT_LOAD_OP_CLEAR),
	MkDepthAttachment(ms),
	MkColorAttachment(1, 1, VK_ATTACHMENT_LOAD_OP_LOAD)
	};
	VkAttachmentReference ref[3] = { MkColorRef(0), MkDepthRef(1), MkColorRef(2) };
	VkSubpassDescription subpass = MkSubpassDesc(1);
	subpass.pColorAttachments = &ref[0];
	subpass.pDepthStencilAttachment = &ref[1];
	subpass.pResolveAttachments = &ref[2];
	return MkRenderPass(sizeof(attachments) / sizeof(VkAttachmentDescription), attachments, 1, &subpass, 0, 0);
}