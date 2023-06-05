#pragma once

#include "basic_task.h"

#include <cassert>
#include <memory>
#include <mutex>
#include <thread>


namespace vob::mismt
{
	using task_id = std::size_t;

	template <typename TAllocator>
	struct basic_task_description
	{
		task_id m_id;
		std::vector<task_id, TAllocator> m_dependencies;
	};

	using task_description = basic_task_description<std::allocator<task_id>>;

	template <typename TAllocator>
	using basic_thread_schedule = std::vector<
		basic_task_description<TAllocator>,
		typename std::allocator_traits<TAllocator>::template rebind_alloc<basic_task_description<TAllocator>>
	>;

	using thread_schedule = basic_thread_schedule<std::allocator<task_id>>;

	template <typename TAllocator>
	using basic_schedule = std::vector<
		basic_thread_schedule<TAllocator>,
		typename std::allocator_traits<TAllocator>::template rebind_alloc<basic_thread_schedule<TAllocator>>
	>;

	using schedule = basic_schedule<std::allocator<task_id>>;

	namespace detail
	{
		class task_state
		{
		public:
			// Methods
			void wait_until_done()
			{
				std::unique_lock<std::mutex> lock{ m_mutex };
				m_sync.wait(lock, [this]
					{
						return m_isDone;
					});
			}

			void set_is_done(bool a_isDone)
			{
				{
					std::lock_guard<std::mutex> lock{ m_mutex };
					m_isDone = a_isDone;
				}
				m_sync.notify_all();
			}

		private:
			// Attributes
			bool m_isDone = true;
			std::mutex m_mutex;
			std::condition_variable m_sync;
		};

		template <typename TAllocator>
		using basic_task_state_list = std::vector<std::shared_ptr<task_state>, TAllocator>;

		using task_state_list = basic_task_state_list<std::allocator<std::shared_ptr<task_state>>>;

		template <typename TScheduleAllocator, typename TTaskStateAllocator>
		void execute_thread_schedule(
			basic_thread_schedule<TScheduleAllocator> const& a_schedule,
			task_span const& a_tasks,
			basic_task_state_list<TTaskStateAllocator>& m_taskStates)
		{
			for (auto& taskDescription : a_schedule)
			{
				for (auto& dependency : taskDescription.m_dependencies)
				{
					m_taskStates[dependency]->wait_until_done();
				}
				a_tasks[taskDescription.m_id]->execute();
				m_taskStates[taskDescription.m_id]->set_is_done(true);
			}
		}

		template <typename TScheduleAllocator, typename TTaskStateAllocator>
		class basic_thread_worker
		{
		public:
			// Constructors
			basic_thread_worker() = delete;

			basic_thread_worker(basic_thread_worker&&) = delete;

			basic_thread_worker(basic_thread_worker const&) = delete;

			basic_thread_worker(
				task_span const& a_tasks,
				basic_task_state_list<TTaskStateAllocator>& a_taskStates,
				basic_thread_schedule<TScheduleAllocator> a_schedule)
				: m_tasks{ a_tasks }
				, m_taskStates{ a_taskStates }
				, m_schedule{ std::move(a_schedule) }
			{
				m_thread = std::thread{ &basic_thread_worker::start, this };
			}

			~basic_thread_worker()
			{
				set_pending_query(Query::Stop);
				m_thread.join();
			}

			// Methods
			void request_execute()
			{
				set_pending_query(Query::Execute);
			}

			void wait_until_done()
			{
				std::unique_lock<std::mutex> t_lock{ m_mutex };
				m_sync.wait(t_lock, [this]
					{
						return m_pendingQuery == Query::None;
					});
			}

			// Operators
			basic_thread_worker& operator=(basic_thread_worker&&) = delete;
			basic_thread_worker& operator=(basic_thread_worker const&) = delete;

		private:
			// Types
			enum class Query
			{
				None,
				Execute,
				Stop
			};

			// Attributes
			Query m_pendingQuery = Query::None;
			std::mutex m_mutex;
			std::condition_variable m_sync;
			std::thread m_thread;

			task_span const& m_tasks;
			basic_task_state_list<TTaskStateAllocator>& m_taskStates;
			basic_thread_schedule<TScheduleAllocator> m_schedule;

