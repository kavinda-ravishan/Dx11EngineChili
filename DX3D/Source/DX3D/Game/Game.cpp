#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>

#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Window/ImguiManager.hpp>

#include <DX3D/Math/Math.hpp>
#include <DX3D/Core/Logger.hpp>

#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>
#include <DX3D/IO/Mouse.hpp>
#include <DX3D/IO/Keyboard.hpp>
#include <DX3D/Timer/Timer.hpp>

#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
#include <DX3D/Graphics/Camera/Camera.hpp>
#include <DX3D/Graphics/Light/PointLight.hpp>

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
	_camera = std::make_unique<Camera>();
	_timer = std::make_unique<Timer>();

	_point_light = std::make_unique<PointLight>(
		BaseDesc(_logger), DrawableDesc({_logger}), BindableDesc({_logger}), *_graphics
	);

	_nano = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\nanosuit.obj");

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
	const auto dt = _timer->Mark() * _speed_factor;

	IOControls();

	_graphics->BeginFrame(0.07f, 0.0f, 0.12f);
	_graphics->SetCamera(_camera->GetMatrix());
	_point_light->Bind(*_graphics, _camera->GetMatrix());

	const auto nano_transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	_nano->Draw(*_graphics, nano_transform);

	_point_light->Draw(*_graphics);

	if (_graphics->IsImguiEnabled()) {
		ShowModelWindow();
		SpawnSimulationWindow();
		_camera->SpawnControlWindow();
		_point_light->SpawnControlWindow();
	}

	_graphics->EndFrame();
}

void dx3d::Game::IOControls() noexcept {
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
		_speed_factor = std::min(_speed_factor + 0.1f, 4.0f);
		break;
	case '-':
		_speed_factor = std::max(0.0f, _speed_factor - 0.1f);
		break;
	case ' ':
		_pause = !_pause;
		break;
	case '\t':
		if (_graphics->IsImguiEnabled()) {
			_graphics->DisableImgui();
		}
		else {
			_graphics->EnableImgui();
		}
		break;
	default:
		break;
	}

	_window->_mouse->Clear();
	_window->_keyboard->Clear();
}

void dx3d::Game::SpawnSimulationWindow() noexcept {
	if (ImGui::Begin("Simulation Speed")) {
		ImGui::SliderFloat("Speed Factor", &_speed_factor, 0.0f, 4.0f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", _pause ? "PAUSED" : "RUNNING (press spacebar to pause)");
	}
	ImGui::End();
}

void dx3d::Game::ShowModelWindow() {
	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}
