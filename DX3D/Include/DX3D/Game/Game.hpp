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

	private:
		std::unique_ptr<Logger> _logger_ptr{};
		std::unique_ptr<Window> _window{};
		std::unique_ptr<GraphicsDevice> _graphics_device{};

		bool _is_running{true};
	};
} // namespace dx3d
