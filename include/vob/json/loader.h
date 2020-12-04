#pragma once

#include <iomanip>

#include "value.h"


namespace vob::json
{
	enum class load_error
	{
		none = 0, // No error
		extra_character_found, // A json value can be read but there are extra chars
		invalid_character_found, // Character read was an expected Json Value
		expect_object_key, // Json Object elements must be indexed by "key"
		expect_colon_separator, // Json Object key must be followed by a colon
		expect_coma_separator, // Between Object/Array elements, a comma is expected
		bad_boolean_format, // Read t*** or f**** but not true/false
		bad_null_format, // Read n*** but not null
		missing_leading_digit, // A number must start by a (signed) digit
		missing_decimal, // In number after "." must be a digit
		missing_exponent, // In number after "e"/"E" must be a digit
	};

	class loader
	{
	public:
		#pragma region Methods
		template <typename Char8AllocatorType = std::allocator<char8>>
		auto load_from(std::istream& a_is, Char8AllocatorType a_allocator = {})
		{
			using value_type = value<Char8AllocatorType>;
			m_error = load_error::none;

			value_type t_value;
			load(a_is, t_value);

			a_is >> std::ws;
			if (m_error == load_error::none && !a_is.eof())
			{
				set_error(a_is, load_error::extra_character_found);
			}

			return t_value;
		}

		auto load_from(std::istream& a_is)
		{
			return load_from<>(a_is);
		}

		load_error get_error() const
		{
			return m_error;
		}
		
		std::size_t get_error_position() const
		{
			return m_error_position;
		}
		#pragma endregion

	private:
		load_error m_error{ load_error::none };
		std::size_t m_error_position{ 0 };

		template <typename Char8AllocatorType>
		void load(std::istream& a_is, value<Char8AllocatorType>& a_value)
		{
			using string_type = string<Char8AllocatorType>;
			using object_type = object<Char8AllocatorType>;
			using array_type = array<Char8AllocatorType>;
			using number_type = number<Char8AllocatorType>;

			a_is >> std::ws;
			switch (a_is.peek())
			{
			case '"': // String
				a_value = string_type{};
				load(a_is, std::get<string_type>(a_value));
				break;
			case '{': // Object
				a_value = object_type{};
				load(a_is, std::get<object_type>(a_value));
				break;
			case '[': // Array
				a_value = array_type{};
				load(a_is, std::get<array_type>(a_value));
				break;
			case 't': // Boolean
			case 'f':
				a_value = boolean{};
				load(a_is, std::get<boolean>(a_value));
				break;
			case 'n': // Null
				a_value = null{};
				load(a_is, std::get<null>(a_value));
				break;
			default: // Number
				a_value = number_type{};
				load(a_is, std::get<number_type>(a_value));
				break;
			}
		}

		template <typename Char8AllocatorType>
		void load(std::istream& a_is, string<Char8AllocatorType>& a_string)
		{
			if (a_is.peek() != '"') { set_error(a_is, load_error::expect_object_key); return; }
			a_is >> std::quoted(a_string);
			// Todo detect missing '\"'
		}

		template <typename Char8AllocatorType>
		void load(std::istream& a_is, object<Char8AllocatorType>& a_object)
		{
			using string_type = string<Char8AllocatorType>;
			using value_type = value<Char8AllocatorType>;

			a_is.get();
			a_is >> std::ws;
			auto c = a_is.peek();
			while (c != '}')
			{
				string_type t_key;
				load(a_is, t_key);
				if (m_error != load_error::none) { return; }
				a_is >> std::ws;
				c = a_is.get();
				if (c != ':') { set_error(a_is, load_error::expect_colon_separator); return; }
				value_type t_value;
				load(a_is, t_value);
				if (m_error != load_error::none) { return; }
				a_object.m_values.emplace(t_key, t_value);
				a_is >> std::ws;
				c = a_is.peek();
				if (c == ',')
				{
					a_is.get();
					a_is >> std::ws;
					c = a_is.peek();
				}
				else if (c != '}') { set_error(a_is, load_error::expect_coma_separator); return; }
			}
			a_is.get();
		}

		template <typename Char8AllocatorType>
		void load(std::istream& a_is, array<Char8AllocatorType>& a_array)
		{
			using value_type = value<Char8AllocatorType>;

			a_is.get();
			a_is >> std::ws;
			auto c = a_is.peek();
			while (c != ']')
			{
				value_type t_value;
				load(a_is, t_value);
				if (m_error != load_error::none) { return; }
				a_array.m_values.emplace_back(t_value);
				a_is >> std::ws;
				c = a_is.peek();
				if (c == ',')
				{
					a_is.get();
					a_is >> std::ws;
					c = a_is.peek();
				}
				else if (c != ']') { set_error(a_is, load_error::expect_coma_separator); return; }
			}
			a_is.get();
		}

		void load(std::istream& a_is, boolean& a_boolean)
		{
			if (a_is.get() == 't')
			{
				if (a_is.get() != 'r') { set_error(a_is, load_error::bad_boolean_format); return; }
				if (a_is.get() != 'u') { set_error(a_is, load_error::bad_boolean_format); return; }
				if (a_is.get() != 'e') { set_error(a_is, load_error::bad_boolean_format); return; }
			}
			else
			{
				if (a_is.get() != 'a') { set_error(a_is, load_error::bad_boolean_format); return; }
				if (a_is.get() != 'l') { set_error(a_is, load_error::bad_boolean_format); return; }
				if (a_is.get() != 's') { set_error(a_is, load_error::bad_boolean_format); return; }
				if (a_is.get() != 'e') { set_error(a_is, load_error::bad_boolean_format); return; }
			}
		}

		void load(std::istream& a_is, null& a_null)
		{
			a_is.get();
			if (a_is.get() != 'u') { set_error(a_is, load_error::bad_null_format); return; }
			if (a_is.get() != 'l') { set_error(a_is, load_error::bad_null_format); return; }
			if (a_is.get() != 'l') { set_error(a_is, load_error::bad_null_format); return; }
		}

		template <typename Char8AllocatorType>
		void load(std::istream& a_is, number<Char8AllocatorType>& a_number)
		{
			a_is >> std::ws;
			auto const try_read_range = [&a_is, &a_number](char const a_min = '0', char const a_max = '9') {
				auto const c = a_is.peek();
				if (c >= a_min && c <= a_max)
				{
					a_number.m_representation += static_cast<char>(a_is.get());
					return true;
				}
				return false;
			};

			auto const t_is_negative = try_read_range('-', '-');
			if (try_read_range('0', '0')) {}
			else
			{
				if (!try_read_range('1'))
				{
					if (t_is_negative) set_error(a_is, load_error::missing_leading_digit);
					else set_error(a_is, load_error::invalid_character_found);
					return;
				}
				while (try_read_range()) {}
			}

			if (try_read_range('.', '.'))
			{
				if (!try_read_range()) { set_error(a_is, load_error::missing_decimal); return; }
				while (try_read_range()) {}
			}

			if (try_read_range('e', 'e') || try_read_range('E', 'E'))
			{
				auto const c = a_is.peek();
				if (c == '-' || c == '+')
				{
					a_number.m_representation += static_cast<char>(a_is.get());
				}
				if (!try_read_range()) { set_error(a_is, load_error::missing_exponent); return; }
				while (try_read_range()) {}
			}

		}

		void set_error(std::istream& a_is, load_error a_error)
		{
			m_error = a_error;
			m_error_position = a_is.tellg();
		}
	};
}
