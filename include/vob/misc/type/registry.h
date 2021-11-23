#pragma once

#include "../hash/string_id.h"

#include <cassert>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <unordered_map>


namespace vob::misty
{
	/// @brief A class to handle & cast objects of types unknown at compilation.
	template <typename TAllocator>
	class basic_registry
	{
#pragma region PRIVATE_TYPES
		/// @brief Links the provided type identifier to its associated internal index.
		struct type_data
		{
			mishs::string_id id;
			std::size_t index;
		};

		using type_data_map_allocator = typename std::allocator_traits<TAllocator>
			::template rebind_alloc<std::pair<std::type_index const, type_data>>;
		
		using type_index_map_allocator = typename std::allocator_traits<TAllocator>
			::template rebind_alloc<std::pair<mishs::string_id const, std::type_index>>;

		using hierarchy_allocator = typename std::allocator_traits<TAllocator>
			::template rebind_alloc<std::size_t>;
#pragma endregion

#pragma region PRIVATE_CLASS_DATA
		constexpr std::size_t void_type_index = 0;
#pragma region

	public:
#pragma region CLASS_DATA
		constexpr mishs::string_id void_type_id = 0;
#pragma endregion

#pragma region CREATORS
		/// @brief Creates a basic_registry from the allocator used for its internal structure.
		explicit basic_registry(TAllocator const& a_allocator = {})
			: m_typeData{ type_data_map_allocator{ a_allocator } }
			, m_idToTypeIndex{ type_data_map_allocator{ a_allocator } }
			, m_hierarchy{ hierarchy_node_allocator{ a_allocator } }
		{
			m_typeData.emplace(typeid(void), type_data{ void_type_id, void_type_index });
			m_idToTypeIndex.emplace(0, typeid(void));
			m_hierarchy.emplace_back(hierarchy_node{ typeid(void), void_type_index });
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Returns whether or not a type has been previously registered to this registry.
		[[nodiscard]] bool is_registered(std::type_index const a_type) const
		{
			return m_typeData.find(a_type) != m_typeData.end();
		}

		/// @brief Returns whether or not a type has been previously registered to this registry.
		template <typename TValue>
		[[nodiscard]] bool is_registered() const
		{
			return is_registered(typeid(TValue));
		}

		/// @brief Returns whether or not an id has been already associated to a type registered to this registry.
		[[nodiscard]] bool is_used(mishs::string_id const a_id) const
		{
			return m_idToTypeIndex.find(a_id) != m_idToTypeIndex.end();
		}

		/// @brief Checks if a provided type is derived from another.
		[[nodiscard]] bool is_base_of(std::type_index const a_baseType, std::type_index const a_derivedType) const
		{
			assert(is_registered(a_baseType) && is_registered(a_derivedType));
			auto const baseTypeDataIt = m_typeData.find(a_baseType);
			auto const derivedTypeDataIt = m_typeData.find(a_derivedType);
			return is_base_of_impl(baseTypeDataIt->second.index, derivedTypeDataIt->second.index);
		}

		/// @brief Checks if a provided type is derived from another.
		template <typename TBase>
		[[nodiscard]] bool is_base_of(std::type_index const a_type) const
		{
			return is_base_of(typeid(TBase), a_type);
		}

		/// @brief Checks if a provided type is derived from another.
		template <typename TBase>
		[[nodiscard]] bool is_base_of(mishs::string_id const a_id) const
		{
			assert(is_used(a_id));
			return is_base_of<TBase>(m_idToTypeIndex.find(a_id)->second);
		}

		/// @brief Finds the std::type_index registered with provided id, or null if no type was registered with it.
		[[nodiscard]] std::type_index const* find_type_index(mishs::string_id const a_id) const
		{
			auto it = m_idToTypeIndex.find(a_id);
			return it != m_idToTypeIndex.end() ? &it->second : nullptr;
		}

		/// @brief Finds the registered id associated to the type of a provided std::type_index.
		[[nodiscard]] auto find_id(std::type_index const a_typeIndex) const
		{
			assert(is_registered(a_typeIndex));
			auto it = m_typeData.find(a_typeIndex);
			return it != m_typeData.end() ? it->second.m_id : void_type_id;
		}

		/// @brief Finds the registered id associated to the type of a provided type.
		template <typename TValue>
		[[nodiscard]] auto find_id() const
		{
			return find_id(typeid(TValue));
		}

		/// @brief Finds the registered id associated to the type of a provided object.
		template <typename TBase>
		[[nodiscard]] auto find_id(TBase* a_ptr) const
		{
			if (a_ptr == nullptr)
			{
				return void_type_id;
			}
			return find_id(typeid(*a_ptr));
		}

		/// @brief TODO
		/// @brief Safely casts a std::shared_pointer to provided type if hierarchy is known by the registry.
		template <typename TDerived, typename TBase>
		[[nodiscard]] auto fast_cast(std::shared_ptr<TBase> const& a_ptr) const -> std::shared_ptr<TDerived>
		{
			if (a_ptr != nullptr && is_base_of<TDerived>(typeid(*a_ptr)))
			{
				return std::static_pointer_cast<TDerived>(a_ptr);
			}
			return nullptr;
		}

		/// @brief Safely casts a pointer to provided type if hierarchy is known by the registry.
		template <typename TDerived, typename TBase>
		requires std::is_base_of_v<TBase, TDerived>
		[[nodiscard]] auto fast_cast(TBase* a_ptr) const -> TDerived*
		{
			if (a_ptr != nullptr && is_base_of<TDerived>(typeid(*a_ptr)))
			{
				return static_cast<TDerived*>(a_ptr);
			}
			return nullptr;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Registers a type without parent to the registry, and links it to provided id.
		template <typename TValue>
		void register_type(mishs::string_id const a_id)
		{
			assert(!is_registered<TValue>() && !is_used(a_id));
			m_typeData.emplace(typeid(TValue), type_data{ a_id, m_hierarchy.size() });
			m_idToTypeIndex.emplace(a_id, typeid(TValue));
			m_hierarchy.emplace_back(void_type_id);
		}

		/// @brief Registers a type and its parent to the registry, and links it to provided id.
		template <typename TDerived, typename TBase>
		requires std::is_base_of_v<TBase, TDerived>
		void register_type(mishs::string_id const a_id)
		{
			assert(!is_registered<TDerived>() && !is_used(a_id) && is_registered<TBase>());
			m_typeData.emplace(typeid(TDerived), type_data{ a_id, m_hierarchy.size() });
			m_idToTypeIndex.emplace(a_id, typeid(TDerived));
			m_hierarchy.emplace_back(m_typeData.find(typeid(TBase))->second.index);
		}
#pragma endregion

	private:
#pragma region DATA
		std::unordered_map<
			std::type_index,
			type_data,
			std::hash<std::type_index>,
			std::equal_to<>,
			type_data_map_allocator> m_typeData;

		std::unordered_map<
			mishs::string_id,
			std::type_index,
			mishs::string_id_hash,
			std::equal_to<>,
			type_index_map_allocator> m_idToTypeIndex;

		std::vector<std::size_t, hierarchy_allocator> m_hierarchy;
#pragma endregion	

#pragma region PRIVATE_ACCESSORS
		/// @brief Checks if a provided type is derived from another, from their internal indices.
		[[nodiscard]] bool is_base_of_impl(
			std::size_t const a_baseTypeIndex, std::size_t const a_derivedTypeIndex) const
		{
			if (a_derivedTypeIndex == a_baseTypeIndex)
			{
				return true;
			}
			else if (a_derivedTypeIndex == void_type_index)
			{
				return false;
			}
			else
			{
				return is_base_of_impl(a_baseTypeIndex, m_hierarchy[a_derivedTypeIndex].base_index);
			}
		}
#pragma endregion
	};

	/// @brief TODO
	using registry = basic_registry<std::allocator<char>>;

	namespace pmr
	{
		/// @brief TODO
		using registry = basic_registry<std::pmr::polymorphic_allocator<char>>;
	}
}