#pragma once

#include <mutex>
#include <vector>


namespace vob::mismt
{
	class basic_task
	{
	public:
		virtual void execute() const = 0;
	};

	using task_list = std::vector<std::unique_ptr<basic_task>>;
}
