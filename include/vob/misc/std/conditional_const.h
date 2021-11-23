#pragma once

#include <type_traits>


namespace vob::mistd
{
	template <bool t_condition, typename TValue>
	using conditional_const = std::conditional<t_condition, TValue const, std::remove_const_t<TValue>>;

	template <bool t_condition, typename TValue>
	using conditional_const_t = typename conditional_const<t_condition, TValue>::type;
}
