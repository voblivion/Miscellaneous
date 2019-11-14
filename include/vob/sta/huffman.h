#pragma once

#include <vob/sta/stream_reader.h>


namespace vob::sta
{
	template <uint8_t MaxBits>
	struct huffman_code
	{
	public:
#pragma region Aliases
		using buffer_type = smallest_uint_t<MaxBits>;
#pragma endregion
#pragma region Constructors
		constexpr huffman_code() = default;

		constexpr huffman_code(buffer_type const a_buffer, uint8_t const a_buffer_bit_size)
			: m_buffer{ a_buffer }
			, m_buffer_bit_size{ a_buffer_bit_size }
		{}
#pragma endregion
#pragma region Methods
		void add(bool bit)
		{
			m_buffer = static_cast<buffer_type>(m_buffer << 1) | static_cast<buffer_type>(bit);
			++m_buffer_bit_size;
		}

		auto buffer() const
		{
			return m_buffer;
		}

		auto buffer_bit_size() const
		{
			return m_buffer_bit_size;
		}
#pragma endregion
#pragma region Operators
		constexpr friend bool operator<(huffman_code const& a_lhs, huffman_code const& a_rhs)
		{
			return a_lhs.m_buffer_bit_size < a_rhs.m_buffer_bit_size
				|| (a_lhs.m_buffer_bit_size == a_rhs.m_buffer_bit_size
					&& a_lhs.m_buffer < a_rhs.m_buffer);
		}

		constexpr friend bool operator==(huffman_code const& a_lhs, huffman_code const& a_rhs)
		{
			return a_lhs.m_buffer == a_rhs.m_buffer
				&& a_lhs.m_buffer_bit_size == a_rhs.m_buffer_bit_size;
		}

		constexpr friend bool operator!=(huffman_code const& a_lhs, huffman_code const& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}
#pragma endregion
	private:
#pragma region Attributes
		buffer_type m_buffer = 0;
		uint8_t m_buffer_bit_size = 0;
#pragma endregion
	};

	template <uint8_t MaxCodeBits, uint32_t AlphabetSize>
	struct huffman_entry
	{
#pragma region Aliases
		using code_type = huffman_code<MaxCodeBits>;
		static constexpr uint8_t max_value_bits = ceil_log2_v<AlphabetSize>;
		using value_type = smallest_uint_t<max_value_bits>;
#pragma endregion
#pragma region Attributes
		code_type code;
		value_type value = 0;
#pragma endregion
#pragma region Operators
		constexpr friend bool operator<(huffman_entry const& a_lhs, huffman_entry const& a_rhs)
		{
			return a_lhs.code < a_rhs.code;
		}

		constexpr friend bool operator==(huffman_entry const& a_lhs, huffman_entry const& a_rhs)
		{
			return a_lhs.code == a_rhs.code && a_lhs.value == a_rhs.value;
		}
#pragma endregion
	};

	template <typename EntryListT, uint8_t MaxCodeBits, uint32_t AlphabetSize>
	class huffman_coder
	{
#pragma region Aliases & Statics
		using entry_type = huffman_entry<MaxCodeBits, AlphabetSize>;
		static constexpr uint8_t max_value_bits = entry_type::max_value_bits;
		using value_type = typename entry_type::value_type;
		using code_type = typename entry_type::code_type;
#pragma endregion
	public:
#pragma region Constructors
		explicit constexpr huffman_coder(EntryListT a_huffman_entry_list)
			: m_huffman_entry_list{ a_huffman_entry_list }
		{
			std::sort(m_huffman_entry_list.begin(), m_huffman_entry_list.end());
		}

		// TODO : remove when std::sort is constexpr (c++20)
		static constexpr auto make_huffman_coder_from_sorted(EntryListT a_huffman_entry_list)
		{
			return huffman_coder{ a_huffman_entry_list, dummy{} };
		}
#pragma endregion
#pragma region Methods
		bool get(
			std::istream& a_input_stream
			, bit_stream_reader& a_bit_stream_reader
			, value_type& a_value
		) const
		{
			bool bit;
			code_type code;
			auto it = m_huffman_entry_list.begin();
			while (
				it != m_huffman_entry_list.end()
				&& it->code != code
				&& a_bit_stream_reader.get<1>(a_input_stream, bit)
				)
			{
				code.add(bit);
				it = std::lower_bound(
					it
					, m_huffman_entry_list.end()
					, code
					, [](entry_type const& a_entry, code_type a_code)
				{
					return a_entry.code < a_code;
				});
			}

			if (it != m_huffman_entry_list.end() && it->code == code)
			{
				a_value = it->value;
				return true;
			}
			return false;
		}
#pragma endregion
	private:
#pragma region Attributes
		EntryListT m_huffman_entry_list;
#pragma endregion
#pragma region Constrcutors
		// TODO : remove when std::sort is constexpr (c++20)
		struct dummy {};
		explicit constexpr huffman_coder(EntryListT a_huffman_entry_list, dummy)
			: m_huffman_entry_list{ std::move(a_huffman_entry_list) }
		{}
#pragma endregion
	};

	template <uint8_t MaxCodeBits, uint32_t AlphabetSize>
	using huffman_entry_list = bounded_vector<
		huffman_entry<MaxCodeBits, AlphabetSize>
		, AlphabetSize
	>;

	template <uint8_t MaxCodeBits, uint32_t AlphabetSize>
	constexpr auto make_huffman_coder(
		bounded_vector<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize> const& a_huffman_entry_list
	)
	{
		return huffman_coder<
			bounded_vector<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize>
			, MaxCodeBits
			, AlphabetSize
		>{ a_huffman_entry_list };
	}

	template <uint8_t MaxCodeBits, uint32_t AlphabetSize>
	constexpr auto make_huffman_coder(
		std::array<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize> const& a_huffman_entry_list
	)
	{
		return huffman_coder<
			std::array<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize>
			, MaxCodeBits
			, AlphabetSize
		>{ a_huffman_entry_list };
	}

	// TODO : remove when std::sorted is constexpr (c++20)
	template <uint8_t MaxCodeBits, uint32_t AlphabetSize>
	constexpr auto make_huffman_coder_from_sorted(
		std::array<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize> const& a_huffman_entry_list
	)
	{
		return huffman_coder<
			std::array<huffman_entry<MaxCodeBits, AlphabetSize>, AlphabetSize>
			, MaxCodeBits
			, AlphabetSize
		>::make_huffman_coder_from_sorted(a_huffman_entry_list);
	}
}