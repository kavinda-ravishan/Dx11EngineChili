#include <string>
#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Drawable/Models/Plane.hpp>
#include <DX3D/Graphics/Drawable/TestPlane.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::TestPlane::TestPlane(const DrawableDesc& desc, Graphics& graphics, float size, DirectX::XMFLOAT4 color)
	: Drawable(desc), _pmc({color})
{
	auto model = Plane::Make();
	model.Transform(DirectX::XMMatrixScaling(size, size, 1.0));

	const auto geometry_tag = "$plane." + std::to_string(size);
	AddBind(BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices));
	AddBind(BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices));

	auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(BPixelShader::Resolve(BINDABLE_DESC, graphics, "SolidPS.cso"));

	AddBind(
		std::make_shared<BPixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, _pmc, 1u)
	);

	AddBind(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

	AddBind(BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(BBlender::Resolve(BINDABLE_DESC, graphics, true, 0.5f));

	AddBind(BRasterizer::Resolve(BINDABLE_DESC, graphics, true));

	AddBind(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics, *this));
}

void dx3d::TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

void dx3d::TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept {
	_roll = roll;
	_pitch = pitch;
	_yaw = yaw;
}

DirectX::XMMATRIX dx3d::TestPlane::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(_roll, _pitch, _yaw) *
		DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}

void dx3d::TestPlane::SpawnControlWindow(Graphics& graphics, const std::string& name) noexcept {
	if (ImGui::Begin(name.c_str()))
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
		auto blender = QueryBindable<BBlender>();
		float factor = blender->GetFactor();
		ImGui::SliderFloat("Translucency", &factor, 0.0f, 1.0f);
		blender->SetFactor(factor);
	}
	ImGui::End();
}
