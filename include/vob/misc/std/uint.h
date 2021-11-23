#pragma once

#include "math_util.h"


namespace vob::mistd
{
	/// @brief Generator for unsigned integer type of desired bit-size.
	template <std::size_t t_bitCount>
	struct uint;

	/// @brief Generator for unsigned integer type of desired bit-size.
	template <>
	struct uint<1u>
	{
		using type = bool;
	};

	/// @brief Generator for unsigned integer type of desired bit-size.
	template <>
	struct uint<8u>
	{
		using type = std::uint8_t;
	};

	/// @brief Generator for unsigned integer type of desired bit-size.
	template <>
	struct uint<16u>
	{
		using type = std::uint16_t;
	};

	/// @brief Generator for unsigned integer type of desired bit-size.
	template <>
	struct uint<32u>
	{
		using type = std::uint32_t;
	};

	/// @brief Generator for unsigned integer type of desired bit-size.
	template <>
	struct uint<64u>
	{
		using type = std::uint64_t;
	};

	/// @brief Unsigned integer type of desired bit-size.
	template <std::size_t t_bitCount>
	using uint_t = typename uint<t_bitCount>::type;
}
