#pragma once

#include <array>
#include <cassert>
#include <vector>

#include <vob/sta/allocator.h>
#include <vob/sta/adam7.h>
#include <vob/sta/expect.h>
#include <vob/sta/streambuffer.h>
#include <vob/sta/streamreader.h>
#include <vob/sta/zlib.h>

#include <vob/img/image.h>

namespace vob::img::png
{
	inline bool canLoad(std::istream& a_inputStream)
	{
		std::array<char, 8> header{};
		a_inputStream.read(&header[0], header.size());
		a_inputStream.seekg(-8, std::ios_base::cur);
		return header[0] == '\x89' && header[1] == 'P' && header[2] == 'N'
			&& header[3] == 'G' && header[4] == '\r' && header[5] == '\n'
			&& header[6] == '\x1a' && header[7] == '\n';
	}

	struct InvalidPngStream final
		: std::exception
	{};

	struct MustStartByIhdrChunk final
		: std::exception
	{};

	struct InvalidColorType final
		: std::exception
	{
		explicit InvalidColorType(std::uint8_t const a_colorType)
			: m_colorType{ a_colorType }
		{}

		std::uint8_t m_colorType;
	};

	struct InvalidChannelDepth final
		: std::exception
	{
		explicit InvalidChannelDepth(std::uint8_t const a_channelDepth)
			: m_channelDepth{ a_channelDepth }
		{}

		std::uint8_t m_channelDepth;
	};

	struct IncompatibleColorTypeAndChannelDepth final
		: std::exception
	{
		explicit IncompatibleColorTypeAndChannelDepth(
			std::uint8_t const a_channelDepth
			, std::uint8_t const a_colorType
		)
			: m_channelDepth{ a_channelDepth }
			, m_colorType{ a_colorType }
		{}

		std::uint8_t m_channelDepth;
		std::uint8_t m_colorType;
	};

	struct ZlibDictNotAllowed final
		: std::exception
	{};

	struct InvalidZlibCompressionMethod final
		: std::exception
	{};
}

namespace vob::img::png::detail
{
	enum class ColorType : std::uint8_t
	{
		GrayScale = 0
		, RGB = 2
		, Palette = 3
		, GrayScaleAlpha = 4
		, RGBA = 6
	};

	constexpr static std::array<std::uint8_t, 7> s_channelCount = {
		1, 0, 3, 0, 2, 0, 4
	};

	struct ChunkHeader
	{
		std::uint32_t m_length;
		std::uint32_t m_type;
	};

	inline ChunkHeader readChunkHeader(std::istream& a_inputStream)
	{
		return {
			sta::endian(sta::read<std::uint32_t>(a_inputStream))
			, sta::endian(sta::read<std::uint32_t>(a_inputStream))
		};
	}

	inline std::uint32_t peekChunkType(std::istream& a_inputStream)
	{
		auto const chunkHeader = readChunkHeader(a_inputStream);
		a_inputStream.seekg(-8, std::ios_base::cur);
		return chunkHeader.m_type;
	}

	struct ChunkFooter
	{
		std::uint32_t m_crc;
	};

	inline ChunkFooter readChunkFooter(std::istream& a_inputStream)
	{
		return { sta::endian(sta::read<std::uint32_t>(a_inputStream)) };
	}

	inline void ignoreChunk(std::istream& a_inputStream)
	{
		auto const chunkHeader = readChunkHeader(a_inputStream);

		a_inputStream.ignore(chunkHeader.m_length);

		readChunkFooter(a_inputStream);
	}

	struct Header
	{
		std::uint32_t m_width;
		std::uint32_t m_height;
		std::uint8_t m_channelDepth;
		std::uint8_t m_colorType;
		std::uint8_t m_compressionMethod;
		std::uint8_t m_filterMethod;
		std::uint8_t m_interlaceMethod;
	};

