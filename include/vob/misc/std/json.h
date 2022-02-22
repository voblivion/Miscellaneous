#pragma once

#include "../std/string_vector_map.h"
#include "../std/polymorphic_ptr.h"
#include "../std/polymorphic_ptr_util.h"

#include <iomanip>
#include <variant>
#include <vector>
#include <string>
#include <string_view>

namespace vob::mistd
{
	/// @brief TODO
	enum class json_value_type
	{
		null,
		boolean,
		number,
		string,
		array,
		object
	};

	namespace detail
	{
		template <typename TAllocator>
		struct basic_json_value_base
		{
		public:
			basic_json_value_base() = delete;

#ifndef NDEBUG
			/// @brief TODO
			virtual ~basic_json_value_base() = default;
#endif

			explicit basic_json_value_base(json_value_type a_type) noexcept
				: m_type{ a_type }
			{}

			[[nodiscard]] auto get_type() const
			{
				return m_type;
			}

		private:
			json_value_type m_type;
		};
	}

	template <typename TAllocator = std::allocator<char>>
	class basic_json_value;

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_null
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::null;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_null() noexcept
			: detail::basic_json_value_base<TAllocator>{ type }
		{}
#pragma endregion
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_boolean
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::boolean;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_boolean() noexcept
			: detail::basic_json_value_base<TAllocator>{ type }
		{}

		basic_json_boolean(bool a_value)
			: detail::basic_json_value_base<TAllocator>{ type }
			, value{ a_value }
		{}
#pragma endregion

#pragma region DATA
		bool value = false;
#pragma endregion
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_number
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::number;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_number() noexcept
			: detail::basic_json_value_base<TAllocator>{ type }
		{}

		/// @brief TODO
		basic_json_number(std::int64_t a_value)
			: detail::basic_json_value_base<TAllocator>{ type }
			, value{ a_value }
		{}

		/// @brief TODO
		basic_json_number(std::uint64_t a_value)
			: detail::basic_json_value_base<TAllocator>{ type }
			, value{ a_value }
		{}

		/// @brief TODO
		basic_json_number(long double a_value)
			: detail::basic_json_value_base<TAllocator>{ type }
			, value{ a_value }
		{}
#pragma endregion

#pragma region DATA
		std::variant<std::int64_t, std::uint64_t, long double> value;
#pragma endregion
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_string
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::string;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		template <typename... TArgs>
		basic_json_string(TArgs&&... a_args)
			: detail::basic_json_value_base<TAllocator>{ type }
			, value{ std::forward<TArgs>(a_args)... }
		{}
#pragma endregion

#pragma region DATA
		std::basic_string<char, std::char_traits<char>, TAllocator> value;
#pragma endregion
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_array
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::array;
#pragma endregion

#pragma region TYPES
		using value_type = basic_json_value<TAllocator>;
		using allocator_type = std::allocator_traits<TAllocator>::template rebind_alloc<value_type>;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_array(TAllocator const& a_allocator = {}) noexcept
			: detail::basic_json_value_base<TAllocator>{ type }
			, data{ allocator_type{ a_allocator } }
		{}
#pragma endregion

#pragma region DATA
		std::vector<value_type, allocator_type> data;
#pragma endregion
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_json_object
		: public detail::basic_json_value_base<TAllocator>
	{
#pragma region CLASS_DATA
		constexpr static auto type = json_value_type::object;
#pragma endregion

#pragma region TYPES
		using string_type = std::basic_string<char, std::char_traits<char>, TAllocator>;
		using key_type = basic_string_map_key<string_type>;
		using value_type = basic_json_value<TAllocator>;
		using allocator_type = std::allocator_traits<TAllocator>::template rebind_alloc<
			std::pair<key_type const, value_type>>;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_object(TAllocator const& a_allocator = {}) noexcept
			: detail::basic_json_value_base<TAllocator>{ type }
			, data{ a_allocator }
		{}
#pragma endregion

#pragma region DATA
		string_vector_map<
			value_type,
			string_type,
			std::string_view,
			std::equal_to<>,
			allocator_type> data;
#pragma endregion
	};

	template <typename TAllocator>
	class basic_json_value
	{
	public:
#pragma region TYPES
		using null_type = basic_json_null<TAllocator>;
		using boolean_type = basic_json_boolean<TAllocator>;
		using number_type = basic_json_number<TAllocator>;
		using string_type = basic_json_string<TAllocator>;
		using array_type = basic_json_array<TAllocator>;
		using object_type = basic_json_object<TAllocator>;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_value(TAllocator const& a_allocator = {})
			: m_allocator{ a_allocator }
		{}

