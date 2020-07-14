#pragma once
#include "vulkan/vulkan.h"
#include "iostream"
#include "string.h"
#include "string"

#include "stdlib.h"
#include "stdio.h"
#include "algorithm"
#include "fstream"
#include "xtimec.h"
#ifndef NFRAMES
#define NFRAMES 3
#endif // !NFRAMES

typedef unsigned uint;
typedef unsigned long long uint64;
typedef long long int64;
using std::string;


template<class T>
struct AliasType
{
    T handle;
    operator T() { return handle; }
    T* operator &() { return &handle; }
    operator bool() { return handle; }
};

enum DeviceMemoryTypeIndex
{
    LOCAL_MEMORY = 0,
    SHARED_MEMORY = 1,
};

