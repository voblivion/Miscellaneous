#pragma once

#include <vob/misc/std/debug_break.h>

#if defined(_MSC_VER)
#define ignorable_assert(cond) __pragma(warning(suppress: 4127)) if (!(cond)) debug_break();
#else
#define ignorable_assert(cond) if (!(cond)) debug_break();
#endif