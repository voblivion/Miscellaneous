#pragma once


namespace vob::misvi
{
	/// @brief TODO
	template <typename TBase>
	struct dynamic_value
	{
		TBase& value;
	};

	/// @brief TODO
	template <typename TBase>
	dynamic_value<TBase> dnv(TBase& a_value)
	{
		return { a_value };
	}
}
