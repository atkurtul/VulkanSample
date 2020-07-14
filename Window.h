#pragma once
#include "pch.h"
#include "vector"
#include "string"
#include "VirtualKeys.h"
#include "vmath.h"

using std::string;
using std::vector;

struct Window
{
    struct InputState {
        vec2	pos;
        double	time;
        int		state;
    } input[259];

    vec2 pos;
    vec2 size;
    vec2 mpos;
    vec2 mDelta;
    vec2 invSize;
    vec2 mNorm;
    std::string text;
    double step;
    double time;
    long long now;
    void* hwnd;
	Window(int x, int y, int fs);
    void Update();

    VkExtent2D GetExtent()
    {
        return { (uint)size.x, (uint)size.y };
    }

    bool InRect(vec2 p, vec2 s)
    {
        return
        mNorm.x > p.x && mNorm.x < p.x + s.x &&
        mNorm.y > p.y && mNorm.y < p.y + s.y;
    }

    bool GetInput(int key)
    {
        return input[key].state && input[key].time + step * 0.5 > time;
    }

    bool GetInputRepeat(int key)
    {
        return input[key].state;
    }

    void Resize(int x, int y);
};

int PollWindows();
string OpenFile();
