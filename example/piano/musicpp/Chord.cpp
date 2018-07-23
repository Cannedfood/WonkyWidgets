#include "Chord.hpp"

#include "Enums.hpp"

#include <algorithm>

namespace music {

unsigned Chord::apply(unsigned* values, unsigned base) const noexcept {
	unsigned count = 0;
	for(unsigned i = 0; i < this->notes.size(); ++i) {
		if(notes[i]) {
			values[count] = base + i;
			count++;
		}
	}
	return count;
}

std::string Chord::to_string() const noexcept {
	std::string result;
	for(unsigned i = 1; i < this->notes.size(); ++i) {
		if(notes[i]) {
			result += music::to_string((Interval)i);
			result += ", ";
		}
	}

	if(result.size() > 2)
		result.resize(result.size() - 2);

	return result;
}

std::string Chord::to_string(unsigned base) const noexcept {
	std::string result;
	unsigned notes[12];
	unsigned const count = apply(notes, base);

	for(unsigned i = 0; i < count; i++) {
		result += music::to_string((NoteValue) (i % 12));
		result += " ";
	}

	if(result.size() > 1)
		result.pop_back();

	return result;
}

} // namespace music