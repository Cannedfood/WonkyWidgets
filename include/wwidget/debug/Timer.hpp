#pragma once

#include <chrono>

namespace wwidget {

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

class Timer {
	using clock      = std::chrono::high_resolution_clock;
	using time_point = clock::time_point;

	time_point m_begin;

	static inline
	time_point now() noexcept { return clock::now(); }

public:
	Timer() { reset(); }

	void reset() noexcept { m_begin = now(); }

	double days() const noexcept {
		return duration_cast<duration<double, ratio<86400>>>(now() - m_begin).count();
	}
	double hours() const noexcept {
		return duration_cast<duration<double, ratio<3600>>>(now() - m_begin).count();
	}
	double minutes() const noexcept {
		return duration_cast<duration<double, ratio<60>>>(now() - m_begin).count();
	}
	double seconds() const noexcept {
		return duration_cast<duration<double, ratio<1>>>(now() - m_begin).count();
	}
	double millis() const noexcept {
		return duration_cast<duration<double, milli>>(now() - m_begin).count();
	}
	double micros() const noexcept {
		return duration_cast<duration<double, micro>>(now() - m_begin).count();
	}
	double nanos() const noexcept {
		return duration_cast<duration<double, nano>>(now() - m_begin).count();
	}
};

} // namespace stx
