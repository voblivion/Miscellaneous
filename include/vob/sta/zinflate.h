#pragma once

#include <algorithm>
#include <array>
#include <exception>
#include <variant>

#include <vob/sta/expect.h>
#include <vob/sta/huffman.h>
#include <vob/sta/stream.h>
#include <vob/sta/zinflate_decoders.h>


namespace vob::sta
{
	struct InflateException
		: std::exception
	{};

	struct InvalidLiteralAndLengthCount final
		: InflateException
	{
		explicit InvalidLiteralAndLengthCount(std::uint32_t const a_count)
			: m_count{ a_count }
		{}

		std::uint32_t m_count;
	};

	struct InvalidDistanceCount final
		: InflateException
	{
		explicit InvalidDistanceCount(std::uint32_t const a_count)
			: m_count{ a_count }
		{}

		std::uint32_t m_count;
	};

	struct InvalidCodeLengthCount final
		: InflateException
	{
		explicit InvalidCodeLengthCount(std::uint32_t const a_count)
			: m_count{ a_count }
		{}

		std::uint32_t m_count;
	};

	struct InvalidLiteralOrLengthLengthValue final
		: InflateException
	{
		explicit InvalidLiteralOrLengthLengthValue(std::uint32_t const a_value)
			: m_value{ a_value }
		{}

		std::uint32_t m_value;
	};

	struct InvalidLiteralOrLengthValue final
		: InflateException
	{
		explicit InvalidLiteralOrLengthValue(std::uint32_t const a_value)
			: m_value{ a_value }
		{}

		std::uint32_t m_value;
	};

	struct InvalidExtraDistanceCode final
		: InflateException
	{
		explicit InvalidExtraDistanceCode(std::uint32_t const a_code)
			: m_code{ a_code }
		{}

		std::uint32_t m_code;
	};

	struct TooBigDistance final
		: InflateException
	{
		explicit TooBigDistance(std::uint32_t const a_distance)
			: m_distance{ a_distance }
		{}

		std::uint32_t m_distance;
	};

	struct InvalidChunkType final
		: InflateException
	{
		explicit InvalidChunkType(std::uint32_t const a_type)
			: m_type{ a_type }
		{}

		std::uint32_t m_type;
	};
}

namespace vob::sta::detail
{
	static constexpr std::array<std::uint8_t, 19> s_bitOrders = {
		16,17,18,0,
		8,7,9,6,
		10,5,11,4,
		12,3,13,2,
		14,1,15
	};

	static constexpr std::array<std::uint16_t, 29> s_lengthCodes = {
		3,4,5,6,7
		,8,9,10,11,13
		,15,17,19,23,27
		,31,35,43,51,59
		,67,83,99,115,131
		,163,195, 227,258
	};

	static constexpr std::array<std::uint8_t, 29> s_lengthExtraBits = {
		0,0,0,0,0
		,0,0,0,1,1
		,1,1,2,2,2
		,2,3,3,3,3
		,4,4,4,4,5
		,5,5,5,0
	};

	static constexpr std::array<std::uint16_t, 30> s_baseDistances = {
		1,    2,    3,    4,    5,    7,    9,    13,
		17,   25,   33,   49,   65,   97,   129,  193,
		257,  385,  513,  769,  1025, 1537, 2049, 3073,
		4097, 6145, 8193, 12289,16385,24577
	};

	static constexpr std::array<std::uint8_t, 30> s_distanceExtraBits = {
		0,0,0,0,1,1,2,2,3,3,
		4,4,5,5,6,6,7,7,8,8,
		9,9,10,10,11,11,12,12,13,13
	};

