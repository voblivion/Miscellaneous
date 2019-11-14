#pragma once

#include <cinttypes>


namespace vob::sta
{
#pragma region ceil_log
	template <std::uint64_t N>
	struct ceil_log2
	{
		static constexpr std::uint32_t value = 1u + ceil_log2<(N + 1u) / 2u>::value;
	};

	template <>
	struct ceil_log2<0u>
	{
		static constexpr std::uint32_t value = 1u;
	};

	template <>
	struct ceil_log2<1u>
	{
		static constexpr std::uint32_t value = 1u;
	};

	template <std::uint64_t N>
	static constexpr auto ceil_log2_v = ceil_log2<N>::value;
#pragma endregion

#pragma region uint
	template <std::uint8_t Bits>
	struct uint;

	template <>
	struct uint<1u>
	{
		using type = bool;
	};

	template <>
	struct uint<8u>
	{
		using type = std::uint8_t;
	};

	template <>
	struct uint<16u>
	{
		using type = std::uint16_t;
	};

	template <>
	struct uint<32u>
	{
		using type = std::uint32_t;
	};

	template <>
	struct uint<64u>
	{
		using type = std::uint64_t;
	};

	template <uint8_t Bits>
	using uint_t = typename uint<Bits>::type;
#pragma endregion

#pragma region smallest_uint
	template <uint8_t Bits>
	struct smallest_uint
	{
		using type = uint_t<(1u << (ceil_log2_v<Bits> -1u))>;
	};

	template <>
	struct smallest_uint<2>
	{
		using type = std::uint8_t;
	};

	template <>
	struct smallest_uint<3>
	{
		using type = std::uint8_t;
	};

	template <>
	struct smallest_uint<4>
	{
		using type = std::uint8_t;
	};

	template <uint8_t Bits>
	using smallest_uint_t = typename smallest_uint<Bits>::type;
#pragma endregion

#pragma region endian
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
#pragma endregion
}