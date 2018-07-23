// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#pragma once

#include <bitset>
#include <unordered_map>

#include "Enums.hpp"

namespace music {

enum ChordFormat {
	ABSOLUTE,
	RELATIVE,
	NOTES
};

class Chord {
public:
	std::bitset<12> notes;

	template<class FwdIter> constexpr
	Chord(ChordFormat format, FwdIter begin, FwdIter end) noexcept {
		using iterator = std::remove_cv_t<std::remove_reference_t<FwdIter>>;

		notes[0] = true;
		switch(format) {
			case NOTES: {
				unsigned offset = ~0u;
				for(iterator iter = begin; iter != end; ++iter) {
					unsigned v = *iter;
					offset = offset < v ? offset : v;
				}
				for(iterator iter = begin; iter != end; ++iter) {
					notes[((*iter) - offset) % 12] = true;
				}
			} break;
			case ABSOLUTE: {
				for(iterator iter = begin; iter != end; ++iter) {
					notes[(*iter) % 12] = true;
				}
			} break;
			case RELATIVE: {
				uint32_t index = 0;
				for(iterator iter = begin; iter != end; ++iter) {
					index += (*iter);
					notes[index % 12] = true;
				}
			} break;
		}
	}

	constexpr
	Chord(ChordFormat format, std::initializer_list<unsigned> intervals) noexcept :
		Chord(format, std::begin(intervals), std::end(intervals))
	{}

	template<class Container> constexpr
	Chord(ChordFormat format, Container&& intervals) noexcept :
		Chord(format, std::begin(intervals), std::end(intervals))
	{}

	unsigned apply(unsigned* notes, unsigned base = C) const noexcept;

	std::string to_string() const noexcept;
	std::string to_string(unsigned base) const noexcept;

	bool operator==(Chord const& other) const noexcept { return notes == other.notes; }
};

} // namespace music

namespace std {
template<>
struct hash<music::Chord> {
	size_t operator()(music::Chord const& chord) const noexcept {
		return std::hash<std::bitset<12>>()(chord.notes);
	}
};
} // namespace std

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