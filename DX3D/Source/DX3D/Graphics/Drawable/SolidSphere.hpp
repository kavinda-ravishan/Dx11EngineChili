#pragma once
#include <DX3D/Graphics/Drawable/DrawableBase.hpp>

namespace dx3d {
	class SolidSphere : public DrawableBase<SolidSphere> {
	public:
		SolidSphere(const DrawableDesc& desc, Graphics& graphics, float radius);
		void Update(float dt) noexcept override;
		void SetPos(DirectX::XMFLOAT3 pos) noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	
	private:
		DirectX::XMFLOAT3 _pos{ 1.0f,1.0f,1.0f };
	};
} // namespace dx3d 
