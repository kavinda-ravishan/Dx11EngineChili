#pragma once

#include <cstdint>
#include <stdexcept>
#include <memory>

#ifdef NDEBUG
constexpr bool IS_DEBUG = false;
#else
constexpr bool IS_DEBUG = true;
#endif

namespace dx3d {

	class Logger;
	class Base;
	class Game;
	class Window;
	class Keyboard;
	class Mouse;
	class Graphics;
	class Bindable;

} // namespace dx3d
