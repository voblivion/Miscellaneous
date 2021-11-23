#pragma once

#include "../std/ignorable_assert.h"
#include "../std/same_const.h"

#include <cassert>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace vob::misty
{
	namespace detail
	{
		/// @brief TODO
		template <bool t_const, typename... TArgs>
		class type_applicator_base
		{
		public:
#pragma region CREATORS
			/// @brief TODO
			virtual ~type_applicator_base() = default;
#pragma endregion

#pragma region ACCESSORS
			/// @brief TODO
			virtual void apply(mistd::conditional_const_t<t_const, void>* a_object, TArgs&&... a_args) const = 0;
#pragma endregion
		};

		/// @brief TODO
		template <
			typename TType,
			template <typename> typename TFunc,
			bool t_const,
			typename... TArgs
		>
		class basic_type_applicator final
			: public type_applicator_base<t_const, TArgs...>
		{
		public:
#pragma region CREATORS
			/// @brief TODO
			explicit basic_type_applicator(TFunc<TType> a_functor)
				: m_functor{ std::move(a_functor) }
			{}
#pragma endregion

#pragma region ACCESSORS
			/// @brief TODO
			void apply(mistd::conditional_const_t<t_const, void>* a_object, TArgs&&... a_args) const override
			{
				using Type = mistd::conditional_const_t<t_const, TType>;
				auto& t_object = *reinterpret_cast<Type*>(a_object);
				m_functor(t_object, std::forward<TArgs>(a_args)...);
			}
#pragma endregion

		private:
#pragma region PRIVATE_DATA
			TFunc<TType> m_functor;
#pragma endregion
		};
	}

	/// @brief TODO
	template <
		template <typename> typename TFunc,
		typename TAllocator,
		bool t_const,
		typename... TArgs>
	class basic_applicator
	{
#pragma region PRIVATE_TYPES
		using type_applicator_base = detail::type_applicator_base<t_const, TArgs...>;

		template <typename TType>
		using basic_type_applicator = detail::basic_type_applicator<TType, TFunc, t_const, TArgs...>;
#pragma endregion

	public:
#pragma region CREATORS
		/// @brief TODO
		explicit basic_applicator(TAllocator const& a_allocator = {})
			: m_typeApplicators{ allocator{ a_allocator } }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		bool is_registered(std::type_index const a_typeIndex) const
		{
			return m_typeApplicators.find(a_typeIndex) != m_typeApplicators.end();
		}

		/// @brief TODO
		template <typename TType>
		bool is_registered() const
		{
			return is_registered(typeid(TType));
		}

		/// @brief TODO
		template <typename TType>
		void apply(TType& a_object, TArgs&&... a_args) const
		{
			ignorable_assert(is_registered(typeid(a_object)));
			auto const t_it = m_typeApplicators.find(typeid(a_object));
			if(t_it != m_typeApplicators.end())
			{
				t_it->second->apply(&a_object, std::forward<TArgs>(a_args)...);
			}
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		template <typename TType>
		void register_type(TFunc<TType> a_functor = {})
		{
			assert(!is_registered<TType>());
			auto allocator = m_typeApplicators.get_allocator();
			m_typeApplicators.emplace(
				typeid(TType)
				, std::allocate_shared<basic_type_applicator<TType>>(allocator, a_functor)
			);
		}
#pragma endregion

	private:
#pragma region PRIVATE_TYPES
		using allocator = typename std::allocator_traits<TAllocator>::template rebind_alloc<
			std::pair<std::type_index const, std::shared_ptr<type_applicator_base>>
		>;
#pragma endregion

#pragma region PRIVATE_DATA
		std::unordered_map<
			std::type_index
			, std::shared_ptr<type_applicator_base>
			, std::hash<std::type_index>
			, std::equal_to<>
			, allocator
		> m_typeApplicators;
#pragma endregion
	};

	/// @brief TODO
	template <
		template <typename> typename TFunc,
		bool t_const,
		typename... TArgs>
	using applicator = basic_applicator<TFunc, std::allocator<char>, t_const, TArgs...>;

	namespace pmr
	{
		/// @brief TODO
		template <
			template <typename> typename TFunc,
			bool t_const,
			typename... TArgs>
		using applicator = basic_applicator<TFunc, std::pmr::polymorphic_allocator<char>, t_const, TArgs...>;
	}
}
