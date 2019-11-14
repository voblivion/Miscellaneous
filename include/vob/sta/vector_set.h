#pragma once

#include <algorithm>
#include <vector>


namespace vob::sta
{
	using namespace std;

	template <
		typename Key
		, typename KeyEqual = std::equal_to<>
		, typename Allocator = std::allocator<Key>
	>
	class vector_set
	{
		using vector_type = vector<Key, Allocator>;
	public:
#pragma region Aliases
		using iterator = typename vector_type::iterator;
		using const_iterator = typename vector_type::const_iterator;
		using size_type = typename vector_type::size_type;
#pragma endregion

		// Constructors
		vector_set() = default;

		explicit vector_set(Allocator const& a_allocator)
			: m_data{ a_allocator }
		{}

		// Methods
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

		template <typename OtherKey>
		decltype(auto) find(OtherKey const& a_key)
		{
			return find_if(begin(), end(), [&a_key](auto const& a_item)
			{
				return KeyEqual{}(a_item, a_key);
			});
		}

		template <typename OtherKey>
		decltype(auto) find(OtherKey const& a_key) const
		{
			return find_if(begin(), end(), [&a_key](auto const& a_item)
			{
				return KeyEqual{}(a_item, a_key);
			});
		}

		template <typename OtherKey>
		auto contains(OtherKey const& a_key) const
		{
			return find(a_key) != end();
		}

		decltype(auto) emplace(Key&& a_key)
		{
			auto t_it = find(a_key);
			if (t_it != end())
			{
				return std::make_pair(t_it, false);
			}

			m_data.emplace_back(std::move(a_key));
			t_it = end();
			--t_it;
			return std::make_pair(t_it, true);
		}

		iterator erase(const_iterator a_it)
		{
			auto begin_it = const_cast<std::vector<Key, Allocator> const&>(m_data).begin();
			auto index = distance(begin_it, a_it);
			iter_swap(begin() + index, m_data.end() - 1);
			m_data.pop_back();
			return begin() + index;
		}

		size_type erase(Key const& a_key)
		{
			auto remove_count = size_type{ 0 };
			auto it = begin();
			while (it != end())
			{
				if (KeyEqual{}(*it, a_key))
				{
					it = erase(it);
					++remove_count;
				}
				else
				{
					++it;
				}
			}
			return remove_count;
		}
	private:
		std::vector<Key, Allocator> m_data;
	};

	namespace pmr
	{
		template <typename Key, typename KeyEqual = std::equal_to<>>
		using vector_set = sta::vector_set<
			Key, KeyEqual, ::std::pmr::polymorphic_allocator<Key>
		>;
	}
}