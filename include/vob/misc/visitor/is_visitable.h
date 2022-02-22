#pragma once


#include <type_traits>

namespace vob::misvi
{
	template <typename TVisitor, typename TValue>
	concept is_visitable_static = requires(TValue& a_value)
	{
		{ TValue::accept(std::declval<TVisitor&>(), a_value) };
	};

	template <typename TVisitor, typename TValue>
	concept is_visitable_free = requires(TValue& a_value)
	{
		{ accept(std::declval<TVisitor&>(), a_value) };
	};

	template <typename TVisitor, typename TValue>
	concept is_visitable_member = requires(TValue& a_value)
	{
		{ a_value.accept(std::declval<TVisitor&>()) };
	};

	template <typename TVisitor, typename TValue>
	concept is_visitable = (
		is_visitable_static<TVisitor, TValue>
		|| is_visitable_free<TVisitor, TValue>
		|| is_visitable_member<TVisitor, TValue>);
}
