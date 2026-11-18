#include <DX3D/Graphics/Drawable/SolidSphere.hpp>
#include <DX3D/Graphics/Drawable/Models/Sphere.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::SolidSphere::SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius)
    : Drawable(desc)
{
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	const auto geometry_tag = "$sphere." + std::to_string(radius);
	_vertices = BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices);
	_indices = BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices);
	_topology = BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only(0);

		auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(BPixelShader::Resolve(BINDABLE_DESC, graphics, "SolidPS.cso"));

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} color_const;

		only.AddBindable(BPixelConstantBuffer<PSColorConstant>::Resolve(BINDABLE_DESC, graphics, color_const, 1u));

		only.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

		only.AddBindable(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics));

		only.AddBindable(BBlender::Resolve(BINDABLE_DESC, graphics, false));

		only.AddBindable(BRasterizer::Resolve(BINDABLE_DESC, graphics, false));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void dx3d::SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

DirectX::XMMATRIX dx3d::SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}
