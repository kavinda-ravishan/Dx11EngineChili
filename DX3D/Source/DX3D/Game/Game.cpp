#include <Windows.h>
#include <cmath>
#include <sstream>
#include <DX3D/Math/Constants.hpp>
#include <DX3D/Core/Logger.hpp>
#include <DX3D/IO/Mouse.hpp>
#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>
#include <DX3D/Graphics/GraphicsDevice.hpp>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.log_level).release() }),
    _logger_ptr{ &_logger } {
	DX3DLogInfo("Initializing game");

	_window = std::make_unique<Window>(WindowDesc({ _logger, desc.window_size }));
	_graphics_device = std::make_unique<GraphicsDevice>(
		GraphicsDeviceDesc({ _logger }, 
		_window->GetWindowHandle(), 
		_window->GetWindowSize())
	);
}

dx3d::Game::~Game() {
	DX3DLogInfo("Destroying game");
}

void dx3d::Game::Run() {

	MSG msg{};
	BOOL peek_msg_res = 0;
	while (_is_running) {

		while (peek_msg_res = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {
				_is_running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		OnInternalUpdate();
	}

	if (peek_msg_res == -1) {
		DX3DLogWarning("Peek message returned with -1");
	}
	else if(msg.wParam != 0) {
		std::ostringstream oss;
		oss << "Window procedure returned with " << msg.wParam;
		DX3DLogWarning(oss.str().c_str());
	}
}

void dx3d::Game::OnInternalUpdate() {
	static float curr_ang = 0;
	static float cube_z = 0;

	constexpr float ang_inc = dx3d::PI / 120;
	const float c = sin(curr_ang) / 2.0f + 0.5f;

	const auto& mouse_pos = _window->_mouse->GetPos();
	const auto wheel_delta = _window->_mouse->GetWheelDelta();
	const auto& window_size = _window->GetWindowSize();

	_graphics_device->ClearBuffer(c, c, 1.0f);

	_graphics_device->DrawTestCube(
		-curr_ang,
		0, 0, 0
	);

	cube_z += ((float)wheel_delta / 10);
	_graphics_device->DrawTestCube(
		curr_ang, 
		(float(mouse_pos[0] * 2) / window_size.width) - 1, 
		-((float(mouse_pos[1] * 2) / window_size.height) - 1), 
		cube_z
	);

	_graphics_device->EndFrame();

	curr_ang = curr_ang < 2 * dx3d::PI ? curr_ang + ang_inc : 0;
}
