#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>

#include <sstream>

#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Window/ImguiManager.hpp>

#include <DX3D/Math/Math.hpp>
#include <DX3D/Core/Logger.hpp>
#include <DX3D/Timer/Timer.hpp>

#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
#include <DX3D/Graphics/Camera/Camera.hpp>
#include <DX3D/Graphics/Light/PointLight.hpp>

#include <DX3D/IO/Mouse.hpp>
#include <DX3D/IO/Keyboard.hpp>

#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.log_level).release() }),
    _logger_ptr{ &_logger } 
{
	DX3DLogInfo("Initializing game");

	_imgui = std::make_unique<ImguiManager>();
	_window = std::make_unique<Window>(WindowDesc({ _logger, desc.window_size }));
	_graphics = std::make_unique<Graphics>(
		GraphicsDesc({ _logger }, 
		_window->GetWindowHandle(), 
		_window->GetWindowSize())
	);
	_timer = std::make_unique<Timer>();
	_camera = std::make_unique<Camera>();

	_point_light = std::make_unique<PointLight>(
		BaseDesc(_logger), DrawableDesc({_logger}), BindableDesc({_logger}), *_graphics
	);

	_nano = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\nano.gltf");

	const float aspect_ratio = float(_window->GetWindowSize().height) / _window->GetWindowSize().width;
	_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 40.0f));
	_graphics->SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
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
	IOControls();

	_graphics->BeginFrame(0.07f, 0.0f, 0.12f);
	_graphics->SetCamera(_camera->GetMatrix());
	_point_light->Bind(*_graphics, _camera->GetMatrix());

	_nano->Draw(*_graphics);
	_point_light->Draw(*_graphics);

	if (_graphics->IsImguiEnabled()) {
		_camera->SpawnControlWindow();
		_point_light->SpawnControlWindow();
		_nano->ShowWindow();
	}

	_graphics->EndFrame();
}

void dx3d::Game::IOControls() noexcept {
	constexpr float translation_multiplier{ 2 };
	constexpr float mouse_wheel_zoom_multiplier{ 1.0f/10.0f };

	const bool free_camera_movments = !_window->GetMenuState();

	const auto dt = _timer->Mark();

	const auto& mouse_delta = _window->_mouse->GetRawDelta();
	const auto& mouse_wheel_delta = _window->_mouse->GetWheelDelta();

	const char key = _window->_keyboard->GetCharKey();

	if (free_camera_movments) {
		_camera->Rotate(static_cast<float>(mouse_delta[0]), static_cast<float>(mouse_delta[1]));
		_camera->Translate({ 0.0f, 0.0f, static_cast<float>(mouse_wheel_delta) * mouse_wheel_zoom_multiplier });
	}

	switch (key) {
	
	// Menu
	case 27: // Esc
	{
		if (_window->GetMenuState()) {
			_window->DisableMenuState();
			//_graphics->DisableImgui();
		}
		else {
			_window->EnableMenuState();
			//_graphics->EnableImgui();
		}
		break;
	}

	// movement controls
	case 'w':
		if (free_camera_movments) _camera->Translate({ 0.0f, 0.0f, dt * translation_multiplier });
		break;
	case 's':
		if (free_camera_movments) _camera->Translate({ 0.0f, 0.0f, -dt * translation_multiplier });
		break;
	case 'a':
		if (free_camera_movments) _camera->Translate({ -dt * translation_multiplier, 0.0f, 0.0f });
		break;
	case 'd':
		if (free_camera_movments) _camera->Translate({ dt * translation_multiplier, 0.0f, 0.0f });
		break;
	case 'r':
		if (free_camera_movments) _camera->Translate({ 0.0f, dt * translation_multiplier, 0.0f });
		break;
	case 'f':
		if (free_camera_movments) _camera->Translate({ 0.0f, -dt * translation_multiplier, 0.0f });
		break;
	
	default:
		break;
	}

	_window->_mouse->Clear();
	_window->_keyboard->Clear();
}