	inline Header readHeaderChunk(std::istream& a_inputStream)
	{
		readChunkHeader(a_inputStream);

		Header const r_header{
			sta::endian(sta::read<std::uint32_t>(a_inputStream))
			, sta::endian(sta::read<std::uint32_t>(a_inputStream))
			, sta::endian(sta::read<std::uint8_t>(a_inputStream))
			, sta::endian(sta::read<std::uint8_t>(a_inputStream))
			, sta::endian(sta::read<std::uint8_t>(a_inputStream))
			, sta::endian(sta::read<std::uint8_t>(a_inputStream))
			, sta::endian(sta::read<std::uint8_t>(a_inputStream))
		};

		sta::expect(
			r_header.m_channelDepth == 1
			|| r_header.m_channelDepth == 2
			|| r_header.m_channelDepth == 4
			|| r_header.m_channelDepth == 8
			|| r_header.m_channelDepth == 16
			, InvalidChannelDepth{ r_header.m_channelDepth }
		);

		sta::expect(
			r_header.m_colorType < 7
			&& r_header.m_colorType != 1
			&& r_header.m_colorType != 5
			, InvalidColorType{ r_header.m_colorType }
		);

		sta::expect(
			(r_header.m_channelDepth < 8
				&& (r_header.m_colorType == 0 || r_header.m_colorType == 3))
			|| r_header.m_channelDepth == 8
			|| r_header.m_colorType != 3
			, IncompatibleColorTypeAndChannelDepth{
				r_header.m_channelDepth, r_header.m_colorType
			}
		);

		readChunkFooter(a_inputStream);

		return r_header;
	}

	/*struct PaletteColor
	{
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
	};

	struct Palette
	{
		std::vector<PaletteColor> m_colors;
	};

	inline Palette readPalette(std::istream& a_inputStream)
	{
		auto const chunkHeader = readChunkHeader(a_inputStream);

		Palette r_palette;
		r_palette.m_colors.resize(chunkHeader.m_length / 3);
		a_inputStream.read(
			reinterpret_cast<char*>(&r_palette.m_colors[0])
			, chunkHeader.m_length
		);

		auto const chunkFooter = readChunkFooter(a_inputStream);

		return r_palette;
	}*/

	inline void readDataChunk(
		std::istream& a_inputStream
		, std::basic_ostream<std::uint8_t>& a_outputStream
	)
	{
		readChunkHeader(a_inputStream);

		auto const zlibHeader = sta::peekZlibHeader(a_inputStream);
		if (zlibHeader.m_dict != 0)
		{
			throw ZlibDictNotAllowed{};
		}
		if (zlibHeader.m_method != 8)
		{
			throw InvalidZlibCompressionMethod{};
		}
		sta::readZlib(a_inputStream, a_outputStream);

		readChunkFooter(a_inputStream);

	}

	enum class FilterType
	{
		None
		, Sub
		, Up
		, Average
		, Paeth
	};

	template <FilterType t_filterType, typename AllocatorType>
	struct UnfilterFunction;