			// Methods
			void start()
			{
				auto query = Query::None;
				while (query != Query::Stop)
				{
					query = wait_for_query();
					if (query == Query::Execute)
					{
						detail::execute_thread_schedule(m_schedule, m_tasks, m_taskStates);
						set_pending_query(Query::None);
					}
				}
			}

			void set_pending_query(Query a_query)
			{
				{
					std::lock_guard<std::mutex> t_lock{ m_mutex };
					m_pendingQuery = a_query;
				}
				m_sync.notify_all();
			}

			Query wait_for_query()
			{
				std::unique_lock<std::mutex> t_lock{ m_mutex };
				m_sync.wait(t_lock, [this]
					{
						return m_pendingQuery != Query::None;
					});
				return m_pendingQuery;
			}
		};
	}

	template <typename TScheduleAllocator, typename TAllocator>
	class basic_worker
	{
		// TYpes
		using task_state_allocator =
			typename std::allocator_traits<TAllocator>::template rebind_alloc<std::shared_ptr<detail::task_state>>;
		using task_state_list = detail::basic_task_state_list<task_state_allocator>;
		using thread_worker = detail::basic_thread_worker<TScheduleAllocator, task_state_allocator>;
		using thread_worker_allocator =
			typename std::allocator_traits<TAllocator>::template rebind_alloc<std::shared_ptr<thread_worker>>;
		using thread_worker_list = std::vector<std::shared_ptr<thread_worker>, thread_worker_allocator>;

	public:
		// Types
		using schedule = basic_schedule<TScheduleAllocator>;
		using thread_schedule = basic_thread_schedule<TScheduleAllocator>;

		// Constructors
		basic_worker(basic_worker&&) = default;

		basic_worker(basic_worker const&) = delete;

		basic_worker(task_span const& a_tasks, schedule a_schedule)
			: m_tasks{ a_tasks }
			, m_mainThreadSchedule{ std::move(a_schedule.front()) }
		{
			assert(!a_schedule.empty());

			m_taskStates.resize(a_tasks.size());
			for (auto& taskState : m_taskStates)
			{
				taskState = std::allocate_shared<detail::task_state>(m_taskStates.get_allocator());
			}

			m_threadWorkers.reserve(a_schedule.size() - 1);
			auto t_it = a_schedule.begin();
			while (++t_it != a_schedule.end())
			{
				m_threadWorkers.emplace_back(std::allocate_shared<thread_worker>(
					m_threadWorkers.get_allocator(), a_tasks, m_taskStates, *t_it));
			}
		}

		~basic_worker() = default;

		// Methods
		void execute()
		{
			reset_task_states();
			for (auto& threadWorker : m_threadWorkers)
			{
				threadWorker->request_execute();
			}
			execute_main_thread_schedule();
			for (auto& threadWorker : m_threadWorkers)
			{
				threadWorker->wait_until_done();
			}
		}

		// Operators
		basic_worker& operator=(basic_worker&&) = default;

		basic_worker& operator=(basic_worker const&) = delete;

	private:
		// Attributes

		thread_worker_list m_threadWorkers;
		task_span const& m_tasks;
		
		task_state_list m_taskStates;
		thread_schedule m_mainThreadSchedule;

		// Methods
		void reset_task_states()
		{
			for (auto& taskState : m_taskStates)
			{
				taskState->set_is_done(false);
			}
		}

		void execute_main_thread_schedule()
		{
			detail::execute_thread_schedule(m_mainThreadSchedule, m_tasks, m_taskStates);
		}
	};

	using worker = basic_worker<std::allocator<task_id>, std::allocator<void>>;

	namespace pmr
	{
		using task_description = basic_task_description<std::pmr::polymorphic_allocator<task_id>>;

		using thread_schedule = basic_thread_schedule<std::pmr::polymorphic_allocator<task_id>>;

		using schedule = basic_schedule<std::pmr::polymorphic_allocator<task_id>>;

		using worker = basic_worker<
			std::pmr::polymorphic_allocator<task_id>,
			std::pmr::polymorphic_allocator<void>
		>;
	}
}

