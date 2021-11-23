#pragma once

#include <vob/sta/huffman.h>


namespace vob::zlib::detail
{
	template <typename LiteralEntryList, typename DistanceEntryList>
	struct coder_pair
	{
		sta::huffman_coder<LiteralEntryList, 16, 287> literal;
		sta::huffman_coder<DistanceEntryList, 16, 32> distance;
	};

	constexpr auto make_static_literal_entry_list()
	{
		std::array<sta::huffman_entry<16, 287>, 287> list{};

		auto v = 0u;
		for (std::uint16_t i = 256u; i < 280u; ++i)
		{
			list[v++] = sta::huffman_entry<16, 287>{
				{ static_cast<std::uint16_t>(i - 256u), 7 }
				, i
			};
		}
		for (std::uint16_t i = 0u; i < 144u; ++i)
		{
			list[v++] = sta::huffman_entry<16, 287>{
				{ static_cast<std::uint16_t>(i + 48), 8 }
				, i
			};
		}
		for (std::uint16_t i = 280u; i < 287u; ++i)
		{
			list[v++] = sta::huffman_entry<16, 287>{
				{ static_cast<std::uint16_t>(i - 88u), 8 }
				, i
			};
		}
		for (std::uint16_t i = 144u; i < 256u; ++i)
		{
			list[v++] = sta::huffman_entry<16, 287>{
				{ static_cast<std::uint16_t>(i + 256), 9 }
				, i
			};
		}

		return list;
	}

	constexpr auto make_static_distance_entry_list()
	{
		std::array<sta::huffman_entry<16, 32>, 32> list{};

		for (std::uint8_t i = 0u; i < 32u; ++i)
		{
			list[i] = sta::huffman_entry<16, 32>{ {i, 5}, i };
		}

		return list;
	}

	// TODO : use default make_huffman_coder when std::sort is constexpr (c++20)
	constexpr coder_pair<
		std::array<sta::huffman_entry<16, 287>, 287>
		, std::array<sta::huffman_entry<16, 32>, 32>
	> static_coder_pair = {
		make_huffman_coder_from_sorted(make_static_literal_entry_list())
		, make_huffman_coder_from_sorted(make_static_distance_entry_list())
	};

	static constexpr std::array<std::uint8_t, 19> static_bit_orders = {
		16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
	};

	static constexpr std::array<std::uint16_t, 29> static_length_codes = {
		3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258
	};

	static constexpr std::array<std::uint8_t, 29> static_length_extra_bits = {
		0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
	};

	static constexpr std::array<std::uint16_t, 30> static_base_distances = {
		1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513
		,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577
	};

	static constexpr std::array<std::uint8_t, 30> static_distance_extra_bits = {
		0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
	};

}