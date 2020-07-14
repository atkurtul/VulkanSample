#pragma once
#include "pch.h"
#include "Bindable.h"
#include "Descriptor.h"
#include "vmath.h"

struct Transform
{
    vec4 pos = { 0, 0, 0, 1 };
    vec4 rot = { };
    vec4 sca = 1;
    mat Get()
    {
        return mat::scale(sca.x, sca.y, sca.z) * rotxyz(rot * rad, pos);

        //vec4 x = MkQuat(vec4(1, 0, 0, 0), rot.x * rad);
        //vec4 y = MkQuat(vec4(0, 1, 0, 0), rot.y * rad);
        //vec4 z = MkQuat(vec4(0, 0, 1, 0), rot.z * rad);
        //return compose(qmul(qmul(x, y), z), pos);
    }

    mat blend(Transform const& xf, float t)
    {
        return compose(qlerp(rot, xf.rot, t), vlerp(pos, xf.pos, t));
    }
};

struct Submesh
{
    uint		nidx;
    uint		nvertex;
    uint		ioffset;
    uint		voffset;
    Material    mat;
    string      name;
};

struct Vertex
{
    vec3    pos;
    vec3    normal;
    vec3    tangent;
    vec3    bitangent;
    vec2    tex;
};

struct AnimationData
{
    vec3    pos;
    vec3    normal;
    vec3    tangent;
    vec3    bitangent;
    vec2    tex;

    uint boneID[3] = { };
    float weight[3] = { -0.f, -0.f , -0.f };

    void Set(uint id, float wgt)
    {
        if (weight[0] == -0.f) {
            boneID[0] = id;
            weight[0] = wgt;
            return;
        }
        if (weight[1] == -0.f) {
            boneID[1] = id;
            weight[1] = wgt;
            return;
        }
        if (weight[2] == -0.f) {
            boneID[2] = id;
            weight[2] = wgt;
            return;
        }
    }
};

struct Bone {
    mat                 bind;
    mat                 offset;
    string              name;
    vector<Transform>	poses;
    vector<Bone*>		children;
    uint                id;

    void update(float blend, uint f1, uint f2, mat* uniform, mat const& xf = mat::id())
    {
#if 1
        //if (poses.empty())
        //{
        //    mat pass = xf * bind;
        //    uniform[id] = pass * offset;
        //    for (auto c : children)
        //        c->update(blend, f1, f2, uniform, pass);
        //    return;
        //}
        mat pass = xf * poses[f1].blend(poses[f2], blend);
        uniform[id] = pass * offset;
        for (auto c : children)
            c->update(blend, f1, f2, uniform, pass);
#else
        mat pass =  poses[f1].blend(poses[f2], blend).tpos() * xf;
        uniform[id] = offset * pass;
        for (auto c : children)
            c->update(blend, f1, f2, uniform, pass);
#endif
    }
};

struct Animation
{
    float			dur;
    float			time;
    vector<float>	ticks;
    Bone*           root;
    uint			nBones;

    void update(float t, mat* uniform)
    {
        time += t;
        if (time >= dur) 
            time = 0;

        auto inBetween = [](float t, float a, float b)
        {
            return t >= a && t <= b;
        };

        float blend = 0;
        uint f1 = 0, f2 = 0;
        for (uint i = 1; i < ticks.size(); ++i)
        {
            if (inBetween(time, ticks[i - 1], ticks[i]))
            {
                float diff = ticks[i] - ticks[i - 1];
                blend = (blend - ticks[i - 1]) / diff;
                f1 = i - 1;
                f2 = i;
                break;
            }
        }
        root->update(blend, f1, f2, uniform);
    }
};


struct Submesh_cache
{
    vector<vec3u>   idx;
    vector<Vertex>  vert;
    uint            matidx;
    string          name;
};

struct Mesh
{
    string	            name;
    uint		        ioffset;
    vector<Submesh>	    submesh;
    Buffer*			    buffer;
    Buffer*             anim;

    Animation           animation;
    static Mesh* Create(const char* path, int extra_flags = 0);

    void UpdateAnimation(float t)
    {
        animation.update(t, anim->Get<mat>());
    }
};

struct MeshInstance
{
    Transform   xform;
    Mesh*       mesh;

    vector<Submesh>    submesh;
    MeshInstance(Mesh* origin) :
        mesh(origin), 
        submesh(origin->submesh)
    {

    }
};


bool LoadAnimation(const char* path, vector<AnimationData>& skin, Animation& anim, vector<vec3u>& idx);

void CompareFiles(const char* path1, const char* path2);

void AddAnimation(Mesh* mesh, const char* path);

unordered_map<string, Mesh*>& GetModels();
