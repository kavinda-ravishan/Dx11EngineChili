#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Window final : public Base {
	public:
		Window(const WindowDesc& desc);

	private:
		const Rect _window_size{};
		void* _window_handle{};
	};

} // namespace dx3d
