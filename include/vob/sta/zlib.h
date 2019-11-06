#pragma once

#include <vob/sta/integer.h>
#include <vob/sta/streamreader.h>
#include <vob/sta/zinflate.h>

namespace vob::sta
{
	struct BadZlibHeader final
		: std::exception
	{};

	struct ZlibHeader
	{
		// Constructors
		explicit ZlibHeader(std::uint16_t const a_value)
			: m_value{ a_value }
		{}

		// Attributes
		union
		{
			struct
			{
				std::uint8_t m_cmf;
				std::uint8_t m_flg;
			};
			struct
			{
				std::uint16_t m_method : 4;
				std::uint16_t m_info : 4;
				std::uint16_t m_check : 5;
				std::uint16_t m_dict : 1;
				std::uint16_t m_level : 2;
			};
			std::uint16_t m_value;
		};
	};

	inline auto readZlibHeader(std::istream& a_inputStream)
	{
		auto zlibHeader = ZlibHeader{ read<std::uint16_t>(a_inputStream) };
		if ((std::uint16_t{ zlibHeader.m_cmf } * 256 + zlibHeader.m_flg) % 31 != 0)
		{
			throw BadZlibHeader{};
		}
		return zlibHeader;
	}

	inline auto peekZlibHeader(std::istream& a_inputStream)
	{
		auto zlibHeader = readZlibHeader(a_inputStream);
		a_inputStream.seekg(-2, std::ios_base::cur);
		return zlibHeader;
	}

	inline void readZlib(
		std::istream& a_inputStream
		, std::basic_ostream<unsigned char>& a_outputStream
	)
	{
		auto const t_zlibHeader = readZlibHeader(a_inputStream);

		auto t_dictCrc = std::uint32_t{ 0u };
		if (t_zlibHeader.m_dict)
		{
			t_dictCrc = endian(read<std::uint32_t>(a_inputStream));
		}

		zinflate(a_inputStream, a_outputStream);

		endian(read<std::uint32_t>(a_inputStream)); // CRC
	}
}