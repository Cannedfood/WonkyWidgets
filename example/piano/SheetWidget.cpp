#include "SheetWidget.hpp"

using namespace wwidget;

void SheetWidget::onDraw(Canvas& canvas) {
	drawGrandStaff(canvas);

	// {
	// 	ChromaticNote notes[] = {
	// 		// C, D, E, F, G, A, B
	// 		{B, 3}, C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, {C, 5}
	// 	};

	// 	float position = lineHeight();
	// 	for(auto& note : notes) {
	// 		position = drawNote(canvas, position, note);
	// 	}
	// }
}

float SheetWidget::drawNote(Canvas& c, float x, SheetNote note) {
	// auto staffNote = note.toStaffNote();
	KeyedNote keyedNote = keySignature().apply(note);

	float y = lineHeight(keyedNote.grandStaffLine());

	float l = lineHeight();
	Size radii = {
		lineHeight() * .75f,
		lineHeight() * .5f
	};

	c.fillColor(note.color);

	if(keyedNote.accidental != NATURAL) {
		const char* text = keyedNote.accidental == FLAT ? "b" : "#";
		c.fontSize(lineHeight() * 2);

		Rect bounds = c.textBounds({}, text);
		c.text({x - bounds.width(), y + lineHeight() * .5f}, text);
	}
	x += radii.x;

	c.elipse({x, y}, radii.x, radii.y)
	 .fill();

	if(note.textColor.a) {
		const char* text     = to_string(note.value);
		Point       position = {x, y};

		c.font("mono")
		 .fontSize(lineHeight() * .75f);

		position -= c.textBounds({x, y}, text).center() - position;

		c.fillColor(Color::white())
		 .text(position, text);
	}

	return x + l * 2;
}

void SheetWidget::drawGrandStaff(Canvas& c) {
	Note lines[] = {
		// Treble clef
		{E, 4}, {G, 4}, {B, 4}, {D, 5}, {F, 5},
		// Bass clef
		{A, 3}, {F, 3}, {D, 3}, {B, 2}, {G, 2}
	};

	c.strokeColor(Color::black());
	for(auto& note : lines) {
		auto staffNote = KeySignature().apply(note);
		float y = lineHeight(staffNote.grandStaffLine());
		c.moveTo({0,       y}).lineTo({width(), y});
	}
	c.stroke();
}

void  SheetWidget::drawKey(Canvas& canvas, KeySignature const& key) {
	// TODO
}