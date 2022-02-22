#pragma once

#include "container.h"
#include "dynamic_value.h"
#include "factory.h"
#include "index_value_pair.h"
#include "name_value_pair.h"
#include "size_tag.h"

#include "../hash/string_id.h"
#include "../std/enum_traits.h"
#include "../std/ignorable_assert.h"
#include "../std/polymorphic_ptr.h"
#include "../std/vector_map.h"
#include "../std/vector_set.h"
#include "../type/clone.h"

#include <array>
#include <filesystem>
#include <memory>
#include <utility>
#include <unordered_map>
#include <variant>


namespace vob::misvi
{
#pragma region std::pair
	/// @brief TODO
	template <typename TVisitor, typename TLeft, typename TRight>
	bool accept(TVisitor& a_visitor, std::pair<TLeft, TRight>& a_pair)
	{
		auto result = true;
		result &= a_visitor.visit(nvp("first", a_pair.first));
		result &= a_visitor.visit(nvp("second", a_pair.second));
		return result;
	}

	/// @brief TODO
	template <typename TVisitor, typename TLeft, typename TRight>
	bool accept(TVisitor& a_visitor, std::pair<TLeft, TRight> const& a_pair)
	{
		a_visitor.visit(nvp("first", a_pair.first));
		a_visitor.visit(nvp("second", a_pair.second));
		return true;
	}
#pragma endregion

#pragma region std::unordered_map
	template <
		typename TVisitor,
		typename TKey,
		typename TValue,
		typename THash,
		typename TEqualTo,
		typename TAllocator,
		typename TFactory = factory<std::pair<TKey, TValue>>>
	bool accept(
		TVisitor& a_visitor,
		std::unordered_map<TKey, TValue, THash, TEqualTo, TAllocator>& a_map,
		TFactory a_factory = {})
	{
		size_tag sizeTag{};
		if (!a_visitor.visit(sizeTag))
		{
			return false;
		}
		a_map.clear();
		a_map.reserve(sizeTag.size);
		for (auto index = 0u; index < sizeTag.size; ++index)
		{
			auto pair = a_factory();
			a_visitor.visit(ivp(index, pair));
			a_map.emplace(std::move(pair.first), std::move(pair.second));
		}
		return true;
	}

	template <
		typename TVisitor,
		typename TKey,
		typename TValue,
		typename THash,
		typename TEqualTo,
		typename TAllocator>
	bool accept(TVisitor& a_visitor, std::unordered_map<TKey, TValue, THash, TEqualTo, TAllocator> const& a_map)
	{
		size_tag sizeTag{ a_map.size() };
		a_visitor.visit(sizeTag);
		auto index = 0u;
		for (auto pair : a_map)
		{
			a_visitor.visit(ivp(index++, pair));
		}
		return true;
	}
#pragma endregion

#pragma region mistd::vector_map
	template <
		typename TVisitor,
		typename TKey,
		typename TValue,
		typename TKeyEqual,
		typename TAllocator,
		typename TFactory = factory<std::pair<TKey, TValue>>>
	bool accept(
		TVisitor& a_visitor,
		mistd::vector_map<TKey, TValue, TKeyEqual, TAllocator>& a_map,
		TFactory a_factory = {})
	{
		size_tag sizeTag{};
		if (!a_visitor.visit(sizeTag))
		{
			return false;
		}
		a_map.clear();
		a_map.reserve(sizeTag.size);
		for (auto index = 0u; index < sizeTag.size; ++index)
		{
			auto pair = a_factory();
			a_visitor.visit(ivp(index, pair));
			a_map.emplace(std::move(pair.first), std::move(pair.second));
		}
		return true;
	}

	template <typename TVisitor, typename TKey, typename TValue, typename TKeyEqual, typename TAllocator>
	bool accept(TVisitor& a_visitor, mistd::vector_map<TKey, TValue, TKeyEqual, TAllocator> const& a_map)
	{
		size_tag sizeTag{ a_map.size() };
		auto index = 0u;
		for (auto pair : a_map)
		{
			a_visitor.visit(ivp(index++, pair));
		}
		return true;
	}
#pragma endregion

#pragma region mistd::vector_set
	template <
		typename TVisitor,
		typename TKey,
		typename TKeyEqual,
		typename TAllocator,
		typename TFactory = factory<TKey>>
	bool accept(TVisitor& a_visitor, mistd::vector_set<TKey, TKeyEqual, TAllocator>& a_set, TFactory a_factory = {})
	{
		size_tag sizeTag{};
		a_visitor.visit(sizeTag);
		a_set.clear();
		a_set.reserve(sizeTag.size);
		for (auto index = 0u; index < sizeTag.size; ++index)
		{
			auto item = a_factory();
			a_visitor.visit(ivp(index, item));
			a_set.emplace(std::move(item));
		}
		return true;
	}

