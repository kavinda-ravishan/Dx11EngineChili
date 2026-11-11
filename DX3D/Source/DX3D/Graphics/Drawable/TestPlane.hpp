#pragma once
#include <DX3D/Graphics/Drawable/Drawable.hpp>

namespace dx3d {
	class TestPlane : public Drawable {
	public:
		TestPlane(const DrawableDesc& desc, Graphics& graphics, float size);
		void SetPos(DirectX::XMFLOAT3 pos) noexcept;
		void SetRotation(float roll, float pitch, float yaw) noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	
	private:
		DirectX::XMFLOAT3 _pos{ 1.0f,1.0f,1.0f };
		float _roll{ 0.0f };
		float _pitch{ 0.0f };
		float _yaw{ 0.0f };
	};
} // namespace dx3d
