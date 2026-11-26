#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>

#include <memory>
#include <sstream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Window/ImguiManager.hpp>

#include <DX3D/Math/Math.hpp>
#include <DX3D/Core/Logger.hpp>
#include <DX3D/Utils/Timer.hpp>

#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/RenderJobs/FrameCommander.hpp>
#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>
#include <DX3D/Graphics/Drawable/Constants.hpp>
#include <DX3D/Graphics/Drawable/Material.hpp>
#include <DX3D/Graphics/Drawable/Node.hpp>
#include <DX3D/Graphics/Drawable/ModelProbe.hpp>
#include <DX3D/Graphics/Drawable/Model.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
//#include <DX3D/Graphics/Drawable/TestPlane.hpp>
//#include <DX3D/Graphics/Drawable/TestCube.hpp>
#include <DX3D/Graphics/Camera/Camera.hpp>
#include <DX3D/Graphics/Light/PointLight.hpp>

#include <DX3D/IO/Mouse.hpp>
#include <DX3D/IO/Keyboard.hpp>

#include <DX3D/Window/Window.hpp>
#include <DX3D/Game/Game.hpp>

static void ShowPerformanceWindow()
{
	static float frame_times[120] = {};
	static int offset = 0;

	ImGuiIO& io = ImGui::GetIO();

	float current_frame_time = 1000.0f / io.Framerate;

	frame_times[offset] = current_frame_time;
	offset = (offset + 1) % IM_ARRAYSIZE(frame_times);

	// Calculate averages
	float avg_frame_time = 0.0f;
	float min_frame_time = FLT_MAX;
	float max_frame_time = 0.0f;

	for (float ft : frame_times)
	{
		avg_frame_time += ft;
		min_frame_time = std::min(min_frame_time, ft);
		max_frame_time = std::max(max_frame_time, ft);
	}

	avg_frame_time /= IM_ARRAYSIZE(frame_times);

	float avg_fps = 1000.0f / avg_frame_time;
	float min_fps = 1000.0f / max_frame_time;
	float max_fps = 1000.0f / min_frame_time;

	ImGui::Begin("Performance Monitor");

	// FPS color
	ImVec4 fps_color;
	if (io.Framerate > 58.0f)
		fps_color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);      // Green
	else if (io.Framerate > 28.0f)
		fps_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);      // Yellow
	else
		fps_color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);      // Red

	ImGui::TextColored(fps_color, "FPS: %.1f", io.Framerate);
	
	ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f),
		"Average FPS: %.1f", avg_fps);
	
	ImGui::Spacing();

	ImGui::Text("FPS: %.1f (Min) - %.1f (Max)", min_fps, max_fps);

	ImGui::Separator();
	
	ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), 
		"Average Frame: %.2f ms", avg_frame_time);
	
	ImGui::Separator();

	// Blue graph
	ImGui::PushStyleColor(ImGuiCol_PlotLines,
		ImVec4(0.2f, 0.7f, 1.0f, 1.0f));

	char overlay[64];
	snprintf(overlay, sizeof(overlay), "Frame Time: %.2f ms", current_frame_time);

	ImGui::PushItemWidth(-1);
	ImGui::PlotLines(
		"##FTG",
		frame_times,
		IM_ARRAYSIZE(frame_times),
		offset,
		overlay,       // overlay text
		0.0f,
		40.0f,
		ImVec2(0, 100)
	);
	ImGui::PopItemWidth();
	
	ImGui::PopStyleColor();

	ImGui::End();
}

namespace dx3d {

	// Mesh techniques window
	class TP final : public TechniqueProbe {
	public:
		TP(const bool& imgui_enabled): _imgui_enabled(imgui_enabled) {}

