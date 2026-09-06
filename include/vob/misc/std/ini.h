#pragma once

#include "../std/string_vector_map.h"
#include "../std/polymorphic_ptr.h"
#include "../std/polymorphic_ptr_util.h"

#include <istream>
#include <string>
#include <string_view>
#include <vector>

namespace vob::mistd
{
	enum class ini_value_type
	{
		literal,
		array,
		object
	};

	namespace detail
	{
		template <typename TAllocator>
		struct basic_ini_value_base
		{
		public:
			basic_ini_value_base() = delete;

#ifndef NDEBUG
			virtual ~basic_ini_value_base() = default;
#endif

			explicit basic_ini_value_base(ini_value_type a_type) noexcept
				: m_type{ a_type }
			{}

			[[nodiscard]] auto get_type() const
			{
				return m_type;
			}

		private:
			ini_value_type m_type;
		};
	}

	template <typename TAllocator = std::allocator<char>>
	class basic_ini_value;

	template <typename TAllocator = std::allocator<char>>
	struct basic_ini_literal
		: public detail::basic_ini_value_base<TAllocator>
	{
		constexpr static auto type = ini_value_type::literal;

		template <typename... TArgs>
		basic_ini_literal(TArgs&&... a_args)
			: detail::basic_ini_value_base<TAllocator>{ type }
			, value{ std::forward<TArgs>(a_args)... }
		{}
		
		std::basic_string<char, std::char_traits<char>, TAllocator> value;
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_ini_array
		: public detail::basic_ini_value_base<TAllocator>
	{
		constexpr static auto type = ini_value_type::array;

		using value_type = basic_ini_value<TAllocator>;
		using allocator_type = std::allocator_traits<TAllocator>::template rebind_alloc<value_type>;

		basic_ini_array(TAllocator const& a_allocator = {}) noexcept
			: detail::basic_ini_value_base<TAllocator>{ type }
			, data{ allocator_type{ a_allocator } }
		{}

		std::vector<value_type, allocator_type> data;
	};

	template <typename TAllocator = std::allocator<char>>
	struct basic_ini_object
		: public detail::basic_ini_value_base<TAllocator>
	{
		constexpr static auto type = ini_value_type::object;

		using string_type = std::basic_string<char, std::char_traits<char>, TAllocator>;
		using key_type = basic_string_map_key<string_type>;
		using value_type = basic_ini_value<TAllocator>;
		using allocator_type = std::allocator_traits<TAllocator>::template rebind_alloc<
			std::pair<key_type const, value_type>>;

		basic_ini_object(TAllocator const& a_allocator = {}) noexcept
			: detail::basic_ini_value_base<TAllocator>{ type }
			, data{ a_allocator }
		{}

		string_vector_map<
			value_type,
			string_type,
			std::string_view,
			std::equal_to<>,
			allocator_type> data;
	};

	template <typename TAllocator>
	class basic_ini_value
	{
	public:
		using literal_type = basic_ini_literal<TAllocator>;
		using array_type = basic_ini_array<TAllocator>;
		using object_type = basic_ini_object<TAllocator>;

		basic_ini_value(TAllocator const& a_allocator = {})
			: m_allocator{ a_allocator }
		{}

		template <typename TValueType>
		basic_ini_value(TValueType a_value, TAllocator const& a_allocator)
			: m_allocator{ a_allocator }
		{
			set<TValueType>(std::move(a_value));
		}

		template <typename TValueType>
		[[nodiscard]] auto get() const -> TValueType const*
		{
			if (m_value != nullptr && m_value->get_type() == TValueType::type)
			{
				return static_cast<TValueType const*>(m_value.get());
			}
			return nullptr;
		}

		[[nodiscard]] auto get_allocator() const
		{
			return m_allocator;
		}

		template <typename TValueType, typename... TArgs>
		requires std::is_base_of_v<detail::basic_ini_value_base<TAllocator>, TValueType>
		decltype(auto) set(TArgs&&... a_args)
		{
			auto value = polymorphic_ptr_util::allocate<TValueType>(m_allocator, std::forward<TArgs>(a_args)...);
			auto& ref = *value.get();
			m_value = polymorphic_ptr_util::cast<detail::basic_ini_value_base<TAllocator>>(std::move(value));
			return ref;
		}

		template <typename TValueType>
		[[nodiscard]] auto get() -> TValueType*
		{
			if (m_value != nullptr && m_value->get_type() == TValueType::type)
			{
				return static_cast<TValueType*>(m_value.get());
			}
			return nullptr;
		}

	private:
		TAllocator m_allocator;
		polymorphic_ptr<detail::basic_ini_value_base<TAllocator>> m_value;
	};

	using ini_literal = basic_ini_literal<>;
	using ini_array = basic_ini_array<>;
	using ini_object = basic_ini_object<>;
	using ini_value = basic_ini_value<>;

	namespace pmr
	{
		using ini_literal = basic_ini_literal<std::pmr::polymorphic_allocator<char>>;
		using ini_array = basic_ini_array<std::pmr::polymorphic_allocator<char>>;
		using ini_object = basic_ini_object<std::pmr::polymorphic_allocator<char>>;
		using ini_value = basic_ini_value<std::pmr::polymorphic_allocator<char>>;
	}

	namespace detail
	{
		inline bool is_ini_space(int a_character)
		{
			return a_character == ' ' || a_character == '\t' || a_character == '\r';
		}

		inline void skip_ini_spaces(std::istream& a_inputStream)
		{
			while (is_ini_space(a_inputStream.peek()))
			{
				a_inputStream.get();
			}
		}

		inline void skip_ini_blanks(std::istream& a_inputStream)
		{
			auto character = a_inputStream.peek();
			while (is_ini_space(character) || character == '\n' || character == ';')
			{
				if (character == ';')
				{
					while (!a_inputStream.eof() && a_inputStream.peek() != '\n')
					{
						a_inputStream.get();
					}
				}
				else
				{
					a_inputStream.get();
				}
				character = a_inputStream.peek();
			}
		}

		template <typename TString>
		void trim_ini_literal(TString& a_text)
		{
			auto const isSpace = [](char a_character) { return is_ini_space(a_character); };
			while (!a_text.empty() && isSpace(a_text.back()))
			{
				a_text.pop_back();
			}
		}

		template <typename TString>
		void read_ini_quoted(std::istream& a_inputStream, TString& a_text)
		{
			a_inputStream.get();
			auto character = a_inputStream.get();
			while (character != '"')
			{
				if (a_inputStream.eof())
				{
					a_inputStream.setstate(std::ios_base::failbit);
					return;
				}
				if (character == '\\')
				{
					character = a_inputStream.get();
					if (a_inputStream.eof())
					{
						a_inputStream.setstate(std::ios_base::failbit);
						return;
					}
				}
				a_text.push_back(static_cast<char>(character));
				character = a_inputStream.get();
			}
		}

		template <typename TString>
		void read_ini_key(std::istream& a_inputStream, TString& a_key)
		{
			if (a_inputStream.peek() == '"')
			{
				read_ini_quoted(a_inputStream, a_key);
				skip_ini_spaces(a_inputStream);
				return;
			}

			auto character = a_inputStream.peek();
			while (!a_inputStream.eof() && character != '=' && character != '\n')
			{
				a_key.push_back(static_cast<char>(a_inputStream.get()));
				character = a_inputStream.peek();
			}
			trim_ini_literal(a_key);
		}

		template <typename TObject, typename TString, typename TValue>
		void set_ini_entry(TObject& a_object, TString a_key, TValue&& a_value)
		{
			auto const entryIt = a_object.data.find(a_key);
			if (entryIt != a_object.data.end())
			{
				entryIt->second = std::forward<TValue>(a_value);
				return;
			}
			a_object.data.emplace(std::move(a_key), std::forward<TValue>(a_value));
		}

		template <typename TAllocator>
		void read_ini_value(
			std::istream& a_inputStream, basic_ini_value<TAllocator>& a_value, bool a_isNested);

		template <typename TAllocator>
		void read_ini_array(std::istream& a_inputStream, basic_ini_array<TAllocator>& a_array)
		{
			a_inputStream.get();
			skip_ini_blanks(a_inputStream);
			while (!a_inputStream.eof() && a_inputStream.peek() != ']')
			{
				basic_ini_value<TAllocator> value{ TAllocator{ a_array.data.get_allocator() } };
				read_ini_value(a_inputStream, value, true);
				a_array.data.emplace_back(std::move(value));

				skip_ini_blanks(a_inputStream);
				if (a_inputStream.peek() == ',')
				{
					a_inputStream.get();
					skip_ini_blanks(a_inputStream);
				}
			}
			a_inputStream.get();
		}

		template <typename TAllocator>
		void read_ini_braced_object(
			std::istream& a_inputStream, basic_ini_object<TAllocator>& a_object)
		{
			a_inputStream.get();
			skip_ini_blanks(a_inputStream);
			while (!a_inputStream.eof() && a_inputStream.peek() != '}')
			{
				typename basic_ini_object<TAllocator>::string_type key;
				read_ini_key(a_inputStream, key);
				if (a_inputStream.peek() != '=')
				{
					a_inputStream.setstate(std::ios_base::failbit);
					return;
				}
				a_inputStream.get();

				basic_ini_value<TAllocator> value{ TAllocator{ a_object.data.get_allocator() } };
				read_ini_value(a_inputStream, value, true);
				set_ini_entry(a_object, std::move(key), std::move(value));

				skip_ini_blanks(a_inputStream);
				if (a_inputStream.peek() == ',')
				{
					a_inputStream.get();
					skip_ini_blanks(a_inputStream);
				}
			}
			a_inputStream.get();
		}

		template <typename TAllocator>
		void read_ini_value(
			std::istream& a_inputStream, basic_ini_value<TAllocator>& a_value, bool a_isNested)
		{
			skip_ini_blanks(a_inputStream);
			switch (a_inputStream.peek())
			{
			case '{':
			{
				auto& object = a_value.template set<basic_ini_object<TAllocator>>(a_value.get_allocator());
				read_ini_braced_object(a_inputStream, object);
				break;
			}
			case '[':
			{
				auto& array = a_value.template set<basic_ini_array<TAllocator>>(a_value.get_allocator());
				read_ini_array(a_inputStream, array);
				break;
			}
			case '"':
			{
				auto& literal = a_value.template set<basic_ini_literal<TAllocator>>(a_value.get_allocator());
				read_ini_quoted(a_inputStream, literal.value);
				break;
			}
			default:
			{
				auto& literal = a_value.template set<basic_ini_literal<TAllocator>>(a_value.get_allocator());
				auto character = a_inputStream.peek();
				while (!a_inputStream.eof()
					&& character != '\n'
					&& !(a_isNested && (character == ',' || character == '}' || character == ']')))
				{
					literal.value.push_back(static_cast<char>(a_inputStream.get()));
					character = a_inputStream.peek();
				}
				trim_ini_literal(literal.value);
				break;
			}
			}
		}
	}

	template <typename TAllocator>
	std::istream& operator>>(std::istream& a_inputStream, basic_ini_object<TAllocator>& a_document)
	{
		detail::skip_ini_blanks(a_inputStream);
		while (!a_inputStream.eof())
		{
			if (a_inputStream.peek() != '[')
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
			a_inputStream.get();

			typename basic_ini_object<TAllocator>::string_type sectionName;
			auto character = a_inputStream.peek();
			while (!a_inputStream.eof() && character != ']' && character != '\n')
			{
				sectionName.push_back(static_cast<char>(a_inputStream.get()));
				character = a_inputStream.peek();
			}
			if (character != ']')
			{
				a_inputStream.setstate(std::ios_base::failbit);
				return a_inputStream;
			}
			a_inputStream.get();

			basic_ini_value<TAllocator> sectionValue{ TAllocator{ a_document.data.get_allocator() } };
			auto& section = sectionValue.template set<basic_ini_object<TAllocator>>(
				sectionValue.get_allocator());

			detail::skip_ini_blanks(a_inputStream);
			while (!a_inputStream.eof() && a_inputStream.peek() != '[')
			{
				typename basic_ini_object<TAllocator>::string_type key;
				detail::read_ini_key(a_inputStream, key);
				if (a_inputStream.peek() != '=')
				{
					a_inputStream.setstate(std::ios_base::failbit);
					return a_inputStream;
				}
				a_inputStream.get();

				basic_ini_value<TAllocator> value{ TAllocator{ section.data.get_allocator() } };
				detail::read_ini_value(a_inputStream, value, false);
				detail::set_ini_entry(section, std::move(key), std::move(value));

				detail::skip_ini_blanks(a_inputStream);
			}

			detail::set_ini_entry(a_document, std::move(sectionName), std::move(sectionValue));
			detail::skip_ini_blanks(a_inputStream);
		}
		return a_inputStream;
	}

	namespace detail
	{
		inline bool needs_ini_quotes(std::string_view const a_text, bool const a_isNested)
		{
			if (a_text.empty())
			{
				return true;
			}
			if (a_text.front() == '{' || a_text.front() == '[' || a_text.front() == '"')
			{
				return true;
			}
			if (is_ini_space(a_text.front()) || is_ini_space(a_text.back()))
			{
				return true;
			}
			if (a_text.find('\n') != std::string_view::npos)
			{
				return true;
			}
			return a_isNested && a_text.find_first_of(",}]") != std::string_view::npos;
		}

		inline void write_ini_text(
			std::ostream& a_outputStream, std::string_view const a_text, bool const a_isNested)
		{
			if (!needs_ini_quotes(a_text, a_isNested))
			{
				a_outputStream << a_text;
				return;
			}

			a_outputStream << '"';
			for (auto const character : a_text)
			{
				if (character == '"' || character == '\\')
				{
					a_outputStream << '\\';
				}
				a_outputStream << character;
			}
			a_outputStream << '"';
		}

		template <typename TAllocator>
		void write_ini_value(
			std::ostream& a_outputStream, basic_ini_value<TAllocator> const& a_value, bool a_isNested);

		template <typename TAllocator>
		void write_ini_braced_object(
			std::ostream& a_outputStream, basic_ini_object<TAllocator> const& a_object)
		{
			a_outputStream << "{ ";
			auto isFirst = true;
			for (auto const& entry : a_object.data)
			{
				if (!std::exchange(isFirst, false))
				{
					a_outputStream << ", ";
				}
				write_ini_text(a_outputStream, entry.first.view(), true);
				a_outputStream << " = ";
				write_ini_value(a_outputStream, entry.second, true);
			}
			a_outputStream << " }";
		}

		template <typename TAllocator>
		void write_ini_array(
			std::ostream& a_outputStream, basic_ini_array<TAllocator> const& a_array)
		{
			a_outputStream << "[ ";
			auto isFirst = true;
			for (auto const& element : a_array.data)
			{
				if (!std::exchange(isFirst, false))
				{
					a_outputStream << ", ";
				}
				write_ini_value(a_outputStream, element, true);
			}
			a_outputStream << " ]";
		}

		template <typename TAllocator>
		void write_ini_value(
			std::ostream& a_outputStream, basic_ini_value<TAllocator> const& a_value, bool a_isNested)
		{
			if (auto const* const object = a_value.template get<basic_ini_object<TAllocator>>())
			{
				write_ini_braced_object(a_outputStream, *object);
				return;
			}
			if (auto const* const array = a_value.template get<basic_ini_array<TAllocator>>())
			{
				write_ini_array(a_outputStream, *array);
				return;
			}
			if (auto const* const literal = a_value.template get<basic_ini_literal<TAllocator>>())
			{
				write_ini_text(a_outputStream, literal->value, a_isNested);
				return;
			}
			a_outputStream << "\"\"";
		}
	}

	template <typename TAllocator>
	std::ostream& operator<<(
		std::ostream& a_outputStream, basic_ini_object<TAllocator> const& a_document)
	{
		for (auto const& entry : a_document.data)
		{
			auto const* const section = entry.second.template get<basic_ini_object<TAllocator>>();
			if (section == nullptr)
			{
				a_outputStream.setstate(std::ios_base::failbit);
				return a_outputStream;
			}

			a_outputStream << '[' << entry.first.view() << "]\n";
			for (auto const& sectionEntry : section->data)
			{
				detail::write_ini_text(a_outputStream, sectionEntry.first.view(), false);
				a_outputStream << " = ";
				detail::write_ini_value(a_outputStream, sectionEntry.second, false);
				a_outputStream << '\n';
			}
			a_outputStream << '\n';
		}
		return a_outputStream;
	}
}
