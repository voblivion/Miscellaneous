#pragma once

#include <utility>

#include <vob/sta/fnv1a.h>
#include <vob/sta/bounded_string.h>


#ifndef VOB_STA_STRING_ID_STORE_STRING
#ifdef NDEBUG
#define VOB_STA_STRING_ID_STORE_STRING 0
#define VOB_STA_STRING_ID_UNKNOWN std::string_view{ "" }
#else
#define VOB_STA_STRING_ID_STORE_STRING 1
#define VOB_STA_STRING_ID_UNKNOWN std::string_view{ "<Unknown>" }
#endif
#endif

namespace vob::sta
{
	class string_id
	{
	public:
		// Constructors
		constexpr string_id(std::uint64_t const a_id)
			: string_id{ a_id, VOB_STA_STRING_ID_UNKNOWN }
		{}

		constexpr explicit string_id(std::string_view a_string)
			: string_id{ fnv1a(a_string), a_string }
		{}

		// Methods
		constexpr auto assign(std::uint64_t const a_id, std::string_view a_string)
		{
			m_id = a_id;
#if VOB_STA_STRING_ID_STORE_STRING
			m_string.assign(a_string);
#endif
		}

		constexpr auto assign(std::string_view a_string)
		{
			m_id = fnv1a(a_string);
#if VOB_STA_STRING_ID_STORE_STRING
			m_string.assign(a_string);
#endif
		}

		constexpr auto id() const
		{
			return m_id;
		}

#if VOB_STA_STRING_ID_STORE_STRING
		auto const& string() const
		{
			return m_string;
		}
#endif

		// Operators
		constexpr operator std::uint64_t() const
		{
			return id();
		}

	private:
		// Attributes
		std::uint64_t m_id;
#if VOB_STA_STRING_ID_STORE_STRING
		bounded_string<64, char> m_string;
#endif

		// Constructors
		template <typename StringType>
		constexpr string_id(std::uint64_t const a_id, StringType const& a_string)
			: m_id{ a_id }
		{
#if VOB_STA_STRING_ID_STORE_STRING
			m_string.assign(a_string);
#endif
		}
	};

	namespace literals
	{
		constexpr string_id operator ""_id(char const* const a_str, std::size_t const a_size)
		{
			return string_id{ std::string_view(a_str, a_size) };
		}
	}
}

namespace std
{
	template <>
	struct hash<vob::sta::string_id>
	{
		std::size_t operator()(vob::sta::string_id const& a_string_id) const
		{
			return a_string_id.id();
		}
	};
}

#define ID(str) CONSTEVAL(#str##_id)
