#pragma once

#include <bitset>

namespace wwidget {

struct Mouse {
	float x, y;
	std::bitset<5> buttons;
	std::bitset<5> mods;
};

struct Event {
	enum State {
		UP,
		DOWN,
		DOWN_REPEATING
	};

	mutable float x, y; // Changes when propagating down the widgets
	mutable bool handled = false;

	inline
	operator bool() const {
		return handled;
	}
};

struct Click : public Event {
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

struct KeyEvent : public Event {
	int   mods;
	int   key;
	int   scancode;
	State state;
};

struct TextInput : public Event {
	int      mods    = 0;
	uint32_t utf32   = 0;
	char     utf8[8] = "\0\0\0\0\0\0\0";

	void calcUtf8();
	void calcUtf32();
};

} /* widget */
