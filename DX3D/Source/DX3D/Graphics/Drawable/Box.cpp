#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>
#include <DX3D/Graphics/Drawable/Box.hpp>

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
	TestObject<Box>(desc, rng, adist, ddist, odist, rdist)
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
	material_constants.color = material_color;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, material_constants, 1u));

	// model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(
		&_mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

DirectX::XMMATRIX dx3d::Box::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat3x3(&_mt) * TestObject::GetTransformXM();
}

bool dx3d::Box::SpawnControlWindow(int id, Graphics& gfx) noexcept {
	using namespace std::string_literals;

	bool changed = false;
	bool open = true;
	if (ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
	{
		ImGui::Text("Material Properties");
		const auto cd = ImGui::ColorEdit3("Material Color", &material_constants.color.x);
		const auto sid = ImGui::SliderFloat("Specular Intensity", &material_constants.specular_intensity, 0.05f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		const auto spd = ImGui::SliderFloat("Specular Power", &material_constants.specular_power, 1.0f, 200.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		changed = cd || sid || spd;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &_r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &_theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &_phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if (changed)
	{
		SyncMaterial(gfx);
	}

	return open;
}

void dx3d::Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG) {
	auto pConstPS = QueryBindable<PixelConstantBuffer<PSMaterialConstant>>();
	assert(pConstPS != nullptr);
	pConstPS->Update(gfx, material_constants);
}
