#pragma once

#include <vector>

namespace vob::mistd
{
	struct size2d
	{
		int32_t x;
		int32_t y;

		friend bool operator==(size2d const& a_lhs, size2d const& a_rhs)
		{
			return a_lhs.x == a_rhs.x && a_lhs.y == a_rhs.y;
		}
	};

	template <typename TValue, typename TAllocator = std::allocator<TValue>>
	class vector2d
	{
	public:
#pragma region member types
		using underlying_container_type = std::vector<TValue, TAllocator>;
		using value_type = TValue;
		using allocator_type = TAllocator;
		using size_type = size2d;
		using difference_type = size2d;
		using reference = value_type&;
		using const_reference = value_type const&;
		using pointer = std::allocator_traits<TAllocator>::pointer;
		using const_pointer = std::allocator_traits<TAllocator>::const_pointer;
		using iterator = underlying_container_type::iterator;
		using const_iterator = underlying_container_type::const_iterator;

		struct keys_type
		{
			struct const_iterator
			{
				size_type const& operator*()
				{
					return m_key;
				}

				const_iterator& operator++()
				{
					++m_key.x;
					if (m_key.x == m_width)
					{
						m_key.x = 0;
						++m_key.y;
					}
					return *this;
				}

				const_iterator operator++(int)
				{
					const_iterator cpy = *this;
					++(*this);
					return cpy;
				}

				friend bool operator==(const_iterator const& a_lhs, const_iterator const& a_rhs)
				{
					return a_lhs.m_key == a_rhs.m_key;
				}

				friend bool operator!=(const_iterator const& a_lhs, const_iterator const& a_rhs)
				{
					return !(a_lhs == a_rhs);
				}

				size_type m_key;
				int32_t m_width;
			};

			const_iterator begin() const
			{
				return { size_type{0, 0}, m_size.x };
			}

			const_iterator end() const
			{
				return { size_type{0, m_size.y}, m_size.x };
			}

			size_type m_size;
		};
#pragma endregion member types

#pragma region <unnamed>
		constexpr vector2d() noexcept = default;
		constexpr vector2d(vector2d const&) = default;
		constexpr vector2d(vector2d&&) noexcept = default;
		constexpr vector2d(TAllocator const& a_allocator) noexcept
			: m_data{ a_allocator }
		{
		}
		constexpr vector2d(size2d const& a_size, TAllocator const& a_allocator = TAllocator())
			: m_width{ a_size.x }
			, m_data{ static_cast<underlying_container_type::size_type>(a_size.x * a_size.y), a_allocator }
		{
		}
		constexpr vector2d(size2d const& a_size, TValue const& a_value, TAllocator const& a_allocator = TAllocator())
			: m_width{ a_size.x }
			, m_data{ static_cast<underlying_container_type::size_type>(a_size.x * a_size.y), a_value, a_allocator }
		{
		}

		constexpr vector2d& operator=(vector2d const&) = default;
		constexpr vector2d& operator=(vector2d&&) = default;

		constexpr void assign(size_type a_count, TValue const& a_value)
		{
			m_width = a_count.x;
			m_data.assign(a_count.x * a_count.y, a_value);
		}

		constexpr allocator_type get_allocator() const noexcept
		{
			return m_data.get_allocator();
		}
#pragma endregion <unnamed>

#pragma region element access
		constexpr reference at(size_type a_pos)
		{
			return m_data.at(a_pos.y * m_width + a_pos.x);
		}
		constexpr const_reference at(size_type a_pos) const
		{
			return m_data.at(a_pos.y * m_width + a_pos.x);
		}

		constexpr reference operator[](size_type a_pos)
		{
			return m_data[a_pos.y * m_width + a_pos.x];
		}
		constexpr const_reference operator[](size_type a_pos) const
		{
			return m_data[a_pos.y * m_width + a_pos.x];
		}

		constexpr reference front()
		{
			return m_data.front();
		}
		constexpr const_reference front() const
		{
			return m_data.front();
		}

		constexpr reference back()
		{
			return m_data.back();
		}
		constexpr const_reference back() const
		{
			return m_data.back();
		}

		constexpr pointer data()
		{
			return m_data.data();
		}
		constexpr const_pointer data() const
		{
			return m_data.data();
		}

