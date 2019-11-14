#pragma once

#include <algorithm>
#include <string_view>

namespace vob::sta
{
	using namespace std;

#if defined(_MSC_VER)
	namespace detail
	{
		constexpr auto last_template_parameter(
			string_view a_params
			, size_t const a_scope_count = 0
			, string_view::size_type const a_pos = string_view::npos
		)
			-> string_view::size_type
		{
			auto const scope_begin = a_params.find_last_of('<', a_pos);
			auto const scope_end = a_params.find_last_of('>', a_pos);

			if (a_scope_count > 0)
			{
				if (scope_end != string_view::npos && scope_end > scope_begin)
				{
					return last_template_parameter(a_params, a_scope_count + 1, scope_end - 1);
				}
				return last_template_parameter(a_params, a_scope_count - 1, scope_begin - 1);
			}
			else
			{
				auto sep = a_params.find_last_of(',', a_pos);
				sep = (sep != string_view::npos && sep > scope_begin) ? sep : scope_end;
				if (scope_end != string_view::npos && scope_end > sep)
				{
					return last_template_parameter(a_params, a_scope_count + 1, scope_end - 1);
				}
				return sep;
			}
		}
	}

	template <typename... Types>
	constexpr auto template_type_parameters()
	{
		constexpr auto n = sizeof...(Types);
		array<string_view, n> params{};
		string_view function_name = { __FUNCSIG__, sizeof(__FUNCSIG__) };
		function_name = function_name.substr(0, function_name.find_last_of('>'));
		for (auto i = 0u; i < n; ++i)
		{
			auto const pos = detail::last_template_parameter(function_name);
			auto const param = function_name.substr(pos + 1);
			auto const sep = param.find_last_of(' ');
			params[n - 1 - i] = sep < string_view::npos
				? param.substr(sep + 1)
				: param;
			function_name = function_name.substr(0, pos);
		}
		return params;
	}

	template <typename Type, Type... Values>
	constexpr auto template_parameters()
	{
		constexpr auto n = sizeof...(Values);
		array<string_view, n> params{};
		string_view function_name = { __FUNCSIG__, sizeof(__FUNCSIG__) };
		function_name = function_name.substr(0, function_name.find_last_of('>'));
		for (auto i = 0u; i < n; ++i)
		{
			auto const pos = detail::last_template_parameter(function_name);
			params[n - 1 - i] = function_name.substr(pos + 1);
			function_name = function_name.substr(0, pos);
		}
		return params;
	}
#endif
}