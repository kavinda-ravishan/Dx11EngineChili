#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>
#include <unordered_map>

namespace dx3d {
	class WindowsMessageMap final
	{
	public:
		WindowsMessageMap() noexcept;
		std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;
	private:
		std::unordered_map<DWORD, std::string> map;
	};
} // namespace dx3d