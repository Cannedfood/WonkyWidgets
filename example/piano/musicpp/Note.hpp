// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#pragma once

#include "Enums.hpp"

#include <cmath>

namespace music {

struct Note {
	NoteValue value;
	int       octave;

	constexpr inline
	Note(NoteValue val = C, int octave = 4) noexcept :
		value(val),
		octave(octave)
	{}

	constexpr explicit inline Note(int midi) noexcept;
	constexpr explicit inline operator int() const noexcept;
};

// ** Frequency *******************************************************
inline double frequency(Note n, double A_freq = 440) noexcept;
// ** Mathematical operators *******************************************************
constexpr inline Note operator+(Note n, int interval) noexcept;
constexpr inline Note operator-(Note n, int interval) noexcept;
constexpr inline int operator-(Note a, Note b) noexcept;





// =============================================================
// == Inline implementation =============================================
// =============================================================

// ** Midi conversion *******************************************************

constexpr inline Note::operator int() const noexcept {
	return (octave + 1) * 12 + value;
}
constexpr inline Note::Note(int i) noexcept :
	Note((NoteValue) (i % 12), (int)(i / 12u) - 1 )
{}

// ** Frequency *******************************************************

inline double frequency(Note n, double A_freq) noexcept {
	return pow(2, (n - Note{A, 3}) / 12.0 ) * A_freq;
}

// ** Mathematical operators *******************************************************

constexpr inline
Note operator+(Note n, int interval) noexcept {
	return Note(int(n) - interval);
}

constexpr inline
Note operator-(Note n, int interval) noexcept {
	return Note(int(n) - interval);
}

constexpr inline
int operator-(Note a, Note b) noexcept {
	return int(a) - int(b);
}

} // namespace music


// The MIT License (MIT)

// Copyright (c) 2018 Benno Straub

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.