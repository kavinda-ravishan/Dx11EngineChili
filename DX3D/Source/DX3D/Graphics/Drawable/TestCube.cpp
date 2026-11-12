#include <string>
#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>
#include <DX3D/Graphics/Drawable/TestCube.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::TestCube::TestCube(const DrawableDesc& desc, Graphics& graphics, float size)
	: Drawable(desc)
{
	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();

	const auto geometry_tag = "$cube." + std::to_string(size);
	AddBind(BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices));
	AddBind(BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices));

	AddBind(BTexture::Resolve(BINDABLE_DESC, graphics, "Assets\\Images\\brickwall.jpg"));
	AddBind(BTexture::Resolve(BINDABLE_DESC, graphics, "Assets\\Images\\brickwall_normal.jpg", 2u));

	auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(BPixelShader::Resolve(BINDABLE_DESC, graphics, "PhongPSNormalMap.cso"));

	AddBind(BPixelConstantBuffer<PSMaterialConstant>::Resolve(BINDABLE_DESC, graphics, _pmc, 1u));

	AddBind(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

	AddBind(BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<BTransformCbufDouble>(BINDABLE_DESC, graphics, *this, 0u, 2u));
}

void dx3d::TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

void dx3d::TestCube::SetRotation(float roll, float pitch, float yaw) noexcept {
	_roll = roll;
	_pitch = pitch;
	_yaw = yaw;
}

DirectX::XMMATRIX dx3d::TestCube::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(_roll, _pitch, _yaw) *
		DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}

void dx3d::TestCube::SpawnControlWindow(Graphics& graphics) noexcept {
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &_pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &_pmc.specular_intensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &_pmc.specular_power, 0.0f, 100.0f);
		bool checkState = _pmc.normal_mapping_enabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		_pmc.normal_mapping_enabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<BPixelConstantBuffer<PSMaterialConstant>>()->Update(graphics, _pmc);
		}
	}
	ImGui::End();
}
