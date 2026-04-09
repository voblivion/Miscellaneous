#pragma once

#include <iterator>


namespace vob::mistd
{
	template <typename TContainer>
	[[nodiscard]] constexpr auto isize(TContainer const& a_container)
	{
		return static_cast<int32_t>(std::ssize(a_container));
	}
}
