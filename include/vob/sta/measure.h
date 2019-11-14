#pragma once

#include <cstdint>


namespace vob::sta
{
	using namespace std;

	template <typename ValueType, int... Units>
	class measure
	{
	public:
#pragma region Statics & Aliases
		using value_type = ValueType;
#pragma endregion
#pragma region Attributes
		value_type value{};
#pragma endregion
#pragma region Constructors
		measure() = default;

		template <typename OtherValueType>
		explicit constexpr measure(OtherValueType a_value)
			: value{ static_cast<value_type>(a_value) }
		{}

		template <typename OtherValueType>
		constexpr measure(measure<OtherValueType, Units...> const& a_other)
			: value{ static_cast<value_type>(a_other.value) }
		{}
#pragma endregion
#pragma region Operators
		template <typename OtherValueType>
		constexpr auto& operator=(measure<OtherValueType, Units...> const& a_other)
		{
			value = static_cast<value_type>(a_other.value);
			return *this;
		}

		template <typename OtherValueType>
		constexpr auto& operator=(OtherValueType a_value)
		{
			value = static_cast<value_type>(a_value);
			return *this;
		}

		template <typename OtherValueType>
		explicit constexpr operator OtherValueType() const
		{
			return static_cast<OtherValueType>(value);
		}
#pragma endregion
	};

	template <int... Units, typename ValueType>
	auto make_measure(ValueType a_value)
	{
		return measure<ValueType, Units...>{ a_value };
	}

	template <typename ValueType, int... Units, typename OtherValueType>
	auto operator+(measure<ValueType, Units...> const& a_lhs, measure<OtherValueType, Units...> const& a_rhs)
	{
		return make_measure<Units...>(a_lhs.value + a_rhs.value);
	}

	template <typename ValueType, int... Units, typename OtherValueType>
	auto operator-(measure<ValueType, Units...> const& a_lhs, measure<OtherValueType, Units...> const& a_rhs)
	{
		return make_measure<Units...>(a_lhs.value - a_rhs.value);
	}

	template <typename ValueType, int... Units, typename OtherValueType, int... OtherUnits>
	auto operator*(measure<ValueType, Units...> const& a_lhs, measure<OtherValueType, OtherUnits...> const& a_rhs)
	{
		return make_measure<(Units + OtherUnits)...>(a_lhs.value * a_rhs.value);
	}

	template <typename ValueType, int... Units, typename OtherValueType, int... OtherUnits>
	auto operator/(measure<ValueType, Units...> const& a_lhs, measure<OtherValueType, OtherUnits...> const& a_rhs)
	{
		return make_measure<(Units - OtherUnits)...>(a_lhs.value / a_rhs.value);
	}

	template <typename ValueType, int... Units, typename Scalar>
	auto operator*(measure<ValueType, Units...> const& a_measure, Scalar a_scalar)
	{
		return make_measure<Units...>(a_measure.value * a_scalar);
	}

	template <typename ValueType, int... Units, typename Scalar>
	auto operator*(Scalar a_scalar, measure<ValueType, Units...> const& a_measure)
	{
		return make_measure<Units...>(a_measure.value * a_scalar);
	}

	template <typename ValueType, int... Units, typename Scalar>
	auto operator/(measure<ValueType, Units...> const& a_measure, Scalar a_scalar)
	{
		return make_measure<Units...>(a_measure.value / a_scalar);
	}

	template <typename ValueType, int... Units, typename Scalar>
	auto operator/(Scalar a_scalar, measure<ValueType, Units...> const& a_measure)
	{
		return make_measure<(-Units)...>(a_scalar / a_measure.value);
	}
}
