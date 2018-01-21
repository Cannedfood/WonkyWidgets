#pragma once

#include <deque>
#include <mutex>
#include <functional>

namespace wwidget {

/// A single consumer multiple provider task-queue.
///  It's still threadsafe for multiple consumers, just not optimized
class TaskQueue {
	std::mutex mMutex;
	std::deque<std::function<void()>> mTasks;
public:
	TaskQueue();
	~TaskQueue();

	void add(std::function<void()> fn);

	size_t executeSingleConsumer();
};

} // namespace wwidget
