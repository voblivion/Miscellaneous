#pragma once

#include <array>


namespace vob::img
{
	enum class side
	{
		height
		, width
	};

	template <side>
	struct adam7;

	template <>
	struct adam7<side::height>
	{
		static constexpr std::array<std::uint8_t, 7> offset = { 0, 0, 4, 0, 2, 0, 1 };

		static constexpr std::array<std::uint8_t, 7> span = { 8, 8, 8, 4, 4, 2, 2 };
	};

	template <>
	struct adam7<side::width>
	{
		static constexpr std::array<std::uint8_t, 7> offset = { 0, 4, 0, 2, 0, 1, 0 };

		static constexpr std::array<std::uint8_t, 7> span = { 8, 8, 4, 4, 2, 2, 1 };
	};

	template <side Side>
	constexpr auto adam7_size(std::uint8_t const a_step, std::size_t const a_size)
	{
		auto const span = adam7<side>::span[a_step];
		auto const start = adam7<side>::offset[a_step];
		return a_size / span + (a_size % span > start ? 1 : 0);
	}

	constexpr auto adam7_index(
		std::size_t const a_width
		, std::uint8_t const a_step
		, std::size_t const a_index
	)
	{
		auto const step_width = adam7_size<side::width>(a_step, a_width);
		auto const height_offset = adam7<side::height>::offset[a_step];
		auto const height_span = adam7<side::height>::span[a_step];
		auto const line = height_offset + (a_index / step_width) * height_span;
		auto const width_offset = adam7<side::width>::offset[a_step];
		auto const width_span = adam7<side::width>::span[a_step];
		auto const column = width_offset + (a_index % step_width) * width_span;
		return line * a_width + column;
	}
}