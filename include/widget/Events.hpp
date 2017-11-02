#pragma once

#include <bitset>

namespace widget {

struct Mouse {
	float x, y;
	std::bitset<3> buttons;
	std::bitset<3> mods;
};

struct Event {
	mutable bool handled = false;

	inline
	operator bool() const {
		return handled;
	}
};

struct Click : public Event {
	enum State {
		UP,
		DOWN,
		DOWN_REPEATING
	};

	mutable float x, y; // Changes when propagating down the widgets
	int   button;
	State state;
};

} /* widget */
