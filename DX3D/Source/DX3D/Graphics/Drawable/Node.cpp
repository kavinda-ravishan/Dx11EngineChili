#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
#include <DX3D/Graphics/Drawable/Node.hpp>

dx3d::Node::Node(const int id, const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noxnd
	: _id(id), _name(name), _mesh_ptrs(std::move(mesh_ptrs))
{
	DirectX::XMStoreFloat4x4(&_transform, transform);
	DirectX::XMStoreFloat4x4(&_applied_transform, DirectX::XMMatrixIdentity());
}

void dx3d::Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd {
	const auto built =
		DirectX::XMLoadFloat4x4(&_applied_transform) *
		DirectX::XMLoadFloat4x4(&_transform) *
		accumulated_tranform;
	for (const auto pm : _mesh_ptrs)
	{
		pm->Submit(frame, built);
	}
	for (const auto& pc : _child_ptrs)
	{
		pc->Submit(frame, built);
	}
}

void dx3d::Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&_applied_transform, transform);
}

const DirectX::XMFLOAT4X4& dx3d::Node::GetAppliedTransform() const noexcept {
	return _applied_transform;
}

//void dx3d::Node::ShowTree(Node*& selected_node_ptr) const noexcept
//{
//	// if there is no selected node, set selectedId to an impossible value
//	const int selected_id = (selected_node_ptr == nullptr) ? -1 : selected_node_ptr->GetID();
//
//	// build up flags for current node
//	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
//		| ((GetID() == selected_id) ? ImGuiTreeNodeFlags_Selected : 0)
//		| ((_child_ptrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0);
//
//	// render this node
//	const auto expanded = ImGui::TreeNodeEx((
//		void*)(intptr_t)GetID(), node_flags, _name.c_str()
//	);
//
//	// process for selection node
//	if (ImGui::IsItemClicked())
//	{
//		selected_node_ptr = const_cast<Node*>(this);
//	}
//
//	//recursively rendering of open node's children
//	if (expanded)
//	{
//		for (const auto& child_ptr : _child_ptrs)
//		{
//			child_ptr->ShowTree(selected_node_ptr);
//		}
//		ImGui::TreePop();
//	}
//}

int dx3d::Node::GetID() const noexcept {
	return _id;
}

void dx3d::Node::AddChild(std::unique_ptr<Node> child_ptr) noxnd
{
	assert(child_ptr);
	_child_ptrs.push_back(std::move(child_ptr));
}
