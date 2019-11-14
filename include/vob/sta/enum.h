#pragma once
#include <array>
#include <string_view>
#include <algorithm>
#include <optional>

#include <vob/sta/algorithm.h>
#include <vob/sta/reflection.h>
#include <vob/sta/string_view.h>
#include <vob/sta/utility.h>

#ifndef VOB_STA_ENUM_REFLECTED_RANGE_MIN
#define VOB_STA_ENUM_REFLECTED_RANGE_MIN -128
#endif
#ifndef VOB_STA_ENUM_REFLECTED_RANGE_MAX
#define VOB_STA_ENUM_REFLECTED_RANGE_MAX 128
#endif

namespace vob::sta
{
	template <typename EnumT>
	struct enum_reflected_range
	{
		using type = integer_range<
			underlying_type_t<EnumT>
			, VOB_STA_ENUM_REFLECTED_RANGE_MIN
			, VOB_STA_ENUM_REFLECTED_RANGE_MAX
		>;
	};

	template <typename EnumT>
	using enum_reflected_range_t = typename enum_reflected_range<EnumT>::type;
}

#define VOB_STA_DECLARE_ENUM_REFLECTED_SIGNED(enum_type, reflected_min, reflected_max) \
	namespace vob::sta \
	{ \
		template <> \
		struct enum_reflected_range<enum_type> \
		{ \
			using type = integer_range< \
				underlying_type_t<enum_type> \
				, static_cast<underlying_type_t<enum_type>>(reflected_min) \
				, static_cast<underlying_type_t<enum_type>>(reflected_max) \
			>; \
		}; \
	}

#define VOB_STA_DECLARE_ENUM_REFLECTED_UNSIGNED(enum_type, reflected_count) \
	VOB_STA_DECLARE_ENUM_REFLECTED_SIGNED(enum_type, 0, reflected_count)

namespace vob::sta::detail
{
	template <typename EnumT>
	constexpr auto get_enum_name() noexcept ->enable_if_t<is_enum_v<EnumT>, string_view>
	{
		return template_type_parameters<EnumT>()[0];
	}

	template <typename EnumT>
	constexpr auto enum_name = get_enum_name<EnumT>();

	template <typename EnumT, EnumT Value>
	constexpr auto get_enum_value_name() noexcept -> enable_if_t<is_enum_v<EnumT>, string_view>
	{
		auto name = template_parameters<EnumT, Value>()[0];
		if (name.empty() || name[0] == '0')
		{
			return "";
		}
		auto last_separator = fix_find_last_of(name, ':');
		if (last_separator != string_view::npos)
		{
			return name.substr(last_separator + 1);
		}
		return name;
	}

	template <typename EnumT, EnumT Value>
	constexpr auto enum_value_name = get_enum_value_name<EnumT, Value>();

	template <typename EnumT, EnumT Value>
	constexpr auto enum_value_has_name = !enum_value_name<EnumT, Value>.empty();

	template <typename EnumT>
	struct enum_integer_is_valid_predicate
	{
		template <underlying_type_t<EnumT> Integer>
		struct type
		{
			static constexpr auto value = enum_value_has_name<EnumT, static_cast<EnumT>(Integer)>;
		};
	};

	template <typename EnumT, underlying_type_t<EnumT>... ReflectedIntegers>
	constexpr auto get_enum_valid_integers(
		integer_sequence<underlying_type_t<EnumT>, ReflectedIntegers...>
	)
	{
		using integer_type = underlying_type_t<EnumT>;
		return filter_integer_sequence<
			integer_type
			, enum_integer_is_valid_predicate<EnumT>::type
		>(integer_sequence<underlying_type_t<EnumT>, ReflectedIntegers...>{});
	}

	template <typename EnumT, typename ReflectedRangeT>
	constexpr auto enum_valid_integers =
		get_enum_valid_integers<EnumT>(to_integer_sequence(ReflectedRangeT{}));

	template <typename EnumT, underlying_type_t<EnumT>... ValidIntegers>
	constexpr auto get_enum_values(integer_sequence<underlying_type_t<EnumT>, ValidIntegers...>)
	{
		return std::array<EnumT, sizeof...(ValidIntegers)>{{
			static_cast<EnumT>(ValidIntegers)...
		}};
	}

	template <typename EnumT, typename ReflectedRangeT>
	constexpr auto enum_values =
		get_enum_values<EnumT>(enum_valid_integers<EnumT, ReflectedRangeT>);

