#pragma once

#include "conditional_const.h"


namespace vob::mistd
{
	template <typename TSource, typename TTarget>
	using same_const = conditional_const<std::is_const_v<TSource>, TTarget>;

	template <typename TSource, typename TTarget>
	using same_const_t = typename same_const<TSource, TTarget>::type;
}