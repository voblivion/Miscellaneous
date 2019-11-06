#pragma once

#include <vob/sta/debugbreak.h>

#define ignorableAssert(cond) if (!(cond)) debugBreak();