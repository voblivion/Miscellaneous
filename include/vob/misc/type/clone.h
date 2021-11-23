#pragma once

#include "applicator.h"

#include "../std/polymorphic_ptr.h"
#include "../std/polymorphic_ptr_util.h"

#include <functional>


namespace vob::misty
{
	/// @brief TODO
	template <
		typename TPolymorphicBase,
		typename TCloneAllocator = std::allocator<char>,
		typename TAllocator = TCloneAllocator>
	class clone_copier
	{
#pragma region PRIVATE_TYPES
		/// @brief TODO
		template <typename TValue>
		class clone_op
		{
		public:
			/// @brief TODO
			void operator()(
				TValue const& a_source,
				mistd::polymorphic_ptr<TPolymorphicBase>& a_target,
				TCloneAllocator const& a_allocator) const
			{
				a_target = mistd::polymorphic_ptr_util::allocate<TValue>(a_allocator, a_source);
			}
		};
#pragma endregion

	public:
#pragma region CREATORS
		/// @brief TODO
		explicit clone_copier(TCloneAllocator a_cloneAllocator = {}, TAllocator const& a_allocator = {})
			: m_cloneAllocator{ a_cloneAllocator }
			, m_applicator{ a_allocator }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		template <typename TValue>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		mistd::polymorphic_ptr<TValue> clone(mistd::polymorphic_ptr<TValue> const& a_source) const
		{
			if (a_source == nullptr)
			{
				return nullptr;
			}

			mistd::polymorphic_ptr<TPolymorphicBase> target = nullptr;
			m_applicator.apply(*a_source, target, m_cloneAllocator);
			return mistd::polymorphic_ptr_util::cast<TValue>(std::move(target));
		}

		/// @brief TODO
		template <typename TValue, typename... TArgs>
		auto create(TArgs&&... a_args) const
		{
			return mistd::polymorphic_ptr_util::allocate<TValue>(m_cloneAllocator, std::forward<TArgs>(a_args)...);
		}

		/// @brief TODO
		template <typename TValue>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		bool is_registered() const
		{
			return m_applicator.template is_registered<TValue>();
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		template <typename TValue>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		void register_type()
		{
			m_applicator.template register_type<TValue>();
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		
#pragma endregion
		/// @brief TODO
		TCloneAllocator m_cloneAllocator;
		basic_applicator<
			clone_op,
			TAllocator,
			true,
			mistd::polymorphic_ptr<TPolymorphicBase>&,
			TCloneAllocator const&> m_applicator;
	};

	namespace pmr
	{
		/// @brief TODO
		template <typename TPolymorphicBase>
		using clone_copier = misty::clone_copier<TPolymorphicBase, std::pmr::polymorphic_allocator<char>>;
	}

	/// @brief TODO
	template <
		typename TValue,
		typename TPolymorphicBase,
		typename TCloneCopier = clone_copier<TPolymorphicBase>>
	class clone
	{
	public:
#pragma region CREATORS
		/// @brief TODO
		explicit clone(TCloneCopier const& a_copier)
			: m_copier{ a_copier }
		{}

		/// @brief TODO
		clone(clone&&) = default;

		/// @brief TODO
		clone(clone const& a_other)
			: m_copier{ a_other.m_copier }
			, m_value{ m_copier.get().clone(a_other.m_value) }
		{}

		/// @brief TODO
		~clone()
		{
			reset();
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		TValue const* get() const
		{
			return m_value.get();
		}

		/// @brief TODO
		TValue const* operator->() const
		{
			return m_value.get();
		}

		/// @brief TODO
		TValue const& operator*() const
		{
			return *m_value;
		}

		/// @brief TODO
		bool operator==(std::nullptr_t) const
		{
			return m_value == nullptr;
		}

		/// @brief TODO
		bool operator!=(std::nullptr_t) const
		{
			return m_value == nullptr;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief TODO
		TValue* get()
		{
			return m_value.get();
		}

		/// @brief TODO
		TValue* operator->()
		{
			return m_value.get();
		}

		/// @brief TODO
		TValue& operator*()
		{
			return *m_value;
		}

		/// @brief TODO
		void reset()
		{
			m_value.reset();
		}

		/// @brief TODO
		template <typename TValue2>
		void reset(mistd::polymorphic_ptr<TValue2> a_value)
		{
			m_value = std::move(a_value);
		}

		/// @brief TODO
		template <typename TValue2, typename... TArgs>
		TValue2& init(TArgs&&... a_args)
		{
			m_value = m_copier.get().template create<TValue2>(std::forward<TArgs>(a_args)...);
			return static_cast<TValue2&>(*m_value);
		}
#pragma endregion

	private:
#pragma region PRIVATE_DATA
		std::reference_wrapper<TCloneCopier const> m_copier;
		mistd::polymorphic_ptr<TValue> m_value = nullptr;
#pragma endregion
	};

	namespace pmr
	{
		/// @brief TODO
		template <typename TValue, typename TPolymorphicBase>
		using clone = misty::clone<TValue, TPolymorphicBase, pmr::clone_copier<TPolymorphicBase>>;
	}
}
