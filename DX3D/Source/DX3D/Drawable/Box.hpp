#pragma once
#include <random>
#include <DX3D/Core/Common.hpp>
#include <DX3D/Drawable/DrawableBase.hpp>

namespace dx3d {
	class Box final : public DrawableBase<Box> {
	public:
		Box(const DrawableDesc& desc,
			Graphics& graphics, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist);
		void Update(const float dt) noexcept override;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;

	private:
		// positional
		float _r{};
		float _roll{};
		float _pitch{};
		float _yaw{};
		float _theta{};
		float _phi{};
		float _chi{};
		// speed (delta/s)
		float _droll{};
		float _dpitch{};
		float _dyaw{};
		float _dtheta{};
		float _dphi{};
		float _dchi{};
		// model transform
		DirectX::XMFLOAT3X3 _mt;
	};

} // namespace dx3d
