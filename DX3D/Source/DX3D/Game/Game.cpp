#include <Windows.h>
#include <DX3D/Core/Logger.hpp>
#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.log_level).release() }),
    _logger_ptr{ &_logger },
    _window(std::make_unique<Window>(WindowDesc({ _logger, desc.window_size }))) {
}

dx3d::Game::~Game() {}

void dx3d::Game::Run() {

	MSG msg{};
	while (_is_running) {

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {
				_is_running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
}
