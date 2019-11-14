#pragma once

#include <cassert>
#include <iostream>

#include <vob/sta/integer.h>
#include <vob/sta/bounded_vector.h>


namespace vob::sta
{
	template <typename T, typename InputStreamT>
	bool raw_read(InputStreamT& a_input_stream, T& a_value)
	{
		using char_type = typename InputStreamT::char_type;
		static_assert(sizeof(T) % sizeof(char_type) == 0, "Incompatible T and InputStream.");
		
		a_input_stream.read(reinterpret_cast<char_type*>(&a_value), sizeof(T) / sizeof(char_type));
		return static_cast<bool>(a_input_stream);
	}

	class bit_stream_reader
	{
	public:
#pragma region Methods
		template <uint8_t MaxBits, typename InputStreamT, typename OutBufferT>
		bool get(
			InputStreamT& a_input_stream
			, OutBufferT& a_value
			, uint8_t const a_bits = MaxBits
		)
		{
			using char_type = typename InputStreamT::char_type;
			static_assert(MaxBits < (sizeof(m_buffer) - sizeof(char_type)) * 8 + 1);
			if (fill_buffer(a_input_stream, a_bits))
			{
				auto const mask = (1 << a_bits) - 1;
				a_value = static_cast<OutBufferT>(m_buffer & mask);
				ignore_buffer(a_bits);
				return true;
			}
			return false;
		}

		void ignore_current_byte()
		{
			assert(m_buffer_bits < 8);
			ignore_buffer(m_buffer_bits);
		}
#pragma endregion
	private:
#pragma region Attributes
		std::uint64_t m_buffer = 0;
		std::uint8_t m_buffer_bits = 0;
		std::size_t m_dummy = 0;
#pragma endregion
#pragma region Methods
		template <typename InputStreamT>
		bool fill_buffer(InputStreamT& a_input_stream, uint8_t const a_bits)
		{
			using char_type = typename InputStreamT::char_type;
			assert(a_bits < (sizeof(m_buffer) - sizeof(char_type)) * 8 + 1);
			char_type byte;
			while (m_buffer_bits < a_bits && a_input_stream.get(byte))
			{
				++m_dummy;
				m_buffer |= byte << m_buffer_bits;
				m_buffer_bits += sizeof(char_type) * 8;
			}
			return m_buffer_bits >= a_bits;
		}

		void ignore_buffer(uint8_t const a_bits)
		{
			assert(a_bits <= m_buffer_bits);
			m_buffer >>= a_bits;
			m_buffer_bits -= a_bits;
		}
#pragma endregion
	};
}
