#pragma once

#include <functional>
#include <deque>
#include <condition_variable>
#include <mutex>

#include <xhash>
#include <xmemory>

#include <xwip/threadsafe_queue>

namespace widget {

class UserInterfaceGraphics {
public:
	virtual void startDrawing   () = 0;
	virtual void finishedDrawing() = 0;
};

class Widget;

class UserInterface {
public:
	UserInterface();
	virtual ~UserInterface();
};

} // namespace widget
