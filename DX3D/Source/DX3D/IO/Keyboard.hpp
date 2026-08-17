#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {
	class Keyboard final : public Base {
		friend class Window;

	public:
		explicit Keyboard(const KeyboardDesc& desc);
		~Keyboard();

	private:
		void OnKeyReleased(unsigned char keycode) noexcept;
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnChar(char character) noexcept;

		void ClearState() noexcept;

		bool AutoRepeatIsEnabled();
	};
} // namespace dx3d
