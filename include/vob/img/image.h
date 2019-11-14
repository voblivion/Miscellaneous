#pragma once

#include <vector>


namespace vob::img
{
	enum class color_type : std::uint8_t
	{
		grey_scale
		, grey_scale_alpha
		, rgb
		, rgb_alpha
	};

	template <color_type ColorType>
	constexpr auto channel_count = static_cast<std::underlying_type_t<color_type>>(ColorType);

	template <color_type ColorType, typename ChannelType>
	struct pixel;

	template <typename ChannelT>
	struct pixel<color_type::grey_scale, ChannelT>
	{
		using channel_type = ChannelT;
		channel_type i;
	};

	template <typename ChannelT>
	struct pixel<color_type::grey_scale_alpha, ChannelT>
	{
		using channel_type = ChannelT;
		channel_type i;
		channel_type a;
	};

	template <typename ChannelT>
	struct pixel<color_type::rgb, ChannelT>
	{
		using channel_type = ChannelT;
		channel_type r;
		channel_type g;
		channel_type b;
	};

	template <typename ChannelT>
	struct pixel<color_type::rgb_alpha, ChannelT>
	{
		using channel_type = ChannelT;
		channel_type r;
		channel_type g;
		channel_type b;
		channel_type a;
	};

	template <
		color_type ColorType
		, typename ChannelT
		, typename AllocatorT = std::pmr::polymorphic_allocator<ChannelT>
	>
	struct image
	{
#pragma region Aliases
		using channel_type = ChannelT;
		static constexpr auto color_type = ColorType;
		using pixel_type = pixel<ColorType, channel_type>;
#pragma endregion
#pragma region Constructors
		image(std::size_t const a_height, std::size_t const a_width, AllocatorT const& a_allocator)
			: m_width{ a_width }
			, m_height{ a_height }
			, m_pixels{ a_allocator }
		{
			m_pixels.resize(a_height * a_width);
		}
#pragma endregion
#pragma region Methods
		auto& at(std::size_t const a_line, std::size_t const a_column) const
		{
			return m_pixels[a_line * m_width + a_column];
		}

		auto& at(std::size_t const a_line, std::size_t const a_column)
		{
			return m_pixels[a_line * m_width + a_column];
		}
#pragma endregion
	private:
#pragma region Attributes
		std::size_t m_width;
		std::size_t m_height;
		std::vector<pixel_type, AllocatorT> m_pixels;
#pragma endregion
	};

}