#include <DX3D/Graphics/RenderJobs/FrameCommander.hpp>
#include <DX3D/Graphics/Drawable/Material.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>

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
dx3d::Mesh::Mesh(const DrawableDesc& desc)
	: Drawable(desc)
{}

dx3d::Mesh::Mesh(const DrawableDesc & desc, Graphics & graphics, const Material & mat, const aiMesh & mesh) noexcept(!IS_DEBUG)
	: Drawable(desc, graphics, mat, mesh)
{
}

void dx3d::Mesh::Submit(FrameCommander & frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd {
	DirectX::XMStoreFloat4x4(&_transform, accumulated_tranform);
	Drawable::Submit(frame);
}

DirectX::XMMATRIX dx3d::Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&_transform);
}
