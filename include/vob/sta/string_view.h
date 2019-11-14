#pragma once

#include <string_view>
#include <limits>

namespace vob::sta
{
	using namespace std;

	// MSVC : std::basic_string_view<>::find_first/last_of is not always correct when const-evaluated
	template <typename StringT>
	constexpr auto fix_find_first_of(StringT a_str, char const a_char, typename StringT::size_type a_pos = 0)
		noexcept -> typename StringT::size_type
	{
		for (auto i = a_pos; i < a_str.size(); ++i)
		{
			if (a_str[i] == a_char)
			{
				return i;
			}
		}
		return StringT::npos;
	}

	template <typename StringT>
	constexpr auto fix_find_last_of(StringT a_str, char const a_char, typename StringT::size_type a_pos = StringT::npos)
		noexcept -> typename StringT::size_type
	{
		if (a_str.empty())
		{
			return StringT::npos;
		}
		auto pos = a_pos < a_str.size() ? a_pos : a_str.size() - 1;
		for (auto i = 0u; i < pos + 1; ++i)
		{
			if (a_str[pos - i] == a_char)
			{
				return pos - i;
			}
		}
		return StringT::npos;
	}
}
