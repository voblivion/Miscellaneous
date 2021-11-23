#pragma once

#include "registry.h"

#include "../std/polymorphic_ptr.h"
#include "../std/polymorphic_ptr_util.h"

#include <utility>
#include <unordered_map>


namespace vob::misty
{
	namespace detail
	{
		/// @brief TODO
		template <typename TPolymorphicBase, typename TAllocator>
		class type_factory_base
		{
		public:
#pragma region CREATORS
			/// @brief TODO
			virtual ~type_factory_base() = default;
#pragma endregion

#pragma region ACCESSORS
			/// @brief TODO
			virtual mistd::polymorphic_ptr<TPolymorphicBase> create(TAllocator const& a_allocator) = 0;

			/// @brief TODO
			virtual std::shared_ptr<TPolymorphicBase> create_shared(TAllocator const& a_allocator) = 0;
#pragma endregion
		};

		/// @brief TODO
		template <typename TPolymorphicBase, typename TAllocator, typename TValue, typename... TArgs>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		class type_factory final
			: public type_factory_base<TPolymorphicBase, TAllocator>
		{
		public:
#pragma region CREATORS
			/// @brief TODO
			explicit type_factory(TArgs&&... a_args)
				: m_args{ std::forward<TArgs>(a_args)... }
			{}
#pragma endregion

#pragma region ACCESSORS
			/// @brief TODO
			mistd::polymorphic_ptr<TPolymorphicBase> create(TAllocator const& a_allocator) override
			{
				return create_impl(a_allocator, std::index_sequence_for<TArgs...>{});
			}

			/// @brief TODO
			std::shared_ptr<TPolymorphicBase> create_shared(TAllocator const& a_allocator) override
			{
				return create_shared_impl(a_allocator, std::index_sequence_for<TArgs...>{});
			}
#pragma endregion

		private:
#pragma region PRIVATE_DATA
			std::tuple<TArgs...> m_args;
#pragma endregion

#pragma region PRIVATE_ACCESSORS
			/// @brief TODO
			template <std::size_t... t_indices>
			mistd::polymorphic_ptr<TPolymorphicBase> create_impl(
				TAllocator const& a_allocator, std::index_sequence<t_indices...>)
			{
				return mistd::polymorphic_ptr_util::allocate<TValue>(
					a_allocator, std::forward<TArgs>(std::get<t_indices>(m_args))...);
			}

			/// @brief TODO
			template <std::size_t... t_indices>
			std::shared_ptr<TPolymorphicBase> create_shared_impl(
				TAllocator const& a_allocator, std::index_sequence<t_indices...>)
			{
				return std::allocate_shared<TValue>(
					a_allocator, std::forward<TArgs>(std::get<t_indices>(m_args))...);
			}
#pragma endregion
		};
	}

	/// @brief TODO
	template <
		typename TPolymorphicBase,
		typename TRegistry = registry const&,
		typename TCreateAllocator = std::allocator<TPolymorphicBase>,
		typename TAllocator = TCreateAllocator>
	class factory
	{
#pragma region PRIVATE_TYPES
		using type_factory_base = detail::type_factory_base<TPolymorphicBase, TCreateAllocator>;

		using type_factory_map_allocator = std::allocator_traits<TAllocator>::template rebind_alloc<
			std::pair<std::type_index const, mistd::polymorphic_ptr<type_factory_base>>>;

		template <typename TValue, typename... TArgs>
		using type_factory = detail::type_factory<TPolymorphicBase, TCreateAllocator, TValue, TArgs...>;
#pragma endregion
	public:
#pragma region CREATORS
		/// @brief TODO
		explicit factory(
			TRegistry a_registry,
			TCreateAllocator const& a_createAllocator = {},
			TAllocator const& a_allocator = {})
			: m_registry{ std::move(a_registry) }
			, m_createAllocator{ a_createAllocator }
			, m_typeFactories{ a_allocator }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		auto const& get_registry() const
		{
			return m_registry;
		}

		/// @brief TODO
		auto get_create_allocator() const
		{
			return m_createAllocator;
		}

		/// @brief TODO
		auto get_allocator() const
		{
			return m_typeFactories.get_allocator();
		}

		/// @brief TODO
		template <typename TValue>
		mistd::polymorphic_ptr<TValue> create(std::type_index const a_typeIndex) const
		{
			if (!m_registry.is_base_of<TValue>(a_typeIndex))
			{
				return nullptr;
			}

			auto const typeFactoryIt = m_typeFactories.find(a_typeIndex);
			if (typeFactoryIt == m_typeFactories.end())
			{
				return nullptr;
			}

			return mistd::polymorphic_ptr_util::cast<TValue>(typeFactoryIt->second->create(m_createAllocator));
		}

		/// @brief TODO
		template <typename TValue>
		mistd::polymorphic_ptr<TValue> create(mishs::string_id const a_id) const
		{
			auto typeIndex = m_registry.find_type_index(a_id);
			return typeIndex != nullptr ? create<TValue>(*typeIndex) : nullptr;
		}

		/// @brief TODO
		template <typename TValue>
		std::shared_ptr<TValue> create_shared(std::type_index const a_typeIndex) const
		{
			if (!m_registry.is_base_of<TValue>(a_typeIndex))
			{
				return nullptr;
			}

			auto const typeFactoryIt = m_typeFactories.find(a_typeIndex);
			if (typeFactoryIt == m_typeFactories.end())
			{
				return nullptr;
			}

			return std::static_pointer_cast<TValue>(typeFactoryIt->second->create_shared(m_createAllocator));
		}

		/// @brief TODO
		template <typename TValue>
		mistd::polymorphic_ptr<TValue> create_shared(mishs::string_id const a_id) const
		{
			auto typeIndex = m_registry.find_type_index(a_id);
			return typeIndex != nullptr ? create_shared<TValue>(*typeIndex) : nullptr;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		auto& get_registry()
		{
			return m_registry;
		}

		/// @brief TODO
		template <typename TValue, typename... TArgs>
		void add_type(TArgs&&... a_args)
		{
			auto allocator = m_typeFactories.get_allocator();
			m_typeFactories.emplace(
				typeid(TValue),
				mistd::polymorphic_ptr_util::allocate<type_factory<TValue, TArgs...>>(
					m_createAllocator, std::forward<TArgs>(a_args)...));
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TRegistry const m_registry;
		
		TCreateAllocator m_createAllocator;

		std::unordered_map<
			std::type_index,
			mistd::polymorphic_ptr<type_factory_base>,
			std::hash<std::type_index>,
			std::equal_to<>,
			type_factory_map_allocator> m_typeFactories;
#pragma endregion
	};

	namespace pmr
	{
		/// @brief TODO
		template <typename TPolymorphicBase>
		using factory = misty::factory<
			TPolymorphicBase, registry const&, std::pmr::polymorphic_allocator<TPolymorphicBase>>;
	}
}
