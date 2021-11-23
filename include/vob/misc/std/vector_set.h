#pragma once

#include <algorithm>
#include <vector>


namespace vob::mistd
{
	/// @brief TODO
	template <typename TKey, typename TKeyEqual = std::equal_to<>, typename TAllocator = std::allocator<TKey>>
	class vector_set
	{
	public:
#pragma region TYPES
		using iterator = typename std::vector<TKey, TAllocator>::iterator;
		using const_iterator = typename std::vector<TKey, TAllocator>::const_iterator;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		vector_set() = default;

		/// @brief TODO
		explicit vector_set(TAllocator const& a_allocator)
			: m_data{ a_allocator }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		bool empty() const
		{
			return m_data.empty();
		}

		/// @brief TODO
		auto size() const
		{
			return m_data.size();
		}

		/// @brief TODO 
		template <typename TKey2>
		bool contains(TKey2 const& a_key) const
		{
			return find(a_key) != end();
		}

		/// @brief TODO 
		auto allocator() const
		{
			return m_data.get_allocator();
		}

		/// @brief TODO
		auto begin() const
		{
			return m_data.begin();
		}

		/// @brief TODO
		auto end() const
		{
			return m_data.end();
		}

		/// @brief TODO
		template <typename TKey2>
		decltype(auto) find(TKey2 const& a_key) const
		{
			return std::find_if(begin(), end(), [&a_key](auto const& a_item) { return TKeyEqual{}(a_item, a_key); });
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		void reserve(std::size_t const a_capacity)
		{
			m_data.reserve(a_capacity);
		}

		/// @brief TODO
		decltype(auto) emplace(TKey a_key)
		{
			auto it = find(a_key);
			if (it != end())
			{
				return std::make_pair(it, false);
			}

			m_data.emplace_back(std::move(a_key));
			return std::make_pair(--end(), true);
		}

		/// @brief TODO
		auto erase(const_iterator a_it)
		{
			auto const begin_it = const_cast<vector_set const&>(*this).begin();
			auto const index = std::distance(begin_it, a_it);
			std::iter_swap(begin() + index, --end());
			m_data.pop_back();
			return begin() + index;
		}

		/// @brief TODO 
		std::size_t erase(TKey const& a_key)
		{
			auto it = find(a_key);
			if (it != end())
			{
				erase(it);
				return 1u;
			}
			return 0u;
		}

		/// @brief TODO
		auto begin()
		{
			return m_data.begin();
		}

		/// @brief TODO
		auto end()
		{
			return m_data.end();
		}

		/// @brief TODO
		template <typename TKey2>
		decltype(auto) find(TKey2 const& a_key)
		{
			return std::find_if(begin(), end(), [&a_key](auto const& a_item) { return TKeyEqual{}(a_item, a_key); });
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		std::vector<TKey, TAllocator> m_data;
#pragma endregion
	};
}