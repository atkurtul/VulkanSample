#include "CommandBuffer.h"

void CommandBuffer::BeginCommandBuffer(const VkCommandBufferBeginInfo* pBeginInfo)
{
	vkBeginCommandBuffer(handle, pBeginInfo);
}

void CommandBuffer::EndCommandBuffer()
{
	vkEndCommandBuffer(handle);
}

void CommandBuffer::ResetCommandBuffer(VkCommandBufferResetFlags flags)
{
	vkResetCommandBuffer(handle, flags);
}

void CommandBuffer::BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
	vkCmdBindPipeline(handle, pipelineBindPoint, pipeline);
}

void CommandBuffer::SetViewport(uint firstViewport, uint viewportCount, const VkViewport* pViewports)
{
	vkCmdSetViewport(handle, firstViewport, viewportCount, pViewports);
}

void CommandBuffer::SetScissor(uint firstScissor, uint scissorCount, const VkRect2D* pScissors)
{
	vkCmdSetScissor(handle, firstScissor, scissorCount, pScissors);
}

void CommandBuffer::SetLineWidth(float lineWidth)
{
	vkCmdSetLineWidth(handle, lineWidth);
}

void CommandBuffer::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	vkCmdSetDepthBias(handle, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void CommandBuffer::SetBlendConstants(const float blendConstants[4])
{
	vkCmdSetBlendConstants(handle, blendConstants);
}

void CommandBuffer::SetDepthBounds(float minDepthBounds, float maxDepthBounds)
{
	vkCmdSetDepthBounds(handle, minDepthBounds, maxDepthBounds);
}

void CommandBuffer::SetStencilCompareMask(VkStencilFaceFlags faceMask, uint compareMask)
{
	vkCmdSetStencilCompareMask(handle, faceMask, compareMask);
}

void CommandBuffer::SetStencilWriteMask(VkStencilFaceFlags faceMask, uint writeMask)
{
	vkCmdSetStencilWriteMask(handle, faceMask, writeMask);
}

void CommandBuffer::SetStencilReference(VkStencilFaceFlags faceMask, uint reference)
{
	vkCmdSetStencilReference(handle, faceMask, reference);
}

void CommandBuffer::BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint firstSet, uint descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint dynamicOffsetCount, const uint* pDynamicOffsets)
{
	vkCmdBindDescriptorSets(handle, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

void CommandBuffer::BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(handle, buffer, offset, indexType);
}

void CommandBuffer::BindVertexBuffers(uint firstBinding, uint bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
	vkCmdBindVertexBuffers(handle, firstBinding, bindingCount, pBuffers, pOffsets);
}

void CommandBuffer::Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
{
	vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, int32_t vertexOffset, uint firstInstance)
{
	vkCmdDrawIndexed(handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::DrawIndirect(VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride)
{
	vkCmdDrawIndirect(handle, buffer, offset, drawCount, stride);
}

void CommandBuffer::DrawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride)
{
	vkCmdDrawIndexedIndirect(handle, buffer, offset, drawCount, stride);
}

void CommandBuffer::Dispatch(uint groupCountX, uint groupCountY, uint groupCountZ)
{
	vkCmdDispatch(handle, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::DispatchIndirect(VkBuffer buffer, VkDeviceSize offset)
{
	vkCmdDispatchIndirect(handle, buffer, offset);
}

void CommandBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint regionCount, const VkBufferCopy* pRegions)
{
	vkCmdCopyBuffer(handle, srcBuffer, dstBuffer, regionCount, pRegions);
}

void CommandBuffer::CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageCopy* pRegions)
{
	vkCmdCopyImage(handle, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

void CommandBuffer::BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
	vkCmdBlitImage(handle, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

void CommandBuffer::CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkBufferImageCopy* pRegions)
{
	vkCmdCopyBufferToImage(handle, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

void CommandBuffer::CopyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint regionCount, const VkBufferImageCopy* pRegions)
{
	vkCmdCopyImageToBuffer(handle, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

void CommandBuffer::UpdateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
	vkCmdUpdateBuffer(handle, dstBuffer, dstOffset, dataSize, pData);
}

void CommandBuffer::FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint data)
{
	vkCmdFillBuffer(handle, dstBuffer, dstOffset, size, data);
}

void CommandBuffer::ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint rangeCount, const VkImageSubresourceRange* pRanges)
{
	vkCmdClearColorImage(handle, image, imageLayout, pColor, rangeCount, pRanges);
}

void CommandBuffer::ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint rangeCount, const VkImageSubresourceRange* pRanges)
{
	vkCmdClearDepthStencilImage(handle, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

void CommandBuffer::ClearAttachments(uint attachmentCount, const VkClearAttachment* pAttachments, uint rectCount, const VkClearRect* pRects)
{
	vkCmdClearAttachments(handle, attachmentCount, pAttachments, rectCount, pRects);
}

void CommandBuffer::ResolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageResolve* pRegions)
{
	vkCmdResolveImage(handle, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

void CommandBuffer::SetEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	vkCmdSetEvent(handle, event, stageMask);
}

void CommandBuffer::ResetEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	vkCmdResetEvent(handle, event, stageMask);
}

void CommandBuffer::WaitEvents(uint eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	vkCmdWaitEvents(handle, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

void CommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	vkCmdPipelineBarrier(handle, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

void CommandBuffer::BeginQuery(VkQueryPool queryPool, uint query, VkQueryControlFlags flags)
{
	vkCmdBeginQuery(handle, queryPool, query, flags);
}

void CommandBuffer::EndQuery(VkQueryPool queryPool, uint query)
{
	vkCmdEndQuery(handle, queryPool, query);
}

void CommandBuffer::ResetQueryPool(VkQueryPool queryPool, uint firstQuery, uint queryCount)
{
	vkCmdResetQueryPool(handle, queryPool, firstQuery, queryCount);
}

void CommandBuffer::WriteTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint query)
{
	vkCmdWriteTimestamp(handle, pipelineStage, queryPool, query);
}

void CommandBuffer::CopyQueryPoolResults(VkQueryPool queryPool, uint firstQuery, uint queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	vkCmdCopyQueryPoolResults(handle, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

void CommandBuffer::PushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint offset, uint size, const void* pValues)
{
	vkCmdPushConstants(handle, layout, stageFlags, offset, size, pValues);
}

void CommandBuffer::BeginRenderPass(const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
	vkCmdBeginRenderPass(handle, pRenderPassBegin, contents);
}

void CommandBuffer::NextSubpass(VkSubpassContents contents)
{
	vkCmdNextSubpass(handle, contents);
}

void CommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(handle);
}

void CommandBuffer::ExecuteCommands(uint commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	vkCmdExecuteCommands(handle, commandBufferCount, pCommandBuffers);
}

void CommandBuffer::InsertImageMemoryBarrier(VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange)
{
	VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	PipelineBarrier(srcStageMask, dstStageMask, 0, 0, 0, 0, 0, 1, &imageMemoryBarrier);
}

void CommandBuffer::CopyTexture(VkBuffer src, VkImage image, unsigned width, unsigned height)

{
	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT };
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	InsertImageMemoryBarrier(image, 0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresourceRange);

	// Copy the first mip of the chain, remaining mips will be generated
	VkBufferImageCopy bufferCopyRegion = { 0 };
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.mipLevel = 0;
	bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = width;
	bufferCopyRegion.imageExtent.height = height;
	bufferCopyRegion.imageExtent.depth = 1;


	CopyBufferToImage(src, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

	InsertImageMemoryBarrier(image,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresourceRange);
}

void CommandBuffer::GenerateMips(uint mip, uint width, uint height, VkImage image)
{
	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT };
	subresourceRange.levelCount = mip;
	subresourceRange.layerCount = 1;

	for (uint i = 1; i < mip; ++i)
	{
		VkImageBlit imageBlit = { };
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.srcSubresource.mipLevel = i - 1;
		imageBlit.srcOffsets[1].x = std::max((uint)(width >> (i - 1)), 1u);
		imageBlit.srcOffsets[1].y = std::max((uint)(height >> (i - 1)), 1u);
		imageBlit.srcOffsets[1].z = 1;

		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.layerCount = 1;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstOffsets[1].x = std::max((uint)(width >> i), 1u);
		imageBlit.dstOffsets[1].y = std::max((uint)(height >> i), 1u);
		imageBlit.dstOffsets[1].z = 1;

		VkImageSubresourceRange mipSubRange = { VK_IMAGE_ASPECT_COLOR_BIT };
		mipSubRange.baseMipLevel = i;
		mipSubRange.levelCount = 1;
		mipSubRange.layerCount = 1;

		InsertImageMemoryBarrier(image,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			mipSubRange);

		BlitImage(image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&imageBlit,
			VK_FILTER_LINEAR);

		InsertImageMemoryBarrier(image,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			mipSubRange);
	}

	InsertImageMemoryBarrier(image,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		subresourceRange);
}

void CommandBuffer::PushDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
	VkEXTFN::PushDescriptorSet(handle, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}
