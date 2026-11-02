#pragma once
#include <random>
#include <DX3D/Graphics/Drawable/TestObject.hpp>

namespace dx3d {
	class Melon final : public TestObject<Melon> {
	public:
		Melon(
			const DrawableDesc& desc,
			Graphics& graphics,
			std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_int_distribution<int>& longdist,
			std::uniform_int_distribution<int>& latdist);
	};
} // namespace dx3d 
