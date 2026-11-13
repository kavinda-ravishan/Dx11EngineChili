#include <unordered_map>

#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Utils/Surface.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>

// Mesh
dx3d::Mesh::Mesh(const DrawableDesc& desc, Graphics& graphics, std::vector<std::shared_ptr<Bindable>> bindables)
: Drawable(desc)
{
	AddBind(BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& bindable : bindables) {
		AddBind(std::move(bindable));
	}

	AddBind(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::Mesh::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noxnd
{
	DirectX::XMStoreFloat4x4(&_transform, accumulated_transform);
	Drawable::Draw(graphics);
}

DirectX::XMMATRIX dx3d::Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&_transform);
}

// Node
dx3d::Node::Node(const int id, const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noxnd
	: _id(id), _name(name), _mesh_ptrs(std::move(mesh_ptrs))
{
	DirectX::XMStoreFloat4x4(&_transform, transform);
	DirectX::XMStoreFloat4x4(&_applied_transform, DirectX::XMMatrixIdentity());
}

void dx3d::Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&_applied_transform, transform);
}

void dx3d::Node::ShowTree(Node*& selected_node_ptr) const noexcept
{
	// if there is no selected node, set selectedId to an impossible value
	const int selected_id = (selected_node_ptr == nullptr) ? -1 : selected_node_ptr->GetID();

	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetID() == selected_id) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((_child_ptrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0);

	// render this node
	const auto expanded = ImGui::TreeNodeEx((
		void*)(intptr_t)GetID(), node_flags, _name.c_str()
	);

	// process for selection node
	if (ImGui::IsItemClicked())
	{
		selected_node_ptr = const_cast<Node*>(this);
	}

	//recursively rendering of open node's children
	if (expanded)
	{
		for (const auto& child_ptr : _child_ptrs)
		{
			child_ptr->ShowTree(selected_node_ptr);
		}
		ImGui::TreePop();
	}
}

int dx3d::Node::GetID() const noexcept {
	return _id;
}

void dx3d::Node::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noxnd
{
	const auto built = 
		DirectX::XMLoadFloat4x4(&_applied_transform) * 
		DirectX::XMLoadFloat4x4(&_transform) * 
		accumulated_transform;

	for (const auto pm : _mesh_ptrs)
	{
		pm->Draw(graphics, built);
	}
	for (const auto& pc : _child_ptrs)
	{
		pc->Draw(graphics, built);
	}
}

void dx3d::Node::AddChild(std::unique_ptr<Node> child) noxnd
{
	assert(child);
	_child_ptrs.push_back(std::move(child));
}

// ModelWindow
namespace dx3d {

	class ModelWindow final // pImpl idiom, only defined in this .cpp
	{
	public:
		void Show(const char* window_name, const Node& root) noexcept
		{
			// window name defaults to "Model"
			window_name = window_name ? window_name : "Model";
			// need an ints to track node indices and selected node
			int node_index_tracked = 0;
			if (ImGui::Begin(window_name))
			{
				ImGui::Columns(2, nullptr, true);
				root.ShowTree(_selected_node_ptr);

				ImGui::NextColumn();
				if (_selected_node_ptr != nullptr)
				{
					auto& transform = _transforms[_selected_node_ptr->GetID()];
					ImGui::Text("Orientation");
					ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
					ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
					ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
					ImGui::Text("Position");
					ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
					ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
					ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
				}
			}
			ImGui::End();
		}
		DirectX::XMMATRIX GetTransform() const noexcept
		{
			assert(_selected_node_ptr != nullptr);
			const auto& transform = _transforms.at(_selected_node_ptr->GetID());
			return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
				DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
		}
		Node* GetSelectedNode() const noexcept
		{
			return _selected_node_ptr;
		}
	private:
		Node* _selected_node_ptr{};
		struct TransformParameters
		{
			float roll{ 0.0f };
			float pitch{ 0.0f };
			float yaw{ 0.0f };
			float x{ 0.0f };
			float y{ 0.0f };
			float z{ 0.0f };
		};
		std::unordered_map<int, TransformParameters> _transforms{};
	};
} // namespace dx3d

// Model
dx3d::Model::Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const std::filesystem::path resource_path)
	: Base(desc), _window(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto scene = imp.ReadFile(file_path.string().c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | 
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (scene == nullptr)
	{
		DX3DLogThrowError(("Failed to load the model " + file_path.string()).c_str());
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		_mesh_ptrs.push_back(ParseMesh(desc, graphics, resource_path , *scene->mMeshes[i], scene->mMaterials));
	}

	int next_id{ 0 };
	_root = ParseNode(next_id, *scene->mRootNode);
}

dx3d::Model::~Model() noexcept {}

void dx3d::Model::Draw(Graphics& graphics) const
{
	if (auto node = _window->GetSelectedNode())
	{
		node->SetAppliedTransform(_window->GetTransform());
	}
	_root->Draw(graphics, DirectX::XMMatrixIdentity());
}

void dx3d::Model::ShowWindow(const char* window_name) noexcept
{
	_window->Show(window_name, *_root);
}

void dx3d::Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	_root->SetAppliedTransform(tf);
}

