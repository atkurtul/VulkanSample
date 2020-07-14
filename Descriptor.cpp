#include "Descriptor.h"
#include "Device.h"
#include "Bindable.h"

DescriptorPool::DescriptorPool(vector<LayoutBinding>& bindings)
{
	vector<VkDescriptorPoolSize> poolsize;
	for (auto binding : bindings)
	{
		for (auto& pool : poolsize)
		{
			if (pool.type == binding.type) {
				pool.descriptorCount += 1024;
				continue;
			}
		}
		poolsize.push_back({ binding.type, 1024 });
	}

	VkDescriptorPoolCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	info.maxSets = remaining_sets = 1024;
	info.poolSizeCount = poolsize.size();
	info.pPoolSizes = poolsize.data();
	handle = CreateDescriptorPool(&info);
}

VkDescriptorSet DescriptorPool::TryAlloc(VkDescriptorSetLayout layout, BindableSet const& set)
{
	if (remaining_sets <= 0)
		return 0;
	auto it = sets.find(set);
	if (it != sets.end())
		return it->second;
	remaining_sets--;
	return sets[set] = AllocAndBind(layout, set);
}

VkDescriptorSet DescriptorPool::AllocAndBind(VkDescriptorSetLayout layout, BindableSet const& bset)
{
	VkDescriptorSet set;
	VkDescriptorSetAllocateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	info.descriptorPool = handle;
	info.pSetLayouts = &layout;
	info.descriptorSetCount = 1;
	AllocateDescriptorSets(&info, &set);
	vector<VkWriteDescriptorSet> writes(bset.size());
	uint i = 0;

	auto WriteSet = [](VkDescriptorSet set, VkDescriptorType type, uint binding)
	{
		VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.dstBinding = binding;
		write.dstSet = set;
		return write;
	};

	for (auto bind : bset)
	{
		writes[i] = WriteSet(set, bind->type, i);
		writes[i].pImageInfo = &bind->GetDescriptorInfo().image;
		writes[i].pBufferInfo = &bind->GetDescriptorInfo().buffer;
		i++;
	}
	UpdateDescriptorSets(writes.size(), writes.data(), 0, 0);
	return set;
}

VkDescriptorSet DescriptorSetLayout::FindSet(BindableSet&& set)
{
	for (auto& pool : pools)
		if (auto re = pool.TryAlloc(handle, set))
			return re;
	return pools.emplace_front(bindings).TryAlloc(handle, set);
}

void DescriptorSetLayout::Init()
{
	uint size = bindings.size();
	vector<VkDescriptorSetLayoutBinding> bind(size);
	vector<VkDescriptorBindingFlags> bFlags(size);
	uint i = 0;
	for (auto& type : bindings)
	{
		bFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		bind[i].binding = i;
		bind[i].descriptorType = type.type;
		bind[i].descriptorCount = 1;
		bind[i].stageFlags = type.stage;
		bind[i].pImmutableSamplers = 0;
		i++;
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo extFlags = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
	extFlags.bindingCount = bindings.size();
	extFlags.pBindingFlags = bFlags.data();
	VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	info.bindingCount = bindings.size();
	info.pNext = &extFlags;
	info.pBindings = bind.data();
	info.flags = 0;
	handle = CreateDescriptorSetLayout(&info);
	pools.emplace_front(bindings);
}
