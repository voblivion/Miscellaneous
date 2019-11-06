#pragma once

#include <exception>


namespace vob::sta
{
	template <typename ExceptionType>
	void expect(bool const condition, ExceptionType const exception = {})
	{
		if (!condition)
		{
			throw exception;
		}
	}
}