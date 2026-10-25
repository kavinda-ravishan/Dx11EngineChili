#include <DX3D/Drawable/Box.hpp>
#include <DX3D/Graphics/Bindable/BindableBase.hpp>

#define BINDABLE_DESC BindableDesc({ GetLogger() })

dx3d::Box::Box(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist)
	: 
	Drawable(desc), 
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
	// create and set vertex buffer
	struct VertexStruct {
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	const std::vector<VertexStruct> vertices{
		{-1.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
	};

	AddBind(std::make_unique<VertexBuffer>(BINDABLE_DESC, graphics, vertices));

	// read and set vertex shader
	auto vertex_shader = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"VertexShader.cso");
	auto vertex_shader_byte_code = vertex_shader->GetBytecode();
	AddBind(std::move(vertex_shader));

	// read and set pixel shader
	AddBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"PixelShader.cso"));

	// create and set index buffer
	const std::vector<unsigned short> indices{
		0, 2, 1,  2, 3, 1,
		1, 3, 5,  3, 7, 5,
		2, 6, 3,  3, 6, 7,
		4, 5, 7,  4, 7, 6,
		0, 4, 2,  2, 4, 6,
		0, 1, 4,  1, 5, 4
	};

	AddIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, indices));

	// create constant buffer for cube face colors
	struct PSConstantBufferStruct {
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const PSConstantBufferStruct ps_constant_buffer_data = {
		{
			{1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f, 1.0f}
		}
	};

	AddBind(std::make_unique<PixelConstantBuffer<PSConstantBufferStruct>>(BINDABLE_DESC, graphics, ps_constant_buffer_data));

	// input (vertex) layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> vs_input_element_desc{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	AddBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, vs_input_element_desc, vertex_shader_byte_code));

	// set Primitive Topology to triangle list (groups of 3 vertices)
	AddBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	// bind vertex transform constant buffer
	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::Box::Update(const float dt) noexcept {
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX dx3d::Box::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
