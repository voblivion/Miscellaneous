#pragma once

#include "integer_sequence_util.h"
#include "reflection_util.h"

#include <algorithm>
#include <optional>
#include <string_view>

#ifndef VOB_MISTD_REFLECTED_ENUM_RANGE_MIN
#define VOB_MISTD_REFLECTED_ENUM_RANGE_MIN -128
#endif
#ifndef VOB_MISTD_REFLECTED_ENUM_RANGE_MAX
#define VOB_MISTD_REFLECTED_ENUM_RANGE_MAX +128
#endif


namespace vob::mistd
{
	template <typename TEnum>
	struct reflected_enum_range
	{
		static constexpr auto begin = TEnum{ VOB_MISTD_REFLECTED_ENUM_RANGE_MIN };
		static constexpr auto end = TEnum{ VOB_MISTD_REFLECTED_ENUM_RANGE_MAX };
	};

	namespace detail
	{
		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_reflected_sequence = integer_sequence_util::make_range<
			std::underlying_type_t<TEnum>,
			std::underlying_type_t<TEnum>{ TReflectedRange::begin },
			std::underlying_type_t<TEnum>{ TReflectedRange::end }>();

		template <typename TEnum>
		struct enum_traits_is_valid
		{
			template <std::underlying_type_t<TEnum> t_integer>
			struct type
			{
				static constexpr auto value =
					!reflection_util::enum_value_name<TEnum, TEnum{ t_integer }>().empty();
			};
		};

		template <typename TEnum, std::underlying_type_t<TEnum>... t_indexes>
		constexpr auto enum_traits_get_valid_indexes(
			std::integer_sequence<std::underlying_type_t<TEnum>, t_indexes...> a_sequence)
		{
			using integer_type = std::underlying_type_t<TEnum>;
			return integer_sequence_util::filter<integer_type, enum_traits_is_valid<TEnum>::template type>(a_sequence);
		}

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_valid_indexes = enum_traits_get_valid_indexes<TEnum>(
			enum_traits_reflected_sequence<TEnum, TReflectedRange>);

		template <typename TEnum, std::underlying_type_t<TEnum>... t_indexes>
		constexpr auto enum_traits_get_values(
			std::integer_sequence<std::underlying_type_t<TEnum>, t_indexes...> a_sequence)
		{
			return std::array<TEnum, sizeof...(t_indexes)>{{ TEnum{ t_indexes }... }};
		}

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_valid_values = enum_traits_get_values<TEnum>(
			enum_traits_valid_indexes<TEnum, TReflectedRange>);

		template <typename TEnum, std::underlying_type_t<TEnum>... t_indexes>
		constexpr auto enum_traits_get_value_names(
			std::integer_sequence<std::underlying_type_t<TEnum>, t_indexes...> a_sequence)
		{
			return std::array<std::string_view, sizeof...(t_indexes)>{{
				reflection_util::enum_value_name<TEnum, TEnum{ t_indexes }>()...
			}};
		}

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_valid_value_names = enum_traits_get_value_names<TEnum>(
			enum_traits_valid_indexes<TEnum, TReflectedRange>);

		template <typename TEnum, std::underlying_type_t<TEnum>... t_indexes>
		constexpr auto enum_traits_get_value_name_pairs(
			std::integer_sequence<std::underlying_type_t<TEnum>, t_indexes...> a_sequence)
		{
			return std::array<std::pair<TEnum, std::string_view>, sizeof...(t_indexes)>{{
				{
					TEnum{ t_indexes },
					reflection_util::enum_value_name<TEnum, TEnum{ t_indexes }>()
				}...
			}};
		}

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_valid_value_name_pairs = enum_traits_get_value_name_pairs<TEnum>(
			enum_traits_valid_indexes<TEnum, TReflectedRange>);

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_cast(TEnum const a_value) noexcept -> std::optional<std::string_view>
		{
			constexpr auto pairs = enum_traits_valid_value_name_pairs<TEnum, TReflectedRange>;
			auto it = std::find_if(
				pairs.begin(), pairs.end(), [&a_value](auto pair) { return pair.first == a_value; });
			if (it != pairs.end())
			{
				return it->second;
			}
			return {};
		}

		template <typename TEnum, typename TReflectedRange>
		constexpr auto enum_traits_cast(std::string_view const a_str) noexcept -> std::optional<TEnum>
		{
			constexpr auto pairs = enum_traits_valid_value_name_pairs<TEnum, TReflectedRange>;
			auto it = std::find_if(
				pairs.begin(), pairs.end(), [&a_str](auto pair) { return pair.second.compare(a_str) == 0; });
			if (it != pairs.end())
			{
				return it->first;
			}
			return {};
		}
	}

	template <typename TEnum, typename TReflectedRange = reflected_enum_range<TEnum>>
	struct enum_traits
	{
#pragma region TYPES
		using reflected_range = TReflectedRange;
#pragma endregion

#pragma region CLASS_DATA
		static constexpr auto name = reflection_util::enum_name<TEnum>();
		template <TEnum t_value>
		static constexpr auto value_name = reflection_util::enum_value_name<TEnum>(t_value);

		static constexpr auto valie_indexes = detail::enum_traits_valid_indexes<TEnum, TReflectedRange>;
		static constexpr auto valid_values = detail::enum_traits_valid_values<TEnum, TReflectedRange>;
		static constexpr auto valid_value_names = detail::enum_traits_valid_value_names<TEnum, TReflectedRange>;
		static constexpr auto valid_value_name_pairs =
			detail::enum_traits_valid_value_name_pairs<TEnum, TReflectedRange>;
#pragma endregion

#pragma region CLASS_METHODS
		static constexpr auto cast(std::string_view const a_str)
		{
			return detail::enum_traits_cast<TEnum, TReflectedRange>(a_str);
		}

		static constexpr auto cast(TEnum const a_value) -> std::optional<std::string_view>
		{
			return detail::enum_traits_cast<TEnum, TReflectedRange>(a_value);
		}
#pragma endregion
	};
}