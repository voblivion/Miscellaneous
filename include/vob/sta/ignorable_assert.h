#pragma once

#include <vob/sta/debug_break.h>

#define ignorableAssert(cond) if (!(cond)) debugBreak();