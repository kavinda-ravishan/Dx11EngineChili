#pragma once
#include <random>
#include <DX3D/Graphics/Drawable/TestObject.hpp>

namespace dx3d {

	class SkinnedBox final : public TestObject<SkinnedBox>
	{
	public:
		SkinnedBox(
			const DrawableDesc& desc,
			Graphics& graphics,
			std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist);
	};
} // namespace dx3d
