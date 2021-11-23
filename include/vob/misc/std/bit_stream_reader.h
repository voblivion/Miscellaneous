#pragma once

#include <cassert>
#include <iostream>


namespace vob::mistd
{
	/// @brief Reads bits from an input stream outputing character, saving unused bits for later reads.
	class bit_stream_reader
	{
	public:
#pragma region MANIPULATORS
		/// @brief Reads bit-by-bit up to 32 bits from an input stream to an integral.
		/// 
		/// @tparam TOutputBuffer : any integral type
		/// @param a_bit_count : how many bits to read
		/// @param a_input_stream : stream to read bits from
		/// @param a_value : where to save the read bits into
		/// 
		/// @remarks Unused bits obtained by reading characters from a_input_stream will be saved for later reads in a
		/// buffer. It is guaranteed that the number of significant bits left in the internal buffer will be strictly
		/// lower than the size of a character read from a_input_stream.
		template <typename TChar, typename TCharTraits, typename TOutputBuffer>
		requires (sizeof(TChar) <= 4) && std::is_integral_v<TOutputBuffer> && (sizeof(TOutputBuffer) <= 4)
		bool get(
			std::size_t const a_bit_count
			, std::basic_istream<TChar, TCharTraits>& a_input_stream
			, TOutputBuffer& a_value
		)
		{
			assert(a_bit_count <= sizeof(TOutputBuffer) * 8 && "Trying to read more bits than value can hold.");
			if (fill_buffer(a_bit_count, a_input_stream))
			{
				auto const mask = (1 << a_bit_count) - 1;
				a_value = static_cast<TOutputBuffer>(m_buffer & mask);
				ignore_buffer(a_bit_count);
				return true;
			}
			return false;
		}

		/// @brief Ignores bits from an input stream.
		/// 
		/// @param a_bit_count : how many bits to ignore
		/// @param a_input_stream : stream to read bits from
		/// 
		/// @remarks Unused bits obtained by reading characters from a_input_stream will be saved for later reads in a
		/// buffer. It is guaranteed that the number of significant bits left in the internal buffer will be strictly
		/// lower than the size of a character read from a_input_stream.
		template <typename TChar, typename TCharTraits>
		bool ignore(std::size_t a_bit_count, std::basic_istream<TChar, TCharTraits>& a_input_stream)
		{
			auto const remainder = a_bit_count % 32;
			if (!fill_buffer(remainder, a_input_stream))
			{
				return false;
			}
			ignore_buffer(remainder);

			for (auto i = 0; i < a_bit_count / 32; ++i)
			{
				if (!fill_buffer(32, a_input_stream))
				{
					return false;
				}
				ignore_buffer(32);
			}
			return true;
		}

		/// @brief Empties the inner bit buffer containing remaining bits of last read byte that haven't been consumed.
		/// @return number of bits that went unused, garanteed to be in range [0; 32).
		auto clear()
		{
			auto const bufferSize = m_buffer_size;
			ignore_buffer(m_buffer_size);
			return bufferSize;
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		std::uintmax_t m_buffer = 0;
		std::size_t m_buffer_size = 0;
#pragma endregion

#pragma region PRIVATE_MANIPULATORS
		/// @brief Fills internal buffer with at least a_bit_count and at most sizeof(TChar) * 8 - 1 extra bits.
		template <typename TChar, typename TCharTraits>
		bool fill_buffer(std::size_t const a_bit_count, std::basic_istream<TChar, TCharTraits>& a_input_stream)
		{
			assert(a_bit_count <= 32 && "Internal buffer can't hold that many bits.");
			TChar byte;
			while (m_buffer_size < a_bit_count && a_input_stream.get(byte))
			{
				m_buffer |= static_cast<std::uintmax_t>(byte) << m_buffer_size;
				m_buffer_size += sizeof(TChar) * 8;
			}
			return m_buffer_size >= a_bit_count;
		}

		/// @brief Removes set number of bits already in internal buffer.
		void ignore_buffer(std::size_t const a_bit_count)
		{
			assert(a_bit_count <= m_buffer_size && "Ignoring more bits than available in the read buffer.");
			m_buffer >>= a_bit_count;
			m_buffer_size -= a_bit_count;
		}
#pragma endregion
	};
}