#include "Pipeline.h"
#include "Util.h"
#include "direct.h"
#include "shaderc/shaderc.hpp"
#include "spirv_cross/spirv_glsl.hpp"


using namespace SPIRV_CROSS_NAMESPACE;

static VkPipelineShaderStageCreateInfo* mk_pss_create_info(PipelineCreateInfo& createInfo)
{
	auto info = new VkPipelineShaderStageCreateInfo[2]{};
	info[0].sType = info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info[0].pName = info[1].pName = "main";
	info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkShaderModuleCreateInfo vs = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	VkShaderModuleCreateInfo ps = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	vs.pCode = createInfo.vs.data();
	ps.pCode = createInfo.ps.data();
	vs.codeSize = createInfo.vs.size() * 4;
	ps.codeSize = createInfo.ps.size() * 4;


	info[0].module = CreateShaderModule(&vs);
	info[1].module = CreateShaderModule(&ps);
	return info;
}

static VkPipelineVertexInputStateCreateInfo* mk_vertex_input_layout_desc(Compiler& comp, ShaderResources& res)
{
	auto type_stride = [](SPIRType::BaseType type)
	{
		switch (type)
		{
		case SPIRType::Int: case SPIRType::UInt: case SPIRType::Float: return 4;
		case SPIRType::Int64: case SPIRType::UInt64: case SPIRType::Double:return 8;
		default:return 0;
		}
	};

	auto type_format = [](SPIRType::BaseType type, uint vecsize)
	{
		uint format = 98;
		switch (vecsize)
		{
		case 4:	format += 3;
		case 3:	format += 3;
		case 2: format += 3;
		}
		switch (type)
		{
		case SPIRType::Float: format++;
		case SPIRType::Int: format++;
		}
		return VkFormat(format);
	};

	auto info	 = new VkPipelineVertexInputStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	auto binding = new VkVertexInputBindingDescription{};
	auto attribs = new VkVertexInputAttributeDescription[res.stage_inputs.size()]{};

	vector<Resource*> inputs(res.stage_inputs.size());
	for (auto& in : res.stage_inputs)
		inputs[comp.get_decoration(in.id, spv::DecorationLocation)] = &in;

	for (auto in : inputs)
	{
		SPIRType type = comp.get_type(in->type_id);
		auto location = comp.get_decoration(in->id, spv::DecorationLocation);
		attribs[location].location = location;
		attribs[location].offset = binding->stride;
		attribs[location].format = type_format(type.basetype, type.vecsize);
		binding->stride += type_stride(type.basetype) * type.vecsize;
	}

	info->pVertexAttributeDescriptions = attribs;
	info->pVertexBindingDescriptions = binding;
	info->vertexBindingDescriptionCount = 1;
	info->vertexAttributeDescriptionCount = (uint)res.stage_inputs.size();
	return info;
}

static VkPipelineInputAssemblyStateCreateInfo* mk_assembler_create_info(VkPrimitiveTopology topology)
{
	auto info = new VkPipelineInputAssemblyStateCreateInfo{};
	info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info->topology = topology;
	return info;
}

static VkPipelineViewportStateCreateInfo* mk_viewport_state_create_info(VkExtent2D extent)
{
	auto viewport = new VkViewport{};
	auto scissor = new VkRect2D{};

	viewport->width		= (float)extent.width;
	viewport->height	= (float)extent.height;
	viewport->maxDepth = 1.f;

	scissor->extent = extent;

	auto info = new VkPipelineViewportStateCreateInfo{};
	info->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info->viewportCount = 1;
	info->scissorCount = 1;
	info->pScissors = scissor;
	info->pViewports = viewport;

	return info;
}

static VkPipelineRasterizationStateCreateInfo* mk_rasterizer_create_info(VkPolygonMode mode, int cull)
{
	auto info = new VkPipelineRasterizationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	info->polygonMode = mode;
	info->lineWidth = 1.f;
	info->frontFace = VK_FRONT_FACE_CLOCKWISE;
	info->cullMode = cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
	return info;
}

