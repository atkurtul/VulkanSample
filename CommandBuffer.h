#pragma once
#include "Device.h"

struct CommandBuffer : AliasType<VkCommandBuffer>
{
	inline static VkCommandPool	pool;
	void BeginCommandBuffer(const VkCommandBufferBeginInfo* pBeginInfo);
	void EndCommandBuffer();
	void ResetCommandBuffer(VkCommandBufferResetFlags flags);
	void BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);
	void SetViewport(uint firstViewport, uint viewportCount, const VkViewport* pViewports);
	void SetScissor(uint firstScissor, uint scissorCount, const VkRect2D* pScissors);
	void SetLineWidth(float lineWidth);
	void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
	void SetBlendConstants(const float blendConstants[4]);
	void SetDepthBounds(float minDepthBounds, float maxDepthBounds);
	void SetStencilCompareMask(VkStencilFaceFlags faceMask, uint compareMask);
	void SetStencilWriteMask(VkStencilFaceFlags faceMask, uint writeMask);
	void SetStencilReference(VkStencilFaceFlags faceMask, uint reference);
	void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint firstSet, uint descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint dynamicOffsetCount, const uint* pDynamicOffsets);
	void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
	void BindVertexBuffers(uint firstBinding, uint bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets);
	void Draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance);
	void DrawIndexed(uint indexCount, uint instanceCount, uint firstIndex, int32_t vertexOffset, uint firstInstance);
	void DrawIndirect(VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride);
	void DrawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride);
	void Dispatch(uint groupCountX, uint groupCountY, uint groupCountZ);
	void DispatchIndirect(VkBuffer buffer, VkDeviceSize offset);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint regionCount, const VkBufferCopy* pRegions);
	void CopyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageCopy* pRegions);
	void BlitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageBlit* pRegions, VkFilter filter);
	void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkBufferImageCopy* pRegions);
	void CopyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint regionCount, const VkBufferImageCopy* pRegions);
	void UpdateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData);
	void FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint data);
	void ClearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint rangeCount, const VkImageSubresourceRange* pRanges);
	void ClearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint rangeCount, const VkImageSubresourceRange* pRanges);
	void ClearAttachments(uint attachmentCount, const VkClearAttachment* pAttachments, uint rectCount, const VkClearRect* pRects);
	void ResolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint regionCount, const VkImageResolve* pRegions);
	void SetEvent(VkEvent event, VkPipelineStageFlags stageMask);
	void ResetEvent(VkEvent event, VkPipelineStageFlags stageMask);
	void WaitEvents(uint eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers);
	void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers);
	void BeginQuery(VkQueryPool queryPool, uint query, VkQueryControlFlags flags);
	void EndQuery(VkQueryPool queryPool, uint query);
	void ResetQueryPool(VkQueryPool queryPool, uint firstQuery, uint queryCount);
	void WriteTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint query);
	void CopyQueryPoolResults(VkQueryPool queryPool, uint firstQuery, uint queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags);
	void PushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint offset, uint size, const void* pValues);
	void BeginRenderPass(const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents);
	void NextSubpass(VkSubpassContents contents);
	void EndRenderPass();
	void ExecuteCommands(uint commandBufferCount, const VkCommandBuffer* pCommandBuffers);
	void InsertImageMemoryBarrier(VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);
	void CopyTexture(VkBuffer src, VkImage image, unsigned width, unsigned height);
	void GenerateMips(uint mip, uint width, uint height, VkImage image);
	void PushDescriptorSet(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites);
};


inline CommandBuffer MkCmdBuffer()
{
	VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	info.commandPool = CommandBuffer::pool;
	info.commandBufferCount = 1;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	CommandBuffer cmd;
	AllocateCommandBuffers(&info, &cmd.handle);

	VkCommandBufferBeginInfo begin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	cmd.BeginCommandBuffer(&begin);

	return cmd;
}

inline void SubmitCmd(CommandBuffer cmd)
{
	VkSubmitInfo info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	info.commandBufferCount = 1;
	info.pCommandBuffers = &cmd.handle;
	cmd.EndCommandBuffer();
	VkFence fence = CreateFence(0);
	QueueSubmit(1, &info, fence);
	WaitForFences(1, &fence, 1, -1);
	DestroyFence(fence);
	FreeCommandBuffers(CommandBuffer::pool, 1, &cmd.handle);
}