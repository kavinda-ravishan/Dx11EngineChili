#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/Box.hpp>
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>

dx3d::Box::Box(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material_color)
	: 
	DrawableBase<Box>(desc),
	_r(rdist(rng)),
	_droll(ddist(rng)),
	_dpitch(ddist(rng)),
	_dyaw(ddist(rng)),
	_dphi(odist(rng)),
	_dtheta(odist(rng)),
	_dchi(odist(rng)),
	_chi(adist(rng)),
	_theta(adist(rng)),
	_phi(adist(rng))
{

	if (!IsStaticInitialized()) {
		// create and set vertex buffer
		struct Vertex {
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));

		// read and set vertex shader
		auto vertex_shader = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"PhongVS.cso");
		auto vertex_shader_byte_code = vertex_shader->GetBytecode();
		AddStaticBind(std::move(vertex_shader));

		// read and set pixel shader
		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"PhongPS.cso"));

		// set index buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

		// input (vertex) layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> vs_input_element_desc{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, vs_input_element_desc, vertex_shader_byte_code));

		// set Primitive Topology to triangle list (groups of 3 vertices)
		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	} else {
		SetIndexFromStatic();
	}
	
	// bind vertex transform constant buffer to VS
	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));

	// bind material data to PS
	struct PSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 color;
	} color_const;
	color_const.color = material_color;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, color_const, 1u));

	// model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(
		&_mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

void dx3d::Box::Update(const float dt) noexcept {
	_roll += _droll * dt;
	_pitch += _dpitch * dt;
	_yaw += _dyaw * dt;
	_theta += _dtheta * dt;
	_phi += _dphi * dt;
	_chi += _dchi * dt;
}

DirectX::XMMATRIX dx3d::Box::GetTransformXM() const noexcept {
	return
		DirectX::XMLoadFloat3x3(&_mt) *
		DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		DirectX::XMMatrixTranslation(_r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi);
}
