#pragma once

#include <cstddef>


namespace vob::misvi
{
	/// @brief TODO
	template <typename TValue>
	struct index_value_pair
	{
		std::size_t index;
		TValue& value;
	};

	/// @brief TODO
	template <typename TValue>
	index_value_pair<TValue> ivp(std::size_t const a_index, TValue& a_value)
	{
		return { a_index, a_value };
	}
}