#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Drawable/ModelProbe.hpp>
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

int dx3d::Node::GetID() const noexcept {
	return _id;
}

bool dx3d::Node::HasChildren() const noexcept {
	return _child_ptrs.size() > 0;
}

void dx3d::Node::Accept(ModelProbe& probe) {
	if (probe.PushNode(*this))
	{
		for (auto& cp : _child_ptrs)
		{
			cp->Accept(probe);
		}
		probe.PopNode(*this);
	}
}

const std::string& dx3d::Node::GetName() const {
	return _name;
}

void dx3d::Node::AddChild(std::unique_ptr<Node> child_ptr) noxnd
{
	assert(child_ptr);
	_child_ptrs.push_back(std::move(child_ptr));
}
