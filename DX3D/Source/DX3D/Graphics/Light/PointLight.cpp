#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Light/PointLight.hpp>

dx3d::PointLight::PointLight(const BaseDesc& desc, const DrawableDesc& drawable_desc, const BindableDesc& bindable_desc, Graphics& graphics, float radius)
	: 
	Base(desc), 
	_mesh(drawable_desc, graphics, radius),
	_cbuf(bindable_desc, graphics)
{
	Reset();
}

void dx3d::PointLight::SpawnControlWindow() noexcept {
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_cb_data.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &_cb_data.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &_cb_data.pos.z, -60.0f, 60.0f, "%.1f");
		
		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &_cb_data.diffuse_intensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Color", &_cb_data.diffuse_color.x);
		ImGui::ColorEdit3("Ambient", &_cb_data.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &_cb_data.att_const, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &_cb_data.att_lin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &_cb_data.att_quad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);
		
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void dx3d::PointLight::Reset() noexcept {
	_cb_data = {
		{ 0.0f,0.0f,0.0f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void dx3d::PointLight::Draw(Graphics& graphics) const noexcept(!IS_DEBUG) {
	_mesh.SetPos(_cb_data.pos);
	_mesh.Draw(graphics);
}

void dx3d::PointLight::Bind(Graphics& graphics) const noexcept {
	_cbuf.Update(graphics, PointLightCBuf{ _cb_data });
	_cbuf.Bind(graphics);
}
