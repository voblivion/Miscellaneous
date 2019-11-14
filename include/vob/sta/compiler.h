#pragma once

#include <cstddef>
#include <limits>

// Sadly MSVC 2019 doesn't always evaluate constant expressions when it could
#define CONSTEVAL(expr) [](){ constexpr auto result = expr; return result; }()
