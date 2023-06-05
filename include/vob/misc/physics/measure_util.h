#pragma once

#include "measure.h"


namespace vob::misph
{
	namespace measure_util
	{
		template <typename TMeasure1, typename TMeasure2>
		using multiply_t = decltype(std::declval<TMeasure1>() * std::declval<TMeasure2>());

		template <typename TMeasure1, typename TMeasure2>
		using divide_t = decltype(std::declval<TMeasure1>() / std::declval<TMeasure2>());

		template <typename TMeasure>
		using rate_t = decltype(std::declval<TMeasure>() / 1_s);

		template <typename TUnit, typename TRepresentation>
		constexpr auto sqrt(measure<TUnit, TRepresentation> const& a_measure)
		{
			return detail::measure_make<unit_sqrt_t<TUnit>>(std::sqrt(a_measure.value()));
		}

		template <typename TMeasure>
		using sqrt_t = decltype(sqrt(std::declval<TMeasure>()));
	}
}