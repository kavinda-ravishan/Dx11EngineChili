#pragma once
#include <chrono>

namespace dx3d {

	class Timer {
	public:
		Timer() noexcept;
		float Mark() noexcept;
		float Peek() const noexcept;
	private:
		std::chrono::steady_clock::time_point _last;
	};
} // namespace dx3d