#pragma once

#include <tuple>


namespace vob::sta
{
	template <std::size_t t_index = 0, typename TFunctor, typename... TTypes>
	std::enable_if_t<t_index == sizeof...(TTypes)> for_each(std::tuple<TTypes...> const&, TFunctor)
	{}

	template <std::size_t t_index = 0, typename TFunctor, typename... TTypes>
	std::enable_if_t<t_index < sizeof...(TTypes)> for_each(
		std::tuple<TTypes...> const& a_tuple
		, TFunctor a_functor
	)
	{
		a_functor(std::get<t_index>(a_tuple));
		for_each<t_index + 1, TFunctor, TTypes...>(a_tuple, a_functor);
	}
}
