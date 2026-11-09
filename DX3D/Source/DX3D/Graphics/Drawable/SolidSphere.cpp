#include <DX3D/Graphics/Drawable/SolidSphere.hpp>
#include <DX3D/Graphics/Drawable/Models/Sphere.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::SolidSphere::SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius)
    : Drawable(desc)
{

	auto model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_shared<VertexBufferB>(BINDABLE_DESC, graphics, model._vertices));
	AddBind(std::make_shared<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

	auto pvs = std::make_shared<VertexShader>(BINDABLE_DESC, graphics, L"SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(BINDABLE_DESC, graphics, L"SolidPS.cso"));

	struct PSColorConstant {
		DirectX::XMFLOAT3 color{ 1.0f,1.0f,1.0f };
		float padding{};
	} color_const;

	AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(BINDABLE_DESC, graphics, color_const));

	AddBind(std::make_shared<InputLayout>(BINDABLE_DESC, graphics, model._vertices.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_shared<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));


	AddBind(std::make_shared<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

DirectX::XMMATRIX dx3d::SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}
