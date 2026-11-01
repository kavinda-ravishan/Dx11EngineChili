#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/Models/Prism.hpp>
#include <DX3D/Graphics/Drawable/Cylinder.hpp>

dx3d::Cylinder::Cylinder(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	std::uniform_int_distribution<int>& tdist
) : TestObject<Cylinder>(desc, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};
		auto model = Prism::MakeTesselatedIndependentCapNormals<Vertex>(tdist(rng));

		AddStaticBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));

		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"IndexedPhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3A colors[6] = {
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
			};
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
		} matConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, matConst, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}
