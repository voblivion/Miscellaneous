#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <vector>

#include <vob/sta/algorithm.h>
#include <vob/img/adam7.h>
#include <vob/sta/vector_streambuf.h>
#include <vob/sta/stream_reader.h>
#include <vob/zlib/zlib.h>

#include <vob/img/image.h>
#include <vob/img/error.h>
#include "vob/sta/enum.h"

namespace vob::img::png
{
	inline bool can_load(std::istream& a_input_stream)
	{
		std::array<char, 8> header{};
		auto const pos = a_input_stream.tellg();
		a_input_stream.get(&header[0], header.size());
		a_input_stream.seekg(pos);

		return header[0] == '\x89' && header[1] == 'P' && header[2] == 'N'
			&& header[3] == 'G' && header[4] == '\r' && header[5] == '\n'
			&& header[6] == '\x1a' && header[7] == '\n';
	}
}

namespace vob::img::png::detail
{
	enum class color_type : std::uint8_t
	{
		gray_scale = 0
		, rgb = 2
		, palette = 3
		, gray_scale_alpha = 4
		, rgb_alpha = 6
	};
}

VOB_STA_DECLARE_ENUM_REFLECTED_UNSIGNED(vob::img::png::detail::color_type, 7);

namespace vob::img::png::detail
{
	using color_type_traits = sta::enum_traits<color_type>;
	constexpr auto color_type_count = color_type_traits::values.size();

	constexpr std::array<std::uint8_t, color_type_count> channel_count = {
		1, 3, 0, 2, 4
	};

	constexpr bool is_valid_channel_depth(std::uint8_t const a_channel_depth)
	{
		constexpr std::array<std::uint8_t, 5> valid_channel_depths = { 1, 2, 4, 8, 16 };

		return sta::fix_c20_any_of(
			valid_channel_depths.begin()
			, valid_channel_depths.end()
			, [&a_channel_depth](auto const a_valid_cd) { return a_valid_cd == a_channel_depth; }
		);
	}

	constexpr bool is_valid_color_type_channel_depth_pair(
		color_type const a_color_type
		, std::uint8_t const a_valid_channel_depth
	)
	{
		assert(sta::enum_value_is_valid(a_color_type));
		assert(is_valid_channel_depth(a_valid_channel_depth));
		if (a_color_type == color_type::palette)
		{
			return a_valid_channel_depth <= 8;
		}
		if(a_color_type == color_type::gray_scale)
		{
			return true;
		}
		return a_valid_channel_depth >= 8;
	}

	struct chunk_header
	{
		std::uint32_t length;
		std::uint32_t type;
	};

	inline chunk_header read_chunk_header(std::istream& a_input_stream)
	{
		chunk_header header{};
		img::detail::expect(
			sta::raw_read(a_input_stream, header.length)
			, "not enough bytes to read png chunk length"
		);
		header.length = sta::endian(header.length);

		img::detail::expect(
			sta::raw_read(a_input_stream, header.type)
			, "not enough bytes to read png chunk type"
		);
		header.type = sta::endian(header.type);

		return header;
	}

	inline std::uint32_t peek_chunk_type(std::istream& a_input_stream)
	{
		auto const pos = a_input_stream.tellg();
		auto const png_chunk_header = read_chunk_header(a_input_stream);
		a_input_stream.seekg(pos);
		return png_chunk_header.type;
	}

	struct chunk_footer
	{
		std::uint32_t crc;
	};

	inline chunk_footer read_chunk_footer(std::istream& a_input_stream)
	{
		chunk_footer footer{};
		img::detail::expect(
			sta::raw_read(a_input_stream, footer.crc)
			, "not enough bytes to read png chunk crc"
		);
		footer.crc = sta::endian(footer.crc);

		return footer;
	}

	inline void ignore_chunk(std::istream& a_input_stream)
	{
		auto const header = read_chunk_header(a_input_stream);

		a_input_stream.ignore(header.length);

		read_chunk_footer(a_input_stream);
	}

	struct header_chunk
	{
		std::uint32_t width;
		std::uint32_t height;
		std::uint8_t channel_depth;
		std::uint8_t color_type;
		std::uint8_t compression_method;
		std::uint8_t filter_method;
		std::uint8_t interlace_method;
	};

