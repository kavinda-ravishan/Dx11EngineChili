#include <DX3D/Graphics/Drawable/SolidSphere.hpp>
#include <DX3D/Graphics/Drawable/Sphere.hpp>
#include <DX3D/Graphics/Bindable/BindableBase.hpp>

dx3d::SolidSphere::SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius)
    : DrawableBase(desc)
{
	if (!IsStaticInitialized()) {
		struct Vertex {
			DirectX::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"SolidPS.cso"));

		struct PSColorConstant {
			DirectX::XMFLOAT3 color{ 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;

		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(BINDABLE_DESC, graphics, colorConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied {
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	} else {
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::SolidSphere::Update(float dt) noexcept {}

void dx3d::SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

DirectX::XMMATRIX dx3d::SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}
