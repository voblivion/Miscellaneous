#pragma once


namespace vob::misvi
{
	template <typename TValue>
	struct factory
	{
		TValue operator()() const
		{
			return {};
		}
	};

	/// @brief TODO
	template <typename... TValues>
	struct factory_variant
	{
		// static_assert(false && "To implement.");
	};
}
