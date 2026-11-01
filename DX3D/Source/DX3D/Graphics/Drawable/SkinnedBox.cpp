#include <DX3D/Utils/Surface.hpp>
#include <DX3D/Graphics/Bindable/BindableBase.hpp> 
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>
#include <DX3D/Graphics/Drawable/SkinnedBox.hpp>

dx3d::SkinnedBox::SkinnedBox(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist)
	: 
	TestObject<SkinnedBox>(desc, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
			DirectX::XMFLOAT2 tc;
		};
		auto model = Cube::MakeIndependentTextured<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));

		AddStaticBind(std::make_unique<Texture>(BINDABLE_DESC, graphics, Surface("Assets\\Images\\kappa50.png")));

		AddStaticBind(std::make_unique<Sampler>(BINDABLE_DESC, graphics));

		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"TexturedPhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"TexturedPhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float specular_intensity = 0.6f;
			float specular_power = 30.0f;
			float padding[2];
		} color_const;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, color_const, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}
