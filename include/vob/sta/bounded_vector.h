#pragma once

#include <array>
#include <cassert>
#include <vob/sta/error.h>

namespace vob::sta
{
	template <typename T, size_t MaxSize>
	class bounded_vector
	{
	public:
#pragma region Aliases
		using value_type = T;
		using allocator_type = add_pointer_t<value_type>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using reference = add_lvalue_reference_t<value_type>;
		using const_reference = add_lvalue_reference_t<add_const_t<value_type>>;
		using pointer = add_pointer_t<value_type>;
		using const_pointer = add_pointer_t<add_const_t<value_type>>;
		using iterator = pointer;
		using const_iterator = const_pointer;
#pragma endregion
#pragma region Constructors
		bounded_vector() = default;

		bounded_vector(bounded_vector const& a_other)
		{
			for (auto& value : a_other)
			{
				push_back(value);
			}
		}

		bounded_vector(std::initializer_list<T> a_init)
		{
			T* ptr = begin();
			/*for (auto& value : a_init)
			{
				new(ptr++) Type(value);
				++m_size;
			}*/
		}

		bounded_vector(bounded_vector&&) = delete;

		~bounded_vector()
		{
			while (!empty())
			{
				do_pop_back();
			}
		}
#pragma endregion
#pragma region Methods
		constexpr auto empty() const noexcept
		{
			return m_size == 0;
		}

		constexpr auto size() const noexcept
		{
			return m_size;
		}

		constexpr auto data() noexcept
		{
			return reinterpret_cast<T*>(m_buffer.data());
		}

		constexpr auto data() const noexcept
		{
			return reinterpret_cast<T const*>(m_buffer.data());
		}

		constexpr auto begin() noexcept
		{
			return data();
		}

		constexpr auto begin() const noexcept
		{
			return data();
		}

		constexpr auto end() noexcept
		{
			return begin() + m_size;
		}

		constexpr auto end() const noexcept
		{
			return begin() + m_size;
		}

		auto push_back(T const& a_value)
		{
			expect(m_size < MaxSize, length_error{ "Adding value in full bounded_vector." });
			auto ptr = begin() + m_size++;
			return new(ptr) T(a_value);
		}
		
		auto push_back(T&& a_value)
		{
			expect(m_size < MaxSize, length_error{ "Adding value in full bounded_vector." });
			auto ptr = begin() + m_size++;
			return new(ptr) T(std::forward(a_value));
		}

		template <typename... Args>
		auto emplace_back(Args&&... a_args)
		{
			expect(m_size < MaxSize, length_error{ "Adding value in full bounded_vector." });
			auto ptr = begin() + m_size++;
			ptr = new(ptr) T{ std::forward<Args>(a_args)... };
			return ptr;
		}

		auto pop_back()
		{
			expect(m_size > 0, length_error{ "Calling pop_back on empty bounded_vector." });
			do_pop_back();
		}
#pragma endregion
#pragma region Operators
		bounded_vector& operator=(bounded_vector const& a_other)
		{
			for (auto& value : a_other)
			{
				push_back(value);
			}
			return *this;
		}

		bounded_vector& operator=(bounded_vector&&) = delete;
#pragma endregion

	private:
#pragma region Attributes
		alignas(alignof(T)) std::array<std::byte, sizeof(T) * MaxSize> m_buffer{};
		std::size_t m_size = 0;
#pragma endregion
#pragma region Methods
		auto do_pop_back()
		{
			auto ptr = begin() + --m_size;
			ptr->~T();
		}
#pragma endregion
	};
}