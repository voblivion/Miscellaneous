#pragma once

#include "string_id.h"

#include <string_view>


namespace vob::mishs::literals
{
	constexpr string_id operator ""_id(char const* const a_str, std::size_t const a_size)
	{
		return string_id{ std::string_view(a_str, a_size) };
	}
}
