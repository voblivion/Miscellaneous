#pragma once

#include <vob/misc/std/conditional_const.h>

#include <array>
#include <cassert>
#include <utility>


namespace vob::mistd
{
    template <typename TEnum>
    constexpr TEnum enum_begin()
    {
        return static_cast<TEnum>(0);
    }

    template <typename TEnum>
    constexpr TEnum enum_end()
    {
        return TEnum::count;
    }

    template <typename TEnum, TEnum t_begin = enum_end<TEnum>(), TEnum t_end = enum_begin<TEnum>()>
    class enum_range
    {
        static constexpr auto begin_value = t_begin < t_end ? t_begin : t_end;
        static constexpr auto end_value = t_begin < t_end ? t_end : t_begin;
    public:
        class iterator
        {
        public:
            explicit iterator(TEnum const a_value)
                : m_value{ a_value }
            {
            }

            TEnum operator*() const
            {
                return m_value;
            }

            iterator& operator++()
            {
                m_value = static_cast<TEnum>(
                    static_cast<std::underlying_type_t<TEnum>>(m_value) + 1);
                return *this;
            }

            iterator operator++(int)
            {
                auto const copy = *this;
                ++(*this);
                return copy;
            }

            iterator& operator--()
            {
                m_value = static_cast<TEnum>(
                    static_cast<std::underlying_type_t<TEnum>>(m_value) - 1);
                return *this;
            }

            iterator operator--(int)
            {
                auto const copy = *this;
                --(*this);
                return copy;
            }

            friend bool operator==(iterator const& a_lhs, iterator const& a_rhs)
            {
                return a_lhs.m_value == a_rhs.m_value;
            }

            friend bool operator!=(iterator const& a_lhs, iterator const& a_rhs)
            {
                return a_lhs.m_value != a_rhs.m_value;
            }

        private:
            TEnum m_value;
        };

        iterator begin() const
        {
            return iterator{ begin_value };
        }

        iterator end() const
        {
            return iterator{ end_value };
        }
    };

    template <typename TEnum, typename TValue, TEnum t_begin = enum_end<TEnum>(), TEnum t_end = enum_begin<TEnum>()>
    class enum_map
    {
    public:
#pragma region CLASS_DATA
        static constexpr auto begin_key = t_begin < t_end ? t_begin : t_end;
        static constexpr auto end_key = t_begin < t_end ? t_end : t_begin;
        static constexpr auto begin_index = static_cast<std::underlying_type_t<TEnum>>(begin_key);
        static constexpr auto end_index = static_cast<std::underlying_type_t<TEnum>>(end_key);
        static_assert(begin_index <= end_index && "Invalid enum range for enum_map.");
        static constexpr auto value_count = static_cast<std::size_t>(end_index - begin_index);
#pragma endregion

#pragma region CREATORS
        /// @brief TODO
        constexpr enum_map() = default;

        /// @brief TODO
        explicit constexpr enum_map(TValue const& a_initValue)
            : m_data{ a_initValue }
        {}

        template <typename... TArgs>
        constexpr enum_map(TArgs&&... a_args)
            : m_data{ std::forward<TArgs>(a_args)... }
        {}
#pragma endregion

#pragma region ACCESSORS
        /// @brief TODO
        [[nodiscard]] constexpr auto begin() const
        {
            return m_data.begin();
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto end() const
        {
            return m_data.end();
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto const& operator[](std::size_t a_index) const
        {
            return m_data[a_index];
        }

        [[nodiscard]] constexpr auto const& operator[](TEnum a_key) const
        {
            assert(contains(a_key));
            return m_data[static_cast<std::underlying_type_t<TEnum>>(a_key) - begin_index];
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto empty() const
        {
            return size() == 0ul;
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto size() const
        {
            return m_data.size();
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto contains(TEnum a_key) const
        {
            return static_cast<std::size_t>(a_key) - begin_index < end_index;
        }
#pragma endregion

#pragma region MANIPULATORS
        /// @brief TODO
        [[nodiscard]] constexpr auto begin()
        {
            return m_data.begin();
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto end()
        {
            return m_data.end();
        }

        /// @brief TODO
        [[nodiscard]] constexpr auto& operator[](std::size_t a_index)
        {
            return m_data[a_index];
        }

        [[nodiscard]] constexpr auto& operator[](TEnum a_key)
        {
            assert(contains(a_key));
            return m_data[static_cast<std::underlying_type_t<TEnum>>(a_key) - begin_index];
        }
#pragma endregion

    private:
#pragma region DATA
        std::array<TValue, value_count> m_data;
#pragma endregion
    };
}
