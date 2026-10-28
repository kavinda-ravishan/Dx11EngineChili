#include <DX3D/Drawable/Box.hpp>
#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Drawable/Cube.hpp>

dx3d::Box::Box(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_real_distribution<float>& bdist)
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
		};

		const auto model = Cube::Make<Vertex>();

		AddStaticBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, model._vertices));

		// read and set vertex shader
		auto vertex_shader = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"ColorIndexVS.cso");
		auto vertex_shader_byte_code = vertex_shader->GetBytecode();
		AddStaticBind(std::move(vertex_shader));

		// read and set pixel shader
		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"ColorIndexPS.cso"));

		// set index buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, model._indices));

		// create constant buffer for cube face colors
		struct PSConstantBufferStruct {
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};

		const PSConstantBufferStruct ps_constant_buffer_data = {
			{
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			}
		};

		AddStaticBind(std::make_unique<PixelConstantBuffer<PSConstantBufferStruct>>(BINDABLE_DESC, graphics, ps_constant_buffer_data));

		// input (vertex) layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> vs_input_element_desc{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, vs_input_element_desc, vertex_shader_byte_code));

		// set Primitive Topology to triangle list (groups of 3 vertices)
		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	} else {
		SetIndexFromStatic();
	}
	
	// bind vertex transform constant buffer
	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));

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
		DirectX::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
