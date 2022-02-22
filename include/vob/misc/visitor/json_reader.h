#pragma once

#include "applicator.h"
#include "container.h"
#include "index_value_pair.h"
#include "is_visitable.h"
#include "name_value_pair.h"
#include "size_tag.h"

#include "../std/json.h"

#include <cassert>
#include <deque>
#include <stack>


namespace vob::misvi
{
	template <
		typename TJsonValue,
		typename TFactory,
		typename TApplicatorAllocator,
		typename TContext,
		typename TAllocator>
	class basic_json_reader
	{
#pragma region PRIVATE_TYPES
		using self = basic_json_reader<TJsonValue, TFactory, TApplicatorAllocator, TContext, TAllocator>;
		using json_value_ref = std::reference_wrapper<TJsonValue const>;
#pragma endregion
	public:

#pragma region TYPES
		using allocator = TAllocator;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_json_reader(
			TFactory const& a_factory,
			applicator<false, self, TApplicatorAllocator> const& a_applicator,
			TContext a_context,
			TAllocator a_allocator = {})
			: m_factory{ a_factory }
			, m_applicator{ a_applicator }
			, m_context{ std::forward<TContext>(a_context) }
			, m_stack{ std::deque<json_value_ref, TAllocator>{ a_allocator } }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		[[nodiscard]] auto const& get_factory() const
		{
			return m_factory;
		}

		/// @brief TODO
		[[nodiscard]] auto const& get_applicator() const
		{
			return m_applicator;
		}

		/// @brief TODO
		[[nodiscard]] auto const& get_context() const
		{
			return m_context;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		template <typename TValue>
		void read(TJsonValue const& a_jsonValue, TValue& a_value)
		{
			assert(m_stack.empty());
			m_stack.emplace(a_jsonValue);
			visit(a_value);
			m_stack.pop();
		}

		/// @brief TODO
		template <typename TValue>
		requires is_visitable_free<self, TValue> && (!std::is_arithmetic_v<TValue>)
		bool visit(TValue& a_value)
		{
			return accept(*this, a_value);
		}

		/// @brief TODO
		template <typename TValue>
		requires is_visitable_member<self, TValue>
		bool visit(TValue& a_value)
		{
			return a_value.accept(*this);
		}

		/// @brief TODO
		template <typename TValue>
		requires is_visitable_static<self, TValue>
		bool visit(TValue& a_value)
		{
			return TValue::accept(*this, a_value);
		}

		/// @brief TODO
		template <typename TValue>
		requires std::is_arithmetic_v<TValue>
		bool visit(TValue& a_number)
		{
			auto const& currentValue = m_stack.top().get();
			if (auto const number = currentValue.get<TJsonValue::number_type>())
			{
				std::visit([&a_number](auto const a_value){ a_number = static_cast<TValue>(a_value); }, number->value);
				return true;
			}
			return false;
		}

		/// @brief TODO
		bool visit(bool& a_boolean)
		{
			auto const& currentValue = m_stack.top().get();
			if (auto const boolean = currentValue.get<TJsonValue::boolean_type>())
			{
				a_boolean = boolean->value;
				return true;
			}
			return false;
		}

		/// @brief TODO
		template <typename TChar, typename TCharTraits, typename TAllocator>
		bool visit(std::basic_string<TChar, TCharTraits, TAllocator>& a_string)
		{
			auto const& currentValue = m_stack.top().get();
			if (auto const string = currentValue.get<TJsonValue::string_type>())
			{
				a_string.assign(string->value);
				return true;
			}
			return false;
		}

		/// @brief TODO
		bool visit(size_tag& a_sizeTag)
		{
			auto const& currentValue = m_stack.top().get();
			if (auto const array = currentValue.get<TJsonValue::array_type>())
			{
				a_sizeTag.size = array->data.size();
				return true;
			}
			return false;
		}

		/// @brief TODO
		template <typename TValue>
		bool visit(index_value_pair<TValue> a_indexValuePair)
		{
			// Current node is array
			auto const& currentValue = m_stack.top().get();
			auto const array = currentValue.get<TJsonValue::array_type>();
			if (array == nullptr)
			{
				return false;
			}

			// Index is valid
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

		/// @brief TODO
		template <typename TValue>
		bool visit(name_value_pair<TValue> a_nameValuePair)
		{
			// Current node is object
			auto const& currentValue = m_stack.top().get();
			auto const object = currentValue.get<TJsonValue::object_type>();
			if (object == nullptr)
			{
				return false;
			}

			// Key exists
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

		/// @brief TODO
		template <typename TContainer, typename TFactory>
		bool visit(container<TContainer, TFactory> const& a_container)
		{
			return accept(*this, a_container);
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TFactory const& m_factory;
		applicator<false, self, TApplicatorAllocator> const& m_applicator;
		TContext m_context;
		std::stack<json_value_ref, std::deque<json_value_ref, TAllocator>> m_stack;
#pragma endregion
	};

	/// @brief TODO
	template <typename TContext>
	using json_reader = basic_json_reader<
		mistd::json_value,
		misty::factory,
		std::allocator<char>,
		TContext,
		std::allocator<std::reference_wrapper<mistd::json_value const>>>;

	namespace pmr
	{
		/// @brief TODO
		template <typename TContext>
		using json_reader = basic_json_reader<
			mistd::pmr::json_value,
			misty::pmr::factory,
			std::pmr::polymorphic_allocator<char>,
			TContext,
			std::pmr::polymorphic_allocator<std::reference_wrapper<mistd::pmr::json_value const>>>;
	}
}
