#pragma once

#include <wwidget/Canvas.hpp>
#include <wwidget/Widget.hpp>

#include "musicpp/musicpp.hpp"

using wwidget::Color;
using wwidget::Widget;
using wwidget::Canvas;
using namespace music;

struct SheetNote : public Note {
	Color color, textColor;

	SheetNote(Note note, Color color = Color::black(), Color textColor = Color(0, 0, 0, 0)) :
		Note(note),
		color(color)
	{}
};

class SheetWidget : public Widget {
public:
	void onDraw(Canvas& canvas) override;

private:
	KeySignature           mKey;
	std::vector<SheetNote> mNotes;

protected:
	KeySignature const& keySignature() const noexcept { return mKey; }

	float drawNote(Canvas& canvas, float x, SheetNote note);
	void  drawGrandStaff(Canvas& canvas);
	void  drawKey(Canvas& canvas, KeySignature const& key);

	float lineHeight(int line) const noexcept { return (unsigned)((height() * .5f) - lineHeight() * line * .5f) + .5f; }
	float lineHeight() const noexcept { return height() / 24; }
};