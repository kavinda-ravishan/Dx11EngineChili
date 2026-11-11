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

	private:
		std::unique_ptr<Logger> _logger_ptr{};
		std::unique_ptr<class ImguiManager> _imgui{};
		std::unique_ptr<Window> _window{};
		std::unique_ptr<Graphics> _graphics{};
		std::unique_ptr<class Timer> _timer{};
		std::unique_ptr<class Camera> _camera{};
		std::unique_ptr<class PointLight> _point_light{};
		std::unique_ptr<class Model> _nano{};
		std::unique_ptr<class TestPlane> _plane{};
		std::unique_ptr<class TestCube> _cube{};
		
		bool _is_running{ true };
	};
} // namespace dx3d
