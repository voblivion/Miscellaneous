#pragma once

#include <vob/sta/fnv1a.h>
#include <vob/sta/bounded_string.h>


#ifndef VOB_STA_STRING_ID_STORE_STRING
#ifdef NDEBUG
#define VOB_STA_STRING_ID_STORE_STRING 0
#else
#define VOB_STA_STRING_ID_STORE_STRING 1
#endif
#endif

#if VOB_STA_STRING_ID_STORE_STRING
#ifndef VOB_STA_STRING_ID_UNKNOWN
#define VOB_STA_STRING_ID_UNKNOWN "<Unknown>"
#endif
#endif

namespace vob::sta
{
	using namespace std;

	template <size_t MaxStringSize>
	class string_id
	{
	public:
		// Constructors
		constexpr string_id(uint64_t const a_id)
#if VOB_STA_STRING_ID_STORE_STRING
			: string_id{ a_id, VOB_STA_STRING_ID_UNKNOWN }
#else
			: m_id{ a_id }
#endif
		{}

		template <typename StringType>
		constexpr string_id(StringType const& a_string)
			: string_id{ fnv1a(a_string), a_string }
		{}

		// Methods
#if VOB_STA_STRING_ID_STORE_STRING
		auto const& string() const
		{
			return m_string;
		}
#endif

		// Operators
		constexpr operator std::uint64_t() const
		{
			return m_id;
		}

	private:
		// Attributes
		std::uint64_t const m_id;
#if VOB_STA_STRING_ID_STORE_STRING
		bounded_string<MaxStringSize> m_string;
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
		constexpr string_id<16> operator ""_id(
			char const* const a_str
			, std::size_t const a_size
		)
		{
			return { std::string_view(a_str, a_size) };
		}
	}
}

#define ID(str) CONSTEVAL(#str##_id)
