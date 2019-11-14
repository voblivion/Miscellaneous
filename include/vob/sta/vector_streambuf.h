#pragma once

#include <vector>


namespace vob::sta
{
	using namespace std;

	template <typename CharT = char, typename AllocatorType = allocator<CharT>>
	class vector_streambuf final
		: public basic_streambuf<CharT>
	{
		using base = basic_streambuf<CharT>;
	public:
#pragma region Aliases
		using int_type = typename base::int_type;
#pragma endregion
#pragma region Constructors
		explicit vector_streambuf(std::size_t const a_buffer_size)
		{
			collect(a_buffer_size);
		}
#pragma endregion
#pragma region Methods
		auto collect(std::size_t const a_new_buffer_size)
		{
			auto old_buffer = std::move(m_buffer);
			m_buffer.resize(a_new_buffer_size);
			base::setg(&*m_buffer.begin(), &*m_buffer.begin(), (&*(m_buffer.end() - 1)) + 1);
			base::setp(&*m_buffer.begin(), (&*(m_buffer.end() - 1)) + 1);
			return old_buffer;
		}
#pragma endregion
	private:
#pragma region Attributes
		vector<CharT, AllocatorType>& m_buffer;
#pragma endregion
	};
}