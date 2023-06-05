#pragma once

#include <vob/_todo_/img/image.h>
#include <vob/_todo_/img/png.h>


namespace vob::img
{
	struct unsupported_format final
		: std::runtime_error
	{};

	template <
		color_type ColorType = color_type::rgb_alpha
		, typename ChannelT = std::uint8_t
		, typename ImageAllocatorT =
			std::pmr::polymorphic_allocator<pixel<ColorType, ChannelT>>
		, typename StackAllocatorT =
			typename std::allocator_traits<ImageAllocatorT>::template rebind_alloc<std::uint8_t>
	>
	auto load(
		std::istream& a_input_stream
		, ImageAllocatorT const& a_image_allocator = {}
		, StackAllocatorT const& a_stack_allocator = {}
	)
	{
		if (png::can_load(a_input_stream))
		{
			return png::read<ColorType, ChannelT>(
				a_input_stream
				, a_image_allocator
				, a_stack_allocator
			);
		}
		throw unsupported_format{};
	}
}