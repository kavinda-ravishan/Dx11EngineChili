#include <unordered_map>

#include <DX3D/Graphics/Utils/Surface.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffersEx.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>

// pixel shader const buffer values
#define PSCBV_normal_map_enabled "normal_map_enabled"
#define PSCBV_specular_map_enabled "specular_map_enabled"
#define PSCBV_has_gloss_map "has_gloss_map"
#define PSCBV_specular_power "specular_power"
#define PSCBV_specular_intensity "specular_intensity"
#define PSCBV_specular_color "specular_color"
#define PSCBV_specular_map_weight "specular_map_weight"
#define PSCBV_material_color "material_clor"

static DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& mat)
{
	DirectX::XMFLOAT3 euler{};

	euler.x = asinf(-mat._32);                 // Pitch
	if (cosf(euler.x) > 0.0001)                // Not at poles
	{
		euler.y = atan2f(mat._31, mat._33);      // Yaw
		euler.z = atan2f(mat._12, mat._22);      // Roll
	}
	else
	{
		euler.y = 0.0f;                           // Yaw
		euler.z = atan2f(-mat._21, mat._11);     // Roll
	}

	return euler;
}

static DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix)
{
	return { matrix._41,matrix._42,matrix._43 };
}

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

const DirectX::XMFLOAT4X4& dx3d::Node::GetAppliedTransform() const noexcept {
	return _applied_transform;
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

const dx3d_dcb::Buffer* dx3d::Node::GetMaterialConstants() const noxnd  {
	if (_mesh_ptrs.size() == 0)
	{
		return nullptr;
	}
	auto bindable = _mesh_ptrs.front()->QueryBindable<BCachingPixelConstantBufferEX>();
	return &bindable->GetBuffer();
}

void dx3d::Node::SetMaterialConstants(const dx3d_dcb::Buffer& buf_in) noxnd {
	auto pcb = _mesh_ptrs.front()->QueryBindable<BCachingPixelConstantBufferEX>();
	assert(pcb != nullptr);
	pcb->SetBuffer(buf_in);
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
	class ModelWindow // pImpl idiom, only defined in this .cpp
	{
	public:
		void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
		{
			// window name defaults to "Model"
			windowName = windowName ? windowName : "Model";
			// need an ints to track node indices and selected node
			int nodeIndexTracker = 0;
			if (ImGui::Begin(windowName))
			{
				ImGui::Columns(2, nullptr, true);
				root.ShowTree(pSelectedNode);

				ImGui::NextColumn();
				if (pSelectedNode != nullptr)
				{
					const auto id = pSelectedNode->GetID();
					auto i = _transforms.find(id);
					if (i == _transforms.end())
					{
						const auto& applied = pSelectedNode->GetAppliedTransform();
						const auto angles = ExtractEulerAngles(applied);
						const auto translation = ExtractTranslation(applied);
						TransformParameters tp;
						tp.roll = angles.z;
						tp.pitch = angles.x;
						tp.yaw = angles.y;
						tp.x = translation.x;
						tp.y = translation.y;
						tp.z = translation.z;
						auto pMatConst = pSelectedNode->GetMaterialConstants();
						auto buf = pMatConst != nullptr ? std::optional<dx3d_dcb::Buffer>{ *pMatConst } : std::optional<dx3d_dcb::Buffer>{};
						std::tie(i, std::ignore) = _transforms.insert({ id,{ tp,false,std::move(buf),false } });
					}
					// link imgui ctrl to our cached transform params
					{
						auto& transform = i->second.tranform_params;
						// dirty check
						auto& dirty = i->second.transform_params_changed;
						const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
						// widgets
						ImGui::Text("Orientation");
						dcheck(ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f));
						dcheck(ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f));
						dcheck(ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f));
						ImGui::Text("Position");
						dcheck(ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f));
						dcheck(ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f));
						dcheck(ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f));
					}
					// link imgui ctrl to our cached material params
					if (i->second.material_cbuf)
					{
						auto& mat = *i->second.material_cbuf;
						// dirty check
						auto& dirty = i->second.material_cbuf_changed;
						const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
						// widgets
						ImGui::Text("Material");
						if (auto v = mat[PSCBV_normal_map_enabled]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Norm Map", &v));
						}
						if (auto v = mat[PSCBV_specular_map_enabled]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Spec Map", &v));
						}
						if (auto v = mat[PSCBV_has_gloss_map]; v.Exists())
						{
							dcheck(ImGui::Checkbox("Gloss Map", &v));
						}
						if (auto v = mat[PSCBV_material_color]; v.Exists())
						{
							dcheck(ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
						}
						if (auto v = mat[PSCBV_specular_power]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
						}
						if (auto v = mat[PSCBV_specular_color]; v.Exists())
						{
							dcheck(ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
						}
						if (auto v = mat[PSCBV_specular_map_weight]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f));
						}
						if (auto v = mat[PSCBV_specular_intensity]; v.Exists())
						{
							dcheck(ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f));
						}
					}
				}
			}
			ImGui::End();
		}
		void ApplyParameters() noxnd
		{
			if (TransformChanged())
			{
				pSelectedNode->SetAppliedTransform(GetTransform());
				ResetTransformChanged();
			}
			if (MaterialChanged())
			{
				pSelectedNode->SetMaterialConstants(GetMaterial());
				ResetMaterialChanged();
			}
		}
	private:
		DirectX::XMMATRIX GetTransform() const noxnd
		{
			assert(pSelectedNode != nullptr);
			const auto& transform = _transforms.at(pSelectedNode->GetID()).tranform_params;
			return
				DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
				DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
		}
		const dx3d_dcb::Buffer& GetMaterial() const noxnd
		{
			assert(pSelectedNode != nullptr);
			const auto& mat = _transforms.at(pSelectedNode->GetID()).material_cbuf;
			assert(mat);
			return *mat;
		}
		bool TransformChanged() const noxnd
		{
			return pSelectedNode && _transforms.at(pSelectedNode->GetID()).transform_params_changed;
		}
		void ResetTransformChanged() noxnd
		{
			_transforms.at(pSelectedNode->GetID()).transform_params_changed = false;
		}
		bool MaterialChanged() const noxnd
		{
			return pSelectedNode && _transforms.at(pSelectedNode->GetID()).material_cbuf_changed;
		}
		void ResetMaterialChanged() noxnd
		{
			_transforms.at(pSelectedNode->GetID()).material_cbuf_changed = false;
		}
		bool IsChanged() const noxnd
		{
			return TransformChanged() || MaterialChanged();
		}
	private:
		Node* pSelectedNode;
		struct TransformParameters
		{
			float roll{};
			float pitch{};
			float yaw{};
			float x{};
			float y{};
			float z{};
		};
		struct NodeData
		{
			TransformParameters tranform_params{};
			bool transform_params_changed{};
			std::optional<dx3d_dcb::Buffer> material_cbuf{};
			bool material_cbuf_changed{};
		};
		std::unordered_map<int, NodeData> _transforms{};
	};
} // namespace dx3d

