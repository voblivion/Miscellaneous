#pragma once

#include <array>
#include <cassert>
#include <compare>
#include <stdexcept>
#include <string>
#include <string_view>

#include <vob/sta/compiler.h>

namespace vob::sta
{
    using unicode = std::uint32_t;

	struct utf8_error final
		: std::runtime_error
    {
        explicit utf8_error(char const* a_reason)
            : std::runtime_error{ a_reason }
        {}
	};

    enum class utf8_error_handling
    {
        Throw
        , Empty
        , Stop
    };

	constexpr std::size_t uft8_max_size = 4;
	constexpr unicode invalid_unicode = 0xffff;

	inline std::size_t utf8_code_size(char const a_utf8Header)
	{
		std::uint8_t const h0 = a_utf8Header & 0b11110000;
		return h0 < 0b10000000 ? 1 : (h0 < 0b11100000 ? 2 : (h0 < 0b11110000 ? 3 : 4));
	}

	inline std::size_t utf8_code_size(unicode a_code)
	{
		return a_code < 0x007f ? 1 : (a_code < 0x07ff ? 2 : (a_code < 0xffff ? 3 : 4));
	}

	inline std::size_t utf8_string_size(std::string_view a_string, bool const a_isEmptyOnError = false)
	{
		std::size_t stringSize = 0;
		while (!a_string.empty())
		{
			auto codeSize = utf8_code_size(*a_string.data());
			if (codeSize > a_string.size())
			{
				if (a_isEmptyOnError)
                {
                    return -1;
				}
				return stringSize;
            }
            stringSize += codeSize;
			a_string = a_string.substr(codeSize);
		}
	}

	inline std::size_t utf8_write(char* a_target, unicode a_code)
    {
        if (a_code <= 0x007f)
        {
            *a_target = static_cast<char>(a_code);
			return 1;
        }
        else if (a_code <= 0x07ff)
        {
            *a_target = static_cast<char>(0b11000000 | ((a_code >> 6) & 0b00011111));
            *++a_target = static_cast<char>(0b10000000 | (a_code & 0b00111111));
			return 2;
        }
        else if (a_code <= 0xffff)
        {
            *a_target = static_cast<char>(0b11100000 | ((a_code >> 12) & 0b00001111));
            *++a_target = static_cast<char>(0b10000000 | ((a_code >> 6) & 0b00111111));
            *++a_target = static_cast<char>(0b10000000 | (a_code & 0b00111111));
			return 3;
        }
        else
        {
            *a_target = static_cast<char>(0b11110000 | ((a_code >> 18) & 0b00000111));
            *++a_target = static_cast<char>(0b10000000 | ((a_code >> 12) & 0b00111111));
            *++a_target = static_cast<char>(0b10000000 | ((a_code >> 6) & 0b00111111));
            *++a_target = static_cast<char>(0b10000000 | (a_code & 0b00111111));
			return 4;
        }
	}

	inline unicode utf8_read(std::string_view& a_view)
    {
        if (a_view.empty())
        {
            return invalid_unicode;
        }
        auto ptr = a_view.data();
        auto const charSize = utf8_code_size(*ptr);
        std::uint8_t const mask0 = charSize < 2 ? 0b1111111 : (charSize < 3 ? 0b11111 : (charSize < 4 ? 0b1111 : 0b111));
        if (charSize > a_view.size())
        {
            return invalid_unicode;
        }

        unicode value = (mask0 & *(ptr++));
        constexpr std::uint8_t mask = 0b00111111;
        for (int i = 1; i < charSize; ++i)
        {
            if ((*ptr & 0b11000000) != 0b10000000)
            {
                return invalid_unicode;
            }
            value = (value << 6) | (mask & *ptr++);
        }

		a_view = a_view.substr(charSize);
        return value;
	}

	inline unicode utf8_peek(std::string_view a_view)
	{
		if (a_view.empty())
		{
			return invalid_unicode;
		}
		return utf8_read(a_view);
	}

