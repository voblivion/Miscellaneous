#pragma once

#include "fnv1a.h"

#include <string_view>

#ifndef VOB_MISHS_STRING_ID_DEBUG
#ifndef NDEBUG
#define VOB_MISHS_STRING_ID_DEBUG 1
#else
#define VOB_MISHS_STRING_ID_DEBUG 0
#endif
#endif

#if VOB_MISHS_STRING_ID_DEBUG
#include <optional>
#include "../std/bounded_string.h"
#endif


namespace vob::mishs
{
	/// @brief An identifier that can be constructed from an integer or a compile/run-time string view.
	/// Set VOB_MISHS_STRING_ID_DEBUG to 1 to store a bounded representation of the string used to initialize it.
	template <typename TChar, typename TCharTraits>
	class basic_string_id
	{
	public:
#pragma region CREATORS
		/// @brief Constructs a basic_string_id from a string_view by computing its fnv1a hash.
		constexpr explicit basic_string_id(std::basic_string_view<TChar, TCharTraits> a_string = {})
			: m_id{ fnv1a(a_string) }
#if VOB_MISHS_STRING_ID_DEBUG
			, m_debug{ a_string }
#endif
		{}

		/// @brief Constructs a basic_string_id setting its internal representation directly from provided id.
		constexpr explicit basic_string_id(std::uint64_t const a_id)
			: m_id{ a_id }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Provides the std::uint64_t representation of the basic_string_id.
		constexpr auto get_id() const
		{
			return m_id;
		}

		/// @brief Casts a basic_string_id to an std::uint64_t based on its stored internal representation.
		constexpr explicit operator std::uint64_t() const
		{
			return get_id();
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Assigns the internal representation of a basic_string_id directly from provided id.
		constexpr void assign(std::uint64_t const a_id)
		{
			m_id = a_id;
#if VOB_MISHS_STRING_ID_DEBUG
			m_debug = std::nullopt;
#endif
		}

		/// @brief Update the value of a basic_string_id from a string_view by computing its fnv1a hash.
		constexpr void assign(std::basic_string_view<TChar, TCharTraits> a_string)
		{
			m_id = fnv1a(a_string);
#if VOB_MISHS_STRING_ID_DEBUG
			m_debug = std::basic_string<TChar, TCharTraits>{ a_string };
#endif
		}

#pragma endregion

	private:
#pragma region PRIVATE_DATA
#if VOB_MISHS_STRING_ID_DEBUG
		std::optional<mistd::basic_bounded_string<TChar, TCharTraits, 64>> m_debug;
#endif
		std::uint64_t m_id;
#pragma endregion
	};

	template <typename TChar, typename TCharTraits>
	constexpr auto operator<=>(
		basic_string_id<TChar, TCharTraits> const& a_lhs, basic_string_id<TChar, TCharTraits> const& a_rhs)
	{
		return a_lhs.get_id() <=> a_rhs.get_id();
	}

	template <typename TChar, typename TCharTraits>
	constexpr auto operator==(
		basic_string_id<TChar, TCharTraits> const& a_lhs, basic_string_id<TChar, TCharTraits> const& a_rhs)
	{
		return a_lhs.get_id() == a_rhs.get_id();
	}

	/// @brief An identifier that can be constructed from an integer or a compile/run-time string view.
	/// Set VOB_MISHS_STRING_ID_DEBUG to 1 to store a bounded representation of the string used to initialize it.
	using string_id = basic_string_id<char, std::char_traits<char>>;

	template <typename TChar, typename TCharTraits>
	struct basic_string_id_hash
	{
		std::size_t operator()(basic_string_id<TChar, TCharTraits> const& a_stringId) const
		{
			return a_stringId.get_id();
		}
	};

	using string_id_hash = basic_string_id_hash<char, std::char_traits<char>>;
}