#pragma once

#include <optional>
#include <vector>
#include <set>
#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Game final : public Base {
	public:
		explicit Game(const GameDesc& desc);
		~Game();
		void Run();

	private:
		void OnInternalUpdate();
		void SpawnSimulationWindow() noexcept;
		void SpawnBoxWindowManagerWindow() noexcept;
		void SpawnBoxWindows() noexcept;

	private:
		std::unique_ptr<Logger> _logger_ptr{};
		std::unique_ptr<class ImguiManager> _imgui{};
		std::unique_ptr<Window> _window{};
		std::unique_ptr<Graphics> _graphics{};
		std::unique_ptr<class Camera> _camera{};
		std::unique_ptr <class Timer> _timer{};
		std::unique_ptr<class PointLight> _point_light{};
		std::vector<std::unique_ptr<class Drawable>> _drawables{};
		
		std::vector<class Box*> _boxes{};
		std::optional<int> _combo_box_index{};
		std::set<int> _box_control_ids{};
		
		bool _pause{false};
		float _speed_factor{ 1.0f };
		
		bool _is_running{true};

		static constexpr size_t _n_drawables{ 180 };
	};
} // namespace dx3d