	template <
		std::uint8_t t_maxCodeBits
		, std::uint32_t t_alphabetSize
		, typename IteratorType
	>
	auto computeHuffmanEntries(
		IteratorType const a_codeLengthBegin
		, IteratorType const a_codeLengthEnd
	)
	{
		using HuffmanEntry = HuffmanEntry<t_maxCodeBits, t_alphabetSize>;
		using HuffmanCode = typename HuffmanEntry::Code;
		using HuffmanCodeBuffer = typename HuffmanCode::Buffer;
		using HuffmanValue = typename HuffmanEntry::Value;

		// Step 1
		std::array<std::uint8_t, 32> codeLengthUseCounts{};
		for (auto it = a_codeLengthBegin; it < a_codeLengthEnd; ++it)
		{
			++codeLengthUseCounts[*it];
		}
		codeLengthUseCounts[0] = 0;

		// Step 2
		HuffmanCodeBuffer baseCode = 0u;
		std::array<HuffmanCodeBuffer, 32> nextCodes{};
		for (auto bits = 1; bits < 32; ++bits)
		{
			baseCode = (baseCode + codeLengthUseCounts[bits - 1]) << 1;
			nextCodes[bits] = baseCode;
		}

		// Step 3
		HuffmanEntryList<t_maxCodeBits, t_alphabetSize> r_huffmanEntries;
		auto const valueCount = static_cast<std::uint32_t>(
			std::distance(a_codeLengthBegin, a_codeLengthEnd)
		);
		assert(valueCount <= t_alphabetSize && "Too many values");
		for (HuffmanValue value = 0u; value < valueCount; ++value)
		{
			auto const codeLength = *(a_codeLengthBegin + value);
			if (codeLength != 0)
			{
				auto const code = nextCodes[codeLength]++;
				r_huffmanEntries[value] = {
					value
					, { code, codeLength }
				};
			}
		}

		return r_huffmanEntries;
	}

	template <std::uint8_t t_maxCodeBits, std::uint32_t t_alphabetSize>
	auto computeHuffmanEntries(
		std::array<std::uint8_t, t_alphabetSize> const& a_codeLengths
	)
	{
		return computeHuffmanEntries<t_maxCodeBits, t_alphabetSize>(
			a_codeLengths.begin()
			, a_codeLengths.end()
		);
	}

	inline auto readInflateDecoderPair(LowFirstBitStreamReader& a_bitInputStream)
	{
		auto const literalAndLengthCount = 257u + a_bitInputStream.get<5>();
		sta::expect(
			literalAndLengthCount < 287
			, InvalidLiteralAndLengthCount{ literalAndLengthCount }
		);
		auto const distanceCount = 1u + a_bitInputStream.get<5>();
		sta::expect(
			distanceCount < 32
			, InvalidDistanceCount{ distanceCount }
		);
		auto const codeLengthCount = 4u + a_bitInputStream.get<4>();
		sta::expect(
			codeLengthCount < 20
			, InvalidCodeLengthCount{ codeLengthCount }
		);

		std::array<std::uint8_t, 19> codeLengthCodeLengths{};
		for (auto i = 0u; i < codeLengthCount; ++i)
		{
			codeLengthCodeLengths[s_bitOrders[i]] = a_bitInputStream.get<3>();
		}
		auto codeLengthReader = makeHuffmanDecoder(
			computeHuffmanEntries<8>(codeLengthCodeLengths)
		);

		std::array<std::uint8_t, 286 + 30> allLengths{};
		std::uint8_t previousLength = 0;
		for (auto i = 0u; i < literalAndLengthCount + distanceCount;)
		{
			auto const value = codeLengthReader.get(a_bitInputStream);
			if (value < 16)
			{
				previousLength = value;
				allLengths[i++] = value;
			}
			else
			{
				std::uint8_t duplicatedLength = 0;
				auto duplicates = 0;
				switch (value)
				{
				case 16:
					duplicatedLength = previousLength;
					duplicates = 3 + a_bitInputStream.get<2>();
					break;
				case 17:
					duplicates = 3 + a_bitInputStream.get<3>();
					break;
				case 18:
					duplicates = 11 + a_bitInputStream.get<7>();
					break;
				default:
					sta::expect(
						false
						, InvalidLiteralOrLengthLengthValue{ value }
					);
				}
				std::fill(
					allLengths.begin() + i
					, allLengths.begin() + i + duplicates
					, duplicatedLength
				);
				i += duplicates;
			}
		}

		return InflateDecoderPair{
			makeHuffmanDecoder(
				computeHuffmanEntries<16, 287>(
					allLengths.begin()
					, allLengths.begin() + literalAndLengthCount
				)
			)
			, makeHuffmanDecoder(
				computeHuffmanEntries<16, 32>(
					allLengths.begin() + literalAndLengthCount
					, allLengths.begin() + literalAndLengthCount + distanceCount
					)
			)
		};
	}

