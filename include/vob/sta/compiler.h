#pragma once

// Sadly MSVC 2019 doesn't always evaluate constant expressions when it could
#define CONSTEVAL(expr) [](){ constexpr auto result = expr; return result; }()

namespace vob::sta
{
	// Until c++2020 is supported, this constexpr copy function is necessary
	template <typename Type>
	constexpr auto copy(Type const* a_in, std::size_t const a_size, Type* a_out)
	{
		for (auto i = 0u; i < a_size; ++i)
		{
			*a_out++ = *a_in++;
		}
		return a_out;
	}

}