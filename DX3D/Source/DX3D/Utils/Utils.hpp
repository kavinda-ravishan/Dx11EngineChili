#pragma once
#include <vector>
#include <string>

namespace dx3d {
	std::wstring ToWide(const std::string& narrow);
	std::string ToNarrow(const std::wstring& wide);
} // namespace dx3d
