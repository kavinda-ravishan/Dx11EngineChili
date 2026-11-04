#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Game final : public Base {
	public:
		explicit Game(const GameDesc& desc);
		~Game();
		void Run();

	private:
		void OnInternalUpdate();
		void IOControls() noexcept;
		void ShowModelWindow();

	private:
		std::unique_ptr<Logger> _logger_ptr{};
		std::unique_ptr<class ImguiManager> _imgui{};
		std::unique_ptr<Window> _window{};
		std::unique_ptr<Graphics> _graphics{};
		std::unique_ptr<class Camera> _camera{};
		std::unique_ptr<class PointLight> _point_light{};

		std::unique_ptr<class Model> _nano{};
		struct
		{
			float roll {0.0f};
			float pitch {3.144f};
			float yaw {0.0f};
			float x {0.0f};
			float y {-10.0f};
			float z{15.0f};
		} pos;
		
		bool _is_running{true};
	};
} // namespace dx3d
