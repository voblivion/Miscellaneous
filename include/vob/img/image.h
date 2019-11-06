#pragma once

#include <cinttypes>
#include <vector>
#include <optional>

namespace vob::img
{
	enum class ColorType
	{
		GreyScale = 1
		, GreyScaleAlpha = 2
		, RGB = 3
		, RGBA = 4
	};

	template <ColorType t_colorType>
	constexpr std::uint8_t s_channelCount =
		static_cast<std::underlying_type_t<ColorType>>(t_colorType);

	template <ColorType t_colorType, typename ChannelType>
	struct Pixel;

	template <typename ChannelType>
	struct Pixel<ColorType::GreyScale, ChannelType>
	{
		ChannelType i;
	};

	template <typename ChannelType>
	struct Pixel<ColorType::GreyScaleAlpha, ChannelType>
	{
		ChannelType i;
		ChannelType a;
	};

	template <typename ChannelType>
	struct Pixel<ColorType::RGB, ChannelType>
	{
		ChannelType r;
		ChannelType g;
		ChannelType b;
	};

	template <typename ChannelType>
	struct Pixel<ColorType::RGBA, ChannelType>
	{
		ChannelType r;
		ChannelType g;
		ChannelType b;
		ChannelType a;
	};

	template <
		ColorType t_colorType
		, typename ChannelType
		, typename AllocatorType = std::pmr::polymorphic_allocator<ChannelType>
	>
	struct Image
	{
		// Aliases
		using PixelType = Pixel<t_colorType, ChannelType>;

		// Constructors
		Image(
			std::size_t const a_height
			, std::size_t const a_width
			, AllocatorType const& a_allocator
		)
			: m_width{ a_width }
			, m_height{ a_height }
			, m_pixels{ a_allocator }
		{
			m_pixels.resize(a_height * a_width);
		}

		auto& at(std::size_t const a_line, std::size_t const a_column) const
		{
			return m_pixels[a_line * m_width + a_column];
		}

		auto& at(std::size_t const a_line, std::size_t const a_column)
		{
			return m_pixels[a_line * m_width + a_column];
		}

	private:
		// Attributes
		std::size_t m_width;
		std::size_t m_height;
		std::vector<PixelType, AllocatorType> m_pixels;
	};

}