	inline void readChunkNoEncryption(
		LowFirstBitStreamReader& a_bitInputStream
		, WindowBufferStream<32768, std::uint8_t>& a_windowBufferStream
	)
	{
		a_bitInputStream.ignoreCurrentByte();
		assert(false && "Not encrypted inflate not supported");
	}

	inline void readChunkEncryption(
		LowFirstBitStreamReader& a_bitInputStream
		, WindowBufferStream<32768, std::uint8_t>& a_windowBufferStream
		, InflateDecoderPair const& a_decoder
	)
	{
		std::uint16_t value;
		do
		{
			value = a_decoder.m_literal.get(a_bitInputStream);
			sta::expect(
				value < 286 + 30
				, InvalidLiteralOrLengthValue{ value }
			);

			if (0 <= value && value < 256)
			{
				a_windowBufferStream.put(static_cast<char>(value));
			}
			else if (value > 256)
			{
				auto const duplicateLength = s_lengthCodes[value - 257]
					+ a_bitInputStream.get<5>(
						s_lengthExtraBits[value - 257]);
				auto const extraDistanceCode = a_decoder.m_distance.get(a_bitInputStream);
				sta::expect(
					extraDistanceCode < 30
					, InvalidExtraDistanceCode{ extraDistanceCode }
				);
				std::uint16_t const duplicateDistance = s_baseDistances[extraDistanceCode]
					+ a_bitInputStream.get<13>(s_distanceExtraBits[extraDistanceCode]);

				sta::expect(
					duplicateDistance <=
						static_cast<std::uint16_t>(a_windowBufferStream.windowSize())
					, TooBigDistance{ duplicateDistance }
				);
				for (auto i = 0; i < duplicateLength; ++i)
				{
					a_windowBufferStream.put(a_windowBufferStream.at(-duplicateDistance));
				}
			}

		} while (value != 256);
	}

	inline void readChunkStaticEncryption(
		LowFirstBitStreamReader& a_bitInputStream
		, WindowBufferStream<32768, std::uint8_t>& a_windowBufferStream
	)
	{
		readChunkEncryption(
			a_bitInputStream
			, a_windowBufferStream
			, s_inflateDecoderPair
		);
	}

	inline void readChunkDynamicEncryption(
		LowFirstBitStreamReader& a_bitInputStream
		, WindowBufferStream<32768, std::uint8_t>& a_windowBufferStream
	)
	{
		readChunkEncryption(
			a_bitInputStream
			, a_windowBufferStream
			, readInflateDecoderPair(a_bitInputStream)
		);
	}

	inline bool readChunkNode(
		LowFirstBitStreamReader& a_bitInputStream
		, WindowBufferStream<32768, std::uint8_t>& a_windowBufferStream
	)
	{
		auto const isLastChunk = a_bitInputStream.get<1>();
		auto const chunkType = a_bitInputStream.get<2>();

		switch (chunkType)
		{
		case 0:
		{
			readChunkNoEncryption(a_bitInputStream, a_windowBufferStream);
			break;
		}
		case 1:
		{
			readChunkStaticEncryption(a_bitInputStream, a_windowBufferStream);
			break;
		}
		case 2:
		{
			readChunkDynamicEncryption(a_bitInputStream, a_windowBufferStream);
			break;
		}
		case 3:
		default:
			sta::expect(false, InvalidChunkType{ chunkType });
			return true;
		}
		return isLastChunk;
	}

	inline void readDeflated(
		std::istream& a_inputStream
		, std::basic_ostream<unsigned char>& a_outputStream
	)
	{
		LowFirstBitStreamReader lowFirstBitReader{ a_inputStream };
		WindowBufferStream<32768, std::uint8_t> windowBufferStream{ a_outputStream };

		while (!detail::readChunkNode(lowFirstBitReader, windowBufferStream))
		{}
	}
}

namespace vob::sta
{
	inline void zinflate(
		std::istream& a_inputStream
		, std::basic_ostream<std::uint8_t>& a_outputStream
	)
	{
		detail::readDeflated(a_inputStream, a_outputStream);
	}
}
