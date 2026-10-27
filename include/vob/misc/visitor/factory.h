#pragma once

#include <cstddef>
#include <utility>
#include <variant>


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

	template <typename... TValues>
	struct factory_variant
	{
		std::variant<TValues...> operator()(std::size_t const a_index) const
		{
			return [a_index]<std::size_t... t_indexes>(std::index_sequence<t_indexes...>)
			{
				std::variant<TValues...> result;
				((t_indexes == a_index ? (result.template emplace<t_indexes>(), true) : false) || ...);
				return result;
			}(std::index_sequence_for<TValues...>{});
		}
	};
}
