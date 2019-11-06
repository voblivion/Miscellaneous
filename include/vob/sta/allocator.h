#pragma once

#include <memory>


namespace vob::sta
{

	template <typename AllocatorType, typename Type>
	using ReboundAlloc =
		typename std::allocator_traits<AllocatorType>::template rebind_alloc<Type>;
}