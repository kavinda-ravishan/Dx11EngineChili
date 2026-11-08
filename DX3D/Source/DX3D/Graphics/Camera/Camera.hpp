#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>

namespace dx3d {
	class Camera final {
	public:
		Camera() noexcept;

		DirectX::XMMATRIX GetMatrix() const noexcept;
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
		void Rotate(float dx, float dy) noexcept;
		void Roll(float roll) noexcept;
		void Translate(DirectX::XMFLOAT3 translation) noexcept;

	private:
		DirectX::XMFLOAT3 _pos{};
		float _pitch{ 0.0f };
		const float _pitch_bound_scale{ 0.995f };
		float _yaw{ 0.0f };
		const float _roll{ 0.0f };
		static constexpr float _travel_speed{ 12.0f };
		static constexpr float _rotation_speed{ 0.004f };
	};
} // namespace dx3d
