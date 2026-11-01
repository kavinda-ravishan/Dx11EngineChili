#include <array>
#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/Pyramid.hpp>
#include <DX3D/Graphics/Drawable/Models/Cone.hpp>

dx3d::Pyramid::Pyramid(
	const DrawableDesc& desc,
	Graphics & graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& tdist)
	:
	TestObject<Pyramid>(desc, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"BlendedPhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"BlendedPhongPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} color_const;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, color_const, 1u));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		std::array<char, 4> color;
		char padding;
	};
	const auto tesselation = tdist(rng);
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);
	// set vertex colors for mesh (tip red blending to blue base)
	for (auto& v : model._vertices)
	{
		v.color = { (char)10,(char)10,(char)255 };
	}
	for (int i = 0; i < tesselation; i++)
	{
		model._vertices[i * 3].color = { (char)255,(char)10,(char)10 };
	}
	// squash mesh a bit in the z direction
	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));
	// add normals
	model.SetNormalsIndependentFlat();

	AddBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}
