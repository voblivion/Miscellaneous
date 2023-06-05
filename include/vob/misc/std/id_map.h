#pragma once

#include <cassert>
#include <cstdint>
#include <memory_resource>
#include <variant>
#include <vector>


namespace vob::mistd
{
	/// <summary>
	/// An id-indexed map favoring fast lookup time over memory usage.
	/// Think of it as a vector that doesn't invalidate previous indices when elements are modified.
	/// </summary>
	template <
		typename TValue,
		typename TAllocator = std::allocator<TValue>,
		typename TKey = uint64_t,
		uint8_t t_versionBits = 24>
	class id_map
	{
	public:
		using size_type = size_t;
		using key_type = TKey;
		using value_type = TValue;
		using reference = value_type&;
		using pointer = value_type*;
		using const_reference = value_type const&;
		using const_pointer = value_type const*;

		static_assert(t_versionBits < 8 * sizeof(TKey));
		constexpr static uint8_t k_versionBits = t_versionBits;
		constexpr static uint8_t k_indexBits = 8 * sizeof(TKey) - k_versionBits;
		constexpr static key_type k_indexMask = (static_cast<key_type>(1) << k_indexBits) - 1;
		constexpr static key_type k_versionMask = (~static_cast<key_type>(0)) ^ k_indexMask;

		using version_type = std::conditional_t<
			k_versionBits <= 8, uint8_t, std::conditional_t<
			k_versionBits <= 16, uint16_t, std::conditional_t<
			k_versionBits <= 32, uint32_t, uint64_t>>>;

		class iterator
		{
		public:
			explicit iterator(size_type const a_index, id_map& a_idMap)
				: m_index{ a_index }
				, m_idMap{ a_idMap }
			{
			}

			iterator& operator++()
			{
				do
				{
					++m_index;
				} while (m_index < m_idMap.get().m_values.size()
					&& !std::holds_alternative<value_type>(m_idMap.get().m_values[m_index].second));
				return *this;
			}

			iterator operator++(int)
			{
				iterator copy = *this;
				++(*this);
				return copy;
			}

			reference operator*() const
			{
				return std::get<value_type>(m_idMap.get().m_values[m_index].second);
			}

			pointer operator->() const
			{
				return &std::get<value_type>(m_idMap.get().m_values[m_index].second);
			}

			friend bool operator==(iterator const& a_lhs, iterator const& a_rhs)
			{
				return a_lhs.m_index == a_rhs.m_index && &(a_lhs.m_idMap.get()) == &(a_rhs.m_idMap.get());
			}

			friend bool operator!=(iterator const& a_lhs, iterator const& a_rhs)
			{
				return !(a_lhs == a_rhs);
			}

		private:
			size_type m_index;
			std::reference_wrapper<id_map> m_idMap;
		};

		class const_iterator
		{
		public:
			explicit const_iterator(size_type const a_index, id_map const& a_idMap)
				: m_index{ a_index }
				, m_idMap{ a_idMap }
			{
			}

			const_iterator& operator++()
			{
				do
				{
					++m_index;
				} while (m_index < m_idMap.get().m_values.size()
					&& !std::holds_alternative<value_type>(m_idMap.get().m_values[m_index].second));
				return *this;
			}

			const_iterator operator++(int)
			{
				const_iterator copy = *this;
				++(*this);
				return copy;
			}

			const_reference operator*() const
			{
				return std::get<value_type>(m_idMap.get().m_values[m_index].second);
			}

			const_pointer operator->() const
			{
				return &std::get<value_type>(m_idMap.get().m_values[m_index].second);
			}

			friend bool operator==(const_iterator const& a_lhs, const_iterator const& a_rhs)
			{
				return a_lhs.m_index == a_rhs.m_index && &(a_lhs.m_idMap.get()) == &(a_rhs.m_idMap.get());
			}

			friend bool operator!=(const_iterator const& a_lhs, const_iterator const& a_rhs)
			{
				return !(a_lhs == a_rhs);
			}

		private:
			size_type m_index;
			std::reference_wrapper<id_map const> m_idMap;
		};

		class const_key_iterator
		{
		public:
			explicit const_key_iterator(size_type const a_index, id_map const& a_idMap)
				: m_index{ a_index }
				, m_idMap{ a_idMap }
			{
			}

			const_key_iterator& operator++()
			{
				do
				{
					++m_index;
				} while (m_index < m_idMap.get().m_values.size()
					&& !std::holds_alternative<value_type>(m_idMap.get().m_values[m_index].second));
				return *this;
			}

			const_key_iterator operator++(int)
			{
				const_key_iterator copy = *this;
				++(*this);
				return copy;
			}

			key_type operator*() const
			{
				return (static_cast<key_type>(m_idMap.get().m_values[m_index].first) << k_indexBits) | m_index;
			}

			friend bool operator==(const_key_iterator const& a_lhs, const_key_iterator const& a_rhs)
			{
				return a_lhs.m_index == a_rhs.m_index && &(a_lhs.m_idMap.get()) == &(a_rhs.m_idMap.get());
			}

			friend bool operator!=(const_key_iterator const& a_lhs, const_key_iterator const& a_rhs)
			{
				return !(a_lhs == a_rhs);
			}

		private:
			size_type m_index;
			std::reference_wrapper<id_map const> m_idMap;
		};

