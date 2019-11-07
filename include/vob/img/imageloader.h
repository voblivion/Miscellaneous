#pragma once

#include <vob/sta/allocator.h>

#include <vob/img/image.h>
#include <vob/img/png.h>

namespace vob::img
{
	struct UnsupportedImageFormat
		: std::exception
	{};

	template <
		ColorType t_colorType = ColorType::RGB
		, typename ChannelType = std::uint8_t
		, typename ImageAllocatorType =
			std::pmr::polymorphic_allocator<Pixel<t_colorType, ChannelType>>
		, typename StackAllocatorType =
			sta::ReboundAlloc<ImageAllocatorType, std::uint8_t>
	>
	auto load(
		std::istream& a_inputStream
		, ImageAllocatorType const& a_imageAllocator = {}
		, StackAllocatorType const& a_stackAllocator = {}
	)
	{
		if (png::canLoad(a_inputStream))
		{
			return png::read<t_colorType, ChannelType>(
				a_inputStream
				, a_imageAllocator
				, a_stackAllocator
			);
		}
		throw UnsupportedImageFormat{};
	}
}