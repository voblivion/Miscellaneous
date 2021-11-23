#pragma once

#include <cassert>
#include <memory>


namespace vob::mistd
{
	namespace detail
	{
		struct polymorphic_block_base
		{
			virtual void destroy() = 0;
		};

		template <typename TObject, typename TAllocator>
		struct polymorphic_block final : public polymorphic_block_base
		{
			template <typename... TArgs>
			polymorphic_block(TAllocator a_allocator, TArgs&&... a_args)
				: m_allocator{ std::move(a_allocator) }
				, m_object{ std::forward<TArgs>(a_args)... }
			{}

			void destroy() override
			{
				this->~polymorphic_block();
				using block_allocator =
					typename std::allocator_traits<TAllocator>::template rebind_alloc<polymorphic_block>;
				auto allocator = block_allocator{ m_allocator };
				allocator.deallocate(this, 1);
			}

			TObject m_object;
			TAllocator m_allocator;
		};

		struct polymorphic_deleter
		{
			polymorphic_deleter() = default;

			explicit polymorphic_deleter(polymorphic_block_base* a_block)
				: m_block{ a_block }
			{}

			void operator()(void* a_ptr)
			{
				if (m_block != nullptr)
				{
					m_block->destroy();
				}
			}

			polymorphic_block_base* m_block = nullptr;
		};
	}

	template <typename TObject>
	using polymorphic_ptr = std::unique_ptr<TObject, detail::polymorphic_deleter>;
}