#pragma once
#include <DX3D/Core/ConditionalNoexcept.hpp>

namespace dx3d {
	class Job {
	public:
		Job(const class Step* step_ptr, const class Drawable* drawable_ptr);
		void Execute(class Graphics& graphics) const noxnd;
	
	private:
		const class Step* _step_ptr{};
		const class Drawable* _drawable_ptr{};
	};
} // namespace dx3d
