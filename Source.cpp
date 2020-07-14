#include "Pipeline.h"
#include "Allocator.h"
#include "Util.h"
#include "Renderer.h"
#include "vmath.h"
#include "Model.h"

struct App : Renderer
{
	App() :
		Renderer(800, 600, 1, 8, { { .shader = "shader1", .depth = 1, .cull = 1 },
			{ .shader = "anim",  .depth = 1, .cull = 1 }})
	{
		InitImgui();
	}

	int Run()
	{
		current_scene->mesh.emplace_back(Mesh::Create("models\\batman\\0.obj"));
		current_scene->mesh.emplace_back(Mesh::Create("models\\doom\\0.obj"));
		current_scene->mesh.emplace_back(Mesh::Create("models\\sponza\\sponza.obj"));

		current_scene->mesh[0].xform.pos.x += 2;
		current_scene->mesh[2].xform.sca = vec4(0.01, 0.01, 0.01, 1);
		current_scene->roughness = 144;
		while (PollWindows())
		{
			BeginFrame();
			RenderScene();
			EndFrame();
		}
		return 1;
	}
};

int main()
{
	srand(time(0));
	InitVulkan();
	App().Run();
}