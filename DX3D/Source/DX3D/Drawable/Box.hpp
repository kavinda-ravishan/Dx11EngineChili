#pragma once
#include <random>
#include <DX3D/Core/Common.hpp>
#include <DX3D/Drawable/Drawable.hpp>

namespace dx3d {
	class Box : public Drawable {
	public:
		Box(const DrawableDesc& desc,
			Graphics& graphics, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist);
		void Update(const float dt) noexcept override;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;

	private:
		// positional
		float r{};
		float roll{ 0.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
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
