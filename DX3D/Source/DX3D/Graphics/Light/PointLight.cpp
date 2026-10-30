#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Light/PointLight.hpp>

dx3d::PointLight::PointLight(const BaseDesc& desc, const DrawableDesc& drawable_desc, const BindableDesc& bindable_desc, Graphics& graphics, float radius)
	: 
	Base(desc), 
	_mesh(drawable_desc, graphics, radius),
	_cbuf(bindable_desc, graphics)
{}

void dx3d::PointLight::SpawnControlWindow() noexcept {
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &_pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &_pos.z, -60.0f, 60.0f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void dx3d::PointLight::Reset() noexcept {
	_pos = { 0.0f,0.0f,0.0f };
}

void dx3d::PointLight::Draw(Graphics& graphics) const noexcept(!IS_DEBUG) {
	_mesh.SetPos(_pos);
	_mesh.Draw(graphics);
}

void dx3d::PointLight::Bind(Graphics& graphics) const noexcept {
	_cbuf.Update(graphics, PointLightCBuf{ _pos });
	_cbuf.Bind(graphics);
}
