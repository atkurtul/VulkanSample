#pragma once
#include "Swapchain.h"
#include "Window.h"
#include "CommandBuffer.h"
#include "Pipeline.h"
#include "Camera.h"
#include "Model.h"

struct Scene
{
    struct Light
    {
        vec4 pos;
        vec4 color;
    };

    struct LightWrapper : Light
    {
        float radius = 1;
        float t = 0;
        float speed = 1;
    };

    Buffer*                 cbuffer;
    Camera*                 active_camera;
    vector<Camera*>			cameras;
    vector<MeshInstance>	mesh;
    vector<LightWrapper>    lights;
    Window* io;
    int use_flat_normals = 0;
    int roughness = 255;

    inline static vector<Scene*> scenes;
    static Scene* Create(Window& win)
    {
        Scene* scene = new Scene{};
        scenes.push_back(scene);
        scene->io = &win;
        scene->active_camera = new Camera(&win);
        scene->cameras.push_back(scene->active_camera);
        scene->cbuffer = Buffer::Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1024 * 64);
        scene->lights.resize(32);
        for (auto& light : scene->lights)
        {
            light.t = float(rand() % 255) / 255;
            light.speed = 0.01 + 0.5 * float(rand() % 255) / 255;
            light.radius = 8 * (float(rand() % 255) / 255.f + 0.01);
            light.color.x = float(rand() % 255) / 255;
            light.color.y = float(rand() % 255) / 255;
            light.color.z = float(rand() % 255) / 255;
            light.color.w = 1;
        }
        return scene;
    }

    void UpdateBuffer()
    {
        UpdateLights();
        cbuffer->Get<mat>()[0]  = active_camera->update();
        cbuffer->Get<vec4>()[4] = active_camera->pos;
        cbuffer->Get<int>()[20] = lights.size();
        cbuffer->Get<int>()[21] = use_flat_normals;
        cbuffer->Get<int>()[22] = roughness;
        Light* light = (Light*)(cbuffer->Get<vec4>() + 6);
        for (int i = 0; i < lights.size(); ++i)
            light[i] = lights[i];
    }

    void UpdateLights()
    {
        for (auto& light : lights)
        {
            light.t += io->step * light.speed * 0.1;
            const float angle = 2 * pi  * light.t;
            light.pos = vec4(cosf(angle) * light.radius, 0, sinf(angle) * light.radius, 1);
        }
    }

    void DrawHierarchy();
};

struct Renderer
{
    Window          win;
    Swapchain       sc;
    VkRenderPass    pass;
    uint            ms;
    uint            current;
    Image*          depthBuffer;
    Image*          colorBuffer;
    PipelineManager pipes;

    VkCommandPool   pool;
    VkClearValue    clear[3];
    FrameResource   resource[NFRAMES];
    CommandBuffer   cmd[NFRAMES];
    VkFence         fence[NFRAMES];

    typedef void (*PFN_callback)(void* ptr, struct Renderer* r);

    struct Callback {
        PFN_callback fn;
        void* ptr;
        void call(Renderer* r) { fn(ptr, r); }
    };

    vector<Callback> callbacks;

    Scene* current_scene;

    Renderer(uint x, uint y, bool fs, uint ms, vector<PipelineCreateInfo> createInfos);
    void Init();
    void Recreate();
    void AcquireNextImage();
    void AddCallback(PFN_callback cb, void* ptr);
    void BeginCommands();
    void BeginFrame();
    void EndFrame();
    void BindPipeline(const char* pipeline);
    void SetViewport(float x, float y, float w, float h);
    void BindVertexBuffer(Buffer* buffer);
    void BindIndexBuffer(Buffer* buffer, uint64 offset);
    void Draw(uint nvertex, uint ninstance, uint first_vertex, uint first_inst);
    void DrawIndexed(uint nidx, uint ninstance, uint first_idx, uint voffset, uint first_instance);
    void InitImgui();
    void DrawImguiWindows();

    template<class...T> 
    void BindSet(uint slot, Bindable* head, T*... tail)
    {
        auto set = pipes.FindSet(slot, head, tail...);
        cmd[current].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.layout, slot, 1, &set, 0, 0);
    }

    template<class T>
    void PushConstants(T&& constant)
    {
        cmd[current].PushConstants(pipes.layout, VkShaderStageFlags(17), 0, sizeof(T), (void*)&constant);
    }

    void RenderScene()
    {
        auto scene = current_scene;
        scene->UpdateBuffer();
        BindPipeline("shader1");
        BindSet(0, scene->cbuffer);
        for (auto& m : scene->mesh)
        {
            PushConstants(m.xform.Get());
            BindVertexBuffer(m.mesh->buffer);
            BindIndexBuffer(m.mesh->buffer, m.mesh->ioffset);
            for (auto& sm : m.submesh)
            {
                BindSet(2, sm.mat.textures[0], sm.mat.textures[1], sm.mat.textures[2]);
                DrawIndexed(sm.nidx, 1, sm.ioffset, sm.voffset, 0);
            }
        }     
    }
};

