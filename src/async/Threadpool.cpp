#include "../../include/wwidget/async/Threadpool.hpp"

namespace wwidget {

Threadpool::Threadpool() :
	mRunning(false)
{}
Threadpool::Threadpool(size_t size) :
	Threadpool()
{
	start(size);
}
Threadpool::~Threadpool() {
	stop();
}

void Threadpool::start(size_t size) {
	stop();

	mRunning = true;
	mThreads.reserve(size);
	while(mThreads.size() < size) {
		mThreads.emplace_back([this]() {
			while(auto task = await_pop()) {
				task();
			}
		});
	}
}
void Threadpool::stop() {
	mRunning = false;
	mWaiting.notify_all();
	for(auto& thread : mThreads)
		thread.join();
	mThreads.clear();
}
void Threadpool::add(std::function<void()>&& fn) {
	auto l = std::lock_guard<std::mutex>(mMutex);
	mTasks.emplace_back(fn);
	mWaiting.notify_one();
}

std::function<void()> Threadpool::await_pop() {
	auto l = std::unique_lock<std::mutex>(mMutex);

	std::function<void()> result;
	if(mTasks.empty()) {
		mWaiting.wait(l, [this]() {
			return !running() || !mTasks.empty();
		});

		if(!running()) return nullptr;
	}

	result = mTasks.front();
	mTasks.pop_front();

	return result;
}
std::function<void()> Threadpool::try_pop() {
	auto l = std::unique_lock<std::mutex>(mMutex);
	std::function<void()> result;
	if(!mTasks.empty()) {
		result = mTasks.front();
		mTasks.pop_front();
	}
	return result;
}

} // namespace wwidget