		void OnSetTechnique() override {
			if (!_imgui_enabled) return;

			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, _tech_ptr->GetName().c_str());
			bool active = _tech_ptr->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(_tech_idx)).c_str(), &active);
			_tech_ptr->SetActiveState(active);
		}

		bool OnVisitBuffer(dx3d_dcb::Buffer& buf) override
		{
			if (!_imgui_enabled) return false;

			float changed = false;
			const auto CheckChanges = [&changed](bool updated) {changed = changed || updated; };
			auto tag = [tag_scratch = std::string{}, tag_string = "##" + std::to_string(_buf_idx)]
			(const char* label) mutable
				{
					tag_scratch = label + tag_string;
					return tag_scratch.c_str();
				};

			if (auto v = buf["scale"]; v.Exists())
			{
				CheckChanges(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
			}
			if (auto v = buf[PSCBV_material_color]; v.Exists())
			{
				CheckChanges(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf[PSCBV_specular_color]; v.Exists())
			{
				CheckChanges(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf[PSCBV_specular_gloss]; v.Exists())
			{
				CheckChanges(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
			}
			if (auto v = buf[PSCBV_specular_weight]; v.Exists())
			{
				CheckChanges(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf[PSCBV_use_normal_map]; v.Exists())
			{
				CheckChanges(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf[PSCBV_normal_map_weight]; v.Exists())
			{
				CheckChanges(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return changed;
		}
	
	private:
		const bool &_imgui_enabled;
	};

	class MP : ModelProbe {
	public:
		void SpawnWindow(Model& model) {
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (_selected_node_ptr != nullptr) {}
			ImGui::End();
		}

	protected:
		bool PushNode(Node& node) override {
			// if there is no selected node, set selected_Id to an impossible value
			const int selected_Id = (_selected_node_ptr == nullptr) ? -1 : _selected_node_ptr->GetID();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetID() == selected_Id) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetID(),
				node_flags, node.GetName().c_str()
			);
			// processing for selecting node
			if (ImGui::IsItemClicked()) {
				_selected_node_ptr = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}

		void PopNode(Node& node) override {
			ImGui::TreePop();
		}

	protected:
		Node* _selected_node_ptr{ nullptr };
	};
} // namespace dx3d

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
	_fc = std::make_unique<FrameCommander>();
	_timer = std::make_unique<Timer>();
	_camera = std::make_unique<Camera>();

	_point_light = std::make_unique<PointLight>(
		BaseDesc(_logger), DrawableDesc({_logger}), BindableDesc({_logger}), *_graphics, 0.2f
	);

	
	_sponza = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\Sponza\\sponza.obj", 1.0f / 20.0f);
	_model_probe = std::make_unique<MP>();

	//_stripey = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\Stripey\\stripey.obj", 2.0f);
	//_stripey->SetRootTransform(DirectX::XMMatrixTranslation(-50.0f, 9.125f, -2.75f));
	
	//_nano = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\nano_textured\\nanosuit.obj", 2.0f);
	//_nano->SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));
	
	//_gobber = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\gobber\\GoblinX.obj", 4.0f);

	//_wall = std::make_unique<Model>(BaseDesc({ _logger }), *_graphics, "Assets\\Models\\brick_wall\\brick_wall.obj", 6.0f);
	//_wall->SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));

	//_blue_plane = std::make_unique<TestPlane>(DrawableDesc({ _logger }), *_graphics, 6.0f, DirectX::XMFLOAT4{ 0.3f,0.3f,1.0f,0.0f });
	//_blue_plane->SetPos(_camera->GetPos());
	//
	//_red_plane = std::make_unique<TestPlane>(DrawableDesc({ _logger }), *_graphics, 6.0f, DirectX::XMFLOAT4{ 1.0f,0.3f,0.3f,0.0f });
	//_red_plane->SetPos(_camera->GetPos());

	//_cube = std::make_unique<TestCube>(DrawableDesc({ _logger }), *_graphics, 4.0f);
	//_cube->SetPos({ .0f, 0.0f, 0.0f });
	//_cube2 = std::make_unique<TestCube>(DrawableDesc({ _logger }), *_graphics, 4.0f);
	//_cube2->SetPos({ 2.0f, 2.0f, 2.0f });
	
	const float aspect_ratio = float(_window->GetWindowSize().height) / _window->GetWindowSize().width;
	_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 400.0f));
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

	_point_light->Submit(*_fc);
	_sponza->Submit(*_fc);
	//_stripey->Draw(*_graphics);
	//_nano->Draw(*_graphics);
	//_gobber->Submit(*_fc);
	//_wall->Draw(*_graphics);
	//_blue_plane->Draw(*_graphics);
	//_red_plane->Draw(*_graphics);
	//_cube->Submit(*_fc);
	//_cube2->Submit(*_fc);

	_fc->Execute(BINDABLE_DESC, *_graphics);
	
	//_point_light->Draw(*_graphics);

	if (_graphics->IsImguiEnabled()) {
		_camera->SpawnControlWindow();
		_point_light->SpawnControlWindow();
		_model_probe->SpawnWindow(*_sponza);

		//_sponza->ShowWindow(*_graphics, "Sponza");
		//_stripey->ShowWindow(*_graphics, "Stripey");
		//_nano->ShowWindow(*_graphics, "Nano");
		//_gobber->ShowWindow(*_graphics, "Gobber");
		//_wall->ShowWindow(*_graphics, "Wall");
		//_blue_plane->SpawnControlWindow(*_graphics, "Blue Plane");
		//_red_plane->SpawnControlWindow(*_graphics, "Red Plane");
		//_cube->SpawnControlWindow(*_graphics, "Cube 1");
		//_cube2->SpawnControlWindow(*_graphics, "Cube 2");
		
		ShowPerformanceWindow();
	}

	_graphics->EndFrame();

	_fc->Reset();
}

void dx3d::Game::IOControls() noexcept {
	static float translation_multiplier{ 1.0f };

	const bool free_camera_movments = !_window->IsCursorEnabled();

	const auto dt = _timer->Mark();

	const auto& mouse_delta = _window->_mouse->GetRawDelta();
	const auto& mouse_wheel_delta = _window->_mouse->GetWheelDelta();

	const char key = _window->_keyboard->GetCharKey();

	if (free_camera_movments) {
		_camera->Rotate(static_cast<float>(mouse_delta[0]), static_cast<float>(mouse_delta[1]));
		translation_multiplier = std::clamp(translation_multiplier + (mouse_wheel_delta / 5.0f), 0.1f, 5.0f);
	}

	switch (key) {
	
	// imGUI menu state
	case 27:
	{
		if (_graphics->IsImguiEnabled()) {
			_graphics->DisableImgui();
		} else {
			_graphics->EnableImgui();
		}
		break;
	}

	// Cursor state
	case '\t': // Esc
	{
		if (_window->IsCursorEnabled()) {
			_window->DisableCursor();
		} else {
			_window->EnableCursor();
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


