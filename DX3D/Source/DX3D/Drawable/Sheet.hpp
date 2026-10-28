#pragma once
#include <random>
#include <DX3D/Drawable/DrawableBase.hpp>

namespace dx3d {

	class Sheet : public DrawableBase<Sheet>
	{
	public:
		Sheet(
			const DrawableDesc& desc,
			Graphics& gfx, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist);
		void Update(float dt) noexcept override;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		// positional
		float r{};
		float roll{};
		float pitch{};
		float yaw{};
		float theta{};
		float phi{};
		float chi{};
		// speed (delta/s)
		float droll{};
		float dpitch{};
		float dyaw{};
		float dtheta{};
		float dphi{};
		float dchi{};
	};
} // namespace dx3d
