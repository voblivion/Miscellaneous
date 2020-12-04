#pragma once
#pragma once

#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace vob::json
{
	// TODO change char to char8_t in c++20
	using char8 = char;

	template <typename Char8AllocatorType = std::allocator<char8>>
	using string = std::basic_string<char8, std::char_traits<char8>, Char8AllocatorType>;

	using string_view = std::basic_string_view<char8, std::char_traits<char8>>;

	template <typename Char8AllocatorType = std::allocator<char8>>
	struct number
	{
		void get_as(bool& a_value) const
		{
			char8* t_end;
			a_value = (std::strtod( m_representation.c_str(), &t_end) != 0.0);
		}

		void get_as(std::uint8_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::uint8_t>(std::strtoul( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::int8_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::int8_t>(std::strtol( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::uint16_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::uint16_t>(std::strtoul( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::int16_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::int16_t>(std::strtol( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::uint32_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::uint32_t>(std::strtoul( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::int32_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::int32_t>(std::strtol( m_representation.c_str(), &t_end, 10));
		}

		void get_as(std::uint64_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::uint64_t>(std::strtoull( m_representation.c_str(), &t_end, 10));

		}

		void get_as(std::int64_t& a_value) const
		{
			char8* t_end;
			a_value = static_cast<std::int64_t>(std::strtoll( m_representation.c_str(), &t_end, 10));
		}

		void get_as(float& a_value) const
		{
			char8* t_end;
			a_value = std::strtof( m_representation.c_str(), &t_end);
		}

		void get_as(double& a_value) const
		{
			char8* t_end;
			a_value = std::strtod( m_representation.c_str(), &t_end);
		}

		template <typename ValueType>
		void set_from(ValueType const& a_value)
		{
			m_value = a_value;
			m_representation = std::to_string(a_value);
		}

		std::variant<
			bool
			, std::uint8_t
			, std::int8_t
			, std::uint16_t
			, std::int16_t
			, std::uint32_t
			, std::int32_t
			, std::uint64_t
			, std::int64_t
			, float
			, double
		> m_value;
		string<Char8AllocatorType> m_representation;
	};

	using boolean = bool;
	using null = std::nullptr_t;

	template <typename Char8AllocatorType>
	struct array;
	template <typename Char8AllocatorType>
	struct object;

	template <typename Char8AllocatorType = std::allocator<char8>>
	using value = std::variant<
		null
		, string<Char8AllocatorType>
		, number<Char8AllocatorType>
		, object<Char8AllocatorType>
		, array<Char8AllocatorType>
		, boolean
	>;

	template <typename Char8AllocatorType = std::allocator<char8>>
	struct object
	{
		using value_type = value<Char8AllocatorType>;
		using string_type = string<Char8AllocatorType>;
		using key_type = std::variant<string_type, string_view>;
	private:
		using char8_alloc_traits = std::allocator_traits<Char8AllocatorType>;
		using key_value_pair = std::pair<key_type const, value_type>;
	public:
		using allocator_type = typename char8_alloc_traits::template rebind_alloc<key_value_pair>;

		struct key_hash
		{
			std::size_t operator()(key_type const& a_key) const
			{
				if (auto const t_string_key = std::get_if<string_type>(&a_key))
				{
					return std::hash<string_type>{}(*t_string_key);
				}
				auto const t_string_view_key = std::get<string_view>(a_key);
				return std::hash<string_view>{}(t_string_view_key);
			}
		};

		struct key_equal
		{
			bool operator()(key_type const& a_lhs, key_type const& a_rhs) const
			{
				if (auto const t_string_lhs = std::get_if<string_type>(&a_lhs))
				{
					if (auto const t_string_rhs = std::get_if<string_type>(&a_rhs))
					{
						return *t_string_lhs == *t_string_rhs;
					}
					auto const t_string_view_rhs = std::get<string_view>(a_rhs);
					return *t_string_lhs == t_string_view_rhs;
				}
				auto const t_string_view_lhs = std::get<string_view>(a_lhs);
				if (auto const t_string_rhs = std::get_if<string_type>(&a_rhs))
				{
					return t_string_view_lhs == *t_string_rhs;
				}
				auto const t_string_view_rhs = std::get<string_view>(a_rhs);
				return t_string_view_lhs == t_string_view_rhs;
			}
		};

		std::unordered_map<key_type, value_type, key_hash, key_equal, allocator_type> m_values;
	};

	template <typename Char8AllocatorType = std::allocator<char8>>
	struct array
	{
		using value_type = value<Char8AllocatorType>;
	private:
		using char8_alloc_traits = std::allocator_traits<Char8AllocatorType>;
	public:
		using allocator_type = typename char8_alloc_traits::template rebind_alloc<value_type>;
		using size_type = typename std::vector<value_type, allocator_type>::size_type;

		std::vector<value_type, allocator_type> m_values;
	};
}
