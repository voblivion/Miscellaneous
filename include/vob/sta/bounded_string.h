#pragma once

#include <array>
#include <cstddef>
#include <exception>

#include <vob/sta/compiler.h>
#include <vob/sta/memory_resource.h>


#ifndef VOB_STA_BOUNDED_STRING_DEBUG
#ifdef NDEBUG
#define VOB_STA_BOUNDED_STRING_DEBUG 0
#else
#define VOB_STA_BOUNDED_STRING_DEBUG 1
#endif
#endif

namespace vob::sta
{
	struct Throw {};
	constexpr Throw s_throw = {};

	struct SourceStringTooBig
		: std::exception
	{
		SourceStringTooBig(std::size_t const a_source, std::size_t const a_target)
			: m_source{ a_source }
			, m_target{ a_target }
		{}

		std::size_t m_source;
		std::size_t m_target;
	};

	template <
		std::size_t t_maxSize
		, typename CharType = char
	>
	class BoundedString
	{
	public:
		// Constructors
		constexpr BoundedString() noexcept = default;
		explicit constexpr BoundedString(BoundedString const& a_other) noexcept
		{
			setData(a_other);
		};
		template <typename StringType>
		explicit constexpr BoundedString(StringType const& a_string) noexcept
		{
			setData(a_string);
		}
		template <typename StringType>
		explicit constexpr BoundedString(StringType const& a_string, Throw)
		{
			setData(a_string, s_throw);
		}

		// Methods
		constexpr auto data() const noexcept
		{
			return m_data.data();
		}

		constexpr auto size() const noexcept
		{
			return m_size;
		}

		template <typename StringType>
		constexpr void setData(StringType const& a_string) noexcept
		{
			m_size = std::min(a_string.size(), m_data.size());
			sta::copy(a_string.data(), m_size, m_data.data());
		}

		template <typename StringType>
		constexpr void setData(StringType const& a_string, Throw)
		{
			if (a_string.size() > m_data.size())
			{
				throw SourceStringTooBig{ a_string.size(), m_data.size() };
			}
			m_size = a_string.size();
			sta::copy(a_string.data(), m_size, m_data.data());
		}

		// Operators
		constexpr operator std::basic_string_view<CharType>()
		{
			return { data(), size() };
		}

		constexpr auto& operator=(BoundedString const& a_other)
		{
			setData(std::basic_string_view<CharType>{ a_other });
			return *this;
		}

		constexpr auto& operator=(BoundedString&& a_other)
		{
			setData(a_other);
			return *this;
		}

	private:
		// Attributes
		std::array<CharType, t_maxSize> m_data = {};
		std::size_t m_size = 0;
#if VOB_STA_BOUNDED_STRING_DEBUG == 1
		CharType const* const m_debug = &m_data[0];
#endif
	};
}