#include <DX3D/Utils/Surface.hpp>
#include <DX3D/Drawable/Cube.hpp>
#include <DX3D/Graphics/Bindable/BindableBase.hpp> 
#include <DX3D/Drawable/SkinnedBox.hpp>

dx3d::SkinnedBox::SkinnedBox(const DrawableDesc& desc, Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	:
	DrawableBase<SkinnedBox>(desc),
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};

		auto model = Cube::MakeSkinned<Vertex>();

		AddStaticBind(std::make_unique<Texture>(BINDABLE_DESC, gfx, Surface("Assets\\Images\\cube.png")));

		AddStaticBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, gfx, model._vertices));

		AddStaticBind(std::make_unique<Sampler>(BINDABLE_DESC, gfx));

		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, gfx, L"TextureVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, gfx, L"TexturePS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, gfx, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, gfx, *this));
}

void dx3d::SkinnedBox::Update(float dt) noexcept {
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX dx3d::SkinnedBox::GetTransformXM() const noexcept {
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
