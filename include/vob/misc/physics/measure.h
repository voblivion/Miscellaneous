#pragma once

#include "unit.h"

#include <chrono>


namespace vob::misph
{
	/// @brief Trait to help deternime what time of measure can be converted to another with a different
	/// representation.
	template <typename TRepresentation>
	struct treat_as_floating_point : std::is_floating_point<TRepresentation> {};

	template <typename TRepresentation>
	inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<TRepresentation>::value;

	/// @brief A physic quantity within the International System of Units.
	template <typename TUnit, typename TRepresentation = float>
	class measure
	{
	public:
#pragma region CREATORS
		/// @brief Constructs a measure with uninitialized content.
		constexpr measure() = default;

		/// @brief Constructs a measure with an initial representation.
		/// @param a_value : the value assign to the measure.
		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr explicit measure(TRepresentation2 const& a_value)
			: m_value{ static_cast<TRepresentation>(a_value) }
		{}

		/// @brief Constructs a measure from another one with a different representation.
		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr measure(measure<TUnit, TRepresentation2> const& a_other)
			: m_value{ static_cast<TRepresentation>(a_other.get_value()) }
		{}
#pragma endregion

#pragma region ACCESSORS
		[[nodiscard]] constexpr auto operator+() const -> measure
		{
			return measure{ *this };
		}

		[[nodiscard]] constexpr auto operator-() const -> measure
		{
			return measure{ -m_value };
		}

