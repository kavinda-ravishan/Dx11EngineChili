#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>
#include <cmath>
#include <memory>
#include <random>
#include <sstream>
#include <DX3D/Timer/Timer.hpp>
#include <DX3D/Math/Constants.hpp>
#include <DX3D/Core/Logger.hpp>
#include <DX3D/IO/Mouse.hpp>
#include <DX3D/IO/Keyboard.hpp>
#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Drawable/Box.hpp>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.log_level).release() }),
    _logger_ptr{ &_logger } {
	DX3DLogInfo("Initializing game");

	_window = std::make_unique<Window>(WindowDesc({ _logger, desc.window_size }));
	_graphics= std::make_unique<Graphics>(
		GraphicsDesc({ _logger }, 
		_window->GetWindowHandle(), 
		_window->GetWindowSize())
	);

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);

	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			DrawableDesc({ _logger }), *_graphics, rng, adist,
			ddist, odist, rdist
		));
	}

	const float aspect_ratio = float(_window->GetWindowSize().height) / _window->GetWindowSize().width;
	_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 40.0f));
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
	constexpr float dt = 0.01f;
	static float screen_color = 0.0f;
	const char key = _window->_keyboard->GetCharKey();
	switch (key) {
	case '+':
		screen_color = static_cast<float>(std::max(std::min(1.0f, screen_color + 0.1f), 0.0f));
		break;
	case '-':
		screen_color = static_cast<float>(std::max(std::min(1.0f, screen_color - 0.1f), 0.0f));
		break;
	default:
		break;
	}

	_graphics->ClearBuffer(screen_color, screen_color, screen_color);
	for (auto& boxe : boxes)
	{
		boxe->Update(dt);
		boxe->Draw(*_graphics);
	}
	_graphics->EndFrame();

	_window->_mouse->Clear();
	_window->_keyboard->Clear();
}
