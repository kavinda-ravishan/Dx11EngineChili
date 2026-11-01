#pragma once
#include <DX3D/Graphics/Drawable/TestObject.hpp>

namespace dx3d {
	class Box final : public TestObject<Box> {
	public:
		Box(const DrawableDesc& desc,
			Graphics& graphics, 
			std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist,
			DirectX::XMFLOAT3 material_color);
		
		DirectX::XMMATRIX GetTransformXM() const noexcept override;

	private:
		// model transform
		DirectX::XMFLOAT3X3 _mt;
	};

} // namespace dx3d
