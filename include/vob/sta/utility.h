#pragma once

#include <type_traits>
#include <utility>


namespace std
{
	template <typename Left, typename Right>
	struct hash<pair<Left, Right>>
	{
		size_t operator()(pair<Left, Right> const& a_pair) const
		{
			return hash<Left>{}(a_pair.first) ^ ~(hash<Right>{}(a_pair.second));
		}
	};
}

namespace vob::sta
{
	using namespace std;

	template <typename T, T Offset, T... Integers>
	constexpr auto offset_integer_sequence(integer_sequence<T, Integers...>)
		-> integer_sequence<T, (Offset + Integers)...>
	{
		return {};
	}

	template <typename IntegerT, IntegerT Begin, IntegerT End>
	struct integer_range
	{
		static_assert(Begin < End, "Invalid boundaries");

		static constexpr size_t size() { return End - Begin; }
	};

	template <typename IntegerT, IntegerT Begin, IntegerT End>
	constexpr auto to_integer_sequence(integer_range<IntegerT, Begin, End>)
	{
		return offset_integer_sequence<IntegerT, Begin>(make_integer_sequence<IntegerT, End - Begin>());
	}
	
	template <typename IntegerT, IntegerT... Integers, IntegerT... OtherIntegers>
	constexpr auto operator+(
		integer_sequence<IntegerT, Integers...>
		, integer_sequence<IntegerT, OtherIntegers...>
	)
		-> integer_sequence<IntegerT, Integers..., OtherIntegers...>
	{
		return {};
	}

	template <typename IntegerT, template <IntegerT> typename Predicate, IntegerT Integer>
	constexpr auto filter_integer()
	{
		if constexpr (Predicate<Integer>::value)
		{
			return integer_sequence<IntegerT, Integer>{};
		}
		else
		{
			return integer_sequence<IntegerT>{};
		}
	}

	template <typename IntegerT, template <IntegerT> typename Predicate, IntegerT... Integers>
	constexpr auto filter_integer_sequence(integer_sequence<IntegerT, Integers...>)
	{
		return (filter_integer<IntegerT, Predicate, Integers>() + ...);
	}
}