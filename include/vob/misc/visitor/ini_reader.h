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
#include <optional>
#include <stack>


namespace vob::misvi
{
	template <
		typename TContext,
		typename TIniValue = mistd::ini_value,
		typename TApplicatorAllocator = std::allocator<char>,
		typename TStackAllocator = std::allocator<std::reference_wrapper<mistd::ini_value const>>>
	class ini_reader
	{
		using self = ini_reader<
			TContext, TIniValue, TApplicatorAllocator, TStackAllocator>;
		using ini_value_ref = std::reference_wrapper<TIniValue const>;

	public:
		ini_reader(
			applicator<false, self, TApplicatorAllocator> const& a_applicator,
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
		bool read(TIniValue const& a_iniValue, TValue& a_value)
		{
			assert(m_stack.empty());
			m_stack.emplace(a_iniValue);
			auto const result = visit(a_value);
			m_stack.pop();
			return result;
		}

		template <typename TValue>
		requires is_visitable_free<self, TValue> && (!std::is_arithmetic_v<TValue>)
		bool visit(TValue& a_value)
		{
			return accept(*this, a_value);
		}

		template <typename TValue>
		requires is_visitable_member<self, TValue>
		bool visit(TValue& a_value)
		{
			return a_value.accept(*this);
		}

		template <typename TValue>
		requires is_visitable_static<self, TValue>
		bool visit(TValue& a_value)
		{
			return TValue::accept(*this, a_value);
		}

		template <typename TValue>
		requires std::is_arithmetic_v<TValue>
		bool visit(TValue& a_number)
		{
			auto const literal = m_stack.top().get().template get<typename TIniValue::literal_type>();
			if (literal == nullptr)
			{
				return false;
			}

			auto const* const first = literal->value.data();
			auto const* const last = first + literal->value.size();
			auto const result = std::from_chars(first, last, a_number);
			return result.ec == std::errc{} && result.ptr == last;
		}

		bool visit(bool& a_boolean)
		{
			auto const literal = m_stack.top().get().template get<typename TIniValue::literal_type>();
			if (literal == nullptr)
			{
				return false;
			}

			if (literal->value == "true")
			{
				a_boolean = true;
				return true;
			}
			if (literal->value == "false")
			{
				a_boolean = false;
				return true;
			}
			return false;
		}

		template <typename TChar, typename TCharTraits, typename TAllocator>
		bool visit(std::basic_string<TChar, TCharTraits, TAllocator>& a_string)
		{
			auto const literal = m_stack.top().get().template get<typename TIniValue::literal_type>();
			if (literal == nullptr)
			{
				return false;
			}

			a_string.assign(literal->value);
			return true;
		}

		bool visit(size_tag& a_sizeTag)
		{
			auto const array = m_stack.top().get().template get<typename TIniValue::array_type>();
			if (array == nullptr)
			{
				return false;
			}

			a_sizeTag.size = array->data.size();
			return true;
		}

		template <typename TValue>
		bool visit(index_value_pair<TValue> a_indexValuePair)
		{
			auto const array = m_stack.top().get().template get<typename TIniValue::array_type>();
			if (array == nullptr)
			{
				return false;
			}

			auto const index = a_indexValuePair.index;
			if (index >= array->data.size())
			{
				return false;
			}

			m_stack.emplace(array->data[index]);
			auto result = visit(a_indexValuePair.value);
			m_stack.pop();
			return result;
		}

		template <typename TValue>
		bool visit(name_value_pair<TValue> a_nameValuePair)
		{
			auto const object = m_stack.top().get().template get<typename TIniValue::object_type>();
			if (object == nullptr)
			{
				return false;
			}

			auto const valueIt = object->data.find(a_nameValuePair.name);
			if (valueIt == object->data.end())
			{
				return false;
			}

			m_stack.emplace(valueIt->second);
			auto result = visit(a_nameValuePair.value);
			m_stack.pop();
			return result;
		}

		template <typename TValue>
		bool visit(name_value_pair<std::optional<TValue>> a_nameValuePair)
		{
			auto const object = m_stack.top().get().template get<typename TIniValue::object_type>();
			if (object == nullptr)
			{
				return false;
			}

			auto const valueIt = object->data.find(a_nameValuePair.name);
			if (valueIt == object->data.end())
			{
				a_nameValuePair.value.reset();
				return true;
			}

			m_stack.emplace(valueIt->second);
			auto const result = visit(a_nameValuePair.value.emplace());
			m_stack.pop();
			if (!result)
			{
				a_nameValuePair.value.reset();
			}
			return result;
		}

		template <typename TContainer, typename TFactory>
		bool visit(container<TContainer, TFactory> const& a_container)
		{
			return accept(*this, a_container);
		}

	private:
		applicator<false, self, TApplicatorAllocator> const& m_applicator;
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
				std::reference_wrapper<mistd::pmr::ini_value const>>>
		using ini_reader = ini_reader<
			TContext, TIniValue, TApplicatorAllocator, TStackAllocator>;
	}
}
