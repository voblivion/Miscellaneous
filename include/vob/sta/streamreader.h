#pragma once

#include <cassert>
#include <exception>
#include <iostream>

#include <vob/sta/integer.h>


namespace vob::sta
{
	struct UnexpectedEndOfFile final
		: std::exception
	{};

	template <typename ValueType>
	ValueType read(std::istream& a_inputStream)
	{
		ValueType t_value;
		a_inputStream.read(
			reinterpret_cast<char*>(&t_value)
			, sizeof(ValueType)
		);
		return t_value;
	}

	class LowFirstBitStreamReader
	{
	public:
		// Constructors
		explicit LowFirstBitStreamReader(std::istream& a_inputStream)
			: m_inputStream{ a_inputStream }
		{}

		// Methods
		template <std::uint8_t t_maxBits>
		auto get(std::uint8_t const a_bits = t_maxBits)
		{
			fillBuffer(a_bits);
			auto r_value = peekBuffer<t_maxBits>(a_bits);
			ignoreBuffer(a_bits);
			return r_value;
		}

		void ignoreCurrentByte()
		{
			assert(m_bufferBitSize < 8);
			ignoreBuffer(m_bufferBitSize % 8);
		}

		bool eof() const
		{
			return m_bufferBitSize == 0 && m_inputStream.eof();
		}

	private:
		// Attributes
		std::istream& m_inputStream;
		std::uint64_t m_buffer = 0;
		std::uint8_t m_bufferBitSize = 0;
		std::size_t m_dummy = 0;

		// Methods
		void fillBuffer(std::uint8_t const a_bits)
		{
			// TODO: assert(a_bits < sizeof(m_buffer) * 8 - 7)
			while (m_bufferBitSize < a_bits)
			{
				if (m_inputStream.eof())
				{
					assert(false);
				}
				auto const byte = m_inputStream.get();
				++m_dummy;
				m_buffer |= byte << m_bufferBitSize;
				m_bufferBitSize += 8;
			}
		}

		template <std::uint8_t t_maxBits>
		auto peekBuffer(std::uint8_t const a_bits = t_maxBits) const
		{
			auto const mask = (1 << a_bits) - 1;
			return static_cast<SmallestUInt<t_maxBits>>(m_buffer & mask);
		}

		void ignoreBuffer(std::uint8_t const a_bits)
		{
			// assert(a_bits)
			m_buffer >>= a_bits;
			m_bufferBitSize -= a_bits;
		}
	};
}