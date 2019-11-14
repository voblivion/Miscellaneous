#pragma once

#include <iostream>
#include <algorithm>


namespace vob::sta
{
	using namespace std;

	template <size_t MaxWindowSize, typename CharT = char>
	class window_buffer_writer
	{
	public:
#pragma region Methods
		template <typename OutputStreamT>
		void put(OutputStreamT& a_output_stream, CharT a_char)
		{
			static_assert(is_same_v<typename OutputStreamT::char_type, CharT>
				, "OutputStream::char_type incompatible with window.");
			a_output_stream.put(a_char);
			m_window[m_window_next_pos++ % MaxWindowSize] = a_char;
		}

		template <typename OutputStreamT>
		void put_copy(OutputStreamT& a_output_stream, std::size_t const a_back_offset)
		{
			auto value = m_window[(m_window_next_pos - a_back_offset) % MaxWindowSize];
			put(a_output_stream, value);
		}

		size_t window_size() const
		{
			return min(MaxWindowSize, m_window_next_pos);
		}
#pragma endregion
	private:
#pragma region Attributes
		std::array<CharT, MaxWindowSize> m_window;
		std::uint64_t m_window_next_pos = 0;
#pragma endregion
	};
}