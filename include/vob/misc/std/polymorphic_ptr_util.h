#pragma once

#include "polymorphic_ptr.h"


namespace vob::mistd
{
	namespace polymorphic_ptr_util
	{
		template <typename TObject, typename TAllocator, typename... TArgs>
		inline auto allocate(TAllocator a_allocator, TArgs&&... a_args)
		{
			using block = detail::polymorphic_block<TObject, TAllocator>;
			using block_allocator = typename std::allocator_traits<TAllocator>::template rebind_alloc<block>;
			auto allocator = block_allocator{ a_allocator };

			auto allocated_block = allocator.allocate(1);
			::new (allocated_block) block(a_allocator, std::forward<TArgs>(a_args)...);
			return polymorphic_ptr<TObject>{
				&allocated_block->m_object, detail::polymorphic_deleter{ allocated_block } };
		}

		template <typename TDerived, typename TBase>
		inline auto cast(polymorphic_ptr<TBase> a_ptr)
		{
			return polymorphic_ptr<TDerived>{
				static_cast<TDerived*>(a_ptr.release()), std::move(a_ptr.get_deleter()) };
		}
	}
}