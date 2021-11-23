#pragma once

#include <vob/misc/std/debug_break.h>

#define ignorable_assert(cond) if (!(cond)) debug_break();