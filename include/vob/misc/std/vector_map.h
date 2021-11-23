#pragma once

#include <algorithm>
#include <vector>


namespace vob::mistd
{
	/// @brief TODO
	template <
		typename TKey,
		typename TValue,
		typename TKeyEqual = std::equal_to<>,
		typename TAllocator = std::allocator<std::pair<TKey const, TValue>>>
	class vector_map
	{
	public:
#pragma region TYPES
		using key_type = TKey;
		using value_type = TValue;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		vector_map() = default;

		/// @brief TODO
		explicit vector_map(TAllocator const& a_allocator)
			: m_data{ a_allocator }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		[[nodiscard]] auto empty() const
		{
			return m_data.empty();
		}

		/// @brief TODO
		[[nodiscard]] auto size() const
		{
			return m_data.size();
		}

		/// @brief TODO
		[[nodiscard]] auto begin() const
		{
			return m_data.begin();
		}

		/// @brief TODO
		[[nodiscard]] auto end() const
		{
			return m_data.end();
		}

		/// @brief TODO
		[[nodiscard]] decltype(auto) find(TKey const& a_key) const
		{
			return std::find_if(
				begin(), end(), [&a_key](auto const& a_entry) { return TKeyEqual{}(a_entry.first, a_key); });
		}

		/// @brief TODO
		[[nodiscard]] auto const& operator[](TKey const& a_key) const
		{
			return find(a_key)->second;
		}

		/// @brief TODO
		[[nodiscard]] constexpr auto get_allocator() const
		{
			return m_data.get_allocator();
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		decltype(auto) emplace(std::pair<TKey const, TValue>&& a_entry)
		{
			auto it = find(a_entry.first);
			if (it != end())
			{
				return std::make_pair(it, false);
			}

			m_data.emplace_back(std::move(a_entry));
			return std::make_pair(--end(), true);
		}

		/// @brief TODO
		decltype(auto) emplace(TKey a_key, TValue a_value = {})
		{
			return emplace(std::make_pair(std::move(a_key), std::move(a_value)));
		}

		/// @brief TODO
		void reserve(std::size_t const a_capacity)
		{
			m_data.reserve(a_capacity);
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
		decltype(auto) find(TKey const& a_key)
		{
			return std::find_if(
				begin(), end(), [&a_key](auto const& a_entry) { return TKeyEqual{}(a_entry.first, a_key); });
		}

		/// @brief TODO
		auto& operator[](TKey const& a_key)
		{
			return find(a_key)->second;
		}
#pragma endregion

#pragma region PRIVATE_DATA
		std::vector<std::pair<TKey const, TValue>, TAllocator> m_data;
#pragma endregion
	};

	namespace pmr
	{
		/// @brief TODO
		template <typename TKey, typename TValue, typename TKeyEqual = std::equal_to<>>
		using vector_map = mistd::vector_map<
			TKey,
			TValue,
			TKeyEqual,
			std::pmr::polymorphic_allocator<std::pair<TKey const, TValue>>>;
	}
}