// Model
dx3d::Model::Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale)
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
		_mesh_ptrs.push_back(ParseMesh(desc, graphics, file_path, *scene->mMeshes[i], scene->mMaterials, scale));
	}

	int next_id{ 0 };
	_root = ParseNode(next_id, *scene->mRootNode);
}

dx3d::Model::~Model() noexcept {}

void dx3d::Model::Draw(Graphics& graphics) const
{
	// I'm still not happy about updating parameters (i.e. mutating a bindable GPU state
	// which is part of a mesh which is part of a node which is part of the model that is
	// const in this call) Can probably do this elsewhere
	_window->ApplyParameters();
	_root->Draw(graphics, DirectX::XMMatrixIdentity());
}

void dx3d::Model::ShowWindow(Graphics& graphics, const char* window_name) noexcept
{
	_window->Show(graphics, window_name, *_root);
}

void dx3d::Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	_root->SetAppliedTransform(tf);
}

std::unique_ptr<dx3d::Mesh> dx3d::Model::ParseMesh(
	const BaseDesc& desc, 
	Graphics& graphics, 
	const std::filesystem::path& file_path, 
	const aiMesh& ai_mesh, 
	const aiMaterial* const* ai_materials_ptr, 
	const float scale)
{
	const std::filesystem::path resource_path{ file_path.parent_path() };

	std::vector<std::shared_ptr<Bindable>> bindable_ptrs;

	bool has_specular_map = false;
	bool has_alpha_gloss = false;
	bool has_alpha_diffuse = false;
	bool has_normal_map = false;
	bool has_diffuse_map = false;
	
	float shininess = 2.0f;
	DirectX::XMFLOAT4 specular_color{ 0.18f, 0.18f, 0.18f, 1.0f };
	DirectX::XMFLOAT4 diffuse_color{ 0.45f, 0.45f, 0.85f, 1.0f };
	
	if (ai_mesh.mMaterialIndex >= 0)
	{
		auto& material = *ai_materials_ptr[ai_mesh.mMaterialIndex];

		aiString tex_file_name;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tex_file_name) == aiReturn_SUCCESS)
		{
			auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string());
			has_alpha_diffuse = tex->HasAlpha();
			bindable_ptrs.push_back(std::move(tex));
			has_diffuse_map = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuse_color));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &tex_file_name) == aiReturn_SUCCESS)
		{
			auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string(), 1u);
			has_alpha_gloss = tex->HasAlpha();
			bindable_ptrs.push_back(std::move(tex));
			has_specular_map = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specular_color));
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

	const auto mesh_tag = file_path.string() + "%" + ai_mesh.mName.C_Str();

	if (has_diffuse_map && has_normal_map && has_specular_map)
	{
		dx3d_dvtx::VertexBuffer vbuf(std::move(
			dx3d_dvtx::VertexLayout{}
			.Append(dx3d_dvtx::VertexLayout::Position3D)
			.Append(dx3d_dvtx::VertexLayout::Normal)
			.Append(dx3d_dvtx::VertexLayout::Tangent)
			.Append(dx3d_dvtx::VertexLayout::Bitangent)
			.Append(dx3d_dvtx::VertexLayout::Texture2D)
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

		bindable_ptrs.push_back(BPixelShader::Resolve(
			BindableDesc({ desc.logger }), 
			graphics, 
			has_alpha_diffuse ? "PhongPSSpecNormalMask.cso" : "PhongPSSpecNormalMap.cso"
		));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		dx3d_dcb::RawLayout lay;
		lay.Add<dx3d_dcb::Bool>(PSCBV_normal_map_enabled);
		lay.Add<dx3d_dcb::Bool>(PSCBV_specular_map_enabled);
		lay.Add<dx3d_dcb::Bool>(PSCBV_has_gloss_map);
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);
		lay.Add<dx3d_dcb::Float3>(PSCBV_specular_color);
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_map_weight);

		auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
		ps_cbuf[PSCBV_normal_map_enabled] = true;
		ps_cbuf[PSCBV_specular_map_enabled] = true;
		ps_cbuf[PSCBV_has_gloss_map] = has_alpha_gloss;
		ps_cbuf[PSCBV_specular_power] = shininess;
		ps_cbuf[PSCBV_specular_color] = DirectX::XMFLOAT3{ 0.75f,0.75f,0.75f };
		ps_cbuf[PSCBV_specular_map_weight] = 0.671f;

		bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	}
	else if (has_diffuse_map && has_normal_map)
	{
		dx3d_dvtx::VertexBuffer vbuf(std::move(
			dx3d_dvtx::VertexLayout{}
			.Append(dx3d_dvtx::VertexLayout::Position3D)
			.Append(dx3d_dvtx::VertexLayout::Normal)
			.Append(dx3d_dvtx::VertexLayout::Tangent)
			.Append(dx3d_dvtx::VertexLayout::Bitangent)
			.Append(dx3d_dvtx::VertexLayout::Texture2D)
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

		dx3d_dcb::RawLayout layout;
		layout.Add<dx3d_dcb::Float>(PSCBV_specular_intensity);
		layout.Add<dx3d_dcb::Float>(PSCBV_specular_power);
		layout.Add<dx3d_dcb::Bool>(PSCBV_normal_map_enabled);

		auto ps_cbuf = dx3d_dcb::Buffer(std::move(layout));
		ps_cbuf[PSCBV_specular_intensity] = (specular_color.x + specular_color.y + specular_color.z) / 3.0f;
		ps_cbuf[PSCBV_specular_power] = shininess;
		ps_cbuf[PSCBV_normal_map_enabled] = true;

		bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	}
	else if (has_diffuse_map && !has_normal_map && has_specular_map)
	{
		dx3d_dvtx::VertexBuffer vbuf(std::move(
			dx3d_dvtx::VertexLayout{}
			.Append(dx3d_dvtx::VertexLayout::Position3D)
			.Append(dx3d_dvtx::VertexLayout::Normal)
			.Append(dx3d_dvtx::VertexLayout::Texture2D)
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

		bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSSpec.cso"));

		bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

		dx3d_dcb::RawLayout lay;
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);
		lay.Add<dx3d_dcb::Bool>(PSCBV_has_gloss_map);
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_map_weight);

		auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
		ps_cbuf[PSCBV_specular_power] = shininess;
		ps_cbuf[PSCBV_has_gloss_map] = has_alpha_gloss;
		ps_cbuf[PSCBV_specular_map_weight] = 1.0f;

		bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	}
	else if (has_diffuse_map)
	{
		dx3d_dvtx::VertexBuffer vbuf(std::move(
			dx3d_dvtx::VertexLayout{}
			.Append(dx3d_dvtx::VertexLayout::Position3D)
			.Append(dx3d_dvtx::VertexLayout::Normal)
			.Append(dx3d_dvtx::VertexLayout::Texture2D)
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

		dx3d_dcb::RawLayout lay;
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_intensity);
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);

		auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
		ps_cbuf[PSCBV_specular_intensity] = (specular_color.x + specular_color.y + specular_color.z) / 3.0f;
		ps_cbuf[PSCBV_specular_power] = shininess;

		bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	}
	else if (!has_diffuse_map && !has_normal_map && !has_specular_map)
	{
		dx3d_dvtx::VertexBuffer vbuf(std::move(
			dx3d_dvtx::VertexLayout{}
			.Append(dx3d_dvtx::VertexLayout::Position3D)
			.Append(dx3d_dvtx::VertexLayout::Normal)
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

		dx3d_dcb::RawLayout lay;
		lay.Add<dx3d_dcb::Float4>(PSCBV_material_color);
		lay.Add<dx3d_dcb::Float4>(PSCBV_specular_color);
		lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);

		auto buf = dx3d_dcb::Buffer(std::move(lay));
		buf[PSCBV_material_color] = diffuse_color;
		buf[PSCBV_specular_color] = specular_color;
		buf[PSCBV_specular_power] = shininess;
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material smh");
	}

	// anything with alpha diffuse is 2-sided IN SPONZA, need a better way
	// of signalling 2-sidedness to be more general in the future
	bindable_ptrs.push_back(BRasterizer::Resolve(BindableDesc({ desc.logger }), graphics, has_alpha_diffuse));

	bindable_ptrs.push_back(BBlender::Resolve(BindableDesc({ desc.logger }), graphics, false));

	bindable_ptrs.push_back(
		std::make_shared<BStencil>(
			BindableDesc({ desc.logger }), 
			graphics, 
			BStencil::Mode::Off
		)
	);

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