static VkPipelineMultisampleStateCreateInfo* mk_ms_create_info(uint ms)
{
	auto info = new VkPipelineMultisampleStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	info->rasterizationSamples = VkSampleCountFlagBits(ms);
	info->sampleShadingEnable = ms > 1;
	return info;
}

static VkPipelineDepthStencilStateCreateInfo* mk_dss_create_info(int depth)
{
	auto info = new VkPipelineDepthStencilStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	info->depthTestEnable = depth;
	info->depthWriteEnable = 1;
	info->depthCompareOp = VK_COMPARE_OP_LESS;
	return info;
}

static VkPipelineColorBlendStateCreateInfo* mk_blend_state_create_info(int blend)
{
	auto info = new VkPipelineColorBlendStateCreateInfo{};
	auto attachment = new VkPipelineColorBlendAttachmentState{};
	info->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info->attachmentCount = 1;
	info->pAttachments = attachment;
	attachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	if (!blend) return info;

	attachment->blendEnable = 1;
	attachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	attachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	attachment->colorBlendOp = VK_BLEND_OP_ADD;
	attachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	attachment->alphaBlendOp = VK_BLEND_OP_ADD;

	return info;
}

static void FillPipelineInfo(Pipeline* pipe, Compiler& comp, ShaderResources& res, PipelineCreateInfo& createInfo, VkExtent2D extent, uint ms)
{
	pipe->info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipe->info.stageCount = 2;
	pipe->info.pStages = mk_pss_create_info(createInfo);
	pipe->info.pVertexInputState = mk_vertex_input_layout_desc(comp, res);
	pipe->info.pInputAssemblyState = mk_assembler_create_info(createInfo.topolgy);
	pipe->info.pViewportState = mk_viewport_state_create_info(extent);
	pipe->info.pRasterizationState = mk_rasterizer_create_info(createInfo.mode, createInfo.cull);
	pipe->info.pMultisampleState = mk_ms_create_info(ms);
	pipe->info.pDepthStencilState = mk_dss_create_info(createInfo.depth);
	pipe->info.pColorBlendState = mk_blend_state_create_info(createInfo.blend);
}

static void UpdatePipelineAndDestroy(Pipeline* pipe, VkExtent2D extent, uint ms)
{
	auto viewport = (VkViewport*)pipe->info.pViewportState->pViewports;
	auto scissor  = (VkRect2D*)pipe->info.pViewportState->pScissors;
	viewport->width = (float)extent.width;
	viewport->height = (float)extent.height;
	scissor->extent = extent;
	auto msState = (VkPipelineMultisampleStateCreateInfo*)pipe->info.pMultisampleState;
	msState->rasterizationSamples = VkSampleCountFlagBits(ms);
	msState->sampleShadingEnable = ms > 1;
	DestroyPipeline(pipe->handle);
}

