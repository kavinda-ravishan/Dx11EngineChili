#include "Utils.hpp"

std::wstring dx3d::ToWide(const std::string& narrow) {
    wchar_t wide[512];
    mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
    return wide;
}

std::string dx3d::ToNarrow(const std::wstring& wide) {
    size_t size = 0;
    wcstombs_s(&size, nullptr, 0, wide.c_str(), 0);

    std::string result(size - 1, '\0'); // exclude terminating null

    wcstombs_s(&size, result.data(), size, wide.c_str(), size);

    return result;
}