	std::size_t utf8_clean(std::string_view& a_view)
	{
		std::size_t size = 0;
		auto copy = a_view;
        auto code = utf8_read(copy);
		while (code != invalid_unicode)
		{
			++size;
			code = utf8_read(copy);
		}
		a_view.remove_suffix(copy.size());
		return size;
	}

	template <typename AllocatorT>
	std::size_t utf8_clean(std::basic_string<char, std::char_traits<char>, AllocatorT>& a_string)
	{
		std::string_view view{ a_string };
		auto size = utf8_clean(view);
		a_string.resize(view.size());
		return size;
	}

	template <typename StringT>
	std::size_t utf8_clean(StringT& a_string, utf8_error_handling a_errorHandling)
	{
		auto prevRawSize = a_string.size();
		auto size = utf8_clean(a_string);
		if (a_string.size() != prevRawSize)
		{
			switch (a_errorHandling)
			{
			case utf8_error_handling::Throw:
				throw utf8_error{ "invalid utf8 string" };
			case utf8_error_handling::Empty:
				if constexpr (std::is_same_v<StringT, std::string_view>)
				{
					a_string = {};
				}
				else
                {
                    a_string.clear();
				}
				return 0;
			default:
				break;
			}
		}
		return size;
	}
}

namespace vob::sta::detail
{
	template <typename OwnerT>
	class utf8_const_iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = unicode;

		utf8_const_iterator(char const* a_ptr)
			: m_ptr{ a_ptr }
		{}

		value_type operator*() const
		{
			return utf8_peek(std::string_view{ m_ptr, 4 });
		}

		utf8_const_iterator& operator++()
		{
			m_ptr += utf8_code_size(*m_ptr);
			return *this;
		}

		utf8_const_iterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		utf8_const_iterator& operator--()
		{
			while ((*--m_ptr & 0b11000000) == 0b10000000) {}
			return *this;
		}

