#include "Model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <set>

using std::set;

uint GetID(vector<string>& v, const char* n)
{
	for (int i = 0; i < v.size(); ++i) if (v[i] == n) return i;
	v.push_back(n);
	return v.size() - 1;
}

uint GetIDUnique(vector<string>& v, const char* n)
{
	for (int i = 0; i < v.size(); ++i) 
		if (v[i] == n) return i;
	return -1;
}

vector<string> GetNames(const aiScene* scene)
{
	set<string>  jointIDs;
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		for (int j = 0; j < scene->mMeshes[i]->mNumBones; ++j)
		{
			jointIDs.emplace(scene->mMeshes[i]->mBones[j]->mName.C_Str());
		}
	}
	vector<string> re;
	for (auto s : jointIDs)
		re.push_back(s);
	return re;
}

bool Exists(vector<string>& names, aiNode* node)
{
	for (auto& s : names)
		if (s == node->mName.data)	return true;
	return false;
}

void InitBone(Bone* bone, aiNode* node, vector<string>& names, vector<mat>& offsets)
{
	uint id = bone->id = GetID(names, node->mName.data);
	bone->name = node->mName.data;
#if 0
	bone->bind = mat::load(&node->mTransformation);
	bone->offset = offsets[id];
#else
	bone->bind = mat::load(&node->mTransformation).tpos();
	bone->offset = offsets[id].tpos();
#endif
	printv(bone->bind);
	printv(bone->offset);
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		auto child = node->mChildren[i];
		if (Exists(names, child))
		{
			auto b = new Bone{};
			bone->children.push_back(b);
			InitBone(b, node->mChildren[i], names, offsets);
		}
	}
}

aiNode* FindRoot(aiNode* node, vector<string>& names)
{
	if (Exists(names, node))
		return node;
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		auto mby = FindRoot(node->mChildren[i], names);
		if (mby) return mby;
	}
	return 0;
}

void InsertChannels(Bone* bone, const aiAnimation* anim)
{
	for (int i = 0; i < anim->mNumChannels; ++i)
	{
		if (bone->name == anim->mChannels[i]->mNodeName.C_Str())
		{
			auto channel = anim->mChannels[i];
			for (int j = 0; j < channel->mNumPositionKeys; ++j)
			{
				Transform xf;
				xf.pos = vec4::load(&anim->mChannels[i]->mPositionKeys[j].mValue.x);
				xf.rot = vec4::load(&anim->mChannels[i]->mRotationKeys[j].mValue.x);
				xf.sca = vec4::load(&anim->mChannels[i]->mScalingKeys[j].mValue.x);
				xf.pos.w = xf.sca.w = 1;
				bone->poses.push_back(xf);
			}
		}
	}
}

void FillBones(Bone* bone, const aiAnimation* anim, uint& nBones)
{
	nBones++;
	InsertChannels(bone, anim);
	for (int i = 0; i < bone->children.size(); ++i)
	{
		FillBones(bone->children[i], anim, nBones);
	}
}

bool LoadAnimation(const char* path, vector<AnimationData>& skin, Animation& anim, vector<vec3u>& idx)
{
	Assimp::Importer imp;
	auto scene = imp.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights);

	if (!scene)
		return 0;
	auto names = GetNames(scene);
	vector<mat> bone_offsets(names.size());
	mat root = mat::load(&scene->mRootNode->mTransformation);
	printf("Root is :\n");
	printv(root);
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[i];
		for (int j = 0; j < mesh->mNumBones; ++j)
		{
			auto bone = scene->mMeshes[i]->mBones[j];
			uint boneId = GetID(names, bone->mName.data);
			bone_offsets[boneId] = mat::load(&bone->mOffsetMatrix);
		}
	}

	anim = {};
	anim.root = new Bone;
	InitBone(anim.root, FindRoot(scene->mRootNode, names), names, bone_offsets);
	FillBones(anim.root, scene->mAnimations[0], anim.nBones);
	anim.dur = scene->mAnimations[0]->mDuration;
	for (int i = 0; i < scene->mAnimations[0]->mChannels[0]->mNumRotationKeys; ++i)
		anim.ticks.push_back(scene->mAnimations[0]->mChannels[0]->mRotationKeys[i].mTime);

	skin.clear();
	uint offset = 0;
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[i];
		skin.resize(skin.size() + mesh->mNumVertices);
		for (int j = 0; j < mesh->mNumBones; ++j)
		{
			auto bone = mesh->mBones[j];
			auto id = GetIDUnique(names, bone->mName.data);
			for (int k = 0; k < bone->mNumWeights; ++k)
			{
				auto vertex_id = bone->mWeights[k].mVertexId;
				auto vertex_wgt = bone->mWeights[k].mWeight;
				skin[offset + vertex_id].Set(id, vertex_wgt);
			}
		}
		for (int j = 0; j < mesh->mNumFaces; ++j)
		{
			idx.push_back((vec3u&)*mesh->mFaces[j].mIndices);
		}
		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			if (mesh->HasPositions())
				skin[offset + j].pos = (vec3&)mesh->mVertices[j];
			if (mesh->HasNormals())
				skin[offset + j].normal = (vec3&)mesh->mNormals[j];
			if (mesh->HasTextureCoords(0))
				skin[offset + j].tex = (vec2&)mesh->mTextureCoords[0][j];

			if (!mesh->HasTangentsAndBitangents()) continue;
			skin[offset + j].tangent = (vec3&)mesh->mTangents[j];
			skin[offset + j].bitangent = (vec3&)mesh->mBitangents[j];
		}
		offset += mesh->mNumVertices;
	}

	return 1;
}
