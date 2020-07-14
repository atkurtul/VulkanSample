#pragma once

uint64 VkAllocLocal(uint size);
void VkFreeLocal(uint64 handle);
uint64 VkAllocShared(uint size);
void VkFreeShared(uint64 handle);
void FreeAllmemory();
void BindMem(VkBuffer buffer, uint64 handle);
void BindMem(VkImage image, uint64 handle);
void* MapMem(uint64 handle);
void debug_all_shared_pages();
void debug_all_pages();
