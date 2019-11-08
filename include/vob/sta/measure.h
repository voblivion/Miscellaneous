#pragma once

#include <cstdint>
#include <type_traits>


namespace vob::sta
{
	template <
		int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
		, typename ValueType
		, typename = std::enable_if_t<std::is_floating_point_v<ValueType>>
	>
		struct Measure
	{
		ValueType m_value{};

		Measure() = default;

		template <typename OtherValueType>
		constexpr Measure(OtherValueType a_value)
			: m_value{ static_cast<ValueType>(a_value) }
		{}

		template <typename OtherValueType>
		constexpr Measure(
			Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, OtherValueType> const& a_other
		)
			: m_value{ static_cast<ValueType>(a_other.m_value) }
		{}

		template <typename OtherValueType>
		constexpr auto& operator=(
			Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, OtherValueType> const& a_other
		)
		{
			m_value = static_cast<ValueType>(a_other.m_value);
			return *this;
		}

		template <typename OtherValueType>
		constexpr auto& operator=(OtherValueType a_value)
		{
			m_value = static_cast<ValueType>(a_value);
			return *this;
		}

		template <typename OtherValueType>
		explicit constexpr operator OtherValueType() const
		{
			return static_cast<OtherValueType>(m_value);
		}
	};

	template <
		int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
		, typename ValueType
	>
		auto makeMeasure(ValueType a_value)
	{
		return Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, ValueType>{ a_value };
	}

	template <
		int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
		, std::int8_t Exp
		, typename ValueType
		, std::int8_t OtherExp
		, typename OtherValueType
	>
		auto operator+(
			Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, ValueType> const& a_lhs
			, Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, OtherValueType> const& a_rhs
			)
	{
		return makeMeasure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature>(a_lhs.m_value + a_rhs.m_value);
	}

	template <
		int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
		, typename ValueType
		, typename OtherValueType
	>
		auto operator-(
			Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, ValueType> const& a_lhs
			, Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, OtherValueType> const& a_rhs
			)
	{
		return makeMeasure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature>(a_lhs.m_value - a_rhs.m_value);
	}