		/// @brief TODO
		template <typename TValueType>
		basic_json_value(TValueType a_value, TAllocator const& a_allocator)
			: m_allocator{ a_allocator }
		{
			set<TValueType>(std::move(a_value));
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		template <typename TValueType>
		[[nodiscard]] auto get() const -> TValueType const*
		{
			if (m_value != nullptr && m_value->get_type() == TValueType::type)
			{
				return static_cast<TValueType const*>(m_value.get());
			}
			return nullptr;
		}

		/// @brief TODO
		[[nodiscard]] auto get_allocator() const
		{
			return m_allocator;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		template <typename TValueType, typename... TArgs>
		requires std::is_base_of_v<detail::basic_json_value_base<TAllocator>, TValueType>
		decltype(auto) set(TArgs&&... a_args)
		{
			auto value = polymorphic_ptr_util::allocate<TValueType>(m_allocator, std::forward<TArgs>(a_args)...);
			auto& ref = *value.get();
			m_value = polymorphic_ptr_util::cast<detail::basic_json_value_base<TAllocator>>(std::move(value));
			return ref;
		}

		/// @brief TODO
		template <typename TValueType>
		[[nodiscard]] auto get() -> TValueType*
		{
			if (m_value != nullptr && m_value->get_type() == TValueType::type)
			{
				return static_cast<TValueType*>(m_value.get());
			}
			return nullptr;
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TAllocator m_allocator;
		polymorphic_ptr<detail::basic_json_value_base<TAllocator>> m_value;
#pragma endregion
	};

	using json_null = basic_json_null<>;
	using json_boolean = basic_json_boolean<>;
	using json_number = basic_json_number<>;
	using json_string = basic_json_string<>;
	using json_array = basic_json_array<>;
	using json_object = basic_json_object<>;
	using json_value = basic_json_value<>;

	namespace pmr
	{
		using json_null = basic_json_null<std::pmr::polymorphic_allocator<char>>;
		using json_boolean = basic_json_boolean<std::pmr::polymorphic_allocator<char>>;
		using json_number = basic_json_number<std::pmr::polymorphic_allocator<char>>;
		using json_string = basic_json_string<std::pmr::polymorphic_allocator<char>>;
		using json_array = basic_json_array<std::pmr::polymorphic_allocator<char>>;
		using json_object = basic_json_object<std::pmr::polymorphic_allocator<char>>;
		using json_value = basic_json_value<std::pmr::polymorphic_allocator<char>>;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_value<TAllocator>& a_value)
	{
		a_inputStream >> std::ws;
		switch (a_inputStream.peek())
		{
		case '"':
		{
			auto& string = a_value.set<basic_json_string<TAllocator>>(a_value.get_allocator());
			a_inputStream >> string;
			break;
		}
		case '{':
		{
			auto& object = a_value.set<basic_json_object<TAllocator>>(a_value.get_allocator());
			a_inputStream >> object;
			break;
		}
		case '[':
		{
			auto& array = a_value.set<basic_json_array<TAllocator>>(a_value.get_allocator());
			a_inputStream >> array;
			break;
		}
		case 't':
		case 'f':
		{
			auto& boolean = a_value.set<basic_json_boolean<TAllocator>>();
			a_inputStream >> boolean;
			break;
		}
		case 'n':
		{
			auto& null = a_value.set<basic_json_null<TAllocator>>();
			a_inputStream >> null;
			break;
		}
		default:
		{
			auto& number = a_value.set<basic_json_number<TAllocator>>();
			a_inputStream >> number;
			break;
		}
		}
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_string<TAllocator>& a_string)
	{
		a_inputStream >> std::ws;
		if (a_inputStream.peek() != '"')
		{
			a_inputStream.setstate(std::ios_base::failbit);
			return a_inputStream;
		}
		a_inputStream >> std::quoted(a_string.value);
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_object<TAllocator>& a_object)
	{
		a_inputStream >> std::ws;
		char c = a_inputStream.get();
		if (c != '{')
		{
			a_inputStream.setstate(std::ios_base::failbit);
			return a_inputStream;
		}
		a_inputStream >> std::ws;
		c = a_inputStream.peek();
		while (c != '}' && !a_inputStream.eof())
		{
			typename basic_json_object<TAllocator>::string_type keyString;
			a_inputStream >> std::quoted(keyString) >> std::ws;
			c = a_inputStream.get();
			if (c != ':')
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
			basic_json_value<TAllocator> value{ TAllocator{ a_object.data.get_allocator() } };
			a_inputStream >> std::ws >> value >> std::ws;
			a_object.data.emplace(keyString, std::move(value));
			c = a_inputStream.peek();
			if (c == ',')
			{
				a_inputStream.get();
				a_inputStream >> std::ws;
				c = a_inputStream.peek();
			}
			else if (c != '}')
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
		}
		a_inputStream.get();
		if (a_inputStream.eof())
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_array<TAllocator>& a_array)
	{
		a_inputStream >> std::ws;
		char c = a_inputStream.get();
		if (c != '[')
		{
			a_inputStream.setstate(std::ios_base::failbit);
			return a_inputStream;
		}
		a_inputStream >> std::ws;
		c = a_inputStream.peek();
		while (c != ']' && !a_inputStream.eof())
		{
			basic_json_value<TAllocator> value{ TAllocator{ a_array.data.get_allocator() } };
			a_inputStream >> std::ws >> value >> std::ws;
			a_array.data.emplace_back(std::move(value));
			c = a_inputStream.peek();
			if (c == ',')
			{
				a_inputStream.get();
				c = a_inputStream.peek();
			}
			else if (c != ']')
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
		}
		a_inputStream.get();
		if (a_inputStream.eof())
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_boolean<TAllocator>& a_boolean)
	{
		auto c = a_inputStream.get();
		if (c == 't')
		{
			a_boolean.value = true;
			if (a_inputStream.get() != 'r')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
			else if (a_inputStream.get() != 'u')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
			else if (a_inputStream.get() != 'e')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
		}
		else if (c == 'f')
		{
			a_boolean.value = false;
			if (a_inputStream.get() != 'a')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
			else if (a_inputStream.get() != 'l')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
			else if (a_inputStream.get() != 's')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
			else if (a_inputStream.get() != 'e')
			{
				a_inputStream.setstate(std::ios_base::failbit);
			}
		}
		else
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_null<TAllocator>)
	{
		if (a_inputStream.get() != 'n')
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		else if (a_inputStream.get() != 'u')
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		else if (a_inputStream.get() != 'l')
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		else if (a_inputStream.get() != 'l')
		{
			a_inputStream.setstate(std::ios_base::failbit);
		}
		return a_inputStream;
	}

	/// @brief TODO
	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_json_number<TAllocator>& a_number)
	{
		static constexpr std::size_t k_maxSize = 256;
		std::array<char, k_maxSize + 2> representation;
		std::size_t index = 0;

		auto const tryReadRange = [&a_inputStream, &representation, &index](char const a_min, char const a_max)
		{
			if (index >= k_maxSize)
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return false;
			}
			char const c = a_inputStream.peek();
			if (c >= a_min && c <= a_max)
			{
				representation[index++] = static_cast<char>(a_inputStream.get());
				return true;
			}
			return false;
		};

		auto const tryRead = [&tryReadRange](char const a_value)
		{
			return tryReadRange(a_value, a_value);
		};

		auto const isNegative = tryRead('-');
		if (!tryRead('0'))
		{
			if (!tryReadRange('1', '9'))
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
			while (tryReadRange('0', '9')) {}
		}

		auto const isFloat = tryRead('.');
		if (isFloat)
		{
			if (!tryReadRange('0', '9'))
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
			while (tryReadRange('0', '9')) {}

			if (tryRead('e') || tryRead('E'))
			{
				if (!tryRead('+'))
				{
					tryRead('-');
				}
				if (!tryReadRange('0', '9'))
				{
					a_inputStream.setstate(std::ios_base::failbit);
				}
				while (tryReadRange('0', '9')) {}
			}
		}
		representation[index] = 0;

		if (isFloat)
		{
			char* end;
			a_number.value = std::strtold(representation.data(), &end);
		}
		else if (isNegative)
		{
			char* end;
			a_number.value = std::strtoll(representation.data(), &end, 10);
		}
		else
		{
			char* end;
			a_number.value = std::strtoull(representation.data(), &end, 10);
		}
		return a_inputStream;
	}
}
