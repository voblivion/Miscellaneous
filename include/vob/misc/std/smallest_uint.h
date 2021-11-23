#pragma once

#include "math_util.h"
#include "uint.h"


namespace vob::mistd
{
	/// @brief Generator of the smallest integer type able to contain provided bit count.
	template <typename std::size_t t_bitCount>
	struct smallest_uint
	{
		using type = smallest_uint<(1u << math_util::ceil_log2(t_bitCount))>;
	};

	/// @brief Generator of the smallest integer type able to contain provided bit count.
	template <>
	struct smallest_uint<2>
	{
		using type = std::uint8_t;
	};

	/// @brief Generator of the smallest integer type able to contain provided bit count.
	template <>
	struct smallest_uint<3>
	{
		using type = std::uint8_t;
	};

	/// @brief Generator of the smallest integer type able to contain provided bit count.
	template <>
	struct smallest_uint<4>
	{
		using type = std::uint8_t;
	};

	/// @brief Smallest integer type able to contain provided bit count.
	template <std::size_t t_bitCount>
	using smallest_uint_t = typename smallest_uint<t_bitCount>::type;
}
