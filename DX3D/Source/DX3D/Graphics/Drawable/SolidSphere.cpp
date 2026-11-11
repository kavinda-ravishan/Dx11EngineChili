#include <DX3D/Graphics/Drawable/SolidSphere.hpp>
#include <DX3D/Graphics/Drawable/Models/Sphere.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::SolidSphere::SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius)
    : Drawable(desc)
{
	auto model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	
	const auto geometry_tag{ "$sphere." + std::to_string(radius) };
	AddBind(BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices));
	AddBind(BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices));

	auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");

	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(BPixelShader::Resolve(BINDABLE_DESC, graphics, "SolidPS.cso"));

	struct PSColorConstant {
		DirectX::XMFLOAT3 color{ 1.0f,1.0f,1.0f };
		float padding{};
	} color_const;

	AddBind(BPixelConstantBuffer<PSColorConstant>::Resolve(BINDABLE_DESC, graphics, color_const));

	AddBind(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

	AddBind(BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

DirectX::XMMATRIX dx3d::SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}
