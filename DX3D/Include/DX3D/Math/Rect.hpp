#pragma once

#include <DX3D/Core/Core.hpp>

namespace dx3d {

	class Rect final {

	public:
		Rect() = default;
		Rect(int32_t width, int32_t height) : left(0), top(0), width(width), height(height) {}
		Rect(int32_t left, int32_t top, int32_t width, int32_t height) : left(left), top(top), width(width), height(height) {}

	public:
		int32_t left{}, top{}, width{}, height{};
	};

} // namespace dx3d
