#pragma once

#include "applicator.h"

#include "../std/polymorphic_ptr.h"
#include "../std/polymorphic_ptr_util.h"

#include <functional>


namespace vob::misty
{
	/// @brief A templated class to handle the copy of an unknown polymorphic pointer.
	/// @tparam TPolymorphicBase base class from which all registered types will be based.
	/// @tparam TAllocator allocator used to store the inner logic of cloner_copier.
	/// @tparam TCloneAllocator allocator used to allocate new copy of provided polymorphic objects.
	template <
		typename TPolymorphicBase,
		typename TCloneAllocator = std::allocator<char>,
		typename TAllocator = TCloneAllocator>
	class clone_copier
	{
#pragma region PRIVATE_TYPES
		/// @brief An operator class that allocates a copy of a provided source.
		template <typename TValue>
		struct clone_op
		{
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
		/// @brief Creates a clone_copier from the 2 allocators used by cloner_copier.
		///
		/// @param a_allocator allocator used to store the inner logic of cloner_copier.
		/// @param a_cloneAllocator allocator used to allocate new copy of provided polymorphic objects.
		explicit clone_copier(TAllocator const& a_allocator = {}, TCloneAllocator a_cloneAllocator = {})
			: m_cloneAllocator{ a_cloneAllocator }
			, m_applicator{ a_allocator }
		{}
#pragma endregion

#pragma region ACCESSORS
		/// @brief TODO
		template <typename TValue>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		[[nodiscard]] mistd::polymorphic_ptr<TValue> clone(mistd::polymorphic_ptr<TValue> const& a_source) const
		{
			if (a_source == nullptr)
			{
				return nullptr;
			}

			mistd::polymorphic_ptr<TPolymorphicBase> target = nullptr;
			m_applicator.apply(*a_source, target, m_cloneAllocator);
			return mistd::polymorphic_ptr_util::cast<TValue>(std::move(target));
		}

		/// @brief allocates a TValue instance from provided arguments using clone-allocator this clone_copier
		/// was constructed with.
		template <typename TValue, typename... TArgs>
		[[nodiscard]] auto create(TArgs&&... a_args) const
		{
			return mistd::polymorphic_ptr_util::allocate<TValue>(m_cloneAllocator, std::forward<TArgs>(a_args)...);
		}

		/// @brief Returns whether or not a type has been registered to be cloned by this clone_copier.
		template <typename TValue>
		requires std::is_base_of_v<TPolymorphicBase, TValue>
		[[nodiscard]] bool is_registered() const
		{
			return m_applicator.template is_registered<TValue>();
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Registers a type to be later handled by this clone_copier when passed a polymorphic object of that
		/// type.
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
		/// @brief A basic clone_copier using C++'s polymorphic allocator.
		template <typename TPolymorphicBase>
		using clone_copier = misty::clone_copier<TPolymorphicBase, std::pmr::polymorphic_allocator<char>>;
	}

	/// @brief A templated class to represent a polymorphic object that can be cloned thanks to a clone_copier.
	template <
		typename TValue,
		typename TCloneCopier = clone_copier<TValue>>
	class clone
	{
	public:
#pragma region CREATORS
		/// @brief Creates a clone instance from the clone copier used to perform its copy.
		explicit clone(TCloneCopier const& a_copier)
			: m_copier{ a_copier }
		{}

		clone(clone&&) = default;

		/// @brief Copies a clone instance by allocating a deep copy of its internal value.
		clone(clone const& a_other)
			: m_copier{ a_other.m_copier }
			, m_value{ m_copier.get().clone(a_other.m_value) }
		{}

		~clone()
		{
			reset();
		}
#pragma endregion

#pragma region ACCESSORS
		/// @brief Provides a const pointer to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue const* get() const
		{
			return m_value.get();
		}

		/// @brief Provides a const pointer to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue const* operator->() const
		{
			return m_value.get();
		}

		/// @brief Provides a const reference to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue const& operator*() const
		{
			return *m_value;
		}

		/// @brief Performs the pointer-like == comparison of two clone instances.
		[[nodiscard]] bool operator==(std::nullptr_t) const
		{
			return m_value == nullptr;
		}

		/// @brief Performs the pointer-like != comparison of two clone instance.
		[[nodiscard]] bool operator!=(std::nullptr_t) const
		{
			return m_value != nullptr;
		}
#pragma endregion

#pragma region MANIPULATORS
		/// @brief Provides a pointer to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue* get()
		{
			return m_value.get();
		}

		/// @brief Provides a pointer to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue* operator->()
		{
			return m_value.get();
		}

		/// @brief Provides a reference to the polymorphic type stored in this clone instance.
		[[nodiscard]] TValue& operator*()
		{
			return *m_value;
		}

		/// @brief Resets this clone instance, setting its internal pointer to null.
		void reset()
		{
			m_value.reset();
		}

		/// @brief Resets this clone instance by setting its internal pointer to provided one.
		template <typename TValue2>
		requires std::is_base_of_v<TValue, TValue2>
		void reset(mistd::polymorphic_ptr<TValue2> a_value)
		{
			m_value = std::move(a_value);
		}

		/// @brief Resets this clone instance by allocating a new instance of provided type with
		template <typename TValue2, typename... TArgs>
		requires std::is_base_of_v<TValue, TValue2>
		TValue2& init(TArgs&&... a_args)
		{
			m_value = m_copier.get().template create<TValue2>(std::forward<TArgs>(a_args)...);
			return static_cast<TValue2&>(*m_value);
		}

		clone& operator=(clone&& a_other)
		{
			m_value = std::move(a_other.m_value);
			return *this;
		}

		clone& operator=(clone const& a_other)
		{
			m_value = m_copier.get().clone(a_other.m_value);
			return *this;
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
		/// @brief A clone using C++'s polymorphic allocator.
		template <typename TValue, typename TCloneCopier = pmr::clone_copier<TValue>>
		using clone = misty::clone<TValue, TCloneCopier>;
	}
}
