#pragma once

#include <algorithm>
#include <vector>

#include <vob/sta/expect.h>
#include <vob/sta/integer.h>
#include <vob/sta/streamreader.h>


namespace vob::sta
{
	struct InvalidHuffmanStream final
		: std::exception
	{};

	template <std::uint8_t t_maxBits>
	struct HuffmanCode
	{
		using Buffer = SmallestUInt<t_maxBits>;

		// Attributes
		Buffer m_buffer = 0;
		SmallestUInt<s_ceilLog2<t_maxBits>> m_bufferBits = 0;

		// Methods
		void add(bool bit)
		{
			m_buffer = static_cast<Buffer>(m_buffer << 1) | static_cast<Buffer>(bit);
			++m_bufferBits;
		}

		// Operators
		constexpr friend bool operator<(
			HuffmanCode const& a_lhs
			, HuffmanCode const& a_rhs
		)
		{
			return a_rhs.m_bufferBits == 0 && a_lhs.m_bufferBits != 0
				|| (a_lhs.m_bufferBits != 0
					&& (a_lhs.m_bufferBits < a_rhs.m_bufferBits
						|| (a_lhs.m_bufferBits == a_rhs.m_bufferBits
							&& a_lhs.m_buffer < a_rhs.m_buffer)));
		}

		constexpr friend bool operator==(
			HuffmanCode const& a_lhs
			, HuffmanCode const& a_rhs
		)
		{
			return a_lhs.m_buffer == a_rhs.m_buffer
				&& a_lhs.m_bufferBits == a_rhs.m_bufferBits;
		}

		constexpr friend bool operator!=(
			HuffmanCode const& a_lhs
			, HuffmanCode const& a_rhs
		)
		{
			return !(a_lhs == a_rhs);
		}
	};

	template <std::uint8_t t_maxCodeBits, std::uint32_t t_alphabetSize>
	struct HuffmanEntry
	{
	public:
		static constexpr std::uint8_t s_maxValueBits = s_ceilLog2<t_alphabetSize>;
		using Value = SmallestUInt<s_maxValueBits>;
		using Code = HuffmanCode<t_maxCodeBits>;

		// Attributes
		Value m_value = 0;
		Code m_code;

		// Operators
		constexpr friend bool operator<(
			HuffmanEntry const& a_lhs
			, HuffmanEntry const& a_rhs
		)
		{
			return a_lhs.m_code < a_rhs.m_code;
		}
	};

	template <std::uint8_t t_maxCodeBits, std::uint32_t t_alphabetSize>
	class HuffmanDecoder
	{
		static constexpr std::uint8_t s_maxValueBits = s_ceilLog2<t_alphabetSize>;
		using HuffmanCode = HuffmanCode<t_maxCodeBits>;
		using HuffmanEntry = HuffmanEntry<t_maxCodeBits, t_alphabetSize>;
	public:

		// Constructors
		explicit constexpr HuffmanDecoder(
			std::array<HuffmanEntry, t_alphabetSize> a_huffmanEntryList
		)
			: m_huffmanEntryList{ std::move(a_huffmanEntryList) }
		{
			std::sort(
				m_huffmanEntryList.begin()
				, m_huffmanEntryList.end()
			);

			m_entryListEndIndex = std::distance(
				m_huffmanEntryList.begin()
				, std::find_if(
					m_huffmanEntryList.begin()
					, m_huffmanEntryList.end()
					, [](HuffmanEntry const& a_huffmanEntry) {
						return a_huffmanEntry.m_code.m_bufferBits == 0;
					}
				)
			);
		}

		// Methods
		SmallestUInt<s_maxValueBits> get(
			LowFirstBitStreamReader& a_lowFirstBitReader
		) const
		{
			// TODO : sta::expect
			HuffmanCode code;
			auto it = m_huffmanEntryList.begin();
			do
			{
				sta::expect<UnexpectedEndOfFile>(!a_lowFirstBitReader.eof());
				code.add(a_lowFirstBitReader.get<1>());
				it = std::lower_bound(
					it
					, m_huffmanEntryList.begin() + m_entryListEndIndex
					, code
					, [](HuffmanEntry const& a_huffmanEntry, HuffmanCode a_huffmanCode)
				{
					return a_huffmanEntry.m_code < a_huffmanCode;
				});
			}
			while (it != m_huffmanEntryList.end()
				&& (it->m_code != code || it->m_code.m_bufferBits == 0));

			if (it == m_huffmanEntryList.end())
			{
				throw InvalidHuffmanStream{};
			}
			return it->m_value;
		}

	private:
		// Attributes
		using EntryList = std::array<HuffmanEntry, t_alphabetSize>;
		EntryList m_huffmanEntryList;
		std::size_t m_entryListEndIndex;
	};

	template <std::uint8_t t_maxCodeBits, std::uint32_t t_alphabetSize>
	using HuffmanEntryList = std::array <
		HuffmanEntry<t_maxCodeBits, t_alphabetSize>
		, t_alphabetSize
	>;

	template <std::uint8_t t_maxCodeBits, std::uint32_t t_alphabetSize>
	constexpr auto makeHuffmanDecoder(
		HuffmanEntryList<t_maxCodeBits, t_alphabetSize> const& a_huffmanEntryList
	)
	{
		return HuffmanDecoder<t_maxCodeBits, t_alphabetSize>{
			a_huffmanEntryList
		};
	}
}