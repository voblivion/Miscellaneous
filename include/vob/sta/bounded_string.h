#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <vob/sta/algorithm.h>

#ifndef VOB_STD_BOUNDED_STRING_DEBUG
#ifdef NDEBUG
#define VOB_STD_BOUNDED_STRING_DEBUG 0
#else
#define VOB_STD_BOUNDED_STRING_DEBUG 1
#endif
#endif

namespace vob::sta
{
	struct source_string_too_big final
		: exception
	{
		source_string_too_big(std::size_t const a_source, std::size_t const a_target)
			: source{ a_source }
			, target{ a_target }
		{}

		std::size_t source;
		std::size_t target;
	};

	template <size_t MaxSize, typename CharT = char>
	class bounded_string
	{
	public:
#pragma region Statics & Aliases
		using char_type = CharT;
#pragma endregion
#pragma region Constructors
		constexpr bounded_string() noexcept = default;
		explicit constexpr bounded_string(bounded_string const& a_other) noexcept
		{
			assign(a_other);
		};
		explicit constexpr bounded_string(bounded_string&& a_other) noexcept
		{
			assign(a_other);
		}
		template <typename StringT>
		explicit constexpr bounded_string(StringT const& a_string) noexcept
		{
			assign(a_string);
		}

		// Destructor
		~bounded_string() = default;
#pragma endregion
#pragma region Methods
		constexpr auto data() const noexcept
		{
			return m_data.data();
		}
		constexpr auto size() const noexcept
		{
			return m_size;
		}

		template <typename StringT>
		constexpr void assign(StringT const& a_string) noexcept
		{
			m_size = std::min(a_string.size(), m_data.size());
			fix_c20_copy(a_string.data(), m_size, m_data.data());
		}
#pragma endregion
#pragma region Operators
		explicit constexpr operator std::basic_string_view<CharT>()
		{
			return { data(), size() };
		}

		constexpr auto& operator=(bounded_string const& a_other) noexcept
		{
			assign(std::basic_string_view<CharT>{ a_other });
			return *this;
		}

		constexpr auto& operator=(bounded_string&& a_other) noexcept
		{
			assign(a_other);
			return *this;
		}
#pragma endregion
	private:
#pragma region Attributes
		std::array<CharT, MaxSize> m_data = {};
		std::size_t m_size = 0;
#if VOB_STD_BOUNDED_STRING_DEBUG == 1
		CharT const* const m_debug = &m_data[0];
#endif
#pragma endregion
	};
}