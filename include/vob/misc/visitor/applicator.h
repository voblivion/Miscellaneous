#pragma once

#include "../type/applicator.h"


namespace vob::misvi
{
	namespace detail
	{
		/// @brief TODO
		template <typename TVisitor>
		struct applicator_apply
		{
			/// @brief TODO
			template <typename TValue>
			struct functor
			{
				bool operator()(TValue& a_value, TVisitor& a_visitor) const
				{
					return a_visitor.visit(a_value);
				}
			};
		};
	}

	/// @brief TODO
	template <bool t_const, typename TVisitor, typename TAllocator = std::allocator<char>>
	using applicator = misty::basic_applicator<
		detail::applicator_apply<TVisitor>::template functor,
		TAllocator,
		t_const,
		TVisitor&>;

	namespace pmr
	{
		/// @brief TODO
		template <bool t_const, typename TVisitor>
		using applicator = misvi::applicator<t_const, TVisitor, std::pmr::polymorphic_allocator<char>>;
	}
}