		constexpr keys_type keys() const
		{
			return keys_type{ size2d{width(), height()} };
		}
#pragma endregion element access

#pragma region iterators
		constexpr iterator begin() noexcept
		{
			return m_data.begin();
		}
		constexpr const_iterator begin() const noexcept
		{
			return m_data.begin();
		}
		constexpr const_iterator cbegin() const noexcept
		{
			return m_data.cbegin();
		}

		constexpr iterator end() noexcept
		{
			return m_data.end();
		}
		constexpr const_iterator end() const noexcept
		{
			return m_data.end();
		}
		constexpr const_iterator cend() const noexcept
		{
			return m_data.cend();
		}

		constexpr iterator rbegin() noexcept
		{
			return m_data.rbegin();
		}
		constexpr const_iterator rbegin() const noexcept
		{
			return m_data.rbegin();
		}
		constexpr const_iterator crbegin() const noexcept
		{
			return m_data.crbegin();
		}

		constexpr iterator rend() noexcept
		{
			return m_data.rend();
		}
		constexpr const_iterator rend() const noexcept
		{
			return m_data.rend();
		}
		constexpr const_iterator crend() const noexcept
		{
			return m_data.crend();
		}
#pragma endregion iterators

#pragma region capacity
		constexpr bool empty() const
		{
			return m_data.empty();
		}

		constexpr size2d size() const
		{
			return size2d{ m_width, m_width > 0 ? m_data.size() / m_width : 0 };
		}

		constexpr int32_t width() const
		{
			return m_width;
		}

		constexpr int32_t height() const
		{
			return m_width > 0 ? static_cast<int32_t>(m_data.size() / m_width) : 0;
		}

		constexpr size2d max_size() const noexcept
		{
			constexpr auto maxSideSize = static_cast<int32_t>(std::sqrt(m_data.max_size()));
			return size2d{ maxSideSize, maxSideSize };
		}

		constexpr void reserve(underlying_container_type::size_type const a_capacity)
		{
			m_data.reserve(a_capacity);
		}

		constexpr underlying_container_type::size_type capacity() const
		{
			return m_data.capacity();
		}

		constexpr void shrink_to_fit()
		{
			m_data.shrink_to_fit();
		}
#pragma endregion capacity

#pragma region modifiers
		constexpr void clear()
		{
			m_width = 0;
			m_data.clear();
		}

		constexpr void resize(const size2d& a_size)
		{
			const auto prevSize = size();
			if (a_size.x * a_size.y == prevSize.x * prevSize.y)
			{
				m_width = a_size.x;
				return;
			}

			if (a_size.x <= prevSize.x)
			{
				if (a_size.x < prevSize.x)
				{
					// remove extra cols
					const auto commonSizeY = std::min(prevSize.y, a_size.y);
					const auto safeRangeY = std::min((prevSize.x - 1) / (prevSize.x - a_size.x), commonSizeY);
					for (int32_t y = 1; y < safeRangeY; ++y)
					{
						const auto safeRangeX = y * prevSize.x - y * a_size.x;
						for (int32_t x = 0; x < a_size.x / safeRangeX; ++x)
						{
							std::swap_ranges(
								m_data.begin() + y * prevSize.x + x * safeRangeX,
								m_data.begin() + y * prevSize.x + (x + 1) * safeRangeX,
								m_data.begin() + y * a_size.x + x * safeRangeX);
						}
						std::swap_ranges(
							m_data.begin() + y * prevSize.x + (a_size.x - a_size.x % safeRangeX),
							m_data.begin() + y * prevSize.x + a_size.x,
							m_data.begin() + y * a_size.x + (a_size.x / safeRangeX + 1) * safeRangeX);
					}
					for (int32_t y = safeRangeY; y < commonSizeY; ++y)
					{
						std::swap_ranges(
							m_data.begin() + y * prevSize.x,
							m_data.begin() + y * prevSize.x + a_size.x,
							m_data.begin() + y * a_size.x
						);
					}
					m_data.resize(a_size.x * commonSizeY);
				}
				// add missing rows
				m_data.resize(a_size.x * a_size.y);
				m_width = a_size.x;
			}
			else // a_size.x > prevSize.x
			{
				// remove extra rows
				const auto commonSizeY = std::min(prevSize.y, a_size.y);
				m_data.resize(prevSize.x * commonSizeY);

				// add missing cols
				const auto safeRangeY = std::min((a_size.x - 1) / (a_size.x - prevSize.x), commonSizeY);
				m_data.resize(a_size.x * a_size.y);
				m_width = a_size.x;
				for (int32_t y = commonSizeY - 1; y >= safeRangeY; --y)
				{
					std::swap_ranges(
						m_data.begin() + y * prevSize.x,
						m_data.begin() + (y + 1) * prevSize.x,
						m_data.begin() + y * a_size.x
					);
				}
				for (int32_t y = safeRangeY - 1; y > 0; --y)
				{
					const auto safeRangeX = y * (a_size.x - prevSize.x);
					for (int32_t x = 0; x < prevSize.x / safeRangeX; ++x)
					{
						const auto p = (y + 1) * prevSize.x - (x + 1) * safeRangeX;
						std::swap_ranges(
							m_data.begin() + p,
							m_data.begin() + p + safeRangeX,
							m_data.begin() + p + safeRangeX);
					}
					std::swap_ranges(
						m_data.begin() + y * prevSize.x,
						m_data.begin() + y * prevSize.x + (prevSize.x % safeRangeX),
						m_data.begin() + y * prevSize.x + safeRangeX);
				}
			}
		}

