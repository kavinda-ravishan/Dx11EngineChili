#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Window final : public Base {
	public:
		explicit Window(const WindowDesc& desc);
		~Window();

		void* GetWindowHandle() const noexcept;
		Rect GetWindowSize() const noexcept;
		
		int64_t HandleMessage(void* arg_hwnd, uint32_t arg_msg, uint64_t arg_wparam, int64_t arg_lparam);

	private:
		const Rect _window_size{};
		void* const _h_instance{}; // HINSTANCE
		void* _window_handle{}; // HWND

		std::unique_ptr<Keyboard> _keyboard{};
		std::unique_ptr<Mouse> _mouse{};
	};

} // namespace dx3d