	template <typename AllocatorType>
	struct UnfilterFunction<FilterType::None, AllocatorType>
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
			return 0;
		}
	};

	template <typename AllocatorType>
	struct UnfilterFunction<FilterType::Sub, AllocatorType>
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
			if (a_column == 0)
			{
				return 0;
			}
			auto const pos = (a_width * a_line + (a_column - 1)) * a_dataSize
				+ a_dataOffset;
			return a_data[pos];
		}
	};

	template <typename AllocatorType>
	struct UnfilterFunction<FilterType::Up, AllocatorType>
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
			if (a_line == 0)
			{
				return 0;
			}
			auto const pos = (a_width * (a_line - 1) + a_column) * a_dataSize
				+ a_dataOffset;
			return a_data[pos];
		}
	};

	template <typename AllocatorType>
	struct UnfilterFunction<FilterType::Average, AllocatorType>
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
			auto const sub = UnfilterFunction<FilterType::Sub, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
				);
			auto const up = UnfilterFunction<FilterType::Up, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
				);

			return (std::uint16_t{ sub } +up) / 2;
		}
	};

	template <typename AllocatorType>
	struct UnfilterFunction<FilterType::Paeth, AllocatorType>
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
			auto const sub = UnfilterFunction<FilterType::Sub, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
				);
			auto const up = UnfilterFunction<FilterType::Up, AllocatorType>{}(
				a_data, a_width, a_dataSize, a_line, a_column, a_dataOffset
				);
			auto subUp = std::uint8_t{ 0 };
			if (a_line > 0 && a_column > 0)
			{
				auto const pos = (a_width * (a_line - 1) + (a_column - 1))
					* a_dataSize + a_dataOffset;
				subUp = a_data[pos];
			}
			auto const p = std::int16_t{ sub } +up - subUp;
			auto const pSub = std::abs(p - sub);
			auto const pUp = std::abs(p - up);
			auto const pSubUp = std::abs(p - subUp);
			if (pSub <= pUp && pSub <= pSubUp)
			{
				return sub;
			}
			else if (pUp <= pSubUp)
			{
				return up;
			}
			else
			{
				return subUp;
			}
		}
	};

	template <
		FilterType t_filterType
		, typename IteratorType
		, typename AllocatorType
	>
	void unfilterLine(
		IteratorType& a_in
		, std::vector<std::uint8_t, AllocatorType>& a_data
		, std::size_t const a_width
		, std::size_t const a_dataSize
		, std::size_t const a_line
	)
	{
		using UnfilterFunction = UnfilterFunction<t_filterType, AllocatorType>;
		for (auto j = 0u; j < a_width; ++j)
		{
			for (auto b = 0u; b < a_dataSize; ++b)
			{
				a_data[(a_line * a_width + j) * a_dataSize + b] =
					(*a_in++) + UnfilterFunction{}(
						a_data
						, a_width
						, a_dataSize
						, a_line
						, j
						, b
					);
			}
		}
	}

	template <
		typename IteratorType
		, typename AllocatorType
	>
	void unfilterLine(
		IteratorType& a_in
		, std::vector<std::uint8_t, AllocatorType>& a_data
		, std::size_t const a_width
		, std::size_t const a_dataSize
		, std::size_t const a_line
	)
	{
		auto const filterType = static_cast<FilterType>(*a_in++);
		switch (filterType)
		{
		case FilterType::Sub:
		{
			unfilterLine<FilterType::Sub>(
				a_in
				, a_data
				, a_width
				, a_dataSize
				, a_line
			);
			break;
		}
		case FilterType::Up:
		{
			unfilterLine<FilterType::Up>(
				a_in
				, a_data
				, a_width
				, a_dataSize
				, a_line
			);
			break;
		}
		case FilterType::Average:
		{
			unfilterLine<FilterType::Average>(
				a_in
				, a_data
				, a_width
				, a_dataSize
				, a_line
			);
			break;
		}
		case FilterType::Paeth:
		{
			unfilterLine<FilterType::Paeth>(
				a_in
				, a_data
				, a_width
				, a_dataSize
				, a_line
			);
			break;
		}
		default:
		{
			unfilterLine<FilterType::None>(
				a_in
				, a_data
				, a_width
				, a_dataSize
				, a_line
			);
			break;
		}
		}
	}

	template <typename IteratorType, typename AllocatorType>
	auto unfilter(
		IteratorType& a_in
		, std::size_t const a_height
		, std::size_t const a_width
		, std::uint8_t const a_dataSize
		, AllocatorType const& a_allocator
	)
	{
		std::vector<std::uint8_t, AllocatorType> r_data{ a_allocator };
		r_data.resize(a_height * a_width * a_dataSize);
		for (auto i = 0u; i < a_height; ++i)
		{
			unfilterLine(a_in, r_data, a_width, a_dataSize, i);
		}
		return r_data;
	}

	inline auto computeBytesPerPixel(Header const& a_header)
	{
		auto const bitsPerPixel = a_header.m_channelDepth
			* s_channelCount[a_header.m_colorType];
		return (bitsPerPixel + 7u) / 8u;
	}

	inline auto computeRawDataSize(Header const& a_header)
	{
		auto const bytesPerPixel = computeBytesPerPixel(a_header);

		// No interlace
		if (a_header.m_interlaceMethod == 0)
		{
			return std::size_t{ a_header.m_height }
				* a_header.m_width
				* bytesPerPixel
				+ a_header.m_height;
		}
		// Adam7 interlace
		std::size_t size = 0u;
		for (auto a = 0; a < 7; ++a)
		{
			auto const height = adamSize<Side::Height>(a, a_header.m_height);
			auto const width = adamSize<Side::Width>(a, a_header.m_width);
			size += height * (width * bytesPerPixel + 1);
		}
		return size;
	}

	inline void readDataChunks(
		std::istream& a_inputStream
		, std::basic_ostream<std::uint8_t>& a_outputStream
	)
	{
		auto nextChunkType = peekChunkType(a_inputStream);
		while (nextChunkType == 'IDAT')
		{
			readDataChunk(a_inputStream, a_outputStream);
			nextChunkType = peekChunkType(a_inputStream);
		}
	}

	template <typename AllocatorType>
	inline auto readData(
		Header const& a_header
		, std::istream& a_inputStream
		, AllocatorType a_allocator
	)
	{
		using Uint8Allocator = typename sta::ReboundAlloc<AllocatorType, std::uint8_t>;
		auto const allocator = Uint8Allocator{ a_allocator };

		auto const rawDataSize = computeRawDataSize(a_header);
		std::vector<std::uint8_t, Uint8Allocator> rawData(rawDataSize, allocator);
		auto rawDataStreamBuffer = sta::makeVectorStreamBuffer(rawData);
		std::basic_ostream<std::uint8_t> outputStream{ &rawDataStreamBuffer };
		readDataChunks(a_inputStream, outputStream);

		auto const bytesPerPixel = computeBytesPerPixel(a_header);
		auto begin = rawData.begin();

		// No interlace
		if (a_header.m_interlaceMethod == 0)
		{
			return unfilter(
				begin
				, a_header.m_height
				, a_header.m_width
				, bytesPerPixel
				, a_allocator
			);
		}
		// Adam7 interlace
		auto const dataSize = a_header.m_height * a_header.m_width * bytesPerPixel;
		std::vector<std::uint8_t, Uint8Allocator> r_data(dataSize, allocator);
		for (auto a = 0; a < 7; ++a)
		{
			auto const height = adamSize<Side::Height>(a, a_header.m_height);
			auto const width = adamSize<Side::Width>(a, a_header.m_width);
			if (height == 0 || width == 0)
			{
				continue;
			}

			auto const subData = unfilter(
				begin
				, height
				, width
				, bytesPerPixel
				, a_allocator
			);

			for (auto index = 0u; index < subData.size(); ++index)
			{
				auto const realIndex = adamIndex(
					a_header.m_width, a, index / bytesPerPixel
				) * bytesPerPixel + index % bytesPerPixel;
				r_data[realIndex] = subData[index];
			}
		}

		return r_data;
	}

	template <
		img::ColorType t_outColorType, typename OutChannelType
		, ColorType t_inColorType, std::uint8_t t_inChannelDepth
	>
	struct ToPixel;

	template <>
	struct ToPixel<img::ColorType::RGB, std::uint8_t, ColorType::RGB, 8>
	{
		template <typename IteratorType>
		auto operator()(IteratorType a_firstByte)
		{
			return Pixel<img::ColorType::RGB, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2)
			};
		}
	};

	template <>
	struct ToPixel<img::ColorType::RGBA, std::uint8_t, ColorType::RGB, 8>
	{
		template <typename IteratorType>
		auto operator()(IteratorType a_firstByte)
		{
			return Pixel<img::ColorType::RGB, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2), 1
			};
		}
	};

	template <>
	struct ToPixel<img::ColorType::RGB, std::uint8_t, ColorType::RGBA, 8>
	{
		template <typename IteratorType>
		auto operator()(IteratorType a_firstByte)
		{
			return Pixel<img::ColorType::RGB, std::uint8_t>{
				*a_firstByte, *(a_firstByte + 1), *(a_firstByte + 2)
			};
		}
	};

	template <>
	struct ToPixel<img::ColorType::RGBA, std::uint8_t, ColorType::RGBA, 8>
	{
		template <typename IteratorType>
		auto operator()(IteratorType a_firstByte)
		{
			return Pixel<img::ColorType::RGB, std::uint8_t>{
				*a_firstByte
				, *(a_firstByte + 1)
				, *(a_firstByte + 2)
				, *(a_firstByte + 3)
			};
		}
	};

	template <
		img::ColorType t_outColorType
		, typename OutChannelType
		, ColorType t_inColorType
		, std::uint8_t t_inChannelDepth
		, typename ImageAllocatorType
		, typename DataAllocatorType
	>
	auto toImage(
		Header const& a_header
		, std::vector<std::uint8_t, DataAllocatorType> const& a_data
		, ImageAllocatorType a_imageAllocator
	)
	{
		Image<t_outColorType, OutChannelType, ImageAllocatorType> r_image{
			a_header.m_height
			, a_header.m_width
			, a_imageAllocator
		};

		auto const bytesPerPixel = computeBytesPerPixel(a_header);
		auto currentInIt = a_data.begin();
		for (auto i = 0u; i < a_header.m_height; ++i)
		{
			for (auto j = 0u; j < a_header.m_width; ++j)
			{
				r_image.at(i, j) = ToPixel<
					t_outColorType
					, OutChannelType
					, t_inColorType
					, t_inChannelDepth
				>{}(currentInIt);
				currentInIt += bytesPerPixel;
			}
		}

		return r_image;
	}

	template <
		img::ColorType t_outColorType
		, typename OutChannelType
		, ColorType t_inColorType
		, typename ImageAllocatorType
		, typename DataAllocatorType
	>
	auto toImage(
		Header const& a_header
		, std::vector<std::uint8_t, DataAllocatorType> const& a_data
		, ImageAllocatorType const& a_imageAllocator
	)
	{
		switch (a_header.m_channelDepth)
		{
		case 8:
		{
			return toImage<
				t_outColorType
				, OutChannelType
				, t_inColorType
				, 8
				, ImageAllocatorType
			>(
				a_header
				, a_data
				, a_imageAllocator
			);
		}
		default:
			assert(false && "Channel depth not supported");
			return Image<t_outColorType, OutChannelType, ImageAllocatorType>{
				0, 0
				, a_imageAllocator
			};
		}
	}

	template <
		img::ColorType t_outColorType
		, typename OutChannelType
		, typename ImageAllocatorType
		, typename DataAllocatorType
	>
	auto toImage(
		Header const& a_header
		, std::vector<std::uint8_t, DataAllocatorType> const& a_data
		, ImageAllocatorType const& a_imageAllocator
	)
	{
		switch (static_cast<ColorType>(a_header.m_colorType))
		{
		case ColorType::RGB:
		{
			return toImage<
				t_outColorType
				, OutChannelType
				, ColorType::RGB
				, ImageAllocatorType
			>(
				a_header
				, a_data
				, a_imageAllocator
			);
		}
		case ColorType::RGBA:
		{
			return toImage<
				t_outColorType
				, OutChannelType
				, ColorType::RGBA
				, ImageAllocatorType
			>(
				a_header
				, a_data
				, a_imageAllocator
			);
		}
		default:
			assert(false && "Color type not supported");
			return Image<t_outColorType, OutChannelType, ImageAllocatorType>{
				0, 0
				, a_imageAllocator
			};
		}
	}

	inline void ignoreHeader(std::istream& a_inputStream)
	{
		sta::expect<InvalidPngStream>(canLoad(a_inputStream));
		a_inputStream.ignore(8);
	}
	
	template <
		img::ColorType t_colorType
		, typename ChannelType
		, typename ImageAllocatorType = std::pmr::polymorphic_allocator<ChannelType>
		, typename StackAllocatorType = ImageAllocatorType
	>
	auto readPng(
		std::istream& a_inputStream
		, ImageAllocatorType a_imageAllocator = {}
		, StackAllocatorType a_stackAllocator = a_imageAllocator
	)
	{
		ignoreHeader(a_inputStream);

		// IHDR
		sta::expect<MustStartByIhdrChunk>(peekChunkType(a_inputStream) == 'IHDR');
		auto const header = readHeaderChunk(a_inputStream);
		assert(header.m_colorType == 2 || header.m_colorType == 6
			&& "Color type not supported");
		assert(header.m_channelDepth == 8 || header.m_channelDepth == 16
			&& "Color depth less than 8 not supported");

		// ANY*
		auto chunkType = peekChunkType(a_inputStream);
		while (chunkType != 'IDAT')
		{
			ignoreChunk(a_inputStream);
			chunkType = peekChunkType(a_inputStream);
		}

		// IDAT+
		auto const data = readData(header, a_inputStream, a_stackAllocator);
		auto r_image = toImage<t_colorType, ChannelType>(header, data, a_imageAllocator);

		// ANY*
		chunkType = peekChunkType(a_inputStream);
		while (chunkType != 'IEND')
		{
			ignoreChunk(a_inputStream);
			chunkType = peekChunkType(a_inputStream);
		}

		// IEND
		ignoreChunk(a_inputStream);

		return r_image;
	}
}

namespace vob::img::png
{
	template <
		img::ColorType t_colorType
		, typename ChannelType
		, typename ImageAllocatorType = std::pmr::polymorphic_allocator<ChannelType>
		, typename StackAllocatorType = ImageAllocatorType
	>
	auto read(
		std::istream& a_inputStream
		, ImageAllocatorType a_imageAllocator = {}
		, StackAllocatorType a_stackAllocator = a_imageAllocator
	)
	{
		return detail::readPng<t_colorType, ChannelType>(
			a_inputStream
			, a_imageAllocator
			, a_stackAllocator
		);
	}
}
