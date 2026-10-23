#pragma once
#include <array>

#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Mouse final : public Base {
		friend class Window;

	public:
		explicit Mouse(const MouseDesc& desc);
		~Mouse();

		void Clear() noexcept;

		const std::array<int, 2>& GetPos() const noexcept;
		const int32_t GetWheelDelta() const noexcept;

	private:
		std::array<int, 2> _pos{};
		int32_t _wheel_delta{};

		void OnMouseMove(int x, int y) noexcept;
		void OnLeftPressed(int x, int y) noexcept;
		void OnLeftReleased(int x, int y) noexcept;
		void OnRightPressed(int x, int y) noexcept;
		void OnRightReleased(int x, int y) noexcept;
		void OnMiddlePressed(int x, int y) noexcept;
		void OnMiddleReleased(int x, int y) noexcept;
		void OnWheelMove(int delta, int x, int y) noexcept;
	};

} // namespace dx3d