std::unique_ptr<dx3d::Mesh> dx3d::Model::ParseMesh(
	const BaseDesc& desc, 
	Graphics& graphics, 
	const std::filesystem::path& resource_path,
	const aiMesh& ai_mesh, 
	const aiMaterial* const* ai_materials_ptr)
{
	std::vector<std::shared_ptr<Bindable>> bindable_ptrs;

	bool has_specular_map = false;
	bool has_alpha_gloss = false;
	bool has_normal_map = false;
	bool has_diffuse_map = false;
	float shininess = 35.0f;
	if (ai_mesh.mMaterialIndex >= 0)
	{
		auto& material = *ai_materials_ptr[ai_mesh.mMaterialIndex];

		aiString tex_file_name;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tex_file_name) == aiReturn_SUCCESS)
		{
			bindable_ptrs.push_back(BTexture::Resolve(BindableDesc({desc.logger}), graphics, (resource_path / tex_file_name.C_Str()).string()));
			has_diffuse_map = true;
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &tex_file_name) == aiReturn_SUCCESS)
		{
			auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string(), 1u);
			has_alpha_gloss = tex->HasAlpha();
			bindable_ptrs.push_back(std::move(tex));
			has_specular_map = true;
		}
		
		if(!has_alpha_gloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &tex_file_name) == aiReturn_SUCCESS)
		{
			auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string(), 2u);
			has_alpha_gloss = tex->HasAlpha();
			bindable_ptrs.push_back(std::move(tex));
			has_normal_map = true;
		}

		if (has_diffuse_map || has_specular_map || has_normal_map)
		{
			bindable_ptrs.push_back(BSampler::Resolve(BindableDesc({ desc.logger }), graphics));
		}
	}

	const auto mesh_tag = resource_path.string() + "%" + ai_mesh.mName.C_Str();

	const float scale = 6.0f;

	if (has_diffuse_map && has_normal_map && has_specular_map)
	{
		VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(ai_mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
		{
			const auto& face = ai_mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

		bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

		auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetBytecode();
		bindable_ptrs.push_back(std::move(pvs));

		bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSSpecNormalMap.cso"));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantFullmonte
		{
			BOOL normal_map_enabled{ TRUE };
			BOOL specular_map_enabled{ TRUE };
			BOOL has_gloss_map{};
			float specular_power{};
			DirectX::XMFLOAT3 specular_color{ 0.75f, 0.75f, 0.75f };
			float specular_map_weight{ 0.671f };
		} pmc;
		pmc.specular_power = shininess;
		pmc.has_gloss_map = has_alpha_gloss ? TRUE : FALSE;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindable_ptrs.push_back(BPixelConstantBuffer<PSMaterialConstantFullmonte>::Resolve(BindableDesc({ desc.logger }), graphics, pmc, 1u));
	}
	else if (has_diffuse_map && has_normal_map)
	{
		VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(ai_mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
		{
			const auto& face = ai_mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

		bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

		auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetBytecode();
		bindable_ptrs.push_back(std::move(pvs));

		bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSNormalMap.cso"));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffnorm
		{
			float specular_intensity{ 0.18f };
			float specular_power{};
			BOOL  normal_map_enabled{ TRUE };
			float padding[1]{};
		} pmc;
		pmc.specular_power = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindable_ptrs.push_back(BPixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(BindableDesc({ desc.logger }), graphics, pmc, 1u));
	}
	else if (has_diffuse_map)
	{
		VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(ai_mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
		{
			const auto& face = ai_mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

		bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

		auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindable_ptrs.push_back(std::move(pvs));

		bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPS.cso"));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuse
		{
			float specular_intensity{ 0.18f };
			float specular_power{};
			float padding[2]{};
		} pmc;
		pmc.specular_power = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindable_ptrs.push_back(BPixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(BindableDesc({ desc.logger }), graphics, pmc, 1u));
	}
	else if (!has_diffuse_map && !has_normal_map && !has_specular_map)
	{
		VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(ai_mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
		{
			const auto& face = ai_mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

		bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

		auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNotex.cso");
		auto pvsbc = pvs->GetBytecode();
		bindable_ptrs.push_back(std::move(pvs));

		bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSNotex.cso"));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantNotex
		{
			DirectX::XMFLOAT4 material_color{ 0.45f,0.45f,0.85f,1.0f };
			float specular_intensity{ 0.18f };
			float specular_power{};
			float padding[2]{};
		} pmc;
		pmc.specular_power = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindable_ptrs.push_back(BPixelConstantBuffer<PSMaterialConstantNotex>::Resolve(BindableDesc({ desc.logger }), graphics, pmc, 1u));
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material smh");
	}

	return std::make_unique<Mesh>(DrawableDesc({ desc.logger }), graphics, std::move(bindable_ptrs));
}

std::unique_ptr<dx3d::Node> dx3d::Model::ParseNode(int& next_id, const aiNode& node)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

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
		node_ptr->AddChild(ParseNode(next_id, *node.mChildren[i]));
	}

	return node_ptr;
}
