#pragma once

#include "string_map_key.h"
#include "vector_map.h"

#include <string>
#include <string_view>


namespace vob::mistd
{
	/// @brief TODO
	template <
		typename TValue,
		typename TString = std::string,
		typename TStringView = std::string_view,
		typename TEqualTo = std::equal_to<>,
		typename TAllocator = std::allocator<std::pair<basic_string_map_key<TString, TStringView> const, TValue>>>
	using string_vector_map = vector_map<
		basic_string_map_key<TString, TStringView>, TValue, TEqualTo, TAllocator>;

	namespace pmr
	{
		/// @brief TODO
		template <typename TValue>
		using string_vector_map = mistd::string_vector_map<
			TValue,
			std::pmr::string,
			std::string_view,
			std::equal_to<>,
			std::pmr::polymorphic_allocator<std::pair<pmr::string_map_key const, TValue>>>;
	}
}
