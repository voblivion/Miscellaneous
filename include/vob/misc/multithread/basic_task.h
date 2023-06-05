#pragma once

#include <memory>
#include <span>
#include <vector>


namespace vob::mismt
{
	class basic_task
	{
	public:
		virtual void execute() const = 0;
	};

	using task_span = std::span<std::shared_ptr<basic_task>>;
}
