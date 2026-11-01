#pragma once
#include <DX3D/Graphics/Drawable/TestObject.hpp>

namespace dx3d {

	class Cylinder : public TestObject<Cylinder>
	{
	public:
		Cylinder(
			const DrawableDesc& desc,
			Graphics& graphics, 
			std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist,
			std::uniform_int_distribution<int>& tdist);
	};
} // namespace dx3d
