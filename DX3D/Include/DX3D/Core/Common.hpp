#pragma once

#include <DX3D/Core/Core.hpp>
#include <DX3D/Math/Rect.hpp>
#include <DX3D/Core/Logger.hpp>

namespace dx3d {

	struct BaseDesc {
		Logger& logger;
	};

	struct GameDesc {
		Rect window_size{ 1280, 720 };
		Logger::LogLevel log_level = Logger::LogLevel::Error;
	};

	struct WindowDesc {
		BaseDesc base;
		Rect window_size;
	};

	struct KeyboardDesc {
		BaseDesc base;
	};

	struct MouseDesc {
		BaseDesc base;
	};

	struct GraphicsDeviceDesc {
		BaseDesc base;
		void* win_handle; // HWND
		Rect window_size;
	};

} // namespace dx3d
