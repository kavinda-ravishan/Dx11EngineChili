#pragma once

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Mouse final : public Base {
		friend class Window;

	public:
		explicit Mouse(const MouseDesc& desc);
		~Mouse();

	private:
		void OnMouseMove(int x, int y) noexcept;
		void OnLeftPressed(int x, int y) noexcept;
		void OnLeftReleased(int x, int y) noexcept;
		void OnRightPressed(int x, int y) noexcept;
		void OnRightReleased(int x, int y) noexcept;
		void OnWheelUp(int x, int y) noexcept;
		void OnWheelDown(int x, int y) noexcept;
	};

} // namespace dx3d
