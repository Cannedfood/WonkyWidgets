#include "../../include/wwidget/widget/Knob.hpp"
#include "../../include/wwidget/Canvas.hpp"

#include <cmath>

static constexpr inline
float knob_min_angle = M_PI * .2f;
static constexpr inline
float knob_angle_range = M_PI * 2.f - 2 * knob_min_angle;

namespace wwidget {

Knob::Knob() {}
Knob::~Knob() {}

void Knob::onCalcPreferredSize(PreferredSize& size) {
	size.minx = size.prefx = size.maxx =
	size.miny = size.prefy = size.maxx = 50.f;
}

void Knob::onDrawBackground(Canvas& canvas) {

}
void Knob::onDraw(Canvas& canvas) {
	constexpr
	size_t num_points = 32;
	Point  points[num_points];

	float center = std::min(width(), height()) * .5f;
	float radius = center * .9f;
	float max_angle = M_PI * 2 * fraction();

	for(unsigned i = 0; i < num_points - 1; i++) {
		float fraction = float(i) / (num_points - 2);
		float angle = M_PI * 2 * fraction;
		points[i] = {
			center + center * cosf(angle),
			center + center * sinf(angle)
		};
	}
	canvas.linestrip(num_points - 1, points, rgb(163, 153, 131));

	for(unsigned i = 0; i < num_points - 1; i++) {
		float fraction = float(i) / (num_points - 2);
		float angle = M_PI_2 + max_angle * fraction;
		points[i] = {
			center + radius * cosf(angle),
			center + radius * sinf(angle)
		};
	}
	points[num_points - 1] = { center, center };

	canvas.linestrip(num_points, points, rgb(217, 150, 1));
}

bool Knob::onFocus(bool b, float strength) {
	return strength > 0;
}

void Knob::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	fraction(fraction() + (scroll.clicks_y * .1f) / 20);
	scroll.handled = true;
}
void Knob::on(Click const& click) {
	if(click.down()) {
		requestFocus();
		click.handled = true;
		return;
	}

	if(focused()) {
		removeFocus();
		click.handled = true;
		return;
	}
}
void Knob::on(Dragged const& drag) {
	if(focused()) {
		if(drag.buttons[0]) {
			float radius = std::min(width(), height()) * .5f;

			float angle = atan2f(drag.x - radius, drag.y - radius);
			fraction(1 - std::fmod(angle / M_PI * .5f + 1.f, 1.f));
		}
		else {
			float amount;
			if(fabs(drag.moved_x) > fabs(drag.moved_y))
				amount = drag.moved_x * .005f;
			else
				amount = drag.moved_y * .0005f;
			fraction(fraction() + amount);
		}
		drag.handled = true;
	}
}

} // namespace wwidget
