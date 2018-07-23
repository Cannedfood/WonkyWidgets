// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#include "Key.hpp"

#include <cstdio>


namespace music {

KeyedNote KeySignature::apply(Note n, Accidental hint) const noexcept {
	KeyedNote result;
	result.accidental = NATURAL;
	result.octave     = n.octave;
	switch(n.value) {
		case Cs: result.accidental = SHARP; [[fallthrough]];
		case C:  result.value = NOTE_C; break;
		case Ds: result.accidental = SHARP; [[fallthrough]];
		case D:  result.value = NOTE_D; break;
		case E:  result.value = NOTE_E; break;
		case Fs: result.accidental = SHARP; [[fallthrough]];
		case F:  result.value = NOTE_F; break;
		case Gs: result.accidental = SHARP; [[fallthrough]];
		case G:  result.value = NOTE_G; break;
		case As: result.accidental = SHARP; [[fallthrough]];
		case A:  result.value = NOTE_A; break;
		case B:  result.value = NOTE_B; break;
	}

	// TODO: actually apply key and hint
	return result;
}
Note KeySignature::apply(KeyedNote n) const noexcept {
	Note result;

	result.octave = n.octave;

	int value = lines[n.value] ? accidental : 0;
	switch(n.value) {
		case NOTE_C: value += C; break;
		case NOTE_D: value += D; break;
		case NOTE_E: value += E; break;
		case NOTE_F: value += F; break;
		case NOTE_G: value += G; break;
		case NOTE_A: value += A; break;
		case NOTE_B: value += B; break;
	}

	if(value < 0) {
		value += 12;
		result.octave--;
	}
	else if(value >= 12) {
		value -= 12;
		result.octave++;
	}

	result.value = (NoteValue)value;

	return result;
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