#include <wwidget/Window.hpp>
#include <wwidget/Canvas.hpp>

#include "SheetWidget.hpp"
#include "AlsaMidiClient.hpp"


using namespace wwidget;
using namespace music;

#include <set>
#include <numeric>

class Sheet : public SheetWidget, public AlsaMidiClient {
	std::vector<unsigned> mKeyboardNotes;

	void onDraw(Canvas& c) override {
		SheetWidget::onDraw(c);

		if(!mKeyboardNotes.empty()) {
			c.fillColor(Color::black())
			 .font("mono")
			 .fontSize(lineHeight())
			 .text({10, lineHeight(keySignature().apply(Note(mKeyboardNotes.back())).grandStaffLine()) - lineHeight()}, getChordName());
		}

		for(auto& note : mKeyboardNotes) {
			drawNote(c, 10, {Note(note), Color::black(), Color::white()});
		}
	}

	std::string getChordName() {
		static ChordDatabase chordDatabase;

		if(mKeyboardNotes.empty()) return "";
		if(mKeyboardNotes.size() == 1) return to_string(Note(mKeyboardNotes.back()).value);

		// Calculate intervals
		auto& entry = chordDatabase.get(Chord{NOTES, mKeyboardNotes});

		std::string result = to_string(Note(mKeyboardNotes[entry.root]).value) + std::string(entry.shortName);

		return result;
	}

	// MIDI event handling
	void noteOn (unsigned note) override {
		// Add note into set of notes
		auto iter = std::lower_bound(mKeyboardNotes.begin(), mKeyboardNotes.end(), note);
		if(iter == mKeyboardNotes.end() || *iter != note) {
			mKeyboardNotes.insert(iter, note);
		}
	}
	void noteOff(unsigned note) override {
		// Remove note from set of notes
		auto iter = std::lower_bound(mKeyboardNotes.begin(), mKeyboardNotes.end(), note);
		if(iter == mKeyboardNotes.end() || *iter == note) {
			mKeyboardNotes.erase(iter);
		}
	}
};

int main(int argc, char const* argv[]) {
	Window window;
	Sheet  sheet;
	sheet.align(AlignFill);
	window.add(sheet);
	window.open("Piano", 800, 600, Window::FlagAntialias);
	while(window.update()) {
		sheet.pollMidiEvents();
		window.draw();
	}
}