		utf8_const_iterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}
		constexpr auto operator<=>(utf8_const_iterator const& a_other) const
		{
			return m_ptr <=> a_other.m_ptr;
		}
		constexpr auto operator==(utf8_const_iterator const& a_other) const
		{
			return m_ptr == a_other.m_ptr;
		}

		char const* raw() { return m_ptr; }

	private:
		char const* m_ptr;
	};

	template <typename OwnerT>
	class utf8_const_reverse_iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = unicode;

        utf8_const_reverse_iterator(char const* a_ptr)
            : m_ptr{ a_ptr }
        {}

        value_type operator*() const
        {
			auto ptr = m_ptr;
			while ((*--ptr & 0b11000000) == 0b10000000) {}
			return utf8_peek(std::string_view{ ptr, 4 });
        }

		utf8_const_reverse_iterator& operator++()
        {
            while ((*--m_ptr & 0b11000000) == 0b10000000) {}
			return *this;
        }

		utf8_const_reverse_iterator operator++(int)
        {
            auto tmp = *this;
            ++* this;
            return tmp;
        }

		utf8_const_reverse_iterator& operator--()
        {
            m_ptr += utf8_code_size(*m_ptr);
            return *this;
        }

		utf8_const_reverse_iterator operator--(int)
        {
            auto tmp = *this;
            --* this;
            return tmp;
        }
        constexpr auto operator<=>(utf8_const_reverse_iterator const& a_other) const
        {
            return m_ptr <=> a_other.m_ptr;
        }
        constexpr auto operator==(utf8_const_reverse_iterator const& a_other) const
        {
            return m_ptr == a_other.m_ptr;
        }

    private:
        char const* m_ptr;
	};

	template <typename DerivedT, typename StorageT, typename Utf8StringViewT>
	class utf8_string_base
    {
#pragma STA_TODO("Some of these methods could be made constrepr")
	public:
		#pragma region Types
		using self = DerivedT;
        using traits_type = std::char_traits<char>;
		using value_type = unicode;
		using size_type = std::size_t;
		using const_iterator = utf8_const_iterator<self>;
        using const_reverse_iterator = utf8_const_reverse_iterator<self>;
        using string_view_type = std::string_view;
        static constexpr auto npos{ static_cast<size_type>(-1) };
		#pragma endregion

		#pragma region Constructors
		utf8_string_base() = default;
		#pragma endregion
		
		#pragma region Accessors
        value_type at(size_type a_position) const
        {
            if (a_position >= m_size)
            {
                throw std::out_of_range("invalid utf8_string position");
            }
            return (*this)[a_position];
        }
        value_type front() const
        {
            return *begin();
        }
        value_type back() const
        {
            return *rbegin();
        }
        const StorageT& data() const
        {
            return m_data;
        }
        bool empty() const
        {
            return m_data.empty();
        }
        size_type size() const
        {
            return m_size;
        }
        bool starts_with(unicode a_code) const
        {
            return *begin() == a_code;
        }
        bool ends_with(unicode a_code) const
        {
            return *rbegin() == a_code;
        }
        bool contains(unicode a_code) const
        {
            return find(a_code, begin()) != npos;
        }
        bool contains(self const& a_string) const
        {
            return contains(a_string.m_data);
        }
        bool contains(string_view_type a_string) const
        {
            return find(a_string, begin()) != npos;
        }
        const_iterator begin() const
        {
            return { m_data.data() };
        }
        const_iterator end() const
        {
            return { m_data.data() + m_data.size() };
        }
        const_reverse_iterator rbegin() const
        {
            return { m_data.data() + m_data.size() };
        }
        const_reverse_iterator rend() const
        {
            return { m_data.data() };
        }
        self substr(size_type a_pos = 0, size_type a_count = npos) const
        {
            if (a_pos >= m_size)
            {
                throw std::out_of_range("invalid utf8_string position");
            }
            auto it = iterator_at(a_pos);
            return substr(it, a_count);
        }
        self substr(const_iterator a_first, const_iterator a_last) const
        {
            self sub;
            sub.m_data.reserve(a_last.raw() - a_first.raw());
            while (a_first != a_last)
            {
                sub.push_back(*a_first++);
            }
            return sub;
        }
        self substr(const_iterator a_first, size_type a_count) const
        {
            self sub;
            sub.m_data.reserve(std::min(a_count * uft8_max_size, end().raw() - a_first.raw()));
            for (auto i = 0; i < a_count; ++i)
            {
                sub.push_back(*a_first);
                ++a_first;
            }
            return sub;
        }
		#pragma endregion
		
		#pragma region Finds
		size_type find(Utf8StringViewT a_str) const
		{
			return find(a_str.m_data);
		}
		size_type find(string_view_type a_str) const
		{
			auto pos = npos;
			auto it = m_data.begin();
			while (it != m_data.end())
			{
				pos += (*it & 0b11000000) != 0b10000000 ? 1 : 0;
				auto jt = a_str.begin();
				for (; jt != a_str.end(); ++jt)
				{
					if (*it != *jt)
					{
						break;
					}
				}
				if (jt == a_str.end())
				{
					return pos;
				}
			}
			return a_str.empty() && !m_data.empty() ? 0 : npos;
		}
		size_type find(unicode a_char) const
		{
			auto pos = npos;
			auto it = begin();
			while (it != end())
			{
				++pos;
				if (*it == a_char)
				{
					return pos;
				}
				++it;
			}
			return npos;
		}

		size_type rfind(Utf8StringViewT a_str) const
		{
			return rfind(a_str.m_data);
		}
		size_type rfind(string_view_type a_str) const
		{
			auto pos = m_size;
			auto it = m_data.rbegin();
			while (it != m_data.rend())
			{
				pos -= (*it & 0b11000000) != 0b10000000 ? 1 : 0;
				auto jt = a_str.rbegin();
				for (; jt != a_str.rend(); ++jt)
				{
					if (*it != *jt)
					{
						break;
					}
				}
				if (jt != a_str.rend())
				{
					return pos;
				}
			}
			return a_str.empty() && !m_data.empty() ? m_size : npos;
		}
		size_type rfind(unicode a_char) const
		{
			auto pos = m_size;
			auto it = rbegin();
			while (it != rend())
			{
				--pos;
				if (*it == a_char)
				{
					return pos;
				}
				++it;
			}
			return npos;
		}

		size_type find_first_of(Utf8StringViewT a_str) const
		{
			auto pos = npos;
			auto it = begin();
			while (it != end())
			{
				++pos;
				if (a_str.contains(*it))
				{
					return pos;
				}
				++it;
			}
			return npos;
		}
		size_type find_first_of(
			string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		) const
		{
			return find_first_of(Utf8StringViewT{ a_str, a_errorHandling });
		}
		size_type find_first_of(unicode a_char) const
		{
			return find(a_char);
		}

		size_type find_first_not_of(Utf8StringViewT a_str) const
        {
            auto pos = npos;
            auto it = begin();
            while (it != end())
            {
                ++pos;
                if (!a_str.contains(*it))
                {
                    return pos;
                }
                ++it;
            }
            return npos;
		}
		size_type find_first_not_of(
			string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		) const
		{
			return find_first_not_of(Utf8StringViewT{ a_str, a_errorHandling });
		}
		size_type find_first_not_of(unicode a_char) const
        {
            auto pos = npos;
            auto it = begin();
            while (it != end())
            {
                ++pos;
                if (*it != a_char)
                {
                    return pos;
                }
                ++it;
            }
            return npos;
		}

        size_type find_last_of(Utf8StringViewT a_str) const
        {
            auto pos = m_size;
            auto it = rbegin();
            while (it != rend())
            {
                --pos;
                if (a_str.contains(*it))
                {
                    return pos;
                }
                ++it;
            }
            return npos;
        }
		size_type find_last_of(
			string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
        ) const
        {
            return find_first_not_of(Utf8StringViewT{ a_str, a_errorHandling });
        }
        size_type find_last_of(unicode a_char) const
        {
            return rfind(a_char);
        }

		size_type find_last_not_of(Utf8StringViewT a_str) const
        {
            auto pos = m_size;
            auto it = rbegin();
            while (it != rend())
            {
                --pos;
                if (!a_str.contains(*it))
                {
                    return pos;
                }
                ++it;
            }
            return npos;
		}
		size_type find_last_not_of(
			string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		) const
		{
			return find_last_not_of(Utf8StringViewT{ a_str, a_errorHandling });
		}
		size_type find_last_not_of(unicode a_code) const
        {
            auto searchPosition = m_size;
            auto it = rbegin();
            while (it != rend())
            {
                --searchPosition;
                if (*it != a_code)
                {
                    return searchPosition;
                }
                ++it;
            }
            return npos;
		}
		#pragma endregion
		
		#pragma region Mutators
        void swap(self& a_other)
        {
            std::swap(m_size, a_other.m_size);
            std::swap(m_data, a_other.m_data);
        }
		#pragma endregion

		#pragma region Operators
		constexpr auto operator<=>(self const& rhs) const
		{
			return m_data <=> rhs.m_data;
		}
		constexpr auto operator==(self const& rhs)
		{
			return m_data == rhs.m_data;
		}
		value_type operator[](size_type a_pos) const
		{
			return *iterator_at(a_pos);
		}
		#pragma endregion
		
	protected:
		#pragma region Helpers
        const_iterator iterator_add(const_iterator a_it, size_type a_count)
        {
            while (a_it != end() && a_count-- > 0)
            {
                ++a_it;
            }
            return a_it;
        }
        const_iterator iterator_remove(const_iterator a_it, size_t a_count)
        {
            while (a_it != begin() && a_count-- > 0)
            {
                --a_it;
            }
            return a_it;
        }
        const_iterator iterator_at(size_type a_pos)
        {
            if (a_pos >= m_size)
            {
                throw std::out_of_range("invalid utf8_string position");
            }
            return iterator_add(begin(), a_pos);
        }
		#pragma endregion

        StorageT m_data;
        size_type m_size = 0;

        utf8_string_base(StorageT a_view, size_type a_unicodeCount)
            : m_size{ a_unicodeCount }
            , m_data{ a_view }
        {
        }
	};
}

