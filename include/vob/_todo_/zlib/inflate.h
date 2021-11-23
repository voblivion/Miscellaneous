#pragma once

#include <algorithm>
#include <array>
#include <variant>

#include <vob/sta/huffman.h>
#include <vob/sta/stream_reader.h>
#include <vob/sta/stream_writer.h>

#include <vob/zlib/error.h>
#include <vob/zlib/header.h>
#include <vob/zlib/static.h>

namespace vob::zlib::detail
{
	template <std::uint8_t MaxCodeBits, std::uint32_t AlphabetSize, typename IteratorT>
	auto compute_huffman_entries(
		IteratorT const a_code_length_begin
		, IteratorT const a_code_length_end
	)
	{
		using entry_t = sta::huffman_entry<MaxCodeBits, AlphabetSize>;
		using code_t = typename entry_t::code_type;
		using code_buffer_t = typename code_t::buffer_type;
		using value_t = typename entry_t::value_type;

		// Step 1
		std::array<std::uint8_t, 32> code_length_use_counts{};
		for (auto it = a_code_length_begin; it < a_code_length_end; ++it)
		{
			++code_length_use_counts[*it];
		}
		code_length_use_counts[0] = 0;

		// Step 2
		code_buffer_t base_code = 0u;
		std::array<code_buffer_t, 32> next_codes{};
		for (auto bits = 1; bits < 32; ++bits)
		{
			base_code = (base_code + code_length_use_counts[bits - 1]) << 1;
			next_codes[bits] = base_code;
		}

		// Step 3

		auto const value_count = static_cast<std::uint32_t>(
			std::distance(a_code_length_begin, a_code_length_end)
		);
		assert(value_count <= AlphabetSize && "Too many values");
		sta::bounded_vector<entry_t, AlphabetSize> r_huffman_entries;
		for (value_t value = 0u; value < value_count; ++value)
		{
			auto const code_length = *(a_code_length_begin + value);
			if (code_length != 0)
			{
				auto const code = next_codes[code_length]++;
				r_huffman_entries.emplace_back(value, { code, code_length });
			}
		}

		return r_huffman_entries;
	}

	template <std::uint8_t MaxCodeBits, std::uint32_t t_alphabetSize>
	auto compute_huffman_entries(std::array<std::uint8_t, t_alphabetSize> const& a_codeLengths)
	{
		return compute_huffman_entries<MaxCodeBits, t_alphabetSize>(
			a_codeLengths.begin()
			, a_codeLengths.end()
		);
	}

	template <std::uint8_t Bits>
	auto read(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::uint8_t a_bits = Bits
	)
	{
		sta::smallest_uint_t<Bits> value;
		expect(
			!a_bit_stream_reader.get<5>(a_input_stream, value, a_bits)
			, "not enough bits in stream"
		);
		return value;
	}

	template <typename EntryListT, uint8_t MaxCodeBits, uint32_t AlphabetSize>
	auto decode(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, sta::huffman_coder<EntryListT, MaxCodeBits, AlphabetSize>& a_huffman_coder
	)
	{
		using value_type = typename sta::huffman_coder<
			EntryListT
			, MaxCodeBits
			, AlphabetSize
		>::value_type;
		value_type value;
		expect(
			a_huffman_coder.get(a_input_stream, a_bit_stream_reader, value)
			, "invalid huffman code"
		);
		return value;
	}

	inline auto read_coder_pair(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
	)
	{
		auto const literal_and_length_count = 257u + read<5>(a_input_stream, a_bit_stream_reader);
		expect(literal_and_length_count > 286u, "too many literal and length codes");
		auto const distance_count = 1u + read<5>(a_input_stream, a_bit_stream_reader);
		expect(literal_and_length_count > 31, "too many distance codes");
		auto const code_length_count = 4u + read<4>(a_input_stream, a_bit_stream_reader);
		expect(code_length_count > 19, "too many code length codes");

		std::array<std::uint8_t, 19> code_length_code_lengths{};
		for (auto i = 0u; i < code_length_count; ++i)
		{
			code_length_code_lengths[static_bit_orders[i]] =
				read<3>(a_input_stream, a_bit_stream_reader);
		}
		auto code_length_coder = make_huffman_coder(
			compute_huffman_entries<8>(code_length_code_lengths)
		);

		std::array<std::uint8_t, 286 + 30> all_lengths{};
		std::uint8_t previous_length = 0;
		for (auto i = 0u; i < literal_and_length_count + distance_count;)
		{
			auto const value = decode(a_input_stream, a_bit_stream_reader, code_length_coder);
			if (value < 16)
			{
				previous_length = value;
				all_lengths[i++] = value;
			}
			else
			{
				std::uint8_t duplicated_length = 0;
				std::uint16_t duplicates;
				switch (value)
				{
				case 16:
					duplicated_length = previous_length;
					duplicates = 3 + read<2>(a_input_stream, a_bit_stream_reader);
					break;
				case 17:
					duplicates = 3 + read<3>(a_input_stream, a_bit_stream_reader);
					break;
				case 18:
					duplicates = 11 + read<7>(a_input_stream, a_bit_stream_reader);
					break;
				default:
					throw stream_error{ "invalid length value for literal or length" };
				}
				std::fill(
					all_lengths.begin() + i
					, all_lengths.begin() + i + duplicates
					, duplicated_length
				);
				i += duplicates;
			}
		}

		return detail::coder_pair<
			sta::bounded_vector<sta::huffman_entry<16, 287>, 287>
			, sta::bounded_vector<sta::huffman_entry<16, 32>, 32>
		>{
			make_huffman_coder(
				compute_huffman_entries<16, 287>(
					all_lengths.begin()
					, all_lengths.begin() + literal_and_length_count
				)
			)
			, make_huffman_coder(
				compute_huffman_entries<16, 32>(
					all_lengths.begin() + literal_and_length_count
					, all_lengths.begin() + literal_and_length_count + distance_count
				)
			)
		};
	}

