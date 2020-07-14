#include "Model.h"
#include "Renderer.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

unordered_map<string, Mesh*> models;

bool LoadMesh(const char* path, vector<Submesh_cache>& meshes, vector<Material>& materials, int extra_flags = 0)
{
	Assimp::Importer imp;
	auto scene = imp.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs | 
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		extra_flags);

	if (!scene)
		return 0;
	std::string folder_path = path;
	folder_path = folder_path.substr(0, folder_path.find_last_of("\\") + 1);
	materials.clear();
	materials.resize(scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		auto mat = scene->mMaterials[i];
		if (mat->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			materials[i].SetTexture(Texture::Create((folder_path + str.C_Str()).data(), VK_FORMAT_R8G8B8A8_SRGB), PBR_ALBEDO);
		}
		if (mat->GetTextureCount(aiTextureType_HEIGHT))
		{
			aiString str;
			mat->GetTexture(aiTextureType_HEIGHT, 0, &str);
			materials[i].SetTexture(Texture::Create((folder_path + str.C_Str()).data(), VK_FORMAT_R8G8B8A8_UNORM), PBR_NORMAL);
		}
	}

	meshes.clear();
	meshes.resize(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[i];
		auto& mm = meshes[i];
		mm.name = mesh->mName.data;
		mm.matidx = mesh->mMaterialIndex;
		mm.idx.resize(mesh->mNumFaces);
		for (int j = 0; j < mesh->mNumFaces; ++j)
		{
			mm.idx[j] = (vec3u&)*mesh->mFaces[j].mIndices;
		}

		mm.vert.resize(mesh->mNumVertices);
		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			if (mesh->HasPositions())
				mm.vert[j].pos = (vec3&)mesh->mVertices[j];
			if (mesh->HasNormals())
				mm.vert[j].normal = (vec3&)mesh->mNormals[j];
			if (mesh->HasTextureCoords(0))
				mm.vert[j].tex = (vec2&)mesh->mTextureCoords[0][j];

		
			if (!mesh->HasTangentsAndBitangents()) continue;
			mm.vert[j].tangent = (vec3&)mesh->mTangents[j];
			mm.vert[j].bitangent = (vec3&)mesh->mBitangents[j];
		}
	}

	return 1;
}

Mesh* Mesh::Create(const char* path, int extra_flags)
{
	vector<Submesh_cache> meshes;
	vector<Material> mats;

	if (!LoadMesh(path, meshes, mats, extra_flags))
		return 0;
	Mesh* mesh = new Mesh{};
	mesh->name = path;
	mesh->submesh.resize(meshes.size());
	uint idx = 0;
	uint nidx = 0;
	uint nvertex = 0;
	for (auto& m : meshes)
	{
		mesh->submesh[idx].name = m.name;
		mesh->submesh[idx].mat = mats[m.matidx];
		mesh->submesh[idx].nidx = m.idx.size() * 3;
		mesh->submesh[idx].nvertex = m.vert.size();
		mesh->submesh[idx].ioffset = nidx;
		mesh->submesh[idx].voffset = nvertex;
		nidx += mesh->submesh[idx].nidx;
		nvertex += mesh->submesh[idx].nvertex;
		++idx;
	}
	mesh->ioffset = nvertex * sizeof(Vertex);
	uint64 size = mesh->ioffset + nidx * 4;
	char* blob = (char*)malloc(size);

	idx = 0;
	for (auto& m : meshes)
	{
		auto& sm = mesh->submesh[idx];
		memcpy(blob + sm.voffset * sizeof(Vertex), m.vert.data(), m.vert.size() * sizeof(Vertex));
		memcpy(blob + mesh->ioffset + sm.ioffset * 4, m.idx.data(), m.idx.size() * sizeof(vec3u));
		++idx;
	}
	mesh->buffer = Buffer::Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size, blob);
	free(blob);
	models[path] = mesh;
	return mesh;
}

void AddAnimation(Mesh* mesh, const char* path)
{
	vector<AnimationData> skin;
	vector<vec3u> idx;
	Animation anim;
	if(!LoadAnimation(path, skin, anim, idx))
		return;

	DeviceWaitIdle();
	mesh->buffer->Free();
	mesh->ioffset = skin.size() * sizeof(AnimationData);
	uint total = mesh->ioffset + idx.size() * sizeof(vec3u);
	char* blob = (char*)malloc(total);
	memcpy(blob, skin.data(), mesh->ioffset);
	memcpy(blob + mesh->ioffset, idx.data(), idx.size() * sizeof(vec3u));
	mesh->buffer = Buffer::Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, total, blob);
	mesh->anim	= Buffer::Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 64 * 64);
	mesh->animation = anim;
	free(blob);
}

unordered_map<string, Mesh*>& GetModels()
{
	return models;
}

