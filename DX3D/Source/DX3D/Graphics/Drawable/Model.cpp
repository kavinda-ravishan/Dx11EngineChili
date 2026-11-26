#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DX3D/Math/MathXM.hpp>
#include <DX3D/Graphics/Drawable/ModelWindow.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
#include <DX3D/Graphics/Drawable/Node.hpp>
#include <DX3D/Graphics/Drawable/Model.hpp>
#include <DX3D/Graphics/Drawable/Material.hpp>

// Model
dx3d::Model::Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale)
	: Base(desc)
{
	Assimp::Importer imp;
	const auto scene = imp.ReadFile(file_path.string().c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (scene == nullptr) {
		DX3DLogThrowError(("Failed to load the model " + file_path.string()).c_str());
	}

	// parse materials
	std::vector<Material> materials{};
	materials.reserve(scene->mNumMaterials);
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		materials.emplace_back(BINDABLE_DESC, graphics, *scene->mMaterials[i], file_path);
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		const auto& mesh = *scene->mMeshes[i];
		_mesh_ptrs.push_back(std::make_unique<Mesh>(DRAWABLE_DESC, graphics, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int next_id{ 0 };
	_root = ParseNode(next_id, *scene->mRootNode, scale);
}

dx3d::Model::~Model() noexcept {}

void dx3d::Model::Submit(FrameCommander& frame) const {
	// I'm still not happy about updating parameters (i.e. mutating a bindable GPU state
	// which is part of a mesh which is part of a node which is part of the model that is
	// const in this call) Can probably do this elsewhere
	//_window->ApplyParameters();
	_root->Submit(frame, DirectX::XMMatrixIdentity());
}

void dx3d::Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	_root->SetAppliedTransform(tf);
}

void dx3d::Model::Accept(ModelProbe& probe) {
	_root->Accept(probe);
}

std::unique_ptr<dx3d::Node> dx3d::Model::ParseNode(int& next_id, const aiNode& node, const float scale) {
	const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);

	std::vector<Mesh*> cur_mesh_ptrs;
	cur_mesh_ptrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		cur_mesh_ptrs.push_back(_mesh_ptrs.at(meshIdx).get());
	}

	auto node_ptr = std::make_unique<Node>(next_id++, node.mName.C_Str(), std::move(cur_mesh_ptrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		node_ptr->AddChild(ParseNode(next_id, *node.mChildren[i], scale));
	}

	return node_ptr;
}
