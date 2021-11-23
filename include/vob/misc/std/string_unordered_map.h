#pragma once

#include "string_map_key.h"

#include <unordered_map>
#include <string>
#include <string_view>


namespace vob::mistd
{
	/// @brief TODO
	template <
		typename TValue,
		typename TString = std::string,
		typename TStringView = std::string_view,
		typename THash = basic_string_map_key_hash<TString, TStringView>,
		typename TEqualTo = std::equal_to<>,
		typename TAllocator = std::allocator<std::pair<basic_string_map_key<TString, TStringView> const, TValue>>>
	using string_unordered_map = std::unordered_map<
		basic_string_map_key<TString, TStringView>, TValue, THash, TEqualTo, TAllocator>;

	namespace pmr
	{
		/// @brief TODO
		template <typename TValue>
		using string_unordered_map = mistd::string_unordered_map<
			TValue,
			std::pmr::string,
			std::string_view,
			string_map_key_hash,
			std::equal_to<>,
			std::pmr::polymorphic_allocator<std::pair<pmr::string_map_key const, TValue>>>;
	}
}
