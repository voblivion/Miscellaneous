#pragma once

#include <cstdint>
#include <string_view>


namespace vob::sta
{
	inline constexpr std::uint64_t fnv1a(std::string_view a_str)
	{
		return a_str.size() == 0
			? 0xcbf29ce484222325
			: (fnv1a(a_str.substr(1)) ^ static_cast<std::uint64_t>(a_str.front()))
				* 0x100000001b3;
	}
}