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
		explicit measure(TRepresentation const& a_value)
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
}
