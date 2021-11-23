#pragma once

#include "measure.h"

namespace vob::misph::literals
{
	constexpr auto operator""_s(long double const a_value)
	{
		return measure<unit_time, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_s(unsigned long long int const a_value)
	{
		return measure<unit_time, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_ms(long double const a_value)
	{
		return measure<unit_time, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_ms(unsigned long long int const a_value)
	{
		return measure<unit_time, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_km(long double const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) * 1'000 };
	}

	constexpr auto operator""_km(unsigned long long int const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) * 1'000 };
	}

	constexpr auto operator""_m(long double const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_m(unsigned long long int const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_mm(long double const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_mm(unsigned long long int const a_value)
	{
		return measure<unit_length, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_km2(long double const a_value)
	{
		return measure<unit_area, double>{ static_cast<double>(a_value) * 1'000'000 };
	}

	constexpr auto operator""_km2(unsigned long long int const a_value)
	{
		return measure<unit_area, double>{ static_cast<double>(a_value) * 1'000'000 };
	}

	constexpr auto operator""_m2(long double const a_value)
	{
		return measure<unit_area, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_m2(unsigned long long int const a_value)
	{
		return measure<unit_area, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_m3(long double const a_value)
	{
		return measure<unit_volume, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_m3(unsigned long long int const a_value)
	{
		return measure<unit_volume, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_l(long double const a_value)
	{
		return measure<unit_volume, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_l(unsigned long long int const a_value)
	{
		return measure<unit_volume, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_tonne(long double const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) * 1'000 };
	}

	constexpr auto operator""_tonne(unsigned long long int const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) * 1'000 };
	}

	constexpr auto operator""_kg(long double const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_kg(unsigned long long int const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_g(long double const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_g(unsigned long long int const a_value)
	{
		return measure<unit_mass, double>{ static_cast<double>(a_value) / 1'000 };
	}

	constexpr auto operator""_a(long double const a_value)
	{
		return measure<unit_current, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_a(unsigned long long int const a_value)
	{
		return measure<unit_current, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_K(long double const a_value)
	{
		return measure<unit_temperature, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_K(unsigned long long int const a_value)
	{
		return measure<unit_temperature, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_dC(long double const a_value)
	{
		return measure<unit_temperature, double>{ static_cast<double>(a_value) + 273.15 };
	}

	constexpr auto operator""_dC(unsigned long long int const a_value)
	{
		return measure<unit_temperature, double>{ static_cast<double>(a_value) + 273.15 };
	}

	constexpr auto operator""_Hz(long double const a_value)
	{
		return measure<unit_frequency, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_Hz(unsigned long long int const a_value)
	{
		return measure<unit_frequency, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_mps(long double const a_value)
	{
		return measure<unit_velocity, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_mps(unsigned long long int const a_value)
	{
		return measure<unit_velocity, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_kmph(long double const a_value)
	{
		return measure<unit_velocity, double>{ static_cast<double>(a_value) / 3.6 };
	}

	constexpr auto operator""_kmph(unsigned long long int const a_value)
	{
		return measure<unit_velocity, double>{ static_cast<double>(a_value) / 3.6 };
	}

	constexpr auto operator""_N(long double const a_value)
	{
		return measure<unit_force, double>{ static_cast<double>(a_value) };
	}

	constexpr auto operator""_N(unsigned long long int const a_value)
	{
		return measure<unit_force, double>{ static_cast<double>(a_value) };
	}
}
