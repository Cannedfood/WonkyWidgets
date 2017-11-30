#pragma once

#include <bitset>

namespace widget {

struct Mouse {
	float x, y;
	std::bitset<5> buttons;
	std::bitset<5> mods;
};

struct Event {
	mutable float x, y; // Changes when propagating down the widgets
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

	int   button;
	State state;

	bool down() const noexcept { return state != UP; }
	bool up() const noexcept { return state == UP; }
	bool repeat() const noexcept { return state == DOWN_REPEATING; }
};

struct Scroll : public Event {
	float pixels_x, pixels_y;
	float clicks_x, clicks_y;
};

struct Moved : public Event {
	float old_x;
	float old_y;
	float moved_x;
	float moved_y;
	std::bitset<5> buttons;
};

struct Dragged : public Moved {};

} /* widget */