void PipelineManager::CreatePipelines(vector<PipelineCreateInfo> infos, VkRenderPass pass, VkExtent2D extent, uint ms)
{
	auto load_spirv_from_file = [](const char* path, shaderc_shader_kind shader_type)
	{
		string spath = "shaders\\";
		spath += path;
		std::ifstream code(spath);
		std::string byteCode((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
		code.close();
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		auto pp = compiler.PreprocessGlsl(byteCode, shader_type, spath.data(), options);
		auto compiling = compiler.CompileGlslToSpv(byteCode, shader_type, spath.data(), options);
		auto msg = compiling.GetErrorMessage();
		if(msg.size()) printf("[SPIRV ERROR]\n%s\n", msg.data());
		return vector<uint>(compiling.cbegin(), compiling.cend());
	};

	auto ParseUniforms = [this](vector<std::pair<VkDescriptorType, SmallVector<Resource>*>>&& uniforms, CompilerGLSL& comp, VkShaderStageFlagBits stage)
	{
		for (auto& pair : uniforms)
		{
			for (auto& in : *pair.second)
			{
				uint set = comp.get_decoration(in.id, spv::DecorationDescriptorSet);
				uint binding = comp.get_decoration(in.id, spv::DecorationBinding);
				if (descLayouts.size() <= set) descLayouts.resize(set + 1);
				auto& layout = descLayouts[set];
				layout.idx = set;
				if (layout.bindings.size() <= binding)
					layout.bindings.resize(binding + 1);
				layout.bindings[binding].type = pair.first;
				layout.bindings[binding].stage |= stage;
			}
		}
	};

	for (auto& info : infos)
	{
		string vs = info.shader;
		string ps = info.shader;
		vs += ".vert";
		ps += ".frag";
		info.vs = load_spirv_from_file(vs.data(), shaderc_vertex_shader);
		info.ps = load_spirv_from_file(ps.data(), shaderc_fragment_shader);
		CompilerGLSL comp_vs(info.vs);
		CompilerGLSL comp_ps(info.ps);
		ShaderResources res_vs = comp_vs.get_shader_resources();
		ShaderResources res_ps = comp_ps.get_shader_resources();
		Pipeline* pipe = new Pipeline{};
		pipelines[info.shader] = pipe;
		ParseUniforms({
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &res_vs.uniform_buffers},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &res_vs.storage_buffers },
			}, comp_vs, VK_SHADER_STAGE_VERTEX_BIT);
		ParseUniforms({
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &res_ps.uniform_buffers},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &res_ps.storage_buffers },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, &res_ps.storage_images},
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, &res_ps.subpass_inputs},
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &res_ps.sampled_images}
			}, comp_ps, VK_SHADER_STAGE_FRAGMENT_BIT);
		FillPipelineInfo(pipe, comp_vs, res_vs, info, extent, ms);
	}

	vector<VkDescriptorSetLayout> tmp_layouts;
	for (auto& layout : descLayouts)
	{
		layout.Init();
		tmp_layouts.push_back(layout.handle);
	}

	VkPushConstantRange range = { 17, 0, 256 };
	VkPipelineLayoutCreateInfo layoutinfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutinfo.setLayoutCount = (uint)tmp_layouts.size();
	layoutinfo.pSetLayouts	  = tmp_layouts.data();
	layoutinfo.pushConstantRangeCount = 1;
	layoutinfo.pPushConstantRanges = &range;
	layout = CreatePipelineLayout(&layoutinfo);

	vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos;
	for (auto& pipeline : pipelines) {
		pipeline.second->info.layout = layout;
		pipeline.second->info.renderPass = pass;
		pipelineCreateInfos.push_back(pipeline.second->info);
	}

	vector<VkPipeline> tmp_pipelines(pipelineCreateInfos.size());

	CreateGraphicsPipelines(0, (uint)pipelineCreateInfos.size(), pipelineCreateInfos.data(), tmp_pipelines.data());
	uint i = 0;
	for (auto pipeline : pipelines)
		pipeline.second->handle = tmp_pipelines[i++];
}

void PipelineManager::Recreate(VkRenderPass pass, VkExtent2D extent, uint ms)
{
	printf("Recreating pipelines\n");
	vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos;
	for (auto& pipeline : pipelines) {
		UpdatePipelineAndDestroy(pipeline.second, extent, ms);
		pipeline.second->info.renderPass = pass;
		pipelineCreateInfos.push_back(pipeline.second->info);
	}
	vector<VkPipeline> tmp_pipelines(pipelineCreateInfos.size());
	CreateGraphicsPipelines(0, (uint)pipelineCreateInfos.size(), pipelineCreateInfos.data(), tmp_pipelines.data());
	uint i = 0;
	for (auto pipeline : pipelines)
		pipeline.second->handle = tmp_pipelines[i++];
}