	template <typename TVisitor, typename TKey, typename TKeyEqual, typename TAllocator>
	bool accept(TVisitor& a_visitor, mistd::vector_set<TKey, TKeyEqual, TAllocator> const& a_set)
	{
		size_tag sizeTag{ a_set.size() };
		a_visitor.visit(sizeTag);
		auto index = 0u;
		for (auto item : a_set)
		{
			a_visitor.visit(ivp(index++, item));
		}
		return true;
	}
#pragma endregion

#pragma region std::vector
	template <typename TVisitor, typename TValue, typename TAllocator, typename TFactory = factory<TValue>>
	bool accept(TVisitor& a_visitor, std::vector<TValue, TAllocator>& a_vector, TFactory a_factory = {})
	{
		size_tag sizeTag{};
		if (!a_visitor.visit(sizeTag))
		{
			return false;
		}
		a_vector.clear();
		a_vector.reserve(sizeTag.size);
		for (auto index = 0u; index < sizeTag.size; ++index)
		{
			auto item = a_factory();
			a_visitor.visit(ivp(index, item));
			a_vector.emplace_back(std::move(item));
		}
		return true;
	}

	template <typename TVisitor, typename TValue, typename TAllocator>
	bool accept(TVisitor& a_visitor, std::vector<TValue, TAllocator> const& a_vector)
	{
		size_tag sizeTag{ a_vector.size() };
		a_visitor.visit(sizeTag);
		auto index = 0u;
		for (auto item : a_vector)
		{
			a_visitor.visit(ivp(index++, item));
		}
		return true;
	}
#pragma endregion

#pragma region std::array
	template <typename TVisitor, typename TValue, std::size_t t_size>
	bool accept(TVisitor& a_visitor, std::array<TValue, t_size>& a_array)
	{
		size_tag sizeTag{};
		if (!a_visitor.visit(sizeTag))
		{
			return false;
		}
		for (auto index = 0u; index < t_size; ++index)
		{
			a_visitor.visit(ivp(index, a_array[index]));
		}
		return true;
	}

	template <typename TVisitor, typename TValue, std::size_t t_size>
	bool accept(TVisitor& a_visitor, std::array<TValue, t_size> const& a_array)
	{
		size_tag sizeTag{ t_size };
		a_visitor.visit(sizeTag);
		auto index = 0u;
		for (auto item : a_array)
		{
			a_visitor.visit(ivp(index++, item));
		}
		return true;
	}
#pragma endregion

#pragma region std::variant
	template <typename TVisitor, typename TFactory, typename... TValues>
	bool accept(TVisitor& a_visitor, std::variant<TValues...>& a_variant, TFactory a_factory)
	{
		std::size_t index{ a_variant.index() };
		if (!a_visitor.visit(nvp("index", index)))
		{
			return false;
		}

		a_variant = a_factory(index);
		return std::visit([a_visitor](auto&& a_value)
		{
			return a_visitor.visit(nvp("data", a_value));
		}, a_variant);
	}

	template <typename TVisitor, typename... TValues>
	bool accept(TVisitor& a_visitor, std::variant<TValues...>& a_variant)
	{
		return accept(a_visitor, a_variant, factory_variant<TValues...>{});
	}

	template <typename TVisitor, typename... TValues>
	bool accept(TVisitor& a_visitor, std::variant<TValues...> const& a_variant)
	{
		std::size_t index{ a_variant.index() };
		a_visitor.visit(nvp("index", index));

		std::visit([&a_visitor](auto&& a_value)
		{
			a_visitor.visit(nvp("data", a_value));
		});
		return true;
	}
#pragma endregion

#pragma region container
	template <typename TVisitor, typename TContainer, typename TFactory>
	bool accept(TVisitor& a_visitor, container<TContainer, TFactory> a_container)
	{
		return accept(a_visitor, a_container.container, a_container.factory);
	}

	template <typename TVisitor, typename TContainer, typename TFactory>
	requires std::is_const_v<TContainer>
	bool accept(TVisitor& a_visitor, container<TContainer, TFactory> a_container)
	{
		accept(a_visitor, a_container.container);
		return true;
	}
#pragma endregion

#pragma region std::shared_ptr / mistd::polymorphic_ptr
	namespace detail
	{
		template <typename TVisitor>
		void write_type_id(TVisitor& a_visitor, mishs::string_id const a_id)
		{
			a_visitor.visit(nvp("type_id", a_id));
		}

		template <typename TVisitor, typename TPointer>
		bool visit_data(TVisitor& a_visitor, typename TPointer& a_ptr)
		{
			if (a_ptr == nullptr)
			{
				return false;
			}
			auto dynamicValue = dnv(*a_ptr);
			a_visitor.visit(nvp("data", dynamicValue));
			return true;
		}
	}

	template <typename TVisitor, typename TBase>
	bool accept(TVisitor& a_visitor, std::shared_ptr<TBase>& a_ptr)
	{
		auto const& factory = a_visitor.get_factory();
		auto const& registry = factory.get_registry();
		mishs::string_id id;
		if (!a_visitor.visit(nvp("type_id", id)))
		{
			return false;
		}

		a_ptr = factory.template create_shared<TBase>(id);
		ignorable_assert(a_ptr != nullptr);
		return detail::visit_data(a_visitor, a_ptr);
	}

	template <typename TVisitor, typename TBase>
	bool accept(TVisitor& a_visitor, std::shared_ptr<TBase> const& a_ptr)
	{
		static_assert(false && "TODO");
		return true;
	}

