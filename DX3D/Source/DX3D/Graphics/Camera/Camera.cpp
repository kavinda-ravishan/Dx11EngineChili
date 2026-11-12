#include <algorithm>

#include <DX3D/Graphics/Camera/Camera.hpp>
#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Math/Math.hpp>

dx3d::Camera::Camera() noexcept {
	Reset();
}

DirectX::XMMATRIX dx3d::Camera::GetMatrix() const noexcept {
	using namespace DirectX;

	const DirectX::XMVECTOR forward_base_vector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	
	// apply the camera rotations to a base vector
	const auto look_vector = DirectX::XMVector3Transform(forward_base_vector,
		DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll)
	);

	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto cam_position = DirectX::XMLoadFloat3(&_pos);
	const auto cam_target = cam_position + look_vector;
	
	return DirectX::XMMatrixLookAtLH(cam_position, cam_target, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void dx3d::Camera::SpawnControlWindow() noexcept {
	
	if (ImGui::Begin("Camera")) {
		
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &_pos.z, -80.0f, 80.0f, "%.1f");
		
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &_pitch, -90.0f * _pitch_bound_scale, 90.0f * _pitch_bound_scale);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);
		
		if (ImGui::Button("Reset")) {
			Reset();
		}
	}

	ImGui::End();
}

void dx3d::Camera::Reset() noexcept {
	_pos = { -10.0f, 0.0f, -20.0f };
	_pitch = 0.0f;
	_yaw = 0.45f;
}

void dx3d::Camera::Rotate(float dx, float dy) noexcept {
	_yaw = WrapAngle(_yaw + (dx * _rotation_speed));
	_pitch = std::clamp(_pitch + (dy * _rotation_speed), (-PI * _pitch_bound_scale) / 2.0f, (PI * _pitch_bound_scale) / 2.0f);
}

void dx3d::Camera::Translate(DirectX::XMFLOAT3 translation) noexcept {
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		DirectX::XMMatrixScaling(_travel_speed, _travel_speed, _travel_speed)
	));

	_pos = {
		_pos.x + translation.x,
		_pos.y + translation.y,
		_pos.z + translation.z
	};
}
