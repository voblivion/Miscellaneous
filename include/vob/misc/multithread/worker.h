#pragma once

#include "basic_task.h"

#include <cassert>
#include <thread>


namespace vob::mismt
{
	struct task_description
	{
		std::size_t m_id;
		std::vector<std::size_t> m_dependencies;
	};

	using thread_schedule = std::vector<task_description>;

	using schedule = std::vector<thread_schedule>;

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

		void execute_thread_schedule(
			thread_schedule const& a_schedule,
			task_list const& a_tasks,
			std::vector<std::unique_ptr<task_state>>& m_taskStates)
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

		class thread_worker
		{
		public:
			// Constructors
			thread_worker() = delete;

			thread_worker(thread_worker&&) = delete;

			thread_worker(thread_worker const&) = delete;

			thread_worker(
				task_list const& a_tasks,
				std::vector<std::unique_ptr<task_state>>& a_taskStates,
				thread_schedule a_schedule)
				: m_tasks{ a_tasks }
				, m_taskStates{ a_taskStates }
				, m_schedule{ std::move(a_schedule) }
			{
				m_thread = std::thread{ &thread_worker::start, this };
			}

			~thread_worker()
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
			thread_worker& operator=(thread_worker&&) = delete;
			thread_worker& operator=(thread_worker const&) = delete;

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

			task_list const& m_tasks;
			std::vector<std::unique_ptr<task_state>>& m_taskStates;
			thread_schedule m_schedule;

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

	class worker
	{
	public:
		// Constructors
		worker(worker&&) = default;

		worker(worker const&) = delete;

		worker(task_list const& a_tasks, schedule a_schedule)
			: m_tasks{ a_tasks }
			, m_mainThreadSchedule{ std::move(a_schedule.front()) }
		{
			assert(!a_schedule.empty());

			m_taskStates.resize(a_tasks.size());
			for (auto& taskState : m_taskStates)
			{
				taskState = std::make_unique<detail::task_state>();
			}

			m_threadWorkers.reserve(a_schedule.size() - 1);
			auto t_it = a_schedule.begin();
			while (++t_it != a_schedule.end())
			{
				m_threadWorkers.emplace_back(std::make_unique<detail::thread_worker>(a_tasks, m_taskStates, *t_it));
			}
		}

		~worker() = default;

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
		worker& operator=(worker&&) = default;

		worker& operator=(worker const&) = delete;

	private:
		// Attributes
		std::vector<std::unique_ptr<detail::thread_worker>> m_threadWorkers;
		task_list const& m_tasks;
		std::vector<std::unique_ptr<detail::task_state>> m_taskStates;
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
}

