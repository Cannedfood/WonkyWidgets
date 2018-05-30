#pragma once

#include <functional>

#include <deque>
#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace wwidget {

class Threadpool {
	volatile bool            mRunning;
	std::mutex               mMutex;
	std::condition_variable  mWaiting;
	std::vector<std::thread> mThreads;
	std::deque<std::function<void()>> mTasks;

public:
	Threadpool();
	Threadpool(size_t size);
	~Threadpool();

	void start(size_t size);
	void stop();

	void add(std::function<void()>&& fn);
	std::function<void()> await_pop();
	std::function<void()> try_pop();

	bool running() const noexcept { return mRunning; }
};

} // namespace wwidget