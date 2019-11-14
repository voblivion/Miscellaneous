#pragma once

#include <vob/sta/debug_break.h>

#define ignorable_assert(cond) if (!(cond)) debug_break();