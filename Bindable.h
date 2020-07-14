#pragma once
#include "pch.h"

struct Sampler
{
	static VkSampler Create(VkSamplerAddressMode mode, uint mip);
};

struct Bindable
{
	VkDescriptorType type;
	union DescriptorInfo
	{
		VkDescriptorImageInfo  image;
		VkDescriptorBufferInfo buffer;
	} info;

	DescriptorInfo& GetDescriptorInfo()
	{
		return info;
	}
};

template<class T>
concept Bindable_T = std::is_base_of_v<Bindable, T>;

struct Buffer : Bindable
{
	uint64		memory;
	void*		ptr;
	VkBuffer& handle() { return info.buffer.buffer; }
	static Buffer* Create(VkBufferUsageFlags usage, uint size, DeviceMemoryTypeIndex mem);
	static Buffer* Create(VkBufferUsageFlags usage, uint size);
	static Buffer* Create(VkBufferUsageFlags usage, uint size, void* src);
	void Free();

	template<class T> T* Get() { return (T*)ptr; }
};

struct Image : Bindable
{
	uint64	memory;
	VkImage	handle;
	VkImageView& view() { return info.image.imageView; }
	VkSampler& sampler() { return info.image.sampler; }
	VkImageLayout& layout() { return info.image.imageLayout; }
	static Image* Create(VkFormat format, VkImageUsageFlags usage, VkExtent2D extent, uint mip, uint ms, VkImageAspectFlags aspect);
	void Free();
};

struct Texture : Image
{
	static Texture* Create(const char* path, VkFormat format);
	void Free();
};

enum PBRSlot
{
	PBR_ALBEDO = 0,
	PBR_NORMAL = 1,
	PBR_METALIC = 2,
};

struct Material
{
	Texture* textures[3] = { 
		Texture::Create("white.png", VK_FORMAT_R8G8B8A8_SRGB), 
		Texture::Create("normal.png", VK_FORMAT_R8G8B8A8_UNORM) , 
		Texture::Create("white.png", VK_FORMAT_R8G8B8A8_UNORM) 
	};

	void SetTexture(Texture* tex, PBRSlot slot)
	{
		if(tex)	textures[slot] = tex;
	}
};
