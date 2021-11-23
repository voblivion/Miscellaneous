#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>


namespace vob::mistd
{
	/// @brief A string-like object whose size is bounded by a compile-time maximum.
	template <typename TChar, typename TCharTrait, std::size_t t_maxSize>
	class basic_bounded_string
	{
	public:
#pragma region TYPES
		using char_type  = TChar;
#pragma endregion

#pragma region CREATORS
		/// @brief Constructs an empty string.
		constexpr basic_bounded_string() noexcept = default;

		/// @brief Constructs a string from a c-style string (null-terminated).
		constexpr basic_bounded_string(TChar const* const a_cstr) noexcept
		{
			assign(std::basic_string_view<TChar, TCharTrait>{ a_cstr });
		}

		/// @brief Constructs a string from a string-like object.
		/// @tparam TString : a string-like type with a size() and data() members.
		/// @param a_string : the string-like object to copy the data from.
		template <typename TString>
		constexpr basic_bounded_string(TString const& a_string) noexcept
		{
			assign(a_string);
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Provides c-style string (null-terminated) representation of the instance.
		/// @return 
		constexpr auto c_str() const noexcept
		{
			return m_data.data();
		}

		/// @brief Provides a pointer to the underlying array serving as element storage.
		/// 
		/// detailed The pointer is such that range [data(); data() + size()] is always a valid range and last pointed
		/// element is always zero.
		constexpr auto data() const noexcept
		{
			return m_data.data();
		}

		/// @brief Provides the number of characters in the string.
		constexpr auto size() const noexcept
		{
			return m_size;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Assigns the value of a string-like object to the internal representation of the instance.
		/// @tparam TString : a string-like type with a size() and data() members.
		/// @param a_string : the string-like object to copy the data from.
		template <typename TString>
		constexpr void assign(TString const& a_string) noexcept
		{
			m_size = std::min(a_string.size(), t_maxSize);
			std::copy(a_string.data(), a_string.data() + m_size, &m_data[0]);
			m_data[m_size] = {};
		}

		/// @brief Explicitly converts the internal representation of the instance to a standard string_view
		explicit constexpr operator std::basic_string_view<TChar>()
		{
			return { c_str(), size() };
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		std::array<TChar, t_maxSize + 1> m_data = {};
		std::size_t m_size = 0;
#pragma endregion
	};

	/// @brief A specialization of basic_bounded_string for char type and traits.
	template <std::size_t t_maxSize>
	using bounded_string = basic_bounded_string<char, std::char_traits<char>, t_maxSize>;
}