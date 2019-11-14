#pragma once

#include <cassert>
#include <memory>
#include <memory_resource>


namespace vob::sta::detail
{
	using namespace std;

	class polymorphic_block_base
	{
	public:
		polymorphic_block_base() = default;
		polymorphic_block_base(polymorphic_block_base&&) = delete;
		polymorphic_block_base(polymorphic_block_base const&) = delete;
		virtual ~polymorphic_block_base() = default;

		virtual void* get_ptr() noexcept = 0;
		virtual void destroy() noexcept = 0;

		polymorphic_block_base& operator=(polymorphic_block_base&&) = delete;
		polymorphic_block_base& operator=(polymorphic_block_base const&) = delete;
	};

	template <typename T, typename Allocator>
	class polymorphic_block final
		: public polymorphic_block_base
	{
	public:
#pragma region Aliases
		using storage = aligned_union_t<1, T>;
		using allocator = typename allocator_traits<Allocator>::template rebind_alloc<polymorphic_block>;
		using allocator_traits = allocator_traits<allocator>;
#pragma endregion
#pragma region Constructors
		template <typename OtherAllocator, typename... Args>
		explicit polymorphic_block(OtherAllocator const& a_allocator, Args&&... a_args)
			: m_allocator{ a_allocator }
		{
			::new (static_cast<void*>(&m_storage)) T(forward<Args>(a_args)...);
		}
#pragma endregion
#pragma region Methods
		void* get_ptr() noexcept override
		{
			return get_type_ptr();
		}

		T* get_type_ptr() noexcept
		{
			return reinterpret_cast<T*>(&m_storage);
		}

		virtual void destroy() noexcept override
		{
			get_type_ptr()->~T();
			allocator_traits::destroy(m_allocator, this);
			allocator_traits::deallocate(m_allocator, this, 1);
		}
#pragma endregion
	private:
#pragma region Attributes
		allocator m_allocator;
		storage m_storage;
#pragma endregion
	};

	class polymorphic_deleter
	{
	public:
#pragma region Constructors
		explicit polymorphic_deleter() = default;

		explicit polymorphic_deleter(polymorphic_block_base* a_polymorphic_block)
			: m_polymorphic_block{ a_polymorphic_block }
		{}
#pragma endregion
#pragma region Operators
		void operator()(void* a_ptr)
		{
			assert(m_polymorphic_block != nullptr
				&& m_polymorphic_block->get_ptr() == a_ptr);
			m_polymorphic_block->destroy();
			m_polymorphic_block = nullptr;
		}
#pragma endregion 
	private:
#pragma region Attributes
		polymorphic_block_base* m_polymorphic_block;
#pragma endregion
	};
}

namespace vob::sta
{
	using namespace std;

	template <typename T>
	using polymorphic_ptr = unique_ptr<T, detail::polymorphic_deleter>;

	template <typename T, typename Allocator, typename... Args>
	polymorphic_ptr<T> allocate_polymorphic(Allocator const& a_allocator, Args&&... a_args)
	{
		using block_t = detail::polymorphic_block<T, Allocator>;
		using block_allocator = typename allocator_traits<Allocator>::template rebind_alloc<block_t>;
		using block_allocator_traits = allocator_traits<block_allocator>;

		block_allocator allocator{ a_allocator };
		auto const block = block_allocator_traits::allocate(allocator, 1);

		try
		{
			block_allocator_traits::construct(allocator, block, allocator, std::forward<Args>(a_args)...);
		}
		catch (...)
		{
			block_allocator_traits::deallocate(allocator, block, 1);
			throw;
		}

		return polymorphic_ptr<T>{ block->get_type_ptr(), detail::polymorphic_deleter{ block } };
	}

	template <typename Target, typename Source>
	polymorphic_ptr<Target> static_polymorphic_cast(polymorphic_ptr<Source> a_ptr)
	{
		return { static_cast<Target*>(a_ptr.release()), a_ptr.get_deleter() };
	}
}
