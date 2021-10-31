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
				, m_object{ std::forward<Args>(a_args)... }
			{}

			void destroy() override
			{
				this->~polymorphic_block();
				using Self = polymorphic_block<Type, AllocatorType>;
				using BlockAllocator = typename std::allocator_traits<AllocatorType>::template rebind_alloc<Self>;
				auto allocator = BlockAllocator{ m_allocator };
				allocator.deallocate(this, 1);
			}

			Type m_object;
			AllocatorType m_allocator;
		};

		struct polymorphic_deleter
		{
			polymorphic_deleter() = default;

			explicit polymorphic_deleter(a_polymorphic_block* a_block)
				: m_block{ a_block }
			{}

			void operator()(void* a_ptr)
			{
				assert((a_ptr != nullptr) == (m_block != nullptr));
				if (m_block != nullptr)
				{
					m_block->destroy();
				}
			}

			a_polymorphic_block* m_block = nullptr;
		};
	}

	template <typename Type>
	using polymorphic_ptr = std::unique_ptr<Type, detail::polymorphic_deleter>;

	template <typename Type, typename AllocatorType, typename... Args>
	polymorphic_ptr<Type> allocate_polymorphic(AllocatorType a_allocator, Args&&... a_args)
	{
		using Block = detail::polymorphic_block<Type, AllocatorType>;
		using BlockAllocator = typename std::allocator_traits<AllocatorType>::template rebind_alloc<Block>;
		auto allocator = BlockAllocator{ a_allocator };

		auto block = allocator.allocate(1);
		::new (block) Block(a_allocator, std::forward<Args>(a_args)...);
		return polymorphic_ptr<Type>{ &block->m_object, detail::polymorphic_deleter{ block } };
	}

	template <typename Type, typename BaseType>
	polymorphic_ptr<Type> polymorphic_pointer_cast(polymorphic_ptr<BaseType>& a_ptr)
	{
		return polymorphic_ptr<Type>{ static_cast<Type*>(a_ptr.release()), std::move(a_ptr.get_deleter()) };
	}

	template <typename Type, typename BaseType>
	polymorphic_ptr<Type> polymorphic_pointer_cast(polymorphic_ptr<BaseType>&& a_ptr)
	{
		return polymorphic_ptr<Type>{ static_cast<Type*>(a_ptr.release()), std::move(a_ptr.get_deleter()) };
	}
}
