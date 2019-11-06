#pragma once

#include <array>


namespace vob::img
{
	enum class Side
	{
		Height
		, Width
	};

	template <Side>
	struct Adam;

	template <>
	struct Adam<Side::Height>
	{
		static constexpr std::array<std::uint8_t, 7> s_offset = {
			0, 0, 4, 0
			, 2, 0, 1
		};

		static constexpr std::array<std::uint8_t, 7> s_span = {
			8, 8, 8, 4
			, 4, 2, 2
		};
	};

	template <>
	struct Adam<Side::Width>
	{
		static constexpr std::array<std::uint8_t, 7> s_offset = {
			0, 4, 0, 2
			, 0, 1, 0
		};

		static constexpr std::array<std::uint8_t, 7> s_span = {
			8, 8, 4, 4
			, 2, 2, 1
		};
	};

	template <Side side>
	auto adamSize(std::uint8_t const a_step, std::size_t const a_size)
	{
		auto const span = Adam<side>::s_span[a_step];
		auto const start = Adam<side>::s_offset[a_step];
		return a_size / span + (a_size % span > start ? 1 : 0);
	}

	inline auto adamIndex(
		std::size_t const a_width
		, std::uint8_t const a_step
		, std::size_t const a_index
	)
	{
		auto const stepWidth = adamSize<Side::Width>(a_step, a_width);
		auto const heightOffset = Adam<Side::Height>::s_offset[a_step];
		auto const heightSpan = Adam<Side::Height>::s_span[a_step];
		auto const line = heightOffset + (a_index / stepWidth) * heightSpan;
		auto const widthOffset = Adam<Side::Width>::s_offset[a_step];
		auto const widthSpan = Adam<Side::Width>::s_span[a_step];
		auto const column = widthOffset + (a_index % stepWidth) * widthSpan;
		return line * a_width + column;
	}
}