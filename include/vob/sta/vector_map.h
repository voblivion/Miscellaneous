#pragma once

#include <algorithm>
#include <vector>


namespace vob::sta
{
	template <
		typename KeyType
		, typename ValueType
		, typename AllocatorType = std::allocator<std::pair<KeyType const, ValueType>>
	>
	class vector_map
	{
	public:
#pragma region Constructors
		vector_map() = default;

		template <typename SomeAllocatorType>
		explicit vector_map(SomeAllocatorType const& a_allocator)
			: m_data{ a_allocator }
		{}
#pragma endregion
#pragma region Methods
		auto begin()
		{
			return m_data.begin();
		}

		auto begin() const
		{
			return m_data.begin();
		}

		auto end()
		{
			return m_data.end();
		}

		auto end() const
		{
			return m_data.end();
		}

		auto empty() const
		{
			return m_data.empty();
		}

		decltype(auto) find(KeyType const& a_key)
		{
			return std::find_if(begin(), end(), [&a_key](auto const& a_entry)
			{
				return a_entry.first == a_key;
			});
		}

		decltype(auto) find(KeyType const& a_key) const
		{
			return std::find_if(begin(), end(), [&a_key](auto const& a_entry)
			{
				return a_entry.first == a_key;
			});
		}

		decltype(auto) emplace(std::pair<KeyType const, ValueType>&& a_pair)
		{
			auto t_it = find(a_pair.first);
			if (t_it != end())
			{
				return std::make_pair(t_it, false);
			}

			m_data.emplace_back(std::move(a_pair));
			t_it = end();
			--t_it;
			return make_pair(t_it, true);
		}

		decltype(auto) emplace(KeyType a_key, ValueType a_value = {})
		{
			return emplace(std::make_pair(std::move(a_key), std::move(a_value)));
		}
#pragma endregion
#pragma region Operators
		auto& operator[](KeyType const& a_key)
		{
			auto t_res = emplace(a_key);
			return t_res.first->second;
		}
#pragma endregion
	private:
#pragma region Attributes
		std::vector<std::pair<KeyType const, ValueType>, AllocatorType> m_data;
#pragma endregion
	};

	namespace pmr
	{
		template <typename KeyType, typename ValueType>
		using vector_map = sta::vector_map<
			KeyType, ValueType, std::pmr::polymorphic_allocator<std::pair<KeyType const, ValueType>>
		>;
	}
}