#include "Timer.hpp"

dx3d::Timer::Timer() noexcept {
	using namespace std::chrono;
	_last = steady_clock::now();
}

float dx3d::Timer::Mark() noexcept {
	using namespace std::chrono;

	const auto old = _last;
	_last = steady_clock::now();
	const duration<float> frameTime = _last - old;
	return frameTime.count();
}

float dx3d::Timer::Peek() const noexcept {
	using namespace std::chrono;

	return duration<float>(steady_clock::now() - _last).count();
}
