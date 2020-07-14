#pragma once
#include "pch.h"
#include "vector"
#include "unordered_map"
#include "list"

using std::vector;
using std::unordered_map;
using std::list;

struct Bindable;
struct BindableSet : vector<Bindable*>
{
	using vector<Bindable*>::vector;
};

namespace std
{
	template<> struct hash<BindableSet>
	{
		inline size_t operator()(BindableSet const& set) const
		{
			hash<Bindable*> hash;
			size_t seed = hash(set[0]);
			for (int i = 1; i < set.size(); ++i)
				seed ^= hash(set[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

struct LayoutBinding { VkDescriptorType type; VkShaderStageFlags stage; };

struct DescriptorPool
{
	VkDescriptorPool handle;
	uint			 remaining_sets;
	unordered_map<BindableSet, VkDescriptorSet>	sets;
	DescriptorPool(vector<LayoutBinding>& bindings);

	VkDescriptorSet TryAlloc(VkDescriptorSetLayout layout, BindableSet const& set);

	VkDescriptorSet AllocAndBind(VkDescriptorSetLayout layout, BindableSet const& bset);
};

struct DescriptorSetLayout
{
	uint idx;
	vector<LayoutBinding>	bindings;
	VkDescriptorSetLayout	handle;
	list<DescriptorPool>	pools;
	VkDescriptorSet FindSet(BindableSet&& set);
	void Init();
};