namespace vob::sta
{
    class utf8_string_view
		: public detail::utf8_string_base<utf8_string_view, std::string_view, utf8_string_view>
    {
#pragma STA_TODO("Some of these methods could be made constrepr")
	public:
		#pragma region Types
        using base = detail::utf8_string_base<utf8_string_view, std::string_view, utf8_string_view>;

        using string_view_type = base::string_view_type;
        using size_type = base::size_type;
		#pragma endregion

		#pragma region Constructors
        utf8_string_view() = default;
		utf8_string_view(string_view_type a_view, utf8_error_handling a_errorHandling = utf8_error_handling::Stop)
		{
			m_size = utf8_clean(a_view, a_errorHandling);
			m_data = a_view;
		}
		#pragma endregion

		#pragma region Mutators
		void remove_prefix(size_type a_size)
        {
			assert(a_size <= m_size);
            m_size -= a_size;
			auto copy = m_data;
			while (a_size-- > 0)
			{
				utf8_read(copy);
			}
			m_data = copy;
		}
		void remove_suffix(size_type a_size)
        {
            assert(a_size <= m_size);
			m_size -= a_size;
			auto ptr = m_data.data() + m_data.size();
			auto rawOffset = 0;
			while (a_size > 0)
			{
				++rawOffset;
				--ptr;
				if ((*ptr & 0b11000000) != 0b10000000)
				{
					--a_size;
				}
			}
			m_data.remove_suffix(rawOffset);
        }
		#pragma endregion

	private:
        template <typename> friend class utf8_basic_string;

        utf8_string_view(string_view_type a_view, size_type a_unicodeCount)
            : base{ a_view, a_unicodeCount }
        {
        }
    };

