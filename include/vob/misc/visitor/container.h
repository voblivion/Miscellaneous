#pragma once


namespace vob::misvi
{
	/// @brief TODO
	template <typename TContainer, typename TFactory>
	struct container
	{
		TContainer& container;
		TFactory factory;
	};

	/// @brief TODO
	template <typename TContainer, typename TFactory>
	container<TContainer, TFactory> ctn(TContainer& a_container, TFactory a_factory)
	{
		return { a_container, std::move(a_factory) };
	}
}