	inline header_chunk read_header_chunk(std::istream& a_input_stream)
	{
		read_header_chunk(a_input_stream);

		header_chunk const chunk{};
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.width)
			, "not enough bytes to read header chunk width"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.height)
			, "not enough bytes to read header chunk height"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.channel_depth)
			, "not enough bytes to read header chunk channel depth"
		);
		img::detail::expect(
			is_valid_channel_depth(chunk.channel_depth)
			, "invalid png channel depth"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.color_type)
			, "not enough bytes to read header chunk color type"
		);
		img::detail::expect(
			sta::enum_integer_is_valid<color_type>(chunk.color_type)
			, "invalid png color type"
		);
		img::detail::expect(
			is_valid_color_type_channel_depth_pair(
				static_cast<color_type>(chunk.color_type)
				, chunk.channel_depth
			)
			, "incompatible png channel depth and color type"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.compression_method)
			, "not enough bytes to read header chunk compression method"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.filter_method)
			, "not enough bytes to read header chunk filter method"
		);
		img::detail::expect(
			sta::raw_read(a_input_stream, chunk.interlace_method)
			, "not enough bytes to read header chunk interlace method"
		);

		read_chunk_footer(a_input_stream);
		// TODO : validate crc

		return chunk;
	}

	inline void read_data_chunk(
		std::istream& a_input_stream
		, std::ostream& a_output_stream
	)
	{
		read_chunk_header(a_input_stream);

		auto const zlib_header = zlib::peek_header(a_input_stream);
		img::detail::expect(zlib_header.dict == 0, "zlib data for png can't use dictionary");
		img::detail::expect(zlib_header.method == 8, "zlib compression method for png must be 8");

		zlib::inflate(a_input_stream, a_output_stream);

		read_chunk_footer(a_input_stream);
		// TODO : validate crc
	}

	enum class filter_type
	{
		none
		, sub
		, up
		, average
		, paeth
	};

	template <filter_type FilterType, typename AllocatorT>
	struct unfilter_function;

	template <typename AllocatorT>
	struct unfilter_function<filter_type::none, AllocatorT>
	{
		std::uint8_t operator()(
			std::vector<std::uint8_t, AllocatorT> const& a_data
			, std::size_t const a_width
			, std::size_t const a_data_size
			, std::size_t const a_line
			, std::size_t const a_column
			, std::size_t const a_data_offset
		) const
		{
			return 0;
		}
	};

	template <typename AllocatorT>
	struct unfilter_function<filter_type::sub, AllocatorT>
	{
		std::uint8_t operator()(
			std::vector<std::uint8_t, AllocatorT> const& a_data
			, std::size_t const a_width
			, std::size_t const a_data_size
			, std::size_t const a_line
			, std::size_t const a_column
			, std::size_t const a_data_offset
		) const
		{
			if (a_column == 0)
			{
				return 0;
			}
			auto const pos = (a_width * a_line + (a_column - 1)) * a_data_size
				+ a_data_offset;
			return a_data[pos];
		}
	};

	template <typename AllocatorT>
	struct unfilter_function<filter_type::up, AllocatorT>
	{
		std::uint8_t operator()(
			std::vector<std::uint8_t, AllocatorT> const& a_data
			, std::size_t const a_width
			, std::size_t const a_data_size
			, std::size_t const a_line
			, std::size_t const a_column
			, std::size_t const a_data_offset
		) const
		{
			if (a_line == 0)
			{
				return 0;
			}
			auto const pos = (a_width * (a_line - 1) + a_column) * a_data_size
				+ a_data_offset;
			return a_data[pos];
		}
	};

	template <typename AllocatorT>
	struct unfilter_function<filter_type::average, AllocatorT>
	{
		std::uint8_t operator()(
			std::vector<std::uint8_t, AllocatorT> const& a_data
			, std::size_t const a_width
			, std::size_t const a_data_size
			, std::size_t const a_line
			, std::size_t const a_column
			, std::size_t const a_data_offset
		) const
		{
			auto const sub = unfilter_function<filter_type::sub, AllocatorT>{}(
				a_data, a_width, a_data_size, a_line, a_column, a_data_offset
			);
			auto const up = unfilter_function<filter_type::up, AllocatorT>{}(
				a_data, a_width, a_data_size, a_line, a_column, a_data_offset
			);

			return (std::uint16_t{ sub } +up) / 2;
		}
	};

	template <typename AllocatorType>
	struct unfilter_function<filter_type::paeth, AllocatorType>
	{
		std::uint8_t operator()(
			std::vector<std::uint8_t, AllocatorType> const& a_data
			, std::size_t const a_width
			, std::size_t const a_dataSize
			, std::size_t const a_line
			, std::size_t const a_column
			, std::size_t const a_dataOffset
		) const
		{
			auto const sub = unfilter_function<filter_type::sub, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
			);
			auto const up = unfilter_function<filter_type::up, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
			);
			auto sub_up = std::uint8_t{ 0 };
			if (a_line > 0 && a_column > 0)
			{
				auto const pos = (a_width * (a_line - 1) + (a_column - 1))
					* a_dataSize + a_dataOffset;
				sub_up = a_data[pos];
			}
			auto const p = std::int16_t{ sub } + up - sub_up;
			auto const p_sub = std::abs(p - sub);
			auto const p_up = std::abs(p - up);
			auto const p_sub_up = std::abs(p - sub_up);
			if (p_sub <= p_up && p_sub <= p_sub_up)
			{
				return sub;
			}
			else if (p_up <= p_sub_up)
			{
				return up;
			}
			else
			{
				return sub_up;
			}
		}
	};

	template <
		filter_type FilterType
		, typename IteratorT
		, typename AllocatorT
	>
	void unfilter_line(
		IteratorT& a_in
		, std::vector<std::uint8_t, AllocatorT>& a_data
		, std::size_t const a_width
		, std::size_t const a_data_size
		, std::size_t const a_line
	)
	{
		using unfilter_function = unfilter_function<FilterType, AllocatorT>;
		for (auto j = 0u; j < a_width; ++j)
		{
			for (auto b = 0u; b < a_data_size; ++b)
			{
				a_data[(a_line * a_width + j) * a_data_size + b] = (*a_in++) + unfilter_function{}(
					a_data
					, a_width
					, a_data_size
					, a_line
					, j
					, b
				);
			}
		}
	}

	template <
		typename IteratorT
		, typename AllocatorT
	>
	void unfilter_line(
		IteratorT& a_in
		, std::vector<std::uint8_t, AllocatorT>& a_data
		, std::size_t const a_width
		, std::size_t const a_data_size
		, std::size_t const a_line
	)
	{
		switch (static_cast<filter_type>(*a_in++))
		{
		case filter_type::sub:
		{
			unfilter_line<filter_type::sub>(a_in, a_data, a_width, a_data_size, a_line);
			break;
		}
		case filter_type::up:
		{
			unfilter_line<filter_type::up>(a_in, a_data, a_width, a_data_size, a_line);
			break;
		}
		case filter_type::average:
		{
			unfilter_line<filter_type::average>(a_in, a_data, a_width, a_data_size, a_line);
			break;
		}
		case filter_type::paeth:
		{
			unfilter_line<filter_type::paeth>(a_in, a_data, a_width, a_data_size, a_line);
			break;
		}
		case filter_type::none:
		{
			unfilter_line<filter_type::none>(a_in, a_data, a_width, a_data_size, a_line);
			break;
		}
		default:
			throw stream_error{ "invalid png filter type" };
		}
	}

	template <typename IteratorT, typename AllocatorT>
	auto unfilter(
		IteratorT& a_in
		, std::size_t const a_height
		, std::size_t const a_width
		, std::uint8_t const a_data_size
		, AllocatorT const& a_allocator
	)
	{
		using allocator_traits = std::allocator_traits<AllocatorT>;
		using uint8_allocator = typename allocator_traits::template rebind_alloc<std::uint8_t>;
		std::vector<std::uint8_t, AllocatorT> r_data{ AllocatorT{ a_allocator} };
		r_data.resize(a_height * a_width * a_data_size);
		for (auto i = 0u; i < a_height; ++i)
		{
			unfilter_line(a_in, r_data, a_width, a_data_size, i);
		}
		return r_data;
	}

	inline auto compute_bytes_per_pixel(header_chunk const& a_header_chunk)
	{
		auto const bits_per_pixel =
			a_header_chunk.channel_depth * channel_count[a_header_chunk.color_type];
		return (bits_per_pixel + 7u) / 8u;
	}

	inline auto compute_raw_data_size(header_chunk const& a_header_chunk)
	{
		auto const bytes_per_pixel = compute_bytes_per_pixel(a_header_chunk);

		// No interlace
		if (a_header_chunk.interlace_method == 0)
		{
			return std::size_t{ a_header_chunk.height }
				*a_header_chunk.width
				* bytes_per_pixel
				+ a_header_chunk.height;
		}
		// Adam7 interlace
		std::size_t size = 0u;
		for (auto a = 0; a < 7; ++a)
		{
			auto const height = adam7_size<side::height>(a, a_header_chunk.height);
			auto const width = adam7_size<side::width>(a, a_header_chunk.width);
			size += height * (width * bytes_per_pixel + 1);
		}
		return size;
	}

	inline void read_data_chunks(
		std::istream& a_input_stream
		, std::ostream& a_output_stream
	)
	{
		auto next_chunk_type = peek_chunk_type(a_input_stream);
		while (next_chunk_type == 'IDAT')
		{
			read_data_chunk(a_input_stream, a_output_stream);
			next_chunk_type = peek_chunk_type(a_input_stream);
		}
	}

	template <typename Allocator>
	inline auto read_data(
		header_chunk const& a_header_chunk
		, std::istream& a_input_stream
		, Allocator a_allocator
	)
	{
		using allocator_traits = std::allocator_traits<Allocator>;
		using uint8_allocator = typename allocator_traits::template rebind_alloc<std::uint8_t>;
		auto const allocator = uint8_allocator{ a_allocator };

		auto const raw_data_size = compute_raw_data_size(a_header_chunk);
		sta::vector_streambuf<char, uint8_allocator> raw_data_stream_buffer{ raw_data_size };
		std::ostream output_stream{ &raw_data_stream_buffer };
		read_data_chunks(a_input_stream, output_stream);

		auto const bytes_per_pixel = compute_bytes_per_pixel(a_header_chunk);
		auto raw_data = raw_data_stream_buffer.collect(0);
		auto begin = raw_data.begin();

		// No interlace
		if (a_header_chunk.interlace_method == 0)
		{
			return unfilter(
				begin, a_header_chunk.height, a_header_chunk.width, bytes_per_pixel, a_allocator
			);
		}
		// Adam7 interlace
		auto const data_size = a_header_chunk.height * a_header_chunk.width * bytes_per_pixel;
		std::vector<std::uint8_t, uint8_allocator> r_data(data_size, allocator);
		for (auto a = 0; a < 7; ++a)
		{
			auto const height = adam7_size<side::height>(a, a_header_chunk.height);
			auto const width = adam7_size<side::width>(a, a_header_chunk.width);
			if (height == 0 || width == 0)
			{
				continue;
			}

			auto const sub_data = unfilter(begin, height, width, bytes_per_pixel, a_allocator);

			for (auto index = 0u; index < sub_data.size(); ++index)
			{
				auto const real_index = adam7_index(
					a_header_chunk.width, a, index / bytes_per_pixel
				) * bytes_per_pixel + index % bytes_per_pixel;
				r_data[real_index] = sub_data[index];
			}
		}

		return r_data;
	}

	template <
		img::color_type OutColorType, typename OutChannelT
		, color_type InColorType, std::uint8_t InChannelDepth
	>
	struct to_pixel;

	template <>
	struct to_pixel<img::color_type::rgb, std::uint8_t, color_type::rgb, 8>
	{
		template <typename IteratorT>
		auto operator()(IteratorT a_firstByte)
		{
			return pixel<img::color_type::rgb, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2)
			};
		}
	};

	template <>
	struct to_pixel<img::color_type::rgb_alpha, std::uint8_t, color_type::rgb, 8>
	{
		template <typename IteratorT>
		auto operator()(IteratorT a_firstByte)
		{
			return pixel<img::color_type::rgb, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2), 1
			};
		}
	};

	template <>
	struct to_pixel<img::color_type::rgb, std::uint8_t, color_type::rgb_alpha, 8>
	{
		template <typename IteratorT>
		auto operator()(IteratorT a_firstByte)
		{
			return pixel<img::color_type::rgb, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2)
			};
		}
	};

	template <>
	struct to_pixel<img::color_type::rgb_alpha, std::uint8_t, color_type::rgb_alpha, 8>
	{
		template <typename IteratorT>
		auto operator()(IteratorT a_firstByte)
		{
			return pixel<img::color_type::rgb_alpha, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2), *(a_firstByte + 3)
			};
		}
	};

	template <
		img::color_type OutColorType
		, typename OutChannelT
		, color_type InColorType
		, std::uint8_t InChannelDepth
		, typename ImageAllocatorT
		, typename DataAllocatorT
	>
	auto to_image(
		header_chunk const& a_header_chunk
		, std::vector<std::uint8_t, DataAllocatorT> const& a_data
		, ImageAllocatorT const& a_image_allocator
	)
	{
		using pixel = pixel<OutColorType, OutChannelT>;
		using image_allocator_traits = std::allocator_traits<ImageAllocatorT>;
		using pixel_allocator_type = typename image_allocator_traits::template rebind_alloc<pixel>;
		image<OutColorType, OutChannelT, pixel_allocator_type> r_image{
			a_header_chunk.height
			, a_header_chunk.width
			, pixel_allocator_type{ a_image_allocator }
		};

		auto const bytes_per_pixel = compute_bytes_per_pixel(a_header_chunk);
		auto current_in_it = a_data.begin();
		for (auto i = 0u; i < a_header_chunk.height; ++i)
		{
			for (auto j = 0u; j < a_header_chunk.width; ++j)
			{
				using to_pixel = to_pixel<OutColorType, OutChannelT, InColorType, InChannelDepth>;
				r_image.at(i, j) = to_pixel{}(current_in_it);
				current_in_it += bytes_per_pixel;
			}
		}

		return r_image;
	}

	template <
		img::color_type OutColorType
		, typename OutChannelT
		, color_type InColorType
		, typename ImageAllocatorT
		, typename DataAllocatorT
	>
	auto to_image(
		header_chunk const& a_header_chunk
		, std::vector<std::uint8_t, DataAllocatorT> const& a_data
		, ImageAllocatorT const& a_image_allocator
	)
	{
		switch (a_header_chunk.channel_depth)
		{
		case 8:
		{
			return to_image<OutColorType, OutChannelT, InColorType, 8, ImageAllocatorT>(
				a_header_chunk, a_data, a_image_allocator
			);
		}
		default:
			throw sta::not_implemented{};
		}
	}

	template <
		img::color_type OutColorType
		, typename OutChannelT
		, typename ImageAllocatorT
		, typename DataAllocatorT
	>
	auto to_image(
		header_chunk const& a_header_chunk
		, std::vector<std::uint8_t, DataAllocatorT> const& a_data
		, ImageAllocatorT const& a_image_allocator
	)
	{
		switch (static_cast<color_type>(a_header_chunk.color_type))
		{
		case color_type::rgb:
		{
			return to_image<OutColorType, OutChannelT, color_type::rgb, ImageAllocatorT>(
				a_header_chunk, a_data, a_image_allocator
			);
		}
		case color_type::rgb_alpha:
		{
			return to_image<OutColorType, OutChannelT, color_type::rgb_alpha, ImageAllocatorT>(
				a_header_chunk, a_data, a_image_allocator
			);
		}
		default:
			throw sta::not_implemented{};
		}
	}

	inline void ignore_header(std::istream& a_inputStream)
	{
		img::detail::expect(can_load(a_inputStream), "not enough bytes to read png header");
		a_inputStream.ignore(8);
	}
	
	template <
		img::color_type ColorType
		, typename ChannelT
		, typename ImageAllocatorT = std::pmr::polymorphic_allocator<ChannelT>
		, typename StackAllocatorT = ImageAllocatorT
	>
	auto do_read(
		std::istream& a_input_stream
		, ImageAllocatorT a_image_allocator = {}
		, StackAllocatorT a_stack_allocator = a_image_allocator
	)
	{
		ignore_header(a_input_stream);

		// IHDR
		img::detail::expect(
			peek_chunk_type(a_input_stream) == 'IHDR', "png must start with IHDR chunk"
		);
		auto const header = read_header_chunk(a_input_stream);
		sta::expect<sta::not_implemented>(header.color_type == 2 || header.color_type == 6);
		sta::expect<sta::not_implemented>(header.channel_depth == 8 || header.channel_depth == 16);

		// ANY*
		auto chunk_type = peek_chunk_type(a_input_stream);
		while (chunk_type != 'IDAT')
		{
			ignore_chunk(a_input_stream);
			chunk_type = peek_chunk_type(a_input_stream);
		}

		// IDAT+
		auto const data = read_data(header, a_input_stream, a_stack_allocator);
		auto r_image = to_image<ColorType, ChannelT>(header, data, a_image_allocator);

		// ANY*
		chunk_type = peek_chunk_type(a_input_stream);
		while (chunk_type != 'IEND')
		{
			ignore_chunk(a_input_stream);
			chunk_type = peek_chunk_type(a_input_stream);
		}

		// IEND
		ignore_chunk(a_input_stream);

		return r_image;
	}
}

namespace vob::img::png
{
	template <
		img::color_type ColorType
		, typename ChannelT
		, typename ImageAllocatorT = std::pmr::polymorphic_allocator<ChannelT>
		, typename StackAllocatorT = ImageAllocatorT
	>
	auto read(
		std::istream& a_inputStream
		, ImageAllocatorT a_image_allocator = {}
		, StackAllocatorT a_stack_allocator = a_image_allocator
	)
	{
		return detail::do_read<ColorType, ChannelT>(
			a_inputStream, a_image_allocator, a_stack_allocator
		);
	}
}
