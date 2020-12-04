#pragma once

#include <iomanip>

#include "value.h"


namespace vob::json
{
	class saver
	{
	public:
		void set_indentation(string a_indentation_string)
		{
			m_indentation = std::move(a_indentation_string);
		}

		void save_to(std::ostream& a_os, value const& a_value) const
		{
			m_indentation_level = 0;
			save(a_os, a_value);
		}

	private:
		mutable int m_indentation_level{ 0 };
		string m_indentation{ '\t' };

		void save(std::ostream& a_os, value const& a_value) const
		{
			if (auto const t_null = std::get_if<null>(&a_value))
			{
				save(a_os, *t_null);
			}
			else if (auto const t_boolean = std::get_if<boolean>(&a_value))
			{
				save(a_os, *t_boolean);
			}
			else if (auto const t_string = std::get_if<string>(&a_value))
			{
				save(a_os, *t_string);
			}
			else if (auto const t_number = std::get_if<number>(&a_value))
			{
				save(a_os, *t_number);
			}
			else if (auto const t_array = std::get_if<array>(&a_value))
			{
				save(a_os, *t_array);
			}
			else if (auto const t_object = std::get_if<object>(&a_value))
			{
				save(a_os, *t_object);
			}
		}

		static void save(std::ostream& a_os, null const& a_null)
		{
			a_os << "null";
		}

		static void save(std::ostream& a_os, boolean const& a_boolean)
		{
			a_os << (a_boolean ? "true" : "false");
		}

		static void save(std::ostream& a_os, string const& a_string)
		{
			a_os << std::quoted(a_string);
		}

		static void save(std::ostream& a_os, number const& a_number)
		{
			a_os << a_number.m_representation;
		}

		void save(std::ostream& a_os, array const& a_array) const
		{
			if (a_array.m_values.empty())
			{
				a_os << "[]";
				return;
			}
			a_os << '[';
			++m_indentation_level;
			auto t_first_item = true;
			for (auto const& t_item : a_array.m_values)
			{
				if (t_first_item)
				{
					a_os << '\n';
					t_first_item = false;
				}
				else
				{
					a_os << ",\n";
				}
				save_indentation(a_os);
				save(a_os, t_item);
			}
			a_os << '\n';
			--m_indentation_level;
			save_indentation(a_os);
			a_os << ']';
		}

		void save(std::ostream& a_os, object const& a_object) const
		{
			if (a_object.m_values.empty())
			{
				a_os << "{}";
				return;
			}
			a_os << '{';
			++m_indentation_level;
			auto t_first_item = true;
			for (auto const& t_item : a_object.m_values)
			{
				if (t_first_item)
				{
					a_os << '\n';
					t_first_item = false;
				}
				else
				{
					a_os << ",\n";
				}
				save_indentation(a_os);

				if (auto const t_key = std::get_if<string>(&t_item.first))
				{
					a_os << std::quoted(*t_key);
				}
				else
				{
					a_os << std::quoted(std::get<std::string_view>(t_item.first));
				}
				a_os << ": ";
				save(a_os, t_item.second);
			}
			a_os << '\n';
			--m_indentation_level;
			save_indentation(a_os);
			a_os << '}';
		}

		void save_indentation(std::ostream& a_os) const
		{
			for (auto t_k = 0; t_k < m_indentation_level; ++t_k)
			{
				a_os << m_indentation;
			}
		}
	};

}
