#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>

namespace dx3d {
	class Camera {
	public:
		DirectX::XMMATRIX GetMatrix() const noexcept;
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
	private:
		float _r = 20.0f;
		float _theta = 0.0f;
		float _phi = 0.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;
		float _roll = 0.0f;
	};
} // namespace dx3d