	template <typename AllocatorT = std::allocator<char>>
	class utf8_basic_string
		: public detail::utf8_string_base<
			utf8_basic_string<AllocatorT>
			, std::basic_string<char, std::char_traits<char>, AllocatorT>
			, utf8_string_view
		>
    {
#pragma STA_TODO("Some of these methods could be made constrepr")
#pragma STA_TODO("Need for replace/copy methods?")
	public:
		#pragma region Types
		using base = detail::utf8_string_base<
			utf8_basic_string<AllocatorT>
			, std::basic_string<char, std::char_traits<char>, AllocatorT>
			, utf8_string_view
		>;
		using allocator_type = AllocatorT;
		using string_type = std::basic_string<char, std::char_traits<char>, allocator_type>;

		using string_view_type = base::string_view_type;
		using size_type = base::size_type;
		using self = base::self;
		using const_iterator = base::const_iterator;
		using const_reverse_iterator = base::const_reverse_iterator;
		#pragma endregion
		
		#pragma region Constructors
		utf8_basic_string() = default;
		
		utf8_basic_string(utf8_string_view a_view)
		{
			assign(a_view);
		}

		utf8_basic_string(
			string_view_type a_view
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
		{
			assign(a_view, a_errorHandling);
		}
		utf8_basic_string(
			string_type a_string
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
			: utf8_basic_string()
		{
			assign(std::move(a_string), a_errorHandling);
		}
		#pragma endregion

		#pragma region Accessors
		allocator_type get_allocator() const
		{
			return m_data.get_allocator();
		}
		size_type min_capacity() const
		{
			return std::floor(m_data.capacity() / uft8_max_size);
		}
		size_type max_capacity() const
		{
			return m_data.capacity();
		}
		#pragma endregion

		#pragma region Mutators
		void assign(
			string_view_type a_view
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
		{
			m_size = utf8_clean(a_view, a_errorHandling);
			m_data.assign(a_view);
		}
		void assign(
			string_type a_string
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
        {
            m_size = utf8_clean(a_string, a_errorHandling);
            m_data.assign(std::move(a_string));
		}
		void assign(utf8_string_view a_string)
		{
			m_size = a_string.m_size();
			m_data.assign(a_string.data());
		}

		void reserve(size_type a_minCapacity)
		{
			m_data.reserve(a_minCapacity * uft8_max_size);
		}
		size_type shrink_to_fit()
		{
			m_data.shrink_to_fit();
		}
        void clear()
        {
            m_data.clear();
            m_size = 0;
        }

		self& insert(size_t a_pos, unicode a_char)
		{
			return insert(base::iterator_at(a_pos), a_char);
		}
		self& insert(const_iterator a_pos, unicode a_code)
		{
			std::array<char, 4> utf8char;
			auto charSize = utf8_write(utf8char.data(), a_code);
			m_data.insert(a_pos.raw() - m_data.data(), utf8char.data(), charSize);
			++m_size;
			return *this;
		}
		self& insert(size_t a_pos, utf8_string_view a_str)
		{
			return insert(base::iterator_at(a_pos));
		}
		self& insert(const_iterator a_pos, utf8_string_view a_str)
		{
            m_data.insert(a_pos.raw() - a_pos.raw(), a_str.m_data);
            m_size += a_str.size();
			return *this;
		}
		self& insert(
			size_t a_pos
			, string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
        {
            return insert(a_pos, utf8_string_view{ a_str, a_errorHandling });
		}
		self& insert(
			const_iterator a_pos
			, string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
		{
			return insert(a_pos, utf8_string_view{ a_str, a_errorHandling });
		}
		
		self& erase(size_t a_pos, size_type a_count = base::npos)
		{
			return erase(base::iterator_at(a_pos), a_count);
		}
		const_iterator erase(const_iterator a_pos)
		{
			return erase(a_pos, iterator_add(a_pos, 1));
		}
		const_iterator erase(const_iterator a_first, const_iterator a_last)
        {
			auto it = a_first;
			while (it != a_last)
			{
				--m_size;
				++it;
			}
            m_data.erase(a_first.raw(), a_last.raw() - a_first.raw());
            return a_first;
		}
		const_iterator erase(const_iterator a_first, size_type a_count)
		{
			return erase(a_first, iterator_add(a_first, a_count));
		}
		
		void push_back(unicode a_code)
		{
			insert(base::end(), a_code);
		}
		unicode pop_back()
		{
			auto it = base::end();
			unicode c = *it--;
			m_data.resize(it.raw() - m_data.data());
			return c;
		}
		
		self& append(utf8_string_view a_str)
		{
			m_data.append(a_str.m_data);
			m_size += a_str.size();
			return *this;
		}
		self& append(
			string_view_type a_str
			, utf8_error_handling a_errorHandling = utf8_error_handling::Stop
		)
		{
			return append(utf8_string_view{ a_str, a_errorHandling });
		}

		void resize(size_type a_size, unicode a_code = 0)
		{
			m_data.reserve(m_data.size() + std::max(0, a_size - m_size) * utf8_code_size(a_code));
            while (m_size < a_size)
            {
                push_back(a_code);
                ++m_size;
            }
			auto it = base::begin();
			for (auto i = 0; i < a_size; ++i)
			{
				++it;
			}
			erase(it, base::end());
		}
		#pragma endregion

    private:
        using base::m_data;
        using base::m_size;
    };

	using utf8_string = utf8_basic_string<>;
}