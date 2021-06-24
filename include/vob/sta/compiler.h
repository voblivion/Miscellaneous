#pragma once

#include <cstddef>
#include <limits>

// Sadly MSVC 2019 doesn't always evaluate constant expressions when it could
#define CONSTEVAL(expr) [](){ constexpr auto result = expr; return result; }()

#ifdef STA_ENABLE_TODO
#define STA_TODO(x) message(__FILE__LINE__"\nTODO: " x "\n")
#else
#define STA_TODO(x)
#endif