	template <
		int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
		, typename ValueType
	>
		auto operator-(
			Measure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, ValueType> const& a_measure
			)
	{
		return makeMeasure<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature>(-a_measure.m_value);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType
		, int OT, int OL, int OM, int OEC, int OTT, typename OtherValueType
	>
		auto operator*(
			Measure<T, L, M, EC, TT, ValueType> const& a_lhs
			, Measure<OT, OL, OM, OEC, OTT, OtherValueType> const& a_rhs
			)
	{
		return makeMeasure<T + OT, L + OL, M + OM, EC + OEC, TT + OTT>(a_lhs.m_value * a_rhs.m_value);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType
		, int OT, int OL, int OM, int OEC, int OTT, typename OtherValueType
	>
		auto operator/(
			Measure<T, L, M, EC, TT, ValueType> const& a_lhs
			, Measure<OT, OL, OM, OEC, OTT, OtherValueType> const& a_rhs
			)
	{
		return makeMeasure<T - OT, L - OL, M - OM, EC - OEC, TT - OTT>(a_lhs.m_value / a_rhs.m_value);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType, typename ScalarType
	>
		auto operator*(
			Measure<T, L, M, EC, TT, ValueType> const& a_measure, ScalarType a_scalar
			)
	{
		return makeMeasure<T, L, M, EC, TT>(a_measure.m_value * a_scalar);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType, typename ScalarType
	>
		auto operator*(
			ScalarType a_scalar, Measure<T, L, M, EC, TT, ValueType> const& a_measure
			)
	{
		return makeMeasure<T, L, M, EC, TT>(a_scalar * a_measure.m_value);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType, typename ScalarType
	>
		auto operator/(
			Measure<T, L, M, EC, TT, ValueType> const& a_measure, ScalarType a_scalar
			)
	{
		return makeMeasure<T, L, M, EC, TT>(a_measure.m_value / a_scalar);
	}

	template <
		int T, int L, int M, int EC, int TT, typename ValueType, typename ScalarType
	>
		auto operator/(
			ScalarType a_scalar, Measure<T, L, M, EC, TT, ValueType> const& a_measure
			)
	{
		return makeMeasure<-T, -L, -M, -EC, -TT>(a_scalar / a_measure.m_value);
	}

	template <typename ValueType>
	using Time = Measure<1, 0, 0, 0, 0, ValueType>;

	template <typename ValueType>
	using Length = Measure<0, 1, 0, 0, 0, ValueType>;

	template <typename ValueType>
	using Area = decltype(std::declval<Length<ValueType>>() * std::declval<Length<ValueType>>());

	template <typename ValueType>
	using Volume = decltype(std::declval<Area<ValueType>>() * std::declval<Length<ValueType>>());

	template <typename ValueType>
	using Mass = Measure<0, 0, 1, 0, 0, ValueType>;

	template <typename ValueType>
	using ElectricCurrent = Measure<0, 0, 0, 1, 0, ValueType>;

	template <typename ValueType>
	using ThermodynamicTemperature = Measure<0, 0, 0, 0, 1, ValueType>;

	template <typename ValueType>
	using Speed = decltype(std::declval<Length<ValueType>>() / std::declval<Time<ValueType>>());

	template <typename ValueType>
	using Acceleration = decltype(std::declval<Speed<ValueType>>() / std::declval<Time<ValueType>>());

	template <typename ValueType>
	using Frequency = decltype(std::declval<ValueType>() / std::declval<Time<ValueType>>());

	namespace literals
	{
		auto operator""_s(long double a_value)
		{
			return Time<double>{ a_value };
		}

		auto operator""_s(unsigned long long int a_value)
		{
			return Time<double>{ static_cast<double>(a_value) };
		}

		auto operator""_ms(long double a_value)
		{
			return Time<double>{ a_value / 1000 };
		}

		auto operator""_ms(unsigned long long int a_value)
		{
			return Time<double>{ static_cast<double>(a_value) / 1000 };
		}

		auto operator""_km(long double a_value)
		{
			return Length<double>{ a_value * 1000 };
		}

		auto operator""_km(unsigned long long int a_value)
		{
			return Length<double>{ static_cast<double>(a_value) * 1000 };
		}

		auto operator""_m(long double a_value)
		{
			return Length<double>{ a_value };
		}

		auto operator""_m(unsigned long long int a_value)
		{
			return Length<double>{ static_cast<double>(a_value) };
		}

		auto operator""_mm(long double a_value)
		{
			return Length<double>{ a_value / 1000 };
		}

		auto operator""_mm(unsigned long long int a_value)
		{
			return Length<double>{ static_cast<double>(a_value) / 1000 };
		}

		auto operator""_km2(long double a_value)
		{
			return Area<double>{ a_value * 1000 * 1000 };
		}

		auto operator""_km2(unsigned long long int a_value)
		{
			return Area<double>{ static_cast<double>(a_value) * 1000 * 1000 };
		}

		auto operator""_m2(long double a_value)
		{
			return Area<double>{ a_value };
		}

		auto operator""_m2(unsigned long long int a_value)
		{
			return Area<double>{ static_cast<double>(a_value) };
		}

		auto operator""_m3(long double a_value)
		{
			return Volume<double>{ a_value };
		}

		auto operator""_m3(unsigned long long int a_value)
		{
			return Volume<double>{ static_cast<double>(a_value) };
		}

		auto operator""_L(long double a_value)
		{
			return Volume<double>{ a_value / 1000 };
		}

		auto operator""_L(unsigned long long int a_value)
		{
			return Volume<double>{ static_cast<double>(a_value) / 1000 };
		}

		auto operator""_tonne(long double a_value)
		{
			return Mass<double>{ a_value * 1000 };
		}

		auto operator""_tonne(unsigned long long int a_value)
		{
			return Mass<double>{ static_cast<double>(a_value) * 1000 };
		}

		auto operator""_kg(long double a_value)
		{
			return Mass<double>{ a_value };
		}

		auto operator""_kg(unsigned long long int a_value)
		{
			return Mass<double>{ static_cast<double>(a_value) };
		}

		auto operator""_g(long double a_value)
		{
			return Mass<double>{ a_value / 1000 };
		}

		auto operator""_g(unsigned long long int a_value)
		{
			return Length<double>{ static_cast<double>(a_value) / 1000 };
		}

		auto operator""_A(long double a_value)
		{
			return ElectricCurrent<double>{ a_value };
		}

		auto operator""_A(unsigned long long int a_value)
		{
			return ElectricCurrent<double>{ static_cast<double>(a_value) };
		}

		auto operator""_K(long double a_value)
		{
			return ThermodynamicTemperature<double>{ a_value };
		}

		auto operator""_K(unsigned long long int a_value)
		{
			return ThermodynamicTemperature<double>{ static_cast<double>(a_value) };
		}

		auto operator""_dC(long double a_value)
		{
			return ThermodynamicTemperature<double>{ a_value + 273.15 };
		}

		auto operator""_dC(unsigned long long int a_value)
		{
			return ThermodynamicTemperature<double>{ static_cast<double>(a_value) + 273 };
		}

		auto operator""_Hz(long double a_value)
		{
			return Frequency<double>{ a_value };
		}

		auto operator""_Hz(unsigned long long int a_value)
		{
			return Frequency<double>{ static_cast<double>(a_value) };
		}
	}
}
