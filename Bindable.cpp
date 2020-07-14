#include "Bindable.h"
#include "CommandBuffer.h"
#include "Allocator.h"
#include "Util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "unordered_map"
#include "list"
using std::unordered_map;
using std::list;

unordered_map<uint64, VkSampler> samplers;
unordered_map<string, Texture*> textures;
list<Buffer*> buffers;

#ifdef _DEBUG
#pragma comment(lib, "mangod.lib")
#else
#pragma comment(lib, "mango.lib")
#endif

#include "mango/mango.hpp"

VkSampler Sampler::Create(VkSamplerAddressMode mode, uint mip)
{
	uint64 key = mode;
	key <<= 32;
	key |= mip;
	auto sampler = samplers.find(key);
	if (sampler != samplers.end())
		return sampler->second;

	VkSamplerCreateInfo info = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	info.magFilter = VK_FILTER_LINEAR;
	info.minFilter = VK_FILTER_LINEAR;
	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	info.addressModeU = mode;
	info.addressModeV = mode;
	info.addressModeW = mode;
	info.anisotropyEnable = 1;
	info.maxAnisotropy = 16;
	info.maxLod = (float)mip;
	info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	info.compareOp = VK_COMPARE_OP_NEVER;
	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	return samplers[key] = CreateSampler(&info);
}

Buffer* Buffer::Create(VkBufferUsageFlags usage, uint size, DeviceMemoryTypeIndex mem)
{
	Buffer* buffer = new Buffer{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
	buffers.push_back(buffer);
	VkBufferCreateInfo info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	info.usage = usage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.size = size;
	buffer->handle() = CreateBuffer(&info);
	buffer->memory = mem ? VkAllocShared(size) : VkAllocLocal(size);
	buffer->info.buffer.range = ~0ull;
	BindMem(buffer->handle(), buffer->memory);
	return buffer;
}

Buffer* Buffer::Create(VkBufferUsageFlags usage, uint size)
{
	Buffer* buffer = Create(usage, size, SHARED_MEMORY);
	buffer->ptr = MapMem(buffer->memory);
	return buffer;
}

Buffer* Buffer::Create(VkBufferUsageFlags usage, uint size, void* src)
{
	Buffer* buffer = Create(usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, size, LOCAL_MEMORY);
	Buffer* staging = Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size);
	memcpy(staging->ptr, src, size);

	CommandBuffer cmd = MkCmdBuffer();
	VkBufferCopy copyRegion = { 0, 0, size };
	cmd.CopyBuffer(staging->handle(), buffer->handle(), 1, &copyRegion);
	SubmitCmd(cmd);
	staging->Free();

	return buffer;
}

void Buffer::Free()
{
	DestroyBuffer(handle());
	ptr ? VkFreeShared(memory) : VkFreeLocal(memory);
	buffers.erase(std::find(buffers.begin(), buffers.end(), this));
	delete this;
}

Image* Image::Create(VkFormat format, VkImageUsageFlags usage, VkExtent2D extent, uint mip, uint ms, VkImageAspectFlags aspect)
{
	Image* image = new Image;
	image->handle = MkVkImage(format, usage, extent, mip, ms);
	image->memory = VkAllocLocal((uint)GetImageMemoryRequirements(image->handle).size);
	BindMem(image->handle, image->memory);
	image->view() = MkImageView(image->handle, format, aspect);
	image->sampler() = Sampler::Create(VK_SAMPLER_ADDRESS_MODE_REPEAT, mip);
	switch (usage & (
		VK_IMAGE_USAGE_SAMPLED_BIT | 
		VK_IMAGE_USAGE_STORAGE_BIT | 
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | 
		VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT))
	{
	case VK_IMAGE_USAGE_SAMPLED_BIT:
		image->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		image->layout() = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case VK_IMAGE_USAGE_STORAGE_BIT:
		image->type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		image->layout() = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT:
		image->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		image->layout() = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT:
		image->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		image->layout() = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		break;
	case VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT:
		image->type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		image->layout() = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	}
	return image;
}

void Image::Free()
{
	DestroyImageView(info.image.imageView);
	VkFreeLocal(memory);
	DestroyImage(handle);
	delete this;
}

Buffer* load_raw_stb(const char* path, int* width, int* height, int* size)
{
	int  n;
	unsigned char* data = stbi_load(path, width, height, &n, 4);
	*size = *width * *height * 4;
	if (!data) 
		return 0;
	Buffer* staging = Buffer::Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *size);
	memcpy(staging->ptr, data, *size);
	::free(data);
	return staging;
}

Buffer* load_raw_mango(const char* path, int* width, int* height, int* size)
{
	try
	{
		mango::Bitmap bitmap(path, mango::Format(32, mango::Format::UNORM, mango::Format::RGBA, 8, 8, 8, 8));
		*width	= bitmap.width;
		*height = bitmap.height;
		*size	= *width * *height * 4;
		Buffer* staging = Buffer::Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *size);
		memcpy(staging->ptr, bitmap.address<uint>(0, 0), *size);
		return staging;
	}
	catch (...)
	{
		return 0;
	}
}

Buffer* load_raw(const char* path, int* width, int* height, int* size)
{
	printf("Loading %s\n", path);
	if (auto re = load_raw_stb(path, width, height, size))
		return re;
	if (auto re = load_raw_mango(path, width, height, size))
		return re;
	printf("Failed to load %s \n", path);
	return 0;
}

Texture* Texture::Create(const char* path, VkFormat format)
{
	auto t = textures.find(path);
	if (t != textures.end()) 
		return t->second;

	int width, height, size;
	Buffer* staging;
	if (!(staging = load_raw(path, &width, &height, &size)))
		return 0;

	uint mip = (uint)floor(log2(width > height ? width : height)) + 1;
	VkExtent2D extent = { (uint)width, (uint)height };
	Texture* tex = (Texture*)Image::Create(format,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT,
		extent, mip, 1, VK_IMAGE_ASPECT_COLOR_BIT);
	textures[path] = tex;
	//tex->name = path;
	CommandBuffer cmd = MkCmdBuffer();
	cmd.CopyTexture(staging->handle(), tex->handle, width, height);
	cmd.GenerateMips(mip, width, height, tex->handle);
	SubmitCmd(cmd);
	staging->Free();
	return tex;
}

void Texture::Free()
{
	Image::Free();
	for (auto it = textures.begin(); it != textures.end(); ++it)
	{
		if (it->second == this)
		{
			textures.erase(it);
			return;
		}
	}
}
