#pragma once

#include <algorithm>
#include <vector>


namespace vob::sta
{
	using namespace std;

	template <
		typename Key
		, typename T
		, typename Allocator = allocator<pair<Key const, T>>
	>
	class vector_map
	{
	public:
#pragma region Constructors
		vector_map() = default;

		explicit vector_map(Allocator const& a_allocator)
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

		decltype(auto) find(Key const& a_key)
		{
			return find_if(begin(), end(), [&a_key](auto const& a_pair)
			{
				return a_pair.first == a_key;
			});
		}

		decltype(auto) find(Key const& a_key) const
		{
			return find_if(begin(), end(), [&a_key](auto const& a_pair)
			{
				return a_pair.first == a_key;
			});
		}

		decltype(auto) emplace(pair<Key const, T>&& a_pair)
		{
			auto t_it = find(a_pair.first);
			if (t_it != end())
			{
				return make_pair(t_it, false);
			}

			m_data.emplace_back(move(a_pair));
			t_it = end();
			--t_it;
			return make_pair(t_it, true);
		}

		decltype(auto) emplace(Key a_key, T a_value = {})
		{
			return emplace(make_pair(move(a_key), move(a_value)));
		}
#pragma endregion
#pragma region Operators
		auto& operator[](Key const& a_key)
		{
			auto t_res = emplace(a_key);
			return t_res.first->second;
		}
#pragma endregion
	private:
#pragma region Attributes
		vector<pair<Key const, T>, Allocator> m_data;
#pragma endregion
	};

	namespace pmr
	{
		template <typename Key, typename T>
		using vector_map = sta::vector_map<
			Key, T, ::std::pmr::polymorphic_allocator<pair<Key const, T>>
		>;
	}
}