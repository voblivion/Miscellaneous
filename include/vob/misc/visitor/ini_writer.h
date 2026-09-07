#pragma once

#include "applicator.h"
#include "container.h"
#include "index_value_pair.h"
#include "is_visitable.h"
#include "name_value_pair.h"
#include "size_tag.h"

#include "../std/ini.h"

#include <cassert>
#include <charconv>
#include <deque>
#include <stack>


namespace vob::misvi
{
	template <
		typename TContext,
		typename TIniValue = mistd::ini_value,
		typename TApplicatorAllocator = std::allocator<char>,
		typename TStackAllocator = std::allocator<std::reference_wrapper<mistd::ini_value>>>
	class ini_writer
	{
		using self = ini_writer<
			TContext, TIniValue, TApplicatorAllocator, TStackAllocator>;
		using ini_value_ref = std::reference_wrapper<TIniValue>;

	public:
		ini_writer(
			applicator<true, self, TApplicatorAllocator> const& a_applicator,
			TContext a_context,
			TStackAllocator a_allocator = {})
			: m_applicator{ a_applicator }
			, m_context{ std::forward<TContext>(a_context) }
			, m_stack{ std::deque<ini_value_ref, TStackAllocator>{ a_allocator } }
		{}

		[[nodiscard]] auto const& get_applicator() const
		{
			return m_applicator;
		}

		[[nodiscard]] auto const& get_context() const
		{
			return m_context;
		}

		template <typename TValue>
		bool write(TIniValue& a_iniValue, TValue const& a_value)
		{
			assert(m_stack.empty());
			m_stack.emplace(a_iniValue);
			auto const result = visit(a_value);
			m_stack.pop();
			return result;
		}

		template <typename TValue>
		requires is_visitable_free<self, TValue const> && (!std::is_arithmetic_v<TValue>)
		bool visit(TValue const& a_value)
		{
			return accept(*this, a_value);
		}

		template <typename TValue>
		requires is_visitable_member<self, TValue const>
		bool visit(TValue const& a_value)
		{
			return a_value.accept(*this);
		}

		template <typename TValue>
		requires is_visitable_static<self, TValue const>
		bool visit(TValue const& a_value)
		{
			return TValue::accept(*this, a_value);
		}

		template <typename TValue>
		requires std::is_arithmetic_v<TValue>
		bool visit(TValue const& a_number)
		{
			std::array<char, 64> buffer;
			auto const result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), a_number);
			if (result.ec != std::errc{})
			{
				return false;
			}

			set_literal(std::string_view{ buffer.data(), result.ptr });
			return true;
		}

		bool visit(bool const& a_boolean)
		{
			set_literal(a_boolean ? "true" : "false");
			return true;
		}

		template <typename TChar, typename TCharTraits, typename TAllocator>
		bool visit(std::basic_string<TChar, TCharTraits, TAllocator> const& a_string)
		{
			set_literal(a_string);
			return true;
		}

		bool visit(std::string_view const a_string)
		{
			set_literal(a_string);
			return true;
		}

		bool visit(size_tag& a_sizeTag)
		{
			auto& array = current().template set<typename TIniValue::array_type>(
				current().get_allocator());
			array.data.reserve(a_sizeTag.size);
			return true;
		}

		template <typename TValue>
		bool visit(index_value_pair<TValue> a_indexValuePair)
		{
			auto const array = current().template get<typename TIniValue::array_type>();
			if (array == nullptr)
			{
				return false;
			}

			if (a_indexValuePair.index >= array->data.size())
			{
				array->data.emplace_back(TIniValue{ current().get_allocator() });
			}

			m_stack.emplace(array->data[a_indexValuePair.index]);
			auto result = visit(a_indexValuePair.value);
			m_stack.pop();
			return result;
		}

		template <typename TValue>
		bool visit(name_value_pair<TValue> a_nameValuePair)
		{
			m_stack.emplace(entry(a_nameValuePair.name));
			auto const result = visit(a_nameValuePair.value);
			m_stack.pop();
			return result;
		}

		template <typename TContainer, typename TFactory>
		bool visit(container<TContainer, TFactory> const& a_container)
		{
			return accept(*this, a_container);
		}

	private:
		TIniValue& current()
		{
			return m_stack.top().get();
		}

		typename TIniValue::object_type& ensure_object()
		{
			auto object = current().template get<typename TIniValue::object_type>();
			if (object == nullptr)
			{
				object = &current().template set<typename TIniValue::object_type>(
					current().get_allocator());
			}
			return *object;
		}

		TIniValue& entry(std::string_view const a_name)
		{
			auto& object = ensure_object();
			auto valueIt = object.data.find(a_name);
			if (valueIt == object.data.end())
			{
				using string_type = typename TIniValue::object_type::string_type;
				object.data.emplace(string_type{ a_name }, TIniValue{ current().get_allocator() });
				valueIt = object.data.find(a_name);
			}
			return valueIt->second;
		}

		void set_literal(std::string_view const a_text)
		{
			auto& literal = current().template set<typename TIniValue::literal_type>(
				current().get_allocator());
			literal.value.assign(a_text);
		}

		applicator<true, self, TApplicatorAllocator> const& m_applicator;
		TContext m_context;
		std::stack<ini_value_ref, std::deque<ini_value_ref, TStackAllocator>> m_stack;
	};

	namespace pmr
	{
		template <
			typename TContext,
			typename TIniValue = mistd::pmr::ini_value,
			typename TApplicatorAllocator = std::pmr::polymorphic_allocator<char>,
			typename TStackAllocator = std::pmr::polymorphic_allocator<
				std::reference_wrapper<mistd::pmr::ini_value>>>
		using ini_writer = ini_writer<
			TContext, TIniValue, TApplicatorAllocator, TStackAllocator>;
	}
}
