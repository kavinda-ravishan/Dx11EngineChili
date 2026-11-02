#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/Melon.hpp>
#include <DX3D/Graphics/Drawable/Models/Sphere.hpp>

dx3d::Melon::Melon(
	const DrawableDesc& desc,
	Graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& longdist,
	std::uniform_int_distribution<int>& latdist)
	: TestObject<Melon>(desc, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"ColorIndexVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"ColorIndexPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3A colors[8] = {
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f }
			};
			float specular_intensity{ 0.6f };
			float specular_power{ 30.0f };
		} mat_const;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, mat_const, 1u));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
	};
	auto model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
	// deform vertices of model by linear transformation
	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));
	model.SetNormalsIndependentFlat();

	AddBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}
