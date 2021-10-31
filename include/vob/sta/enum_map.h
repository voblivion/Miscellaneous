#pragma once

#include <array>
#include <cassert>


namespace vob::sta
{
    template <typename EnumType, typename ValueType, EnumType t_begin, EnumType t_end>
    class enum_map
    {
    public:
#pragma region Constructors
        constexpr enum_map() = default;

        explicit constexpr enum_map(ValueType const& a_initValue)
            : m_data{ a_initValue }
        {}
#pragma endregion
#pragma region Methods
        constexpr auto begin()
        {
            return m_data.begin();
        }

        constexpr auto begin() const
        {
            return m_data.begin();
        }

        constexpr auto end()
        {
            return m_data.end();
        }

        constexpr auto end() const
        {
            return m_data.end();
        }

        constexpr auto empty() const
        {
            return c_size == 0ul;
        }
        
        constexpr auto size() const
        {
            return c_size;
        }

        constexpr auto contains(EnumType a_key) const
        {
            return static_cast<std::size_t>(a_key) - c_beginIndex < c_endIndex;
        }

#pragma endregion
#pragma region Operators
        constexpr auto& operator[](EnumType a_key)
        {
            assert(contains(a_key));
            return m_data[std::underlying_type_t<EnumType>(a_key) - c_beginIndex];
        }

        constexpr auto const& operator[](EnumType a_key) const
        {
            assert(contains(a_key));
            return m_data[std::underlying_type_t<EnumType>(a_key) - c_beginIndex];
        }
#pragma endregion
    private:
#pragma region Attributes
        static constexpr auto c_beginIndex = static_cast<std::size_t>(t_begin);
        static constexpr auto c_endIndex = static_cast<std::size_t>(t_end);
        static constexpr auto c_size = c_endIndex - c_beginIndex;

        std::array<ValueType, c_size> m_data;
#pragma endregion
    };
}