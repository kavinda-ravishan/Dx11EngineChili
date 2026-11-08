#include <unordered_map>

#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Drawable/Mesh.hpp>

// Mesh
dx3d::Mesh::Mesh(const DrawableDesc& desc, Graphics& graphics, std::vector<std::unique_ptr<Bindable>> bindables)
: DrawableBase<Mesh>(desc)
{
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}

		for (auto& bindable : bindables)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(bindable.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
				bindable.release();
			}
			else
			{
				AddBind(std::move(bindable));
			}
		}

		AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
	}
}

void dx3d::Mesh::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&_transform, accumulated_transform);
	Drawable::Draw(graphics);
}

DirectX::XMMATRIX dx3d::Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&_transform);
}

// Node
dx3d::Node::Node(const int id, const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
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

void dx3d::Node::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG)
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

void dx3d::Node::AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG)
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
dx3d::Model::Model(const BaseDesc& desc, Graphics& graphics, const std::string file_name)
	: Base(desc), _window(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto scene = imp.ReadFile(file_name.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | 
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (scene == nullptr)
	{
		DX3DLogThrowError(("Failed to load the model " + file_name).c_str());
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		_mesh_ptrs.push_back(ParseMesh(desc, graphics, *scene->mMeshes[i]));
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

std::unique_ptr<dx3d::Mesh> dx3d::Model::ParseMesh(const BaseDesc& desc, Graphics& graphics, const aiMesh& mesh)
{
	VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		auto& vertex = mesh.mVertices[i];
		auto& normal = mesh.mNormals[i];

		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&vertex),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&normal)
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bindable>> bindables;

	bindables.push_back(std::make_unique<VertexBufferB>(BindableDesc({ desc.logger }), graphics, vbuf));

	bindables.push_back(std::make_unique<IndexBuffer>(BindableDesc({ desc.logger }), graphics, indices));

	auto pvs = std::make_unique<VertexShader>(BindableDesc({ desc.logger }), graphics, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindables.push_back(std::move(pvs));

	bindables.push_back(std::make_unique<PixelShader>(BindableDesc({ desc.logger }), graphics, L"PhongPS.cso"));

	bindables.push_back(std::make_unique<InputLayout>(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color{ 0.6f,0.6f,0.8f };
		float specular_intensity{ 0.6f };
		float specular_power{ 30.0f };
		float padding[3]{};
	} pmc;
	bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BindableDesc({ desc.logger }), graphics, pmc, 1u));

	return std::make_unique<Mesh>(DrawableDesc({ desc.logger }), graphics, std::move(bindables));
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
