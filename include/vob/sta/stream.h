#pragma once

#include <iostream>


namespace vob
{
	template <
		std::size_t t_maxWindowSize
		, typename CharType
		, typename CharTraits = std::char_traits<CharType>
	>
	class WindowBufferStream
	{
	public:
		// Constructors
		explicit WindowBufferStream(
			std::basic_ostream<CharType, CharTraits>& a_outputStream
		)
			: m_outputStream{ a_outputStream }
		{}

		// Methods
		void put(CharType a_char)
		{
			m_outputStream.put(a_char);
			m_window[m_windowNextPos++%t_maxWindowSize] = a_char;
			//m_windowNextPos %= t_maxWindowSize;
		}

		std::size_t windowSize() const
		{
			return std::min(t_maxWindowSize, m_windowNextPos);
		}

		char at(std::int32_t const a_offset)
		{
			return m_window[(m_windowNextPos + a_offset) % t_maxWindowSize];
		}

	private:
		// Attributes
		std::basic_ostream<CharType, CharTraits>& m_outputStream;
		std::array<CharType, t_maxWindowSize> m_window;
		std::uint64_t m_windowNextPos = 0;
	};
}