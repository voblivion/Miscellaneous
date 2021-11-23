#pragma once

#include <iostream>

namespace vob::mistd
{
	/// @brief 
	template <std::size_t t_maxHistorySize, typename TChar = char, typename TCharTraits = std::char_traits<TChar>>
	class repeat_stream_writer
	{
	public:
#pragma region ACCESSORS
		/// @brief Provides the size of underlying history buffer.
		auto get_history_size() const
		{
			return m_historySize;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Writes character into output stream and saves it in a history buffer for potential re-write later.
		/// @param a_output_stream : the output stream to write into
		/// @param a_character : the character to write and save in the history buffer
		void put(std::basic_ostream<TChar, TCharTraits>& a_output_stream, TChar a_character)
		{
			a_output_stream.put(a_character);
			m_history[m_next_history_index++ % t_maxHistorySize] = a_character;
			m_historySize = std::min(m_historySize + 1, t_maxHistorySize);
		}

		/// @brief Re-writes into output stream a character that was previously written, updating the history buffer.
		/// @param a_output_stream : the output stream to write into
		/// @param a_back_offset : offset of the character to write, relative to last character written; must be
		/// strictly inferior to get_history_size()
		void repeat(std::basic_ostream<TChar, TCharTraits>& a_output_stream, std::size_t const a_back_offset)
		{
			assert(a_back_offset < t_maxHistorySize && "Offset greater than maximum history size.");
			assert(m_historySize > a_back_offset && "Re-writing a character that wasn't previously written.");
			auto value = m_history[(m_next_history_index - 1 - a_back_offset) % t_maxHistorySize];
			put(a_output_stream, value);
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		std::array<TChar, t_maxHistorySize> m_history;
		std::size_t m_next_history_index = 0;
		std::size_t m_historySize = 0;
#pragma endregion
	};
}