	template <typename TVisitor, typename TBase>
	bool accept(TVisitor& a_visitor, mistd::polymorphic_ptr<TBase>& a_ptr)
	{
		auto const& factory = a_visitor.get_factory();
		auto const& registry = factory.get_registry();
		mishs::string_id id;
		if (!a_visitor.visit(nvp("type_id", id)))
		{
			return false;
		}

		a_ptr = factory.template create<TBase>(id);
		ignorable_assert(a_ptr != nullptr);
		detail::visit_data(a_visitor, a_ptr);
		return true;
	}

	template <typename TVisitor, typename TBase>
	bool accept(TVisitor& a_visitor, mistd::polymorphic_ptr<TBase> const& a_ptr)
	{
		static_assert(false && "TODO");
		return true;
	}

	template <typename TVisitor, typename TBase>
	bool accept(TVisitor& a_visitor, dynamic_value<TBase> const& a_value)
	{
		a_visitor.get_applicator().apply(a_value.value, a_visitor);
		return true;
	}
#pragma endregion

#pragma region misty::clone
	template <typename TVisitor, typename TValue, typename TCloner>
	bool accept(TVisitor& a_visitor, misty::clone<TValue, TCloner>& a_clone)
	{
		mistd::polymorphic_ptr<TValue> ptr;
		if (!accept(a_visitor, ptr))
		{
			return false;
		}
		a_clone.reset(std::move(ptr));
		return true;
	}

	template <typename TVisitor, typename TValue, typename TCloner>
	bool accept(TVisitor& a_visitor, misty::clone<TValue, TCloner> const& a_clone)
	{
		accept(a_visitor, a_clone.get_representation());
		return true;
	}
#pragma endregion

#pragma region std::filesystem::path
	template <typename TVisitor>
	bool accept(TVisitor& a_visitor, std::filesystem::path& a_path)
	{
		std::string pathStr;
		if (!a_visitor.visit(pathStr))
		{
			return false;
		}
		a_path.assign(pathStr);
		return true;
	}

	template <typename TVisitor>
	bool accept(TVisitor& a_visitor, std::filesystem::path const& a_path)
	{
		a_visitor.visit(a_path.native());
		return true;
	}
#pragma endregion

#pragma region enum
	template <typename TVisitor, typename TEnum>
	requires std::is_enum_v<TEnum>
	bool accept(TVisitor& a_visitor, TEnum& a_value)
	{
		// TODO -> only for "text" visitors?
		// TODO -> allocator? read string_view?
		std::string valueStr;
		if (!a_visitor.visit(valueStr))
		{
			return false;
		}
		auto optionalValue = mistd::enum_traits<TEnum>::cast(valueStr);
		if (optionalValue == std::nullopt)
		{
			return false;
		}
		a_value = optionalValue.value();
		return true;
	}

	template <typename TVisitor, typename TEnum>
	requires std::is_enum_v<TEnum>
	bool accept(TVisitor& a_visitor, TEnum const& a_value)
	{
		// TODO -> only for "text" visitors?
		a_visitor.visit(mistd::enum_traits<TEnum>::cast(a_value).value_or(""));
		return true;
	}
#pragma endregion

#pragma region std::optional
	template <typename TVisitor, typename TValue, typename TFactory = factory<TValue>>
	bool accept(TVisitor& a_visitor, std::optional<TValue>& a_optional, TFactory a_factory = {})
	{
		bool hasValue = false;
		if (!a_visitor.visit(nvp("has_value", hasValue)))
		{
			return false;
		}
		if (hasValue)
		{
			TValue value = a_factory();
			if (!a_visitor.visit(nvp("value", value)))
			{
				return false;
			}
			a_optional = std::move(value);
		}
		return true;
	}

	template <typename TVisitor, typename TValue>
	bool accept(TVisitor& a_visitor, std::optional<TValue> const& a_optional)
	{
		a_visitor.visit(nvp("has_value", a_optional.has_value()));
		if (a_visitor.has_value())
		{
			a_visitor.visit(nvp("value", a_optional.value()));
		}
		return true;
	}
#pragma endregion

#pragma region mishs::string_id
	template <typename TVisitor>
	bool accept(TVisitor& a_visitor, mishs::string_id& a_id)
	{
		// TODO -> only for "text" visitors?
		// TODO -> allocator? read string_view?
		std::string valueStr;
		if (a_visitor.visit(valueStr))
		{
			a_id.assign(valueStr);
			return true;
		}
		
		std::uint64_t id = 0;
		if (a_visitor.visit(id))
		{
			a_id.assign(id);
			return true;
		}
		
		return false;
	}

	template <typename TVisitor>
	bool accept(TVisitor& a_visitor, mishs::string_id const& a_id)
	{
		// TODO -> only for "text" visitors?
#if VOB_MISHS_STRING_ID_DEBUG
		if (!a_id.is_debug_truncated())
		{
			return a_visitor.visit(*a_id.get_debug());
		}
#endif
		return a_visitor.visit(a_id.get_id());
	}
#pragma endregion
}