		class keys_ref
		{
		public:
			keys_ref(id_map const& a_idMap)
				: m_idMap{ a_idMap }
			{
			}

			const_key_iterator begin() const
			{
				return m_idMap.get().key_begin();
			}

			const_key_iterator end() const
			{
				return m_idMap.get().key_end();
			}

		private:
			std::reference_wrapper<id_map const> m_idMap;
		};

		explicit id_map(TAllocator const& a_allocator = {})
			: m_values{ vector_allocator_type{ a_allocator } }
		{
		}

		keys_ref keys() const
		{
			return keys_ref{ *this };
		}

		bool contains(key_type const a_key) const
		{
			auto const idx = index(a_key);
			return idx < m_values.size()
				&& m_values[idx].first == version(a_key)
				&& std::holds_alternative<value_type>(m_values[idx].second);
		}

		version_type version(key_type const a_key) const
		{
			return version_type{ (a_key & k_versionMask) >> k_indexBits };
		}

		size_type index(key_type const a_key) const
		{
			return a_key & k_indexMask;
		}

		reference operator[](key_type const a_key)
		{
			assert(contains(a_key) && "Key not found.");
			return std::get<value_type>(m_values[index(a_key)].second);
		}

		const_reference operator[](key_type const a_key) const
		{
			assert(contains(a_key) && "Key not found.");
			return std::get<value_type>(m_values[index(a_key)].second);
		}

		iterator begin()
		{
			if (m_values.size() == m_freeSlotCount)
			{
				return iterator{ 0, *this };
			}

			iterator it{ static_cast<size_t>(-1), *this};
			return ++it;
		}

		const_iterator begin() const
		{
			if (m_values.size() == m_freeSlotCount)
			{
				return const_iterator{ 0, *this };
			}

			const_iterator it{ static_cast<size_t>(-1), *this };
			++it;
			return ++it;
		}

		const_iterator cbegin() const
		{
			if (m_values.size() == m_freeSlotCount)
			{
				return const_iterator{ 0, *this };
			}

			const_iterator it{ static_cast<size_t>(-1), *this };
			++it;
			return ++it;
		}

		iterator end()
		{
			return iterator{ m_values.size(), *this };
		}

		const_iterator end() const
		{
			return const_iterator{ m_values.size(), *this };
		}

		const_iterator cend() const
		{
			return const_iterator{ m_values.size(), *this };
		}

		const_key_iterator key_begin() const
		{
			if (m_values.size() == m_freeSlotCount)
			{
				return const_key_iterator{ 0, *this };
			}

			const_key_iterator it{ static_cast<size_t>(-1), *this };
			++it;
			return ++it;
		}

		const_key_iterator key_end() const
		{
			return const_key_iterator{ m_values.size(), *this };
		}

		iterator find(key_type const& a_key)
		{
			if (contains(a_key))
			{
				return iterator{ index(a_key), *this };
			}

			return end();
		}

		const_iterator find(key_type const& a_key) const
		{
			if (contains(a_key))
			{
				return const_iterator{ index(a_key), *this };
			}

			return end();
		}

		size_type size() const
		{
			return m_values.size() - m_freeSlotCount;
		}
		
		template <typename... TArgs>
		key_type emplace(TArgs&&... a_args)
		{
			if (m_freeSlotCount == 0)
			{
				m_values.emplace_back(version_type{ 0 }, value_type{ std::forward<TArgs>(a_args)... });
				m_nextFreeSlotIndex = m_values.size();
				return m_values.size() - 1;
			}

			auto const idx = m_nextFreeSlotIndex;
			m_nextFreeSlotIndex = std::get<free_slot>(m_values[idx].second).m_nextFreeSlotIndex;
			--m_freeSlotCount;
			m_values[idx].first += 1;
			m_values[idx].second = value_type{ std::forward<TArgs>(a_args)... };
			return (static_cast<key_type>(m_values[idx].first) << k_indexBits) + idx;
		}

		void erase(key_type const a_key)
		{
			if (!contains(a_key))
			{
				return;
			}
			auto const idx = index(a_key & k_indexMask);
			m_values[idx].second = free_slot{ m_nextFreeSlotIndex };
			m_nextFreeSlotIndex = idx;
			++m_freeSlotCount;
		}

		void clear()
		{
			m_values.clear();
			m_nextFreeSlotIndex = 0;
			m_freeSlotCount = 0;
		}

	private:
		friend class iterator;
		friend class const_iterator;
		friend class const_key_iterator;
		friend class keys_ref;

		struct free_slot
		{
			size_type m_nextFreeSlotIndex = 0;
		};

		size_type m_nextFreeSlotIndex = 0;
		size_type m_freeSlotCount = 0;

		using vector_value_type = std::pair<version_type, std::variant<free_slot, value_type>>;
		using vector_allocator_type =
			typename std::allocator_traits<TAllocator>::template rebind_alloc<vector_value_type>;

		std::vector<vector_value_type, vector_allocator_type> m_values = {};
	};

	namespace pmr
	{
		template <typename TValue, typename TKey = uint64_t, uint8_t t_versionBits = 24>
		using id_map = ::vob::mistd::id_map<TValue, std::pmr::polymorphic_allocator<TValue>, TKey, t_versionBits>;
	}
}
