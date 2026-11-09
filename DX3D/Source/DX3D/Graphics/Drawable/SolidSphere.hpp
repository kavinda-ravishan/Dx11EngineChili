#pragma once
#include <DX3D/Graphics/Drawable/Drawable.hpp>

namespace dx3d {
	class SolidSphere final : public Drawable {
	public:
		SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius);
		void SetPos(DirectX::XMFLOAT3 pos) noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	
	private:
		DirectX::XMFLOAT3 _pos{ 1.0f,1.0f,1.0f };
	};
} // namespace dx3d 
