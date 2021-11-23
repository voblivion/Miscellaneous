#pragma once

#include <cstdint>


namespace vob::mistd
{
	namespace uint_util
	{
		/// @brief Returns provided integer with reversed endianness.
		inline auto endian(std::uint8_t const a_number)
		{
			return a_number;
		}

		/// @brief Returns provided integer with reversed endianness.
		inline auto endian(std::uint16_t const a_number)
		{
			return static_cast<std::uint16_t>((a_number << 8) | (a_number >> 8));
		}

		/// @brief Returns provided integer with reversed endianness.
		inline auto endian(std::uint32_t const a_number)
		{
			return ((a_number << 24) & 0xff000000)
				| ((a_number << 8) & 0x00ff0000)
				| ((a_number >> 8) & 0x0000ff00)
				| ((a_number >> 24) & 0x000000ff);
		}

		/// @brief Returns provided integer with reversed endianness.
		inline auto endian(std::uint64_t const a_number)
		{
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
}