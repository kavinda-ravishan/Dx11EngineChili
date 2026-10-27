#pragma once

#include <vector>
#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Game final : public Base {
	public:
		explicit Game(const GameDesc& desc);
		~Game();
		void Run();

	private:
		void OnInternalUpdate();

	private:
		std::unique_ptr<Logger> _logger_ptr{};
		std::unique_ptr<Window> _window{};
		std::unique_ptr<Graphics> _graphics{};
		std::unique_ptr <class Timer> _timer{};
		std::vector<std::unique_ptr<class Drawable>> _drawables{};
		static constexpr size_t _n_drawables{ 180 };

		bool _is_running{true};
	};
} // namespace dx3d
