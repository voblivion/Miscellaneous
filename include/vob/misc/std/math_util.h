#pragma once

#include <array>
#include <cstdint>


namespace vob::mistd
{
	namespace math_util
	{
		/// @brief Computes the least integer greater than or equal to the base-2 logarithm of input value.
		constexpr std::uint8_t ceil_log2(std::uint64_t a_num)
		{
			constexpr std::array<std::uint64_t, 6> masks = {
				0xFFFFFFFF00000000ull
				, 0x00000000FFFF0000ull
				, 0x000000000000FF00ull
				, 0x00000000000000F0ull
				, 0x000000000000000Cull
				, 0x0000000000000002ull
			};

			auto result = std::uint8_t{ (a_num & (a_num - 1)) == 0 ? 0u : 1u };
			auto j = std::uint8_t{ 32 };
			for (auto i = 0u; i < 6; ++i)
			{
				auto k = std::uint8_t{ (a_num & masks[i]) == 0 ? 0u : j };
				result += k;
				a_num >>= k;
				j >>= 1;
			}

			return result;
		}
	}
}
