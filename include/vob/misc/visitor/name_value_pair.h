#pragma once

#include <cstddef>
#include <string_view>


namespace vob::misvi
{
	/// @brief TODO
	template <typename TValue>
	struct name_value_pair
	{
		std::string_view name;
		TValue& value;
	};

	/// @brief TODO
	template <typename TValue>
	name_value_pair<TValue> nvp(std::string_view const a_name, TValue& a_value)
	{
		return { a_name, a_value };
	}
}
