#include <DX3D/Graphics/RenderJobs/FrameCommander.hpp>
#include <DX3D/Graphics/Drawable/Material.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>

dx3d::Mesh::Mesh(const DrawableDesc & desc, Graphics & graphics, const Material & mat, const aiMesh & mesh, const float scale) noexcept(!IS_DEBUG)
	: Drawable(desc, graphics, mat, mesh, scale)
{}

void dx3d::Mesh::Submit(FrameCommander & frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd {
	DirectX::XMStoreFloat4x4(&_transform, accumulated_tranform);
	Drawable::Submit(frame);
}

DirectX::XMMATRIX dx3d::Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&_transform);
}
