#pragma once

#include <cinttypes>


namespace vob::sta
{
	namespace detail
	{
		template <std::uint64_t n> struct CeilLog2
		{
			static constexpr std::uint32_t s_value = 1u + CeilLog2<(n + 1u) / 2u>::s_value;
		};
		template <> struct CeilLog2<0u> { static constexpr std::uint32_t s_value = 1u; };
		template <> struct CeilLog2<1u> { static constexpr std::uint32_t s_value = 1u; };
	}
	
	template <std::uint64_t n>
	static constexpr std::uint32_t s_ceilLog2 = detail::CeilLog2<n>::s_value;

	namespace detail
	{
		template <std::uint8_t t_bits> struct UIntTypeHolder;
		template <> struct UIntTypeHolder<1u> { using Type = bool; };
		template <> struct UIntTypeHolder<8u> { using Type = std::uint8_t; };
		template <> struct UIntTypeHolder<16u> { using Type = std::uint16_t; };
		template <> struct UIntTypeHolder<32u> { using Type = std::uint32_t; };
		template <> struct UIntTypeHolder<64u> { using Type = std::uint64_t; };
	}
	
	template <std::uint8_t t_bits>
	using UInt = typename detail::UIntTypeHolder<t_bits>::Type;

	namespace detail
	{
		template <std::uint8_t t_bits> struct SmallestUIntTypeHolder
		{
			using Type = UInt<(1u << (s_ceilLog2<t_bits> -1u))>;
		};
		template <> struct SmallestUIntTypeHolder<2> { using Type = std::uint8_t; };
		template <> struct SmallestUIntTypeHolder<3> { using Type = std::uint8_t; };
		template <> struct SmallestUIntTypeHolder<4> { using Type = std::uint8_t; };
	}

	template <std::uint8_t t_bits>
	using SmallestUInt = typename detail::SmallestUIntTypeHolder<t_bits>::Type;


	inline auto endian(std::uint8_t const a_number)
	{
		return a_number;
	}

	inline auto endian(std::uint16_t const a_number)
	{
		return static_cast<std::uint16_t>((a_number << 8) | (a_number >> 8));
	}

	inline auto endian(std::uint32_t const a_number)
	{
		auto bytes = reinterpret_cast<std::uint8_t const*>(&a_number);
		return ((a_number << 24) & 0xff000000)
			| ((a_number << 8) & 0x00ff0000)
			| ((a_number >> 8) & 0x0000ff00)
			| ((a_number >> 24) & 0x000000ff);
	}

	inline auto endian(std::uint64_t const a_number)
	{
		auto bytes = reinterpret_cast<std::uint8_t const*>(&a_number);
		return ((a_number << 56) & 0xff00000000000000)
			| ((a_number << 40) & 0x00ff000000000000)
			| ((a_number << 24) & 0x0000ff0000000000)
			| ((a_number << 8) & 0x000000ff00000000)
			| ((a_number >> 8) & 0x00000000ff000000)
			| ((a_number >> 24) & 0x0000000000ff0000)
			| ((a_number >> 40) & 0x000000000000ff00)
			| ((a_number >> 56) & 0x00000000000000ff);
	}
}