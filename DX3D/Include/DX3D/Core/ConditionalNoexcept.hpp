#pragma once

#ifdef NDEBUG
constexpr bool IS_DEBUG = false;
#else
constexpr bool IS_DEBUG = true;
#endif

#define noxnd noexcept(!IS_DEBUG)
