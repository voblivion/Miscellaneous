#pragma once

#include <algorithm>
#include <array>
#include <source_location>
#include <string_view>


namespace vob::mistd
{
	namespace reflection_util
	{
#ifdef _MSC_VER
		template <typename TEnum>
		requires std::is_enum_v<TEnum>
		constexpr auto enum_name()
		{
			std::string_view name = { __FUNCSIG__, sizeof(__FUNCSIG__) };
			name = name.substr(name.find_first_of('<') + sizeof("enum "));
			name = name.substr(0, name.find_last_of('>'));
			return name;
		}

		template <typename TEnum, TEnum t_value>
		requires std::is_enum_v<TEnum>
		constexpr auto enum_value_name()
		{
			std::string_view name = { __FUNCSIG__, sizeof(__FUNCSIG__) };
			name = name.substr(name.find_first_of('<'));
			name = name.substr(7 + enum_name<TEnum>().size());
			name = name.substr(0, name.find_last_of('>'));
			if (name[0] != '(')
			{
				return name;
			}
			return std::string_view{};
		}
#else
#    error enum_name and enum_value_name should be implemented for that platform.
#endif
	}
}
