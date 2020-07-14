#pragma once
#include "pch.h"
#include "Device.h"
#include "Bindable.h"
#include "Descriptor.h"

struct PipelineCreateInfo
{
	const char* shader;
	int depth;
	int cull;
	int blend;
	VkPolygonMode mode = VK_POLYGON_MODE_FILL;
	VkPrimitiveTopology topolgy = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vector<uint> vs, ps;
};

struct Pipeline
{
	VkPipeline handle;
	VkGraphicsPipelineCreateInfo info;
};

struct PipelineManager
{
	unordered_map<const char*, Pipeline*>		pipelines;
	vector<DescriptorSetLayout>					descLayouts;
	VkPipelineLayout							layout;

	template<Bindable_T...T>
	VkDescriptorSet FindSet(uint slot, Bindable* head, T*... tail)
	{
		return descLayouts[slot].FindSet({ head, tail... });
	}

	void CreatePipelines(vector<PipelineCreateInfo> infos, VkRenderPass pass, VkExtent2D extent, uint ms);

	void Recreate(VkRenderPass pass, VkExtent2D extent, uint ms);

	Pipeline* GetPipeline(const char* shader)
	{
		return pipelines[shader];
	}
};