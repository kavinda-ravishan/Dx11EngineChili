#include <DX3D/Graphics/Camera/Camera.hpp>
#include <DX3D/Window/ImGUI/imgui.h>

DirectX::XMMATRIX dx3d::Camera::GetMatrix() const noexcept {
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -_r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(_phi, -_theta, 0.0f)
	);
	return DirectX::XMMatrixLookAtLH(
		pos, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(
		_pitch, -_yaw, _roll
	);
}

void dx3d::Camera::SpawnControlWindow() noexcept {
	
	if (ImGui::Begin("Camera")) {
		
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &_r, 0.1f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &_theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &_phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);
		
		if (ImGui::Button("Reset")) {
			Reset();
		}
	}

	ImGui::End();
}

void dx3d::Camera::Reset() noexcept {
	_r = 20.0f;
	_theta = 0.0f;
	_phi = 0.0f;
	_pitch = 0.0f;
	_yaw = 0.0f;
	_roll = 0.0f;
}
