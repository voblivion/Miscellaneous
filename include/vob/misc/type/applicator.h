#pragma once

#include "../std/conditional_const.h"

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
			virtual ~type_applicator_base() = default;
#pragma endregion

#pragma region ACCESSORS
			/// @brief Applies inner logic to provided object and arguments.
			virtual void apply(mistd::conditional_const_t<t_const, void>* a_object, TArgs&&... a_args) const = 0;
#pragma endregion
		};

		/// @brief An applicator of TFunc to TValue and TArgs
		template <
			typename TValue,
			template <typename> typename TFunc,
			bool t_const,
			typename... TArgs
		>
		class basic_type_applicator final
			: public type_applicator_base<t_const, TArgs...>
		{
		public:
#pragma region CREATORS
			/// @brief Constructs a basic_type_applicator from the functor that should be applied to provided objects.
			explicit basic_type_applicator(TFunc<TValue> a_functor)
				: m_functor{ std::move(a_functor) }
			{}
#pragma endregion

#pragma region ACCESSORS
			/// @brief Applies inner logic to provided object and arguments.
			/// Caller must guarantee that provided object is a TValue pointer.
			void apply(mistd::conditional_const_t<t_const, void>* a_object, TArgs&&... a_args) const override
			{
				using Type = mistd::conditional_const_t<t_const, TValue>;
				auto& t_object = *reinterpret_cast<Type*>(a_object);
				m_functor(t_object, std::forward<TArgs>(a_args)...);
			}
#pragma endregion

		private:
#pragma region PRIVATE_DATA
			TFunc<TValue> m_functor;
#pragma endregion
		};
	}

	/// @brief A templated class to apply some logic to a type unknown at compile time.
	template <
		template <typename> typename TFunc,
		typename TAllocator,
		bool t_const,
		typename... TArgs>
	class basic_applicator
	{
#pragma region PRIVATE_TYPES
		using type_applicator_base = detail::type_applicator_base<t_const, TArgs...>;

		template <typename TValue>
		using basic_type_applicator = detail::basic_type_applicator<TValue, TFunc, t_const, TArgs...>;
#pragma endregion

	public:
#pragma region CREATORS
		/// @brief Constructs a basic_applicator from the allocator used to allocate the typed applicators.
		explicit basic_applicator(TAllocator const& a_allocator = {})
			: m_typeApplicators{ allocator{ a_allocator } }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Returns whether or not a type has been registered to be handled by this applicator.
		bool is_registered(std::type_index const a_typeIndex) const
		{
			return m_typeApplicators.find(a_typeIndex) != m_typeApplicators.end();
		}

		/// @brief Returns whether or not a type has been registered to be handled by this applicator.
		template <typename TValue>
		bool is_registered() const
		{
			return is_registered(typeid(TValue));
		}

		/// @brief Applies this applicator to the exact type of passed object.
		/// Does nothing if object's type hasn't been registered.
		template <typename TValue>
		void apply(TValue& a_object, TArgs&&... a_args) const
		{
			assert(is_registered(typeid(a_object)));
			auto const t_it = m_typeApplicators.find(typeid(a_object));
			if(t_it != m_typeApplicators.end())
			{
				t_it->second->apply(&a_object, std::forward<TArgs>(a_args)...);
			}
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Registers a type to be later handled by this applicator.
		template <typename TValue>
		void register_type(TFunc<TValue> a_functor = {})
		{
			assert(!is_registered<TValue>());
			auto allocator = m_typeApplicators.get_allocator();
			m_typeApplicators.emplace(
				typeid(TValue)
				, std::allocate_shared<basic_type_applicator<TValue>>(allocator, a_functor)
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

	/// @brief A basic_applicator using C++'s default allocator.
	template <
		template <typename> typename TFunc,
		bool t_const,
		typename... TArgs>
	using applicator = basic_applicator<TFunc, std::allocator<char>, t_const, TArgs...>;

	namespace pmr
	{
		/// @brief A basic_applicator using C++'s polymorphic allocator.
		template <
			template <typename> typename TFunc,
			bool t_const,
			typename... TArgs>
		using applicator = basic_applicator<TFunc, std::pmr::polymorphic_allocator<char>, t_const, TArgs...>;
	}
}
