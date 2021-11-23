#pragma once

#include "measure.h"


namespace vob::misph
{
	namespace measure_util
	{
		template <typename TUnit, typename TRepresentation>
		constexpr auto sqrt(measure<TUnit, TRepresentation> const& a_measure)
		{
			return detail::measure_make<unit_sqrt_t<TUnit>>(std::sqrt(a_measure.value()));
		}
	}
}