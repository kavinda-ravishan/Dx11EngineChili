#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Window final : public Base {
	public:
		Window(const WindowDesc& desc);
		
		int64_t HandleMessage(void* arg_hwnd, uint32_t arg_msg, uint64_t arg_wparam, int64_t arg_lparam);

	private:
		const Rect _window_size{};
		void* const _h_instance{}; // HINSTANCE
		void* _window_handle{}; // HWND
	};

} // namespace dx3d
