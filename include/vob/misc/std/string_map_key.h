#pragma once

#include <iostream>
#include <string>
#include <string_view>


namespace vob::mistd
{
	/// @brief TODO
	template <typename TString, typename TStringView = std::string_view>
	class basic_string_map_key
	{
	public:
#pragma region TYPES
		using string_type = TString;
		using string_view_type = TStringView;
#pragma endregion

#pragma region CREATORS
		/// @brief TODO
		basic_string_map_key() = default;

		/// @brief TODO
		basic_string_map_key(TString a_string)
			: m_string{ std::move(a_string) }
		{}

		/// @brief TODO
		basic_string_map_key(TStringView a_stringView)
			: m_stringView{ a_stringView }
		{}

		basic_string_map_key(basic_string_map_key&& a_other)
			: m_string{
				a_other.m_string.data() == a_other.m_stringView.data()
				? std::move(a_other.m_string) : a_other.m_stringView }
		{}

		basic_string_map_key(basic_string_map_key const& a_other)
			: m_string{ a_other.m_stringView }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		auto string() const -> TString const&
		{
			return m_string;
		}

		/// @brief TODO
		auto view() const -> TStringView
		{
			return m_stringView;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		auto string() -> TString&
		{
			return m_string;
		}

		/// @brief TODO
		auto view() -> TStringView&
		{
			return m_stringView;
		}

		auto& operator=(basic_string_map_key&& a_other)
		{
			m_string = a_other.m_string.data() == a_other.m_stringView.data()
				? std::move(a_other.m_string) : TString{ a_other.m_stringView };
			m_stringView = m_string;
		}

		auto& operator=(basic_string_map_key const& a_other)
		{
			m_string = TString{ a_other.m_stringView };
			m_stringView = m_string;
		}
#pragma endregion

	private:
#pragma region DATA
		TString m_string;
		TStringView m_stringView{ m_string };
#pragma endregion
	};

	/// @brief TODO
	template <typename TString, typename TStringView>
	bool operator==(
		basic_string_map_key<TString, TStringView> const& a_lhs,
		basic_string_map_key<TString, TStringView> const& a_rhs)
	{
		return a_lhs.view() == a_rhs.view();
	}

	/// @brief TODO
	template <typename TString, typename TStringView>
	bool operator!=(
		basic_string_map_key<TString, TStringView> const& a_lhs,
		basic_string_map_key<TString, TStringView> const& a_rhs)
	{
		return a_lhs.view() != a_rhs.view();
	}

	/// @brief TODO
	using string_map_key = basic_string_map_key<std::string>;

	/// @brief TODO
	template <typename TString, typename TStringView = std::string_view, typename THash = std::hash<std::string_view>>
	struct basic_string_map_key_hash
	{
		std::size_t operator()(basic_string_map_key<TString, TStringView> const& a_key) const
		{
			return THash{}(a_key.view());
		}
	};

	/// @brief TODO
	using string_map_key_hash = basic_string_map_key_hash<std::string>;

	/// @brief TODO
	template <typename TString, typename TStringView = std::string_view>
	std::istream& operator>>(std::istream& a_inputStream, basic_string_map_key<TString, TStringView>& a_key)
	{
		a_key = basic_string_map_key<TString, TStringView>{};
		a_inputStream >> a_key.string();
		return a_inputStream;
	}

	namespace pmr
	{
		/// @brief TODO
		using string_map_key = basic_string_map_key<std::pmr::string>;

		/// @brief TODO
		using string_map_key_hash = basic_string_map_key_hash<std::pmr::string>;
	}
}