	inline void read_chunk_no_encryption(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::ostream& a_output_stream
		, sta::window_buffer_writer<32768, std::uint8_t>& a_window_buffer_writer
	)
	{
		a_bit_stream_reader.ignore_current_byte();
		throw sta::not_implemented{};
	}

	template <typename LiteralEntryList, typename DistanceEntryList>
	inline void read_chunk_encryption(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::ostream& a_output_stream
		, sta::window_buffer_writer<32768, std::uint8_t>& a_window_buffer_writer
		, coder_pair<LiteralEntryList, DistanceEntryList> const& a_coder_pair
	)
	{
		std::uint16_t value;
		do
		{
			value = decode(a_input_stream, a_bit_stream_reader, a_coder_pair.literal);
			expect(value < 286 + 30, "invalid value for literal or length code");

			if (0 <= value && value < 256)
			{
				a_window_buffer_writer.put(a_output_stream, static_cast<std::uint8_t>(value));
			}
			else if (value > 256)
			{
				auto const length_extra_bits = static_length_extra_bits[value - 257];
				auto const duplicated_length = static_length_codes[value - 257]
					+ read<5>(a_input_stream, a_bit_stream_reader, length_extra_bits);

				auto const extra_distance_code = decode(
					a_input_stream, a_bit_stream_reader, a_coder_pair.distance
				);
				expect(extra_distance_code < 30, "invalid value for extra distance code");
				auto const distance_extra_bits = static_distance_extra_bits[extra_distance_code];
				std::uint16_t const duplicate_distance = static_base_distances[extra_distance_code]
					+ read<13>(a_input_stream, a_bit_stream_reader, distance_extra_bits);
				expect(
					duplicate_distance <= a_window_buffer_writer.window_size()
					, "too big duplicate distance"
				);
				for (auto i = 0; i < duplicated_length; ++i)
				{
					a_window_buffer_writer.put_copy(a_output_stream, duplicate_distance);
				}
			}

		} while (value != 256);
	}

	inline void read_chunk_static_encryption(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::ostream& a_output_stream
		, sta::window_buffer_writer<32768, std::uint8_t>& a_window_buffer_writer
	)
	{
		read_chunk_encryption(
			a_input_stream
			, a_bit_stream_reader
			, a_output_stream
			, a_window_buffer_writer
			, static_coder_pair
		);
	}

	inline void read_chunk_dynamic_encryption(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::ostream& a_output_stream
		, sta::window_buffer_writer<32768, std::uint8_t>& a_window_buffer_writer
	)
	{
		read_chunk_encryption(
			a_input_stream
			, a_bit_stream_reader
			, a_output_stream
			, a_window_buffer_writer
			, read_coder_pair(a_input_stream, a_bit_stream_reader)
		);
	}

	inline bool read_chunk_node(
		std::istream& a_input_stream
		, sta::bit_stream_reader& a_bit_stream_reader
		, std::ostream& a_output_stream
		, sta::window_buffer_writer<32768, std::uint8_t>& a_window_buffer_writer
	)
	{
		auto const is_last_chunk = read<1>(a_input_stream, a_bit_stream_reader);
		auto const chunk_type = read<2>(a_input_stream, a_bit_stream_reader);

		switch (chunk_type)
		{
		case 0:
		{
			read_chunk_no_encryption(
				a_input_stream
				, a_bit_stream_reader
				, a_output_stream
				, a_window_buffer_writer
			);
			break;
		}
		case 1:
		{
			read_chunk_static_encryption(
				a_input_stream
				, a_bit_stream_reader
				, a_output_stream
				, a_window_buffer_writer
			);
			break;
		}
		case 2:
		{
			read_chunk_dynamic_encryption(
				a_input_stream
				, a_bit_stream_reader
				, a_output_stream
				, a_window_buffer_writer
			);
			break;
		}
		case 3:
		default:
			throw stream_error{ "invalid chunk type" };
		}
		return is_last_chunk;
	}

	inline void inflate_impl(std::istream& a_input_stream, std::ostream& a_output_stream)
	{
		sta::bit_stream_reader bit_stream_reader{};
		sta::window_buffer_writer<32768, std::uint8_t> window_buffer_writer{};

		while (!read_chunk_node(
			a_input_stream
			, bit_stream_reader
			, a_output_stream
			, window_buffer_writer))
		{}
	}
}

namespace vob::zlib
{
	inline void inflate(
		std::istream& a_inputStream
		, std::ostream& a_outputStream
	)
	{
		auto const zlib_header = read_header(a_inputStream);

		std::uint32_t dict_crc;
		if (zlib_header.dict)
		{
			detail::expect(
				!sta::raw_read(a_inputStream, dict_crc)
				, "not enough bytes for dictionary crc"
			);
			dict_crc = sta::endian(dict_crc);
		}

		detail::inflate_impl(a_inputStream, a_outputStream);

		std::uint32_t crc;
		detail::expect(
			!sta::raw_read(a_inputStream, crc)
			, "not enough bytes for crc"
		);
		crc = sta::endian(crc);
		// TODO : check crc
	}
}