		constexpr void resize(const size2d& a_size, const value_type& a_value)
		{
			const auto prevSize = size();
			if (a_size.x * a_size.y == prevSize.x * prevSize.y)
			{
				m_width = a_size.x;
				return;
			}

			if (a_size.x <= prevSize.x)
			{
				if (a_size.x < prevSize.x)
				{
					// remove extra cols
					const auto commonSizeY = std::min(prevSize.y, a_size.y);
					const auto safeRangeY = std::min((prevSize.x - 1) / (prevSize.x - a_size.x), commonSizeY);
					for (int32_t y = 1; y < safeRangeY; ++y)
					{
						const auto safeRangeX = y * prevSize.x - y * a_size.x;
						for (int32_t x = 0; x < a_size.x / safeRangeX; ++x)
						{
							std::swap_ranges(
								m_data.begin() + y * prevSize.x + x * safeRangeX,
								m_data.begin() + y * prevSize.x + (x + 1) * safeRangeX,
								m_data.begin() + y * a_size.x + x * safeRangeX);
						}
						std::swap_ranges(
							m_data.begin() + y * prevSize.x + (a_size.x - a_size.x % safeRangeX),
							m_data.begin() + y * prevSize.x + a_size.x,
							m_data.begin() + y * a_size.x + (a_size.x / safeRangeX + 1) * safeRangeX);
					}
					for (int32_t y = safeRangeY; y < commonSizeY; ++y)
					{
						std::swap_ranges(
							m_data.begin() + y * prevSize.x,
							m_data.begin() + y * prevSize.x + a_size.x,
							m_data.begin() + y * a_size.x
						);
					}
					m_data.resize(a_size.x * commonSizeY);
				}
				// add missing rows
				m_data.resize(a_size.x * a_size.y, a_value);
				m_width = a_size.x;
			}
			else // a_size.x > prevSize.x
			{
				// remove extra rows
				const auto commonSizeY = std::min(prevSize.y, a_size.y);
				m_data.resize(prevSize.x * commonSizeY);

				// add missing cols
				const auto safeRangeY = std::min((a_size.x - 1) / (a_size.x - prevSize.x), commonSizeY);
				m_data.resize(a_size.x * a_size.y, a_value);
				m_width = a_size.x;
				for (int32_t y = commonSizeY - 1; y >= safeRangeY; --y)
				{
					std::swap_ranges(
						m_data.begin() + y * prevSize.x,
						m_data.begin() + (y + 1) * prevSize.x,
						m_data.begin() + y * a_size.x
					);
				}
				for (int32_t y = safeRangeY - 1; y > 0; --y)
				{
					const auto safeRangeX = y * (a_size.x - prevSize.x);
					for (int32_t x = 0; x < prevSize.x / safeRangeX; ++x)
					{
						const auto p = (y + 1) * prevSize.x - (x + 1) * safeRangeX;
						std::swap_ranges(
							m_data.begin() + p,
							m_data.begin() + p + safeRangeX,
							m_data.begin() + p + safeRangeX);
					}
					std::swap_ranges(
						m_data.begin() + y * prevSize.x,
						m_data.begin() + y * prevSize.x + (prevSize.x % safeRangeX),
						m_data.begin() + y * prevSize.x + safeRangeX);
				}
			}
		}
#pragma endregion

	private:
		std::int32_t m_width = 0;
		underlying_container_type m_data;
	};

}