		/// @brief Get the internal representation of the measure.
		[[nodiscard]] constexpr TRepresentation get_value() const
		{
			return m_value;
		}
#pragma endregion

#pragma region MANIPULATORS
		constexpr auto operator++() -> measure&
		{
			++m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator++(int) -> measure
		{
			return measure{ m_value++ };
		}

		constexpr auto operator--() -> measure&
		{
			--m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator--(int) -> measure
		{
			return measure{ m_value-- };
		}

		constexpr auto operator+=(measure const& a_rhs) -> measure&
		{
			m_value += a_rhs.m_value;
			return *this;
		}

		constexpr auto operator-=(measure const& a_rhs) -> measure&
		{
			m_value -= a_rhs.m_value;
			return *this;
		}

		constexpr auto operator*=(TRepresentation const& a_rhs) -> measure&
		{
			m_value *= a_rhs;
			return *this;
		}

		constexpr auto operator/=(TRepresentation const& a_rhs) -> measure&
		{
			m_value /= a_rhs;
			return *this;
		}

		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr auto operator=(measure<TUnit, TRepresentation2> const& a_rhs) -> measure&
		{
			m_value = static_cast<TRepresentation>(a_rhs.get_value());
			return *this;
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TRepresentation m_value;
#pragma endregion
	};

	/// @brief A specialization of measure for scalar type.
	/// Drops the explicit requirement on the value-initialized constructor and adds appropriate assignment operator.
	template <typename TRepresentation>
	class measure<unit_scalar, TRepresentation>
	{
	public:
#pragma region CREATORS
		/// @brief Constructs a measure with uninitialized content.
		constexpr measure() = default;

		/// @brief Constructs a measure with an initial representation.
		/// @param a_value : the value assign to the measure.
		constexpr measure(TRepresentation const& a_value)
			: m_value{ a_value }
		{}

		/// @brief Constructs a measure from another one with a different representation.
		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr measure(measure<unit_scalar, TRepresentation2> const& a_other)
			: m_value{ static_cast<TRepresentation2>(a_other.get_value()) }
		{}
#pragma endregion

#pragma region ACCESSORS
		[[nodiscard]] constexpr auto operator+() const -> measure
		{
			return measure{ *this };
		}

		[[nodiscard]] constexpr auto operator-() const -> measure
		{
			return measure{ -m_value };
		}

		/// @brief Get the internal representation of the measure.
		[[nodiscard]] constexpr TRepresentation get_value() const
		{
			return m_value;
		}
#pragma endregion

#pragma region MANIPULATORS
		constexpr auto operator++() -> measure&
		{
			++m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator++(int) -> measure
		{
			return measure{ m_value++ };
		}

		constexpr auto operator--() -> measure&
		{
			--m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator--(int) -> measure
		{
			return measure{ m_value-- };
		}

		constexpr auto operator+=(measure const& a_rhs) -> measure&
		{
			m_value += a_rhs.m_value;
			return *this;
		}

		constexpr auto operator-=(measure const& a_rhs) -> measure&
		{
			m_value -= a_rhs.m_value;
			return *this;
		}

		constexpr auto operator*=(TRepresentation const& a_rhs) -> measure&
		{
			m_value *= a_rhs;
			return *this;
		}

		constexpr auto operator/=(TRepresentation const& a_rhs) -> measure&
		{
			m_value /= a_rhs;
			return *this;
		}

		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr auto operator=(measure<unit_scalar, TRepresentation2> const& a_rhs) -> measure&
		{
			m_value = static_cast<TRepresentation>(a_rhs.get_value());
			return *this;
		}

		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr auto operator=(TRepresentation2 const& a_rhs) -> measure&
		{
			m_value = static_cast<TRepresentation>(a_rhs);
			return *this;
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TRepresentation m_value;
#pragma endregion
	};

	/// @brief A specialization of measure for time type.
	/// Provides an additional conversion from std::chrono::duration.
	template <typename TRepresentation>
	class measure<unit_time, TRepresentation>
	{
	public:
#pragma region CREATORS
		/// @brief Constructs a measure with uninitialized content.
		constexpr measure() = default;

		/// @brief Constructs a measure with an initial representation.
		/// @param a_value : the value assign to the measure.
		constexpr explicit measure(TRepresentation const& a_value)
			: m_value{ a_value }
		{}

		/// @brief Constructs a measure from another one with a different representation.
		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr measure(measure<unit_time, TRepresentation2> const& a_other)
			: m_value{ static_cast<TRepresentation>(a_other.get_value()) }
		{}

		/// @brief Constructs a time measure from an implicitely convertible std::chrono::duration.
		/// @param a_duration : the std::chrono::duration to assign to the measure.
		template <typename TRepresentation2, typename TPeriod>
		constexpr measure(std::chrono::duration<TRepresentation2, TPeriod> const& a_duration)
			: m_value{ std::chrono::duration<TRepresentation>{ a_duration }.count() }
		{}
#pragma endregion

#pragma region ACCESSORS
		[[nodiscard]] constexpr auto operator+() const -> measure
		{
			return measure{ *this };
		}

		[[nodiscard]] constexpr auto operator-() const -> measure
		{
			return measure{ -m_value };
		}

		/// @brief Get the internal representation of the measure.
		[[nodiscard]] constexpr TRepresentation get_value() const
		{
			return m_value;
		}
#pragma endregion

#pragma region MANIPULATORS
		constexpr auto operator++() -> measure&
		{
			++m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator++(int) -> measure
		{
			return measure{ m_value++ };
		}

		constexpr auto operator--() -> measure&
		{
			--m_value;
			return *this;
		}

		[[nodiscard]] constexpr auto operator--(int) -> measure
		{
			return measure{ m_value-- };
		}

		constexpr auto operator+=(measure const& a_rhs) -> measure&
		{
			m_value += a_rhs.m_value;
			return *this;
		}

		constexpr auto operator-=(measure const& a_rhs) -> measure&
		{
			m_value -= a_rhs.m_value;
			return *this;
		}

		constexpr auto operator*=(TRepresentation const& a_rhs) -> measure&
		{
			m_value *= a_rhs;
			return *this;
		}

		constexpr auto operator/=(TRepresentation const& a_rhs) -> measure&
		{
			m_value /= a_rhs;
			return *this;
		}

		template <typename TRepresentation2>
		requires treat_as_floating_point_v<TRepresentation>
			|| (!treat_as_floating_point_v<TRepresentation2>)
		constexpr auto operator=(measure<unit_time, TRepresentation2> const& a_rhs) -> measure&
		{
			m_value = static_cast<TRepresentation>(a_rhs.get_value());
			return *this;
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		TRepresentation m_value;
#pragma endregion
	};

	/// @brief Scalar quantity within the International System of units.
	using measure_scalar = measure<unit_scalar>;
	/// @brief Mass quantity within the International System of units, in kilogram (kg).
	using measure_mass = measure<unit_mass>;
	/// @brief Length quantity within the International System of units, in metre (m).
	using measure_length = measure<unit_length>;
	/// @brief Time quantity within the International System of units, in second (s).
	using measure_time = measure<unit_time>;
	/// @brief Electric current quantity within the International System of units, in Ampere (A).
	using measure_current = measure<unit_current>;
	/// @brief Thermodynamic temperature quantity within the International System of units, in Kelvin (K).
	using measure_temperature = measure<unit_temperature>;
	/// @brief Luminous intensity quantity within the International System of units, in candela (cd).
	using measure_luminous_intensity = measure<unit_luminous_intensity>;

	/// @brief Area quantity within the International System of units, in squared metre (m^2).
	using measure_area = measure<unit_area>;
	/// @brief Volume quantity within the International System of units, in cubic metre (m^3). 
	using measure_volume = measure<unit_volume>;
	/// @brief Frequency quantity within the International System of units, in hertz (Hz = 1/s).
	using measure_frequency = measure<unit_frequency>;
	/// @brief Velocity quantity within the International System of units, in metre per second (m/s).
	using measure_velocity = measure<unit_velocity>;
	/// @brief Acceleration quantity within the International System of units, in meter per second squared (m/s^2).
	using measure_acceleration = measure<unit_acceleration>;
	/// @brief Force quantity within the International System of units, in Newton (N = kg*m/s^2).
	using measure_force = measure<unit_force>;
	/// @brief Energy quantity within the International System of units, in Joules (J = kg*m^2/s^2).
	using measure_energy = measure<unit_energy>;

	namespace detail
	{
		template <typename TUnit, typename TRepresentation>
		constexpr auto measure_make(TRepresentation const& a_value)
		{
			return measure<TUnit, TRepresentation>{ a_value };
		}

		template <typename TRepresentation>
		constexpr auto measure_make(TRepresentation const& a_value)
		{
			return measure<unit_scalar, TRepresentation>{ a_value };
		}

		template <typename TRepresentation, typename TPeriod>
		constexpr auto measure_make(std::chrono::duration<TRepresentation, TPeriod> const& a_duration)
		{
			return measure<unit_time, TRepresentation>{ a_duration };
		}
	}

#pragma region FREE_OPERATORS
	/// @brief Performs the addition of 2 Measures of the same unit.
	/// @return the sum of the 2 Measures, in the same unit
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator+(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		return detail::measure_make<TUnit>(a_lhs.get_value() + a_rhs.get_value());
	}

	/// @brief Performs the substraction of 2 Measures of the same unit.
	/// @return the difference of the 2 Measures, in the same unit
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator-(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		return detail::measure_make<TUnit>(a_lhs.get_value() - a_rhs.get_value());
	}

	/// @brief Performs the modulation of 2 Measures of the same unit.
	/// @return the modulation remainder the 2 Measures, in the same unit
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator%(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		return detail::measure_make<TUnit>(a_lhs.get_value() % a_rhs.get_value());
	}

	/// @brief Performs the multiplication of 2 Measures of (potentially) different units.
	/// @return the product of the 2 Measures, in the appropriate unit
	template <typename TUnit1, typename TUnit2, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator*(
		measure<TUnit1, TRepresentation> const& a_lhs, measure<TUnit2, TRepresentation2> const& a_rhs)
	{
		return detail::measure_make<unit_multiply_t<TUnit1, TUnit2>>(a_lhs.get_value() * a_rhs.get_value());
	}

	/// @brief Performs the division of 2 Measures of (potentially) different units.
	/// @return the quotient of the 2 Measures, in the appropriate unit
	template <typename TUnit1, typename TUnit2, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator/(
		measure<TUnit1, TRepresentation> const& a_lhs, measure<TUnit2, TRepresentation2> const& a_rhs)
	{
		return detail::measure_make<unit_divide_t<TUnit1, TUnit2>>(a_lhs.get_value() / a_rhs.get_value());
	}

	/// @brief Spaceship compare 2 Measures of the same unit.
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator<=>(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		return a_lhs.get_value() <=> a_rhs.get_value();
	}

	/// @brief Checks if 2 Measures of the same unit are equal.
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator==(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		using common_representation = std::common_type_t<TRepresentation, TRepresentation2>;
		return common_representation{ a_lhs.get_value() } == common_representation{ a_rhs.get_value() };
	}
	
	/// @brief Checks if 2 Measures of the same unit are equal.
	template <typename TUnit, typename TRepresentation, typename TRepresentation2>
	requires std::is_convertible_v<TRepresentation2, std::common_type_t<TRepresentation, TRepresentation2>>
	constexpr auto operator!=(
		measure<TUnit, TRepresentation> const& a_lhs, measure<TUnit, TRepresentation2> const& a_rhs)
	{
		using common_representation = std::common_type_t<TRepresentation, TRepresentation2>;
		return common_representation{ a_lhs.get_value() } != common_representation{ a_rhs.get_value() };
	}

	/// @brief Performs the multiplication of a measure by a scalar.
	template <typename TUnit, typename TRepresentation, typename TScalar>
	[[nodiscard]] constexpr auto operator*(measure<TUnit, TRepresentation> const& a_lhs, TScalar const& a_rhs)
	{
		return a_lhs * detail::measure_make(a_rhs);
	}

	/// @brief Performs the multiplication of a scalar by a measure.
	template <typename TScalar, typename TUnit, typename TRepresentation>
	[[nodiscard]] constexpr auto operator*(TScalar const& a_lhs, measure<TUnit, TRepresentation> const& a_rhs)
	{
		return detail::measure_make(a_lhs) * a_rhs;
	}

	/// @brief Performs the division of a measure by a scalar.
	template <typename TUnit, typename TRepresentation, typename TScalar>
	[[nodiscard]] constexpr auto operator/(measure<TUnit, TRepresentation> const& a_lhs, TScalar const& a_rhs)
	{
		return a_lhs / detail::measure_make(a_rhs);
	}

	/// @brief Performs the division of a scalar by a measure.
	template <typename TScalar, typename TUnit, typename TRepresentation>
	[[nodiscard]] constexpr auto operator/(TScalar const& a_lhs, measure<TUnit, TRepresentation> const& a_rhs)
	{
		return detail::measure_make(a_lhs) / a_rhs;
	}
#pragma endregion

#pragma region IOS_OPERATORS
	namespace detail
	{
		template <typename TUnit, typename TRepresentation, typename TSymbol>
		std::ostream& output(std::ostream& o_out, measure<TUnit, TRepresentation> const& a_measure, TSymbol const& a_unit)
		{
			constexpr auto k_zero = static_cast<TRepresentation>(0.0);

			auto const signedValue = a_measure.get_value();
			auto const value = std::abs(signedValue);

			if (signedValue < k_zero)
			{
				o_out << '-';
			}

			if (value == k_zero)
			{
				return o_out << 0 << a_unit;
			}
			if (value < 0.001)
			{
				return o_out << value * static_cast<TRepresentation>(1'000.0) << 'u' << a_unit;
			}
			if (value < 0.01)
			{
				return o_out << value * static_cast<TRepresentation>(100.0) << 'm' << a_unit;
			}
			if (value < 0.1)
			{
				return o_out << value * static_cast<TRepresentation>(10.0) << 'c' << a_unit;
			}
			if (value < 1.0)
			{
				return o_out << value * static_cast<TRepresentation>(10.0) << 'd' << a_unit;
			}
			if (value < 10.0)
			{
				return o_out << value << a_unit;
			}
			if (value < 100.0)
			{
				return o_out << value / static_cast<TRepresentation>(10.0) << "da" << a_unit;
			}
			if (value < 1'000.0)
			{
				return o_out << value / static_cast<TRepresentation>(100.0) << 'h' << a_unit;
			}

			return o_out << value / static_cast<TRepresentation>(1'000.0) << 'k' << a_unit;
		}
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_scalar, TRepresentation> const& a_measure)
	{
		return o_out << a_measure.get_value();
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_mass, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, 'g');
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_length, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, 'm');
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_time, TRepresentation> const& a_measure)
	{
		auto const signedValue = a_measure.get_value();
		auto const value = std::abs(signedValue);

		if (value < 60.0)
		{
			return detail::output(o_out, a_measure, 'm');
		}

		constexpr auto k_secondsInDay = static_cast<TRepresentation>(60.0 * 60.0 * 24.0);
		constexpr auto k_secondsInHour = static_cast<TRepresentation>(60.0 * 60.0);
		constexpr auto k_secondsInMinute = static_cast<TRepresentation>(60.0);
		auto const days = static_cast<uint32_t>(value / k_secondsInDay);
		auto const hours = static_cast<uint32_t>((value - days * k_secondsInDay) / k_secondsInHour);
		auto const minutes = static_cast<uint32_t>((value - days * k_secondsInDay - hours * k_secondsInHour) / k_secondsInMinute);
		auto const seconds = static_cast<uint32_t>(value - days * k_secondsInDay - hours * k_secondsInHour - minutes * k_secondsInMinute);

		if (signedValue < static_cast<TRepresentation>(0.0))
		{
			o_out << '-';
		}

		if (value < k_secondsInMinute)
		{
			return o_out << seconds << "s";
		}
		if (value < k_secondsInHour)
		{
			return o_out << minutes << "min " << seconds << "s";
		}
		if (value < k_secondsInDay)
		{
			return o_out << hours << "h " << minutes << "min " << seconds << "s";
		}

		return o_out << days << "day " << hours << "h " << minutes << "min " << seconds << "s";
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_current, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, 'm');
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_temperature, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, 'K');
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_luminous_intensity, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, "cd");
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_area, TRepresentation> const& a_measure)
	{
		return o_out << a_measure.get_value() << "m^2";
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_volume, TRepresentation> const& a_measure)
	{
		return o_out << a_measure.get_value() << "m^3";
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_frequency, TRepresentation> const& a_measure)
	{
		detail::output(o_out, a_measure, "Hz");
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_velocity, TRepresentation> const& a_measure)
	{
		return o_out << a_measure.get_value() << "m/s";
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_acceleration, TRepresentation> const& a_measure)
	{
		return o_out << a_measure.get_value() << "m/s^2";
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_force, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, "N");
	}

	template <typename TRepresentation>
	std::ostream& operator<<(std::ostream& o_out, measure<unit_energy, TRepresentation> const& a_measure)
	{
		return detail::output(o_out, a_measure, "J");
	}
#pragma endregion
}

namespace std
{
	template <typename TUnit, typename TRepresentation>
	auto abs(vob::misph::measure<TUnit, TRepresentation> const& a_value)
	{
		return a_value.get_value() > TRepresentation{} ? a_value : -a_value;
	}
}
