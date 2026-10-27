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
#include <DX3D/Drawable/Melon.hpp>
#include <DX3D/Drawable/Pyramid.hpp>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.log_level).release() }),
    _logger_ptr{ &_logger } 
{
	DX3DLogInfo("Initializing game");

	class Factory {
	public:
		Factory(Logger& logger, Graphics& graphics) : _logger(logger), _graphics(graphics) {}

		std::unique_ptr<Drawable> operator()() {
			switch (_typedist(_rng)) {
			case 0:
				return std::make_unique<Pyramid>(
					DrawableDesc({ _logger }), _graphics, _rng, _adist, _ddist,
					_odist, _rdist
				);
			case 1:
				return std::make_unique<Box>(
					DrawableDesc({ _logger }), _graphics, _rng, _adist, _ddist,
					_odist, _rdist, _bdist
				);
			case 2:
				return std::make_unique<Melon>(
					DrawableDesc({ _logger }), _graphics, _rng, _adist, _ddist,
					_odist, _rdist, _longdist, _latdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}

	private:
		Logger& _logger;
		Graphics& _graphics;
		std::mt19937 _rng{ std::random_device{}() };
		std::uniform_real_distribution<float> _adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> _ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> _odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> _rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> _bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> _latdist{ 5,20 };
		std::uniform_int_distribution<int> _longdist{ 10,40 };
		std::uniform_int_distribution<int> _typedist{ 0,2 };
	};

	_window = std::make_unique<Window>(WindowDesc({ _logger, desc.window_size }));
	_graphics= std::make_unique<Graphics>(
		GraphicsDesc({ _logger }, 
		_window->GetWindowHandle(), 
		_window->GetWindowSize())
	);
	_timer = std::make_unique<Timer>();

	Factory f(_logger, *_graphics);
	_drawables.reserve(_n_drawables);
	std::generate_n(std::back_inserter(_drawables), _n_drawables, f);

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
	const auto dt = _timer->Mark();

	static float screen_color = 0.0f;

	char key = '\0';
	key = _window->_keyboard->GetCharKey();

	const auto wheel_delta = _window->_mouse->GetWheelDelta();
	if (wheel_delta > 0) {
		key = '+';
	}
	else if (wheel_delta < 0) {
		key = '-';
	}

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
	for (auto& drawable : _drawables)
	{
		drawable->Update(dt);
		drawable->Draw(*_graphics);
	}
	_graphics->EndFrame();

	_window->_mouse->Clear();
	_window->_keyboard->Clear();
}
