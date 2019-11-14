#pragma once

#include <array>
#include <memory_resource>


namespace vob::sta
{
	using namespace std;

	template <size_t BufferSize>
	class buffer_resource final
		: public pmr::memory_resource
	{
	public:
#pragma region Constructors
		explicit buffer_resource(
			memory_resource* const a_resource = pmr::get_default_resource()
		) noexcept
			: m_available_space{ BufferSize }
			, m_resource{ a_resource }
		{
			m_current_buffer = m_buffer.data();
		}

		buffer_resource(buffer_resource&&) = delete;
		buffer_resource(buffer_resource const&) = delete;

		~buffer_resource() = default;
#pragma endregion
#pragma region Methods
		auto upstream_resource() const noexcept
		{
			return m_resource;
		}
#pragma endregion
#pragma region Operators
		buffer_resource& operator=(buffer_resource&&) = delete;
		buffer_resource& operator=(buffer_resource const&) = delete;
#pragma endregion
	protected:
#pragma region Methods
		void* do_allocate(size_t a_bytes, size_t a_alignment) override
		{
			auto const ptr = align(a_alignment, a_bytes, m_current_buffer, m_available_space);
			if (ptr != nullptr)
			{
				m_current_buffer = static_cast<byte*>(ptr) + a_bytes;
				m_available_space -= a_bytes;
				return ptr;
			}

			return m_resource->allocate(a_bytes, a_alignment);
		}

		void do_deallocate(void* a_ptr, size_t a_bytes, size_t a_alignment) override
		{
			if (a_ptr < m_buffer.data() || m_current_buffer <= a_ptr)
			{
				m_resource->deallocate(a_ptr, a_bytes, a_alignment);
			}
		}

		bool do_is_equal(const memory_resource& a_resource) const noexcept override
		{
			return &a_resource == this;
		}
#pragma endregion
	private:
#pragma region Attributes
		alignas(alignof(max_align_t)) array<byte, BufferSize> m_buffer;
		size_t m_available_space;
		void* m_current_buffer;
		memory_resource* m_resource;
#pragma endregion
	};

	class dynamic_buffer_resource final
		: public pmr::memory_resource
	{
	public:
#pragma region Constructors
		explicit dynamic_buffer_resource(
			size_t const a_bufferSize,
			memory_resource* const a_resource = pmr::get_default_resource()
		) noexcept
			: m_buffer_size{ a_bufferSize }
			, m_available_space{ a_bufferSize }
			, m_resource{ a_resource }
		{}

		dynamic_buffer_resource(dynamic_buffer_resource&& a_resource) noexcept
			: m_buffer_size{ a_resource.m_buffer_size }
			, m_available_space{ a_resource.m_available_space }
			, m_buffer{ a_resource.m_buffer }
			, m_current_buffer{ a_resource.m_current_buffer }
			, m_resource{ a_resource.m_resource }
		{
			a_resource.m_buffer = nullptr;
		}

		dynamic_buffer_resource(dynamic_buffer_resource const&) = delete;

		~dynamic_buffer_resource()
		{
			if (m_buffer != nullptr)
			{
				m_resource->deallocate(m_buffer, m_buffer_size, alignof(max_align_t));
			}
		}
#pragma endregion
#pragma region Methods
		auto upstream_resource() const
		{
			return m_resource;
		}
#pragma endregion
#pragma region Operators
		dynamic_buffer_resource& operator=(dynamic_buffer_resource&& a_resource) noexcept
		{
			m_buffer_size = a_resource.m_buffer_size;
			m_available_space = a_resource.m_available_space;
			m_buffer = a_resource.m_buffer;
			a_resource.m_buffer = nullptr;
			m_current_buffer = a_resource.m_current_buffer;
			m_resource = a_resource.m_resource;
			return *this;
		}

		dynamic_buffer_resource& operator=(dynamic_buffer_resource const&) = delete;
#pragma endregion
	protected:
#pragma region Methods
		void* do_allocate(size_t a_bytes, size_t a_alignment) override
		{
			if (m_buffer == nullptr)
			{
				m_buffer = m_resource->allocate(m_buffer_size, alignof(max_align_t));
				m_current_buffer = m_buffer;
			}

			if (auto const ptr = align(a_alignment, a_bytes, m_current_buffer, m_available_space))
			{
				m_current_buffer = static_cast<byte*>(ptr) + a_bytes;
				m_available_space -= a_bytes;
				return ptr;
			}

			return m_resource->allocate(a_bytes, a_alignment);
		}

		void do_deallocate(void* a_ptr, size_t a_bytes, size_t a_alignment) override
		{
			if (a_ptr < m_buffer || m_current_buffer <= a_ptr)
			{
				m_resource->deallocate(a_ptr, a_bytes, a_alignment);
			}
		}

		bool do_is_equal(const memory_resource& a_resource) const noexcept override
		{
			return &a_resource == this;
		}
#pragma endregion
	private:
#pragma region Attributes
		size_t m_buffer_size;
		size_t m_available_space;
		void* m_buffer = nullptr;
		void* m_current_buffer = nullptr;
		memory_resource* m_resource;
#pragma endregion
	};
}