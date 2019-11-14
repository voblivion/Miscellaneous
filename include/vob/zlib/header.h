#pragma once

#include <iostream>
#include <cstdint>

#include <vob/sta/stream_reader.h>
#include <vob/zlib/error.h>

namespace vob::zlib
{

	struct header
	{
		explicit header(std::uint16_t const a_value)
			: value{ a_value }
		{}

		union
		{
			struct
			{
				std::uint8_t cmf;
				std::uint8_t flg;
			};
			struct
			{
				std::uint16_t method : 4;
				std::uint16_t info : 4;
				std::uint16_t check : 5;
				std::uint16_t dict : 1;
				std::uint16_t level : 2;
			};
			std::uint16_t value;
		};
	};

	inline auto read_header(std::istream& a_input_stream)
	{
		std::uint16_t header_flags;
		if (!sta::raw_read(a_input_stream, header_flags))
		{
			throw stream_error{ "not enough bytes in header" };
		}

		auto const r_header = header{ header_flags };
		if ((std::uint16_t{ r_header.cmf } *256 + r_header.flg) % 31 != 0)
		{
			throw stream_error{ "invalid header" };
		}

		return r_header;
	}

	inline auto peek_header(std::istream& a_inputStream)
	{
		auto const r_header = read_header(a_inputStream);
		a_inputStream.seekg(-2, std::ios_base::cur);
		return r_header;
	}
}
