#pragma once

#include <exception>


namespace vob::sta
{
	template <
		typename ExceptionT
		, typename = std::enable_if_t<std::is_base_of_v<std::exception, ExceptionT>>
	>
	void expect(bool const a_condition, ExceptionT const a_exception = {})
	{
		if (!a_condition)
		{
			throw a_exception;
		}
	}

	struct not_implemented final
		: std::exception
	{};
}