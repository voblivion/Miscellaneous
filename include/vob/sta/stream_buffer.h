#pragma once

#include <vector>


namespace vob::sta
{
	template <
		typename CharType = char
		, typename AllocatorType = std::allocator<CharType>
	>
	class VectorStreamBuffer final
		: public std::basic_streambuf<CharType>
	{
	public:
		using Base = std::basic_streambuf<CharType>;

		// Constructors
		explicit VectorStreamBuffer(
			std::vector<CharType, AllocatorType>& a_buffer
		)
			: m_buffer{ a_buffer }
		{
			Base::setg(
				&*m_buffer.begin()
				, &*m_buffer.begin()
				, (&*(m_buffer.end() - 1)) + 1
			);
			Base::setp(&*m_buffer.begin(), (&*(m_buffer.end() - 1)) + 1);
		}

		// Methods
		typename Base::int_type underflow() override
		{
			auto c = Base::underflow();
			auto d = std::char_traits<CharType>::eof();
			return c;
		}

		// Attributes
		std::vector<CharType, AllocatorType>& m_buffer;
	};

	template <
		typename CharType
		, typename AllocatorType
	>
	auto makeVectorStreamBuffer(
		std::vector<CharType, AllocatorType>& a_buffer
	)
	{
		return VectorStreamBuffer<CharType, AllocatorType>{ a_buffer };
	}
}