#pragma once

#include <vob/sta/error.h>

namespace vob::zlib
{
	struct stream_error final
		: std::runtime_error
	{
		explicit stream_error(char const* a_reason)
			: std::runtime_error{ a_reason }
		{}
	};

}

namespace vob::zlib::detail
{
	inline void expect(bool const a_condition, char const* a_reason)
	{
		sta::expect(a_condition, stream_error{ a_reason });
	}
}
