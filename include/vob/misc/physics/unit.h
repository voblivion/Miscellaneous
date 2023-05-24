#pragma once

#include <cstdint>


namespace vob::misph
{
	/// @brief Type representing a metric expressed within the International System of Units.
	template <
		std::int8_t t_mass
		, std::int8_t t_length
		, std::int8_t t_time
		, std::int8_t t_electric_current
		, std::int8_t t_thermodynamic_temperature
		, std::int8_t t_luminous_intensity
	>
	struct unit
	{
		static constexpr std::int8_t mass = t_mass;
		static constexpr std::int8_t length = t_length;
		static constexpr std::int8_t time = t_time;
		static constexpr std::int8_t electric_current = t_electric_current;
		static constexpr std::int8_t thermodynamic_temperature = t_thermodynamic_temperature;
		static constexpr std::int8_t luminous_intensity = t_luminous_intensity;
	};

	/// @brief Performs the logical multiplication of two units of the International System of Units.
	template <typename TUnit1, typename TUnit2>
	struct unit_multiply
	{
		using type = unit<
			TUnit1::mass + TUnit2::mass
			, TUnit1::length + TUnit2::length
			, TUnit1::time + TUnit2::time
			, TUnit1::electric_current + TUnit2::electric_current
			, TUnit1::thermodynamic_temperature + TUnit2::thermodynamic_temperature
			, TUnit1::luminous_intensity + TUnit2::luminous_intensity
		>;
	};

	template <typename TUnit1, typename TUnit2>
	using unit_multiply_t = typename unit_multiply<TUnit1, TUnit2>::type;

	/// @brief Performs the logical division of two units of the International System of Units.
	template <typename TUnit1, typename TUnit2>
	struct unit_divide
	{
		using type = unit<
			TUnit1::mass - TUnit2::mass
			, TUnit1::length - TUnit2::length
			, TUnit1::time - TUnit2::time
			, TUnit1::electric_current - TUnit2::electric_current
			, TUnit1::thermodynamic_temperature - TUnit2::thermodynamic_temperature
			, TUnit1::luminous_intensity - TUnit2::luminous_intensity
		>;
	};

	template <typename TUnit1, typename TUnit2>
	using unit_divide_t = typename unit_divide<TUnit1, TUnit2>::type;

	/// @brief Performs the logical square root of an unit of the International System of Units.
	template <typename TUnit>
	struct unit_sqrt
	{
		static_assert(
			TUnit::mass % 2 == 0
			&& TUnit::length % 2 == 0
			&& TUnit::time % 2 == 0
			&& TUnit::electric_current % 2 == 0
			&& TUnit::thermodynamic_temperature % 2 == 0
			&& TUnit::luminous_intensity % 2 == 0
		);
		using type = unit<
			TUnit::mass / 2
			, TUnit::length / 2
			, TUnit::time / 2
			, TUnit::electric_current / 2
			, TUnit::thermodynamic_temperature / 2
			, TUnit::luminous_intensity / 2
		>;
	};

	template <typename TUnit>
	using unit_sqrt_t = typename unit_sqrt<TUnit>::type;

	/// @brief type representing a scalar within the International System of Units.
	using unit_scalar = unit<0, 0, 0, 0, 0, 0>;
	/// @brief Type representing a mass (kg) within the International System of Units.
	using unit_mass = unit<1, 0, 0, 0, 0, 0>;
	/// @brief Type representing a length (m) within the International System of Units.
	using unit_length = unit<0, 1, 0, 0, 0, 0>;
	/// @brief Type representing a time (s) within the International System of Units.
	using unit_time = unit<0, 0, 1, 0, 0, 0>;
	/// @brief Type representing an electric current (A) within the International System of Units.
	using unit_current = unit<0, 0, 0, 1, 0, 0>;
	/// @brief Type representing a thermodynamic temperature (K) within the International System of Units.
	using unit_temperature = unit<0, 0, 0, 0, 1, 0>;
	/// @brief Type representing a luminous intensity (cd) within the International System of Units.
	using unit_luminous_intensity = unit<0, 0, 0, 0, 0, 1>;

	/// @brief Type representing an area (m^2) within the International System of Units.
	using unit_area = unit_multiply_t<unit_length, unit_length>;
	/// @brief Type representing a volume (m^3) within the International System of Units.
	using unit_volume = unit_multiply_t<unit_area, unit_length>;
	/// @brief Type representing a frequency (Hz = 1/s) within the International System of Units.
	using unit_frequency = unit_divide_t<unit_scalar, unit_time>;
	/// @brief Type representing a velocity (m/s) within the International System of Units.
	using unit_velocity = unit_multiply_t<unit_length, unit_frequency>;
	/// @brief Type representing an acceleration (m/s^2) within the International System of Units.
	using unit_acceleration = unit_multiply_t<unit_velocity, unit_frequency>;
	/// @brief Type representing a force (N = kg*m/s^2) within the International System of Units.
	using unit_force = unit_multiply_t<unit_mass, unit_acceleration>;
	/// @brief Type representing an energy (J = kg*m^2/s^2) within the International System of Units
	using unit_energy = unit_multiply_t<unit_force, unit_length>;
}