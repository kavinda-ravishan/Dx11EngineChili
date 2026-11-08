#pragma once

#include <cmath>

namespace dx3d {
	
	constexpr float PI = 3.14159265f;
	constexpr double PI_D = 3.1415926535897932;

	template <typename T>
	constexpr auto sq(const T& x)
	{
		return x * x;
	}

	template<typename T>
	T WrapAngle(T angle) {
		angle = std::fmod(angle + (T)PI_D, (T)2.0 * (T)PI_D);
		angle = (angle < 0) ? angle + (T)2.0 * (T)PI_D : angle;
		return angle - (T)PI_D;
	}

	template<typename T>
	constexpr T interpolate(const T& src, const T& dst, float alpha)
	{
		return src + (dst - src) * alpha;
	}

	template<typename T>
	constexpr T to_rad(T deg)
	{
		return deg * PI / (T)180.0;
	}
} // namespace dx3d
