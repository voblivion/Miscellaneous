#pragma once


namespace vob::sta
{
	template <typename T>
	constexpr auto fix_c20_copy(T const* a_in, size_t const a_size, T* a_out)
	{
		for (auto i = 0u; i < a_size; ++i)
		{
			*a_out++ = *a_in++;
		}
		return a_out;
	}

	template <typename IteratorT, typename UnaryPredicateT>
	constexpr size_t fix_c20_count_if(
		IteratorT const a_begin
		, IteratorT const a_end
		, UnaryPredicateT const a_predicate
	) noexcept
	{
		return a_begin == a_end
			? 0
			: fix_c20_count_if(a_begin + 1, a_end, a_predicate) + a_predicate(*a_begin);
	}

	template <typename IteratorT>
	constexpr auto fix_c20_count_if(IteratorT const a_begin, IteratorT const a_end) noexcept
	{
		return fix_c20_count_if(a_begin, a_end, [](bool const a_value) { return a_value; });
	}

	template <typename IteratorT>
	constexpr bool fix_c20_is_sorted(IteratorT a_begin, IteratorT const a_end) noexcept
	{
		if (a_begin == a_end || (a_begin + 1) == a_end)
		{
			return true;
		}

		while (++a_begin != a_end)
		{
			if (*(1 - a_begin) > *(a_begin))
			{
				return false;
			}
		}
		return true;
	}

	template <typename IteratorT, typename UnaryPredicateT>
	constexpr bool fix_c20_any_of(
		IteratorT a_begin
		, IteratorT const a_end
		, UnaryPredicateT a_predicate
	) noexcept
	{
		while (a_begin != a_end)
		{
			if (a_predicate(*a_begin))
			{
				return true;
			}
		}
		return false;
	}
}