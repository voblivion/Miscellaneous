#pragma once

#include <algorithm>
#include <array>
#include <cassert>


namespace vob::mistd
{
	/// @brief A vector-like object whose size is bounded by a compile-time maximum.
	template <typename TValue, std::size_t t_maxSize>
	class bounded_vector
	{
	public:

#pragma region CREATORS
		constexpr bounded_vector() = default;

		/// @brief Constructs a bounded_vector by acquiring elements of another, leaving it empty. 
		bounded_vector(bounded_vector&& a_other) noexcept
		{
			for (auto& value : a_other)
			{
				push_back(std::move(value));
			}
			a_other.clear();
		}

		/// @brief Constructs a bounded_vector by copying elements of another, leaving it unchanged. 
		constexpr bounded_vector(bounded_vector const& a_other) noexcept
		{
			for (auto const& value : a_other)
			{
				push_back(value);
			}
		}

		/// @brief Constructs a bounded_vector from an initializer list.
		constexpr bounded_vector(std::initializer_list<TValue> a_values) noexcept
		{
			for (auto& value : a_values)
			{
				push_back(value);
			}
		}

		~bounded_vector()
		{
			while (!empty())
			{
				pop_back();
			}
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Checks if the container has no elements.
		constexpr bool empty() const noexcept
		{
			return m_size == 0;
		}

		/// @brief Provides the number of elements in the container.
		constexpr auto size() const noexcept
		{
			return m_size;
		}

		/// @brief Provides a pointer to the underlying array serving as element storage.
		/// 
		/// The pointer is such that range [data(); data() + size()) is always a valid range, even if the container is
		/// empty.
		constexpr auto data() const noexcept
		{
			return reinterpret_cast<TValue const*>(m_buffer.data());
		}

		/// @brief Provides an iterator (pointer) to the first element of the container.
		constexpr auto begin() const noexcept
		{
			return data();
		}

		/// @brief Provides an iterator (pointer) to the element following the last element of the container.
		constexpr auto end() const noexcept
		{
			return begin() + m_size;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Erases all elements from the container.
		constexpr void clear() noexcept
		{
			for (auto& value : *this)
			{
				value.~TValue();
			}
			m_size = 0;
		}

		/// @brief Provides a pointer to the underlying array serving as element storage.
		/// 
		/// The pointer is such that range [data(); data() + size()) is always a valid range, even if the container is
		/// empty.
		constexpr auto data() noexcept
		{
			return reinterpret_cast<TValue*>(m_buffer.data());
		}

		/// @brief Provides an iterator (pointer) to the first element of the container.
		constexpr auto begin() noexcept
		{
			return data();
		}

		/// @brief Provides an iterator (pointer) to the element following the last element of the container.
		constexpr auto end() noexcept
		{
			return begin() + m_size;
		}

		/// @brief Adds a value at the end of the container, constructed by move.
		constexpr auto push_back(TValue&& a_value) noexcept
		{
			assert(m_size < t_maxSize && "Adding a value in a full bounded_vector.");
			auto ptr = begin() + m_size++;
			return new(ptr) TValue{ std::move(a_value) };
		}

		/// @brief Adds a value at the end of the container, constructed by copy.
		constexpr auto push_back(TValue const& a_value) noexcept
		{
			assert(m_size < t_maxSize && "Adding a value in a full bounded_vector.");
			auto ptr = begin() + m_size++;
			return new(ptr) TValue{ a_value };
		}

		/// @brief Constructs a value directly at the end of the container, increasing its size by one.
		template <typename... TArgs>
		constexpr auto emplace_back(TArgs&&... a_args) noexcept
		{
			assert(m_size < t_maxSize && "Adding a value in a full bounded_vector.");
			auto ptr = begin() + m_size++;
			return new(ptr) TValue{ std::forward<TArgs>(a_args)... };
		}

		/// @brief Removes the last element of the container.
		constexpr void pop_back() noexcept
		{
			assert(m_size > 0 && "Calling pop_back on an empty bounded_vector.");
			auto ptr = begin() + --m_size;
			ptr->~TValue();
		}

		/// @brief Acquires elements of another bounded_vector, leaving it empty.
		auto operator=(bounded_vector&& a_other) noexcept -> bounded_vector&
		{
			for (auto& value : a_other)
			{
				push_back(std::move(value));
			}
			a_other.clear();
		}

		/// @brief Copies elements of another bounded_vector, leaving it unchanged.
		auto operator=(bounded_vector const& a_other) noexcept -> bounded_vector&
		{
			for (auto const& value : a_other)
			{
				push_back(value);
			}
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		alignas(alignof(TValue)) std::array<std::byte, sizeof(TValue) * t_maxSize> m_buffer{};
		std::size_t m_size = 0;
#pragma endregion
	};
}
