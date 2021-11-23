#pragma once

#include "conditional_const.h"

#include <array>
#include <cassert>
#include <utility>


namespace vob::mistd
{
    template <typename TEnum, TEnum t_begin, TEnum t_end, typename TValue>
    class enum_map
    {
    public:
#pragma region CLASS_DATA
        static constexpr auto begin_value = std::underlying_type_t<TEnum>{ t_begin };
        static constexpr auto end_value = std::underlying_type_t<TEnum>{ t_end };
        static_assert(begin_value < end_value && "Invalid enum range for enum_map.");
#pragma endregion

#pragma region CREATORS
        /// @brief TODO
        constexpr enum_map() = default;

        /// @brief TODO
        explicit constexpr enum_map(TValue const& a_init_value)
            : m_data{ a_init_value }
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
            return m_data[std::underlying_type_t<TEnum>{ a_key } - begin_value];
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
            return static_cast<std::size_t>(a_key) - begin_value < end_value;
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
            return m_data[std::underlying_type_t<TEnum>{ a_key } - begin_value];
        }
#pragma endregion

    private:
#pragma region PRIVATE_CLASS_DATA
        static constexpr auto value_count = static_cast<std::size_t>(end_value - begin_value);
#pragma endregion

#pragma region DATA
        std::array<TValue, value_count> m_data;
#pragma endregion
    };
}
