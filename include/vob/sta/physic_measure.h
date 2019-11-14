#pragma once

#include <vob/sta/measure.h>


namespace vob::sta
{
	using namespace std;

	template <
		typename ValueType
		, int Time
		, int Length
		, int Mass
		, int ElectricCurrent
		, int ThermodynamicTemperature
	>
	using physic_measure = measure<
		ValueType
		, Time
		, Length
		, Mass
		, ElectricCurrent
		, ThermodynamicTemperature
	>;

	template <typename ValueType>
	using time_measure = physic_measure<ValueType, 1, 0, 0, 0, 0>;

	template <typename ValueType>
	using length_measure = physic_measure<ValueType, 0, 1, 0, 0, 0>;

	template <typename ValueType>
	using area_measure = decltype(
		declval<length_measure<ValueType>>() * declval<length_measure<ValueType>>()
	);

	template <typename ValueType>
	using volume_measure = decltype(
		declval<area_measure<ValueType>>() * declval<length_measure<ValueType>>()
	);

	template <typename ValueType>
	using mass_measure = physic_measure<ValueType, 0, 0, 1, 0, 0>;

	template <typename ValueType>
	using electric_current_measure = physic_measure<ValueType, 0, 0, 0, 1, 0>;

	template <typename ValueType>
	using thermodynamic_temperature_measure = physic_measure<ValueType, 0, 0, 0, 0, 1>;

	template <typename ValueType>
	using speed_measure = decltype(
		declval<length_measure<ValueType>>() / declval<time_measure<ValueType>>()
	);

	template <typename ValueType>
	using acceleration_measure = decltype(
		declval<speed_measure<ValueType>>() / declval<time_measure<ValueType>>()
	);

	template <typename ValueType>
	using frequency_measure = decltype(
		declval<ValueType>() / declval<time_measure<ValueType>>()
	);

	namespace literals
	{
		constexpr auto operator""_s(long double const a_value)
		{
			return time_measure<double>{ a_value };
		}

		constexpr auto operator""_s(unsigned long long int const a_value)
		{
			return time_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_ms(long double const a_value)
		{
			return time_measure<double>{ a_value / 1000 };
		}

		constexpr auto operator""_ms(unsigned long long int const a_value)
		{
			return time_measure<double>{ static_cast<double>(a_value) / 1000 };
		}

		constexpr auto operator""_km(long double const a_value)
		{
			return length_measure<double>{ a_value * 1000 };
		}

		constexpr auto operator""_km(unsigned long long int const a_value)
		{
			return length_measure<double>{ static_cast<double>(a_value) * 1000 };
		}

		constexpr auto operator""_m(long double const a_value)
		{
			return length_measure<double>{ a_value };
		}

		constexpr auto operator""_m(unsigned long long int const a_value)
		{
			return length_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_mm(long double const a_value)
		{
			return length_measure<double>{ a_value / 1000 };
		}

		constexpr auto operator""_mm(unsigned long long int const a_value)
		{
			return length_measure<double>{ static_cast<double>(a_value) / 1000 };
		}

		constexpr auto operator""_km2(long double const a_value)
		{
			return area_measure<double>{ a_value * 1000 * 1000 };
		}

		constexpr auto operator""_km2(unsigned long long int const a_value)
		{
			return area_measure<double>{ static_cast<double>(a_value) * 1000 * 1000 };
		}

		constexpr auto operator""_m2(long double const a_value)
		{
			return area_measure<double>{ a_value };
		}

		constexpr auto operator""_m2(unsigned long long int const a_value)
		{
			return area_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_m3(long double const a_value)
		{
			return volume_measure<double>{ a_value };
		}

		constexpr auto operator""_m3(unsigned long long int const a_value)
		{
			return volume_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_L(long double const a_value)
		{
			return volume_measure<double>{ a_value / 1000 };
		}

		constexpr auto operator""_L(unsigned long long int const a_value)
		{
			return volume_measure<double>{ static_cast<double>(a_value) / 1000 };
		}

		constexpr auto operator""_tonne(long double const a_value)
		{
			return mass_measure<double>{ a_value * 1000 };
		}

		constexpr auto operator""_tonne(unsigned long long int const a_value)
		{
			return mass_measure<double>{ static_cast<double>(a_value) * 1000 };
		}

		constexpr auto operator""_kg(long double const a_value)
		{
			return mass_measure<double>{ a_value };
		}

		constexpr auto operator""_kg(unsigned long long int const a_value)
		{
			return mass_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_g(long double const a_value)
		{
			return mass_measure<double>{ a_value / 1000 };
		}

		constexpr auto operator""_g(unsigned long long int const a_value)
		{
			return mass_measure<double>{ static_cast<double>(a_value) / 1000 };
		}

		constexpr auto operator""_A(long double const a_value)
		{
			return electric_current_measure<double>{ a_value };
		}

		constexpr auto operator""_A(unsigned long long int const a_value)
		{
			return electric_current_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_K(long double const a_value)
		{
			return thermodynamic_temperature_measure<double>{ a_value };
		}

		constexpr auto operator""_K(unsigned long long int const a_value)
		{
			return thermodynamic_temperature_measure<double>{ static_cast<double>(a_value) };
		}

		constexpr auto operator""_dC(long double const a_value)
		{
			return thermodynamic_temperature_measure<double>{ a_value + 273.15 };
		}

		constexpr auto operator""_dC(unsigned long long int const a_value)
		{
			return thermodynamic_temperature_measure<double>{ static_cast<double>(a_value) + 273 };
		}

		constexpr auto operator""_Hz(long double const a_value)
		{
			return frequency_measure<double>{ a_value };
		}

		constexpr auto operator""_Hz(unsigned long long int const a_value)
		{
			return frequency_measure<double>{ static_cast<double>(a_value) };
		}
	}
}