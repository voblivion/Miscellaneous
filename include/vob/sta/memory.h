#pragma once

#include <memory>

namespace vob::sta
{
	namespace detail
	{
		struct a_polymorphic_block
		{
			virtual void destroy() = 0;
		};

		template <typename Type, typename AllocatorType>
		struct polymorphic_block : public a_polymorphic_block
		{
			template <typename... Args>
			polymorphic_block(AllocatorType a_allocator, Args&&... a_args)
				: m_allocator{ std::move(a_allocator) }
				, m_object{ std::forward<Args>(a_args) }
			{}

			void destroy() override
			{
				this->~polymorphic_block();
				using Self = polymorphic_block<Type, AllocatorType>;
				using BlockAllocator = typename std::allocator_traits<AllocatorType>::template rebind_alloc<Self>;
				auto allocator = BlockAllocator{ m_allocator };
				allocator.deallocate(this, 1);
			}
		};

		struct polymorphic_deleter
		{
			void operator(void* a_ptr)
			{
				(static_cast<a_polymorphic_block*>(a_ptr) - 1)->destroy();
			}
		};
	}

	template <typename Type>
	using polymorphic_ptr = std::unique_ptr<Type, detail::polymorphic_deleter>;

	template <typename Type, typename AllocatorType, typename... Args>
	PolymorphicPtr<Type> allocatePolymorphic(AllocatorType a_allocator, Args&&... a_args)
	{
		using Block = detail::polymorphic_block<Type, AllocatorType>;
		using BlockAllocator = typename std::allocator_traits<AllocatorType>::template rebind_alloc<Block>;
		auto allocator = BlockAllocator{ a_allocator };

		auto block = allocator.allocate(1);
		::new (block) Block(a_allocator, std::forward<Args>(a_args)...);
		return PolymorphicPtr<Type>{ &block->m_object };
	}
}