	template <typename EnumT, underlying_type_t<EnumT>... ValidIntegers>
	constexpr auto get_enum_value_names(integer_sequence<underlying_type_t<EnumT>, ValidIntegers...>)
	{
		return std::array<string_view, sizeof...(ValidIntegers)>{{
			enum_value_name<EnumT, static_cast<EnumT>(ValidIntegers)>...
		}};
	}

	template <typename EnumT, typename ReflectedRangeT>
	constexpr auto enum_value_names =
		get_enum_value_names<EnumT>(enum_valid_integers<EnumT, ReflectedRangeT>);

	template <typename EnumT, underlying_type_t<EnumT>... ValidIntegers>
	constexpr auto get_enum_value_name_pairs(integer_sequence<underlying_type_t<EnumT>, ValidIntegers...>)
	{
		return std::array<pair<EnumT, string_view>, sizeof...(ValidIntegers)>{{
			{
				static_cast<EnumT>(ValidIntegers)
				, enum_value_name<EnumT, static_cast<EnumT>(ValidIntegers)>
			}...
		}};
	}

	template <typename EnumT, typename ReflectedRangeT>
	constexpr auto enum_value_name_pairs =
		get_enum_value_name_pairs<EnumT>(enum_valid_integers<EnumT, ReflectedRangeT>);
}

namespace vob::sta
{
	template <typename EnumT>
	constexpr auto enum_name = detail::enum_name<EnumT>;

	template <typename EnumT, EnumT Value>
	constexpr auto enum_value_name = detail::enum_value_name<EnumT, Value>;

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_valid_integers = detail::enum_valid_integers<EnumT, ReflectedRangeT>;

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_values = detail::enum_values<EnumT, ReflectedRangeT>;

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_value_names = detail::enum_value_names<EnumT, ReflectedRangeT>;

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_value_name_pairs = detail::enum_value_name_pairs<EnumT, ReflectedRangeT>;

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_cast(string_view const a_str) noexcept
		-> optional<EnumT>
	{
		constexpr auto pairs = enum_value_name_pairs<EnumT, ReflectedRangeT>;
		auto it = find_if(
			pairs.begin()
			, pairs.end()
			, [&a_str](auto pair) { return pair.second.compare(a_str) == 0; }
		);
		if (it != pairs.end())
		{
			return it->first;
		}
		return {};
	}

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_cast(EnumT const a_value) noexcept
		-> optional<string_view>
	{
		constexpr auto pairs = enum_value_name_pairs<EnumT, ReflectedRangeT>;
		auto it = find_if(
			pairs.begin()
			, pairs.end()
			, [&a_value](auto pair) { return pair.first == a_value; }
		);
		if (it != pairs.end())
		{
			return it->second;
		}
		return {};
	}

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_integer_is_valid(std::underlying_type_t<EnumT> const a_integer)
	{
		return fix_c20_any_of(
			enum_valid_integers<EnumT, ReflectedRangeT>.begin()
			, enum_valid_integers<EnumT, ReflectedRangeT>.end()
			, [a_integer](auto const a_valid_integer) { return a_integer == a_valid_integer; }
		);
	}

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_value_is_valid(EnumT const a_value)
	{
		return enum_integer_is_valid<EnumT, ReflectedRangeT>(
			static_cast<std::underlying_type_t<EnumT>>(a_value)
		);
	}

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	constexpr auto enum_value_name_is_valid(string_view const a_str)
	{
		return enum_cast<EnumT, ReflectedRangeT>(a_str).has_value();
	}

	template <typename EnumT, typename ReflectedRangeT = enum_reflected_range_t<EnumT>>
	struct enum_traits
	{
		using enum_type = EnumT;
		using reflected_range_type = ReflectedRangeT;

		static constexpr auto name = enum_name<enum_type>;
		template <enum_type Value>
		static constexpr auto value_name = enum_value_name<enum_type, Value>;
		static constexpr auto valid_integers = enum_valid_integers<enum_type, reflected_range_type>;
		static constexpr auto values = enum_values<enum_type, reflected_range_type>;
		static constexpr auto value_names = enum_value_names<enum_type, reflected_range_type>;
		static constexpr auto value_name_pairs = enum_value_name_pairs<enum_type, reflected_range_type>;

		static constexpr auto cast(string_view const a_str)
		{
			return enum_cast<enum_type, reflected_range_type>(a_str);
		}

		static constexpr auto cast(enum_type const a_value)
		{
			return enum_cast<enum_type, reflected_range_type>(a_value);
		}
	};
}