#include "Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_vulkan.h"

VkDescriptorPool imguiPool;

Renderer::Renderer(uint x, uint y, bool fs, uint ms, vector<PipelineCreateInfo> createInfos) :
    win(x, y, fs),
    sc(win.hwnd, win.GetExtent()),
    pass(MkRenderPass(ms)),
    ms(ms),
    current(-1),
    resource{},
    clear{},
    current_scene(Scene::Create(win))
{
    clear[0] = { 0.3, 0, 0.5, 1 };
    pipes.CreatePipelines(std::move(createInfos), pass, win.GetExtent(), ms);
    clear[1].depthStencil.depth = 1;
    clear[1].depthStencil.stencil = 1;
    Init();

    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool = CreateCommandPool(&poolInfo);

    VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = NFRAMES;
    AllocateCommandBuffers(&allocInfo, &cmd->handle);
    for (uint i = 0; i < NFRAMES; ++i)
        fence[i] = CreateFence(1);
}

void Renderer::Init()
{
    depthBuffer = Image::Create(VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        win.GetExtent(), 1, ms, VK_IMAGE_ASPECT_DEPTH_BIT);
    colorBuffer = Image::Create(VK_FORMAT_B8G8R8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        win.GetExtent(), 1, ms, VK_IMAGE_ASPECT_COLOR_BIT);
    uint count;
    VkImage* img = GetSwapchainImages(sc.handle, &count);
    for (uint i = 0; i < NFRAMES; ++i)
        resource[i].Init(img[i], ms, depthBuffer->view(), colorBuffer->view(), pass, win.GetExtent(), &sc.handle, clear, &cmd[i], i);
    delete[] img;
}

void Renderer::Recreate()
{
    DeviceWaitIdle();
    sc.Recreate(win.GetExtent());
    DestroyRenderPass(pass);
    pass = MkRenderPass(ms);
    pipes.Recreate(pass, win.GetExtent(), ms);
    depthBuffer->Free();
    colorBuffer->Free();
    Init();
    for (auto cb : callbacks)
        cb.call(this);
    current = 0;
}

void Renderer::AcquireNextImage()
{
    uint idx;
    current++;
    current %= NFRAMES;
    while (::AcquireNextImage(sc.handle, -1, resource[current].acquireSemaphore, 0, &idx) == VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf("\n----------\noutofdate\n-----------\n");
        Recreate();
    }
}

void Renderer::AddCallback(PFN_callback cb, void* ptr)
{
    callbacks.push_back({ cb, ptr });
}

void Renderer::BeginCommands()
{
    WaitForFences(1, &fence[current], 1, -1);
    ResetFences(1, &fence[current]);
    cmd[current].ResetCommandBuffer(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VkCommandBufferBeginInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    cmd[current].BeginCommandBuffer(&info);
}

void Renderer::BeginFrame()
{
    DrawImguiWindows();
    AcquireNextImage();
    BeginCommands();
    cmd[current].BeginRenderPass(&resource[current].passInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::EndFrame()
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd[current]);
    cmd[current].EndRenderPass();
    cmd[current].EndCommandBuffer();
    QueueSubmit(1, &resource[current].submitInfo, fence[current]);
    QueuePresent(&resource[current].presentInfo);
}

void Renderer::BindPipeline(const char* pipeline)
{
    cmd[current].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipes.GetPipeline(pipeline)->handle);
}

void Renderer::SetViewport(float x, float y, float w, float h)
{
    VkViewport vp = { x, y, w, h, 0.f, 1.f };
    cmd[current].SetViewport(0, 1, &vp);
}

void Renderer::BindVertexBuffer(Buffer* buffer)
{
    uint64 offsets[1] = { 0 };
    cmd[current].BindVertexBuffers(0, 1, &buffer->handle(), offsets);
}

void Renderer::BindIndexBuffer(Buffer* buffer, uint64 offset)
{
    cmd[current].BindIndexBuffer(buffer->handle(), offset, VK_INDEX_TYPE_UINT32);
}

void Renderer::Draw(uint nvertex, uint ninstance, uint first_vertex, uint first_inst)
{
    cmd[current].Draw(nvertex, ninstance, first_vertex, first_inst);
}

void Renderer::DrawIndexed(uint nidx, uint ninstance, uint first_idx, uint voffset, uint first_instance)
{
    cmd[current].DrawIndexed(nidx, ninstance, first_idx, voffset, first_instance);
}

void InitImguiVulkan(VkRenderPass pass, uint ms)
{
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = GetInstance();
    init_info.PhysicalDevice = GetPhysicalDevice();
    init_info.Device = GetDevice();
    init_info.QueueFamily = 0;
    init_info.Queue = GetQueue();
    init_info.PipelineCache = 0;
    init_info.DescriptorPool = imguiPool;
    init_info.Allocator = 0;
    init_info.MinImageCount = NFRAMES;
    init_info.ImageCount = NFRAMES;
    init_info.MSAASamples = VkSampleCountFlagBits(ms);
    init_info.CheckVkResultFn = [](VkResult err) {
        if (err == 0) return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0) abort();
    };

    ImGui_ImplVulkan_Init(&init_info, pass);
    auto cmd = MkCmdBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    SubmitCmd(cmd);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Renderer::InitImgui()
{
    VkDescriptorPoolSize poolsize[2] =  { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 }};
    VkDescriptorPoolCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    info.maxSets = 1024;
    info.poolSizeCount = 2;
    info.pPoolSizes = poolsize;
    imguiPool = CreateDescriptorPool(&info);

    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplWin32_Init(win.hwnd);
    InitImguiVulkan(pass, ms);

    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().ChildRounding = 0.0f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 0.0f;
    ImGui::GetStyle().PopupRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;
}

void Renderer::DrawImguiWindows()
{
    using namespace ImGui;
    ImGui_ImplWin32_NewFrame();
    NewFrame();
    current_scene->DrawHierarchy();
    Render();
}

void Scene::DrawHierarchy()
{
    using namespace ImGui;
    Begin("Scene");
    if (Checkbox("Use Flat Normals", (bool*)&use_flat_normals));
    SliderInt("Roughness", &roughness, 0, 255);
    if (Button("Load Model"))
    {
        if(auto m = Mesh::Create(OpenFile().data()))
            mesh.emplace_back(m);
    }

    if (TreeNode("Models"))
    {
        uint i = 0;
        for (auto& m : mesh)
        {
            PushID(i++);
            Separator();
            Text(m.mesh->name.data());
            //if (Button("Add animation"))
            //    AddAnimation(m.mesh, OpenFile().data());
            Separator();
            DragFloat3("Position", &m.xform.pos.x, 0.1);
            DragFloat3("Rotation", &m.xform.rot.x);
            DragFloat3("Scale", &m.xform.sca.x, 0.1);
            if (TreeNode("Materials"))
            {
                uint j = 0;
                for (auto& sm : m.submesh)
                {
                    PushID(j++);
                    Text(sm.name.data());
                    if (Button("Set Albedo"))
                        sm.mat.SetTexture(Texture::Create(OpenFile().data(), VK_FORMAT_R8G8B8A8_SRGB), PBR_ALBEDO);
                    if (Button("Set Normal"))
                        sm.mat.SetTexture(Texture::Create(OpenFile().data(), VK_FORMAT_R8G8B8A8_UNORM), PBR_NORMAL);
                    PopID();
                }
                TreePop();
            }

            PopID();
        }
        TreePop();
    }
    End();
}
