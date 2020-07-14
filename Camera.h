#pragma once
#include "pch.h"
#include "Window.h"

struct Camera
{
    mat prj;
    mat view;
    vec4 pos;
    vec4 ray;
    vec2 normalizer;
    double sens = 4;
    double speed = 0.2;
    Window* io;
    Camera(Window* io) : io(io)
    {
        view = mat::scale(-1, -1, -1);
        ray = view.z;
        pos = view.w;       
        set_prj(70.f * rad, io->size.x, io->size.y);
    }

    void set_prj(float fov, float x, float y)
    {
        prj = perspective(fov, x, y, 0.1, 4000);
        normalizer = { 1.f / prj.x.x, 1.f / prj.y.y }; 
    };

    void update_rotation()
    {
        if (!io->GetInputRepeat(257)) return;

        float s = rad * sens * 0.0243948;
        mat m = axang(view.x, s * io->mDelta.y);
        mat mx = roty(s * -io->mDelta.x);
        view = view * m * mx;
        vec2 mnorm = io->mNorm * normalizer;
        vec4 mmn = { mnorm.x, mnorm.y, 1, 0 };
        ray = norm(mmn * view);
    }

    void update_position()
    {
        vec4 ss = _mm_set_ps1(speed * io->step);
        if (io->GetInputRepeat('W')) pos = fmadd (ss, view.z, pos);
        if (io->GetInputRepeat('S')) pos = fnmadd(ss, view.z, pos);
        if (io->GetInputRepeat('A')) pos = fnmadd(ss, view.x, pos);
        if (io->GetInputRepeat('D')) pos = fmadd (ss, view.x, pos);
    }

    mat update()
    {
        update_rotation();
        update_position();
        mat view2 = view.tpos();
        view2.w = (pos ^ sign_mask<1, 1, 1, 0>()) * view2;
        return view2 * prj;
    }

};


