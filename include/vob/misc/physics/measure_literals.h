#pragma once

#include "measure.h"

namespace vob::misph::literals
{
	constexpr auto operator""_s(long double const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_s(unsigned long long int const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_ms(long double const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_ms(unsigned long long int const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_min(long double const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value * 60.0) };
	}

	constexpr auto operator""_min(unsigned long long int const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value * 60.0) };
	}

	constexpr auto operator""_h(long double const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value * 60.0 * 60.0) };
	}

	constexpr auto operator""_h(unsigned long long int const a_value)
	{
		return measure<unit_time, float>{ static_cast<float>(a_value * 60.0 * 60.0) };
	}

	constexpr auto operator""_km(long double const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) * 1'000 };
	}

	constexpr auto operator""_km(unsigned long long int const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) * 1'000 };
	}

	constexpr auto operator""_m(long double const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_m(unsigned long long int const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_mm(long double const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_mm(unsigned long long int const a_value)
	{
		return measure<unit_length, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_km2(long double const a_value)
	{
		return measure<unit_area, float>{ static_cast<float>(a_value) * 1'000'000 };
	}

	constexpr auto operator""_km2(unsigned long long int const a_value)
	{
		return measure<unit_area, float>{ static_cast<float>(a_value) * 1'000'000 };
	}

	constexpr auto operator""_m2(long double const a_value)
	{
		return measure<unit_area, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_m2(unsigned long long int const a_value)
	{
		return measure<unit_area, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_m3(long double const a_value)
	{
		return measure<unit_volume, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_m3(unsigned long long int const a_value)
	{
		return measure<unit_volume, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_l(long double const a_value)
	{
		return measure<unit_volume, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_l(unsigned long long int const a_value)
	{
		return measure<unit_volume, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_tonne(long double const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) * 1'000 };
	}

	constexpr auto operator""_tonne(unsigned long long int const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) * 1'000 };
	}

	constexpr auto operator""_kg(long double const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_kg(unsigned long long int const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_g(long double const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_g(unsigned long long int const a_value)
	{
		return measure<unit_mass, float>{ static_cast<float>(a_value) / 1'000 };
	}

	constexpr auto operator""_a(long double const a_value)
	{
		return measure<unit_current, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_a(unsigned long long int const a_value)
	{
		return measure<unit_current, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_K(long double const a_value)
	{
		return measure<unit_temperature, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_K(unsigned long long int const a_value)
	{
		return measure<unit_temperature, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_dC(long double const a_value)
	{
		return measure<unit_temperature, float>{ static_cast<float>(a_value) + 273.15f };
	}

	constexpr auto operator""_dC(unsigned long long int const a_value)
	{
		return measure<unit_temperature, float>{ static_cast<float>(a_value) + 273.15f };
	}

	constexpr auto operator""_Hz(long double const a_value)
	{
		return measure<unit_frequency, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_Hz(unsigned long long int const a_value)
	{
		return measure<unit_frequency, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_mps(long double const a_value)
	{
		return measure<unit_velocity, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_mps(unsigned long long int const a_value)
	{
		return measure<unit_velocity, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_kmph(long double const a_value)
	{
		return measure<unit_velocity, float>{ static_cast<float>(a_value) / 3.6f };
	}

	constexpr auto operator""_kmph(unsigned long long int const a_value)
	{
		return measure<unit_velocity, float>{ static_cast<float>(a_value) / 3.6f };
	}

	constexpr auto operator""_N(long double const a_value)
	{
		return measure<unit_force, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_N(unsigned long long int const a_value)
	{
		return measure<unit_force, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_J(long double const a_value)
	{
		return measure<unit_energy, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_J(unsigned long long int const a_value)
	{
		return measure<unit_energy, float>{ static_cast<float>(a_value) };
	}

	constexpr auto operator""_cal(long double const a_value)
	{
		return measure<unit_energy, float>{ static_cast<float>(a_value * 4.184f) };
	}

	constexpr auto operator""_cal(unsigned long long int const a_value)
	{
		return measure<unit_energy, float>{ static_cast<float>(a_value * 4.184f) };
	}
}
