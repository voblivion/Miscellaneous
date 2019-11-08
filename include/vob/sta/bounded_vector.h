#pragma once

#include <array>
#include <cassert>


namespace vob::sta
{
	template <typename Type, std::size_t t_maxSize>
	class BoundedVector
	{
		using Base = std::pmr::vector<Type>;
		static constexpr std::size_t s_bytes = sizeof(Type);
	public:
		BoundedVector()
			: m_begin{ reinterpret_cast<Type*>(m_buffer.data()) }
		{}

		~BoundedVector()
		{
			while (!empty())
			{
				pop_back();
			}
		}

		auto empty() const
		{
			return m_size == 0;
		}

		auto size() const
		{
			return m_size;
		}

		auto data()
		{
			return reinterpret_cast<Type*>(m_buffer.data());
		}

		auto data() const
		{
			return reinterpret_cast<Type const*>(m_buffer.data());
		}

		auto begin()
		{
			return data();
		}

		auto begin() const
		{
			return data();
		}

		auto end()
		{
			return begin() + m_size;
		}

		auto end() const
		{
			return begin() + m_size;
		}

		auto push_back(Type&& a_value)
		{
			assert(m_size < t_maxSize);
			auto ptr = begin() + s_bytes * ++m_size;
			return new(ptr) Type(std::forward(a_value));
		}

		template <typename... Args>
		auto emplace_back(Args&&... a_args)
		{
			assert(m_size < t_maxSize);
			auto ptr = begin() + s_bytes * m_size++;
			ptr = new(ptr) Type{ std::forward<Args>(a_args)... };
			return ptr;
		}

		auto pop_back()
		{
			assert(m_size > 0);
			auto ptr = begin() + s_bytes * --m_size;
			ptr->~Type();
		}

	private:
		// Attributes
		alignas(alignof(Type))
			std::array<std::byte, s_bytes * t_maxSize> m_buffer;
		Type* m_begin;
		std::size_t m_size = 0;
	};
}