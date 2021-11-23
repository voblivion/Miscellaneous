#pragma once

#include <type_traits>
#include <utility>


namespace vob::mistd
{
	namespace integer_sequence_util
	{
		template <typename TInteger, TInteger t_offset, TInteger... t_integers>
		constexpr auto offset(std::integer_sequence<TInteger, t_integers...>)
		{
			return std::integer_sequence<TInteger, (t_offset + t_integers)...>{};
		}

		template <typename TInteger, TInteger t_begin, TInteger t_end>
		constexpr auto make_range()
		{
			return offset<TInteger, t_begin>(std::make_integer_sequence<TInteger, t_end - t_begin>());
		}

		template <typename TInteger, TInteger... t_lhs, TInteger... t_rhs>
		constexpr auto concat(std::integer_sequence<TInteger, t_lhs...>, std::integer_sequence<TInteger, t_rhs...>)
		{
			return std::integer_sequence<TInteger, t_lhs..., t_rhs...>{};
		}

		template <typename TInteger, TInteger... t_lhs, TInteger... t_rhs>
		constexpr auto operator+(
			std::integer_sequence<TInteger, t_lhs...> a_lhs,
			std::integer_sequence<TInteger, t_rhs...> a_rhs)
		{
			return concat(a_lhs, a_rhs);
		}

		template <typename TInteger, template <TInteger> typename TPredicate, TInteger t_integer>
		constexpr auto filter_one()
		{
			if constexpr (TPredicate<t_integer>::value)
			{
				return std::integer_sequence<TInteger, t_integer>{};
			}
			else
			{
				return std::integer_sequence<TInteger>{};
			}
		}

		template <typename TInteger, template <TInteger> typename TPredicate, TInteger... t_integers>
		constexpr auto filter(std::integer_sequence<TInteger, t_integers...>)
		{
			return (filter_one<TInteger, TPredicate, t_integers>() + ...);
		}
	}
}
