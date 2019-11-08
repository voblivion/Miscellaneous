#pragma once

#include <cassert>
#include <memory>
#include <memory_resource>

#include <vob/sta/allocator.h>

namespace vob::sta
{
	namespace detail
	{
		class APolymorphicBlock
		{
		public:
			// Destructor
			virtual ~APolymorphicBlock() = default;

			// Methods
			virtual void* getPtr() noexcept = 0;
			virtual void destroy() noexcept = 0;
		};

		template <typename Type, typename AllocatorType>
		class PolymorphicBlock final
			: public APolymorphicBlock
		{
			// Aliases
			using Storage = std::aligned_union_t<1, Type>;
			using Block = PolymorphicBlock<Type, AllocatorType>;
			using Allocator = ReboundAlloc<AllocatorType, Block>;
			using AllocatorTraits = std::allocator_traits<Allocator>;

		public:
			// Constructors
			template <typename OtherAllocatorType, typename... Args>
			explicit PolymorphicBlock(
				OtherAllocatorType const& a_allocator
				, Args&&... a_args
			)
				: m_allocator{ a_allocator }
			{
				::new (static_cast<void*>(&m_storage))
					Type(std::forward<Args>(a_args)...);
			}

			// Methods
			virtual void* getPtr() noexcept override
			{
				return getTypePtr();
			}

			Type* getTypePtr() noexcept
			{
				return reinterpret_cast<Type*>(&m_storage);
			}

			virtual void destroy() noexcept override
			{
				getTypePtr()->~Type();
				AllocatorTraits::destroy(m_allocator, this);
				AllocatorTraits::deallocate(m_allocator, this, 1);
			}

		private:
			// Attributes
			Allocator m_allocator;
			Storage m_storage;
		};
	}

	class PolymorphicDeleter
	{
	public:
		// Constructors
		explicit PolymorphicDeleter() = default;
		explicit PolymorphicDeleter(
			detail::APolymorphicBlock* a_polymorphicBlock
		)
			: m_polymorphicBlock{ a_polymorphicBlock }
		{}

		// Operators
		void operator()(void* a_ptr)
		{
			assert(m_polymorphicBlock != nullptr
				&& m_polymorphicBlock->getPtr() == a_ptr);
			m_polymorphicBlock->destroy();
			m_polymorphicBlock = nullptr;
		}

	private:
		// Attributes
		detail::APolymorphicBlock* m_polymorphicBlock;
	};

	template <typename Type>
	using PolymorphicPtr = std::unique_ptr<Type, PolymorphicDeleter>;

	template <typename Type, typename AllocatorType, typename... Args>
	PolymorphicPtr<Type> allocatePolymorphic(
		AllocatorType const& a_allocator
		, Args&&... a_args
	)
	{
		using Block = detail::PolymorphicBlock<Type, AllocatorType>;
		using BlockAllocator = ReboundAlloc<AllocatorType, Block>;
		using BlockAllocatorTraits = std::allocator_traits<BlockAllocator>;
		
		// Allocate block
		BlockAllocator allocator{ a_allocator };
		auto const block = BlockAllocatorTraits::allocate(allocator, 1);

		// Construct block
		try
		{
			BlockAllocatorTraits::construct(
				allocator
				, block
				, allocator
				, std::forward<Args>(a_args)...
			);
		}
		catch (...)
		{
			BlockAllocatorTraits::deallocate(allocator, block, 1);
			throw;
		}

		return PolymorphicPtr<Type>{block->getTypePtr()
			, PolymorphicDeleter{ block } };
	}

	template <typename TargetType, typename SourceType>
	PolymorphicPtr<TargetType> staticPolymorphicCast(
		PolymorphicPtr<SourceType> a_ptr
	)
	{
		return {
			static_cast<TargetType*>(a_ptr.release())
			, a_ptr.get_deleter()
		};
	}

	template <typename PointerType>
	struct PolymorphicEqual
	{
		constexpr auto operator()(
			PointerType const& a_lhs, PointerType const& a_rhs
		) const
		{
			// TODO c++20 : use std::pointer_traits<PointerType>::to_address
			return typeid(*a_lhs) == typeid(*a_rhs);
		}
	};
}
