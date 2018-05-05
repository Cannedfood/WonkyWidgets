#include "../../include/wwidget/async/Queue.hpp"

namespace wwidget {

TaskQueue::TaskQueue() {

}
TaskQueue::~TaskQueue() {

}

void TaskQueue::add(std::function<void()> fn) {
	mMutex.lock();
	mTasks.emplace_back(std::move(fn));
	mMutex.unlock();
}

size_t TaskQueue::executeSingleConsumer() {
	size_t n = 0;
	std::deque<std::function<void()>> tasks;
	while(!mTasks.empty()) {
		mMutex.lock();
		tasks.swap(mTasks);
		mMutex.unlock();
		n += tasks.size();
		for(auto& task : tasks) task();
		tasks.clear();
	}
	return n;
}

} // namespace wwidget
