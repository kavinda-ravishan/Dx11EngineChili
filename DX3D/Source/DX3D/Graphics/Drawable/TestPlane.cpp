#include <string>
#include <DX3D/Graphics/Drawable/Models/Plane.hpp>
#include <DX3D/Graphics/Drawable/TestPlane.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::TestPlane::TestPlane(const DrawableDesc& desc, Graphics& graphics, float size) 
	: Drawable(desc)
{
	auto model = Plane::Make();
	model.Transform(DirectX::XMMatrixScaling(size, size, 1.0));

	const auto geometry_tag = "$plane." + std::to_string(size);
	AddBind(BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices));
	AddBind(BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices));

	AddBind(BTexture::Resolve(BINDABLE_DESC, graphics, "Assets\\Images\\brickwall.jpg"));

	auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(BPixelShader::Resolve(BINDABLE_DESC, graphics, "PhongPS.cso"));

	struct PSMaterialConstant {
		float specularIntensity{ 0.8f };
		float specularPower{ 45.0f };
		float padding[2]{};
	} pmc;
	AddBind(BPixelConstantBuffer<PSMaterialConstant>::Resolve(BINDABLE_DESC, graphics, pmc, 1u));

	AddBind(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

	AddBind(BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

void dx3d::TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept {
	_roll = roll;
	_pitch = pitch;
	_yaw = yaw;
}

DirectX::XMMATRIX dx3d::TestPlane::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(_roll, _pitch, _yaw) *
		DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}
