#include "../../include/wwidget/widget/Knob.hpp"
#include "../../include/wwidget/Canvas.hpp"

#include <cmath>

static constexpr inline
float knob_min_angle = M_PI * .2f;
static constexpr inline
float knob_angle_range = M_PI * 2.f - 2 * knob_min_angle;
static constexpr inline
float knob_inner_radius_frac = .9f;

namespace wwidget {

Knob::Knob() {
	align(AlignCenter);
}
Knob::~Knob() {}

void Knob::onCalcPreferredSize(PreferredSize& size) {
	float upscale = 1.f / (knob_inner_radius_frac - .1f);

	Widget::onCalcPreferredSize(size);
	size.prefx = size.prefy = std::max(std::max(size.prefx, size.prefy), 20.f) * upscale;
	size.minx = size.miny   = std::max(std::max(size.minx, size.miny), 20.f) * upscale;
	size.maxx = size.maxy = std::min(std::min(size.minx, size.maxy), 20.f);
}

void Knob::onDrawBackground(Canvas& canvas) {}
void Knob::onDraw(Canvas& canvas) {
	constexpr
	size_t num_points = 32;
	Point  points[num_points];

	float centerx = width() * .5f;
	float centery = height() * .5f;
	float outer_radius = std::min(centerx, centery);
	float inner_radius = outer_radius * knob_inner_radius_frac;
	float max_angle = M_PI * 2 * fraction();

	for(unsigned i = 0; i < num_points - 1; i++) {
		float fraction = float(i) / (num_points - 2);
		float angle = M_PI * 2 * fraction;
		points[i] = {
			centerx + outer_radius * cosf(angle),
			centery + outer_radius * sinf(angle)
		};
	}

	canvas.linestrip(
		num_points - 1, points,
		focused() ? rgb(205, 171, 95) : rgb(163, 153, 131)
	);

	for(unsigned i = 0; i < num_points - 1; i++) {
		float fraction = float(i) / (num_points - 2);
		float angle = M_PI_2 + max_angle * fraction;
		points[i] = {
			centerx + inner_radius * cosf(angle),
			centery + inner_radius * sinf(angle)
		};
	}
	points[num_points - 1] = { centerx, centery };

	canvas.linestrip(num_points, points, rgb(217, 150, 1));
}

bool Knob::onFocus(bool b, float strength) {
	return strength > 0;
}

void Knob::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	fraction(fraction() + (scroll.clicks_y * .1f));
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
			float angle = atan2f(drag.x - width() * .5f, drag.y - height() * .5f);
			fraction(1 - std::fmod(angle / M_PI * .5f + 1.f, 1.f));
		}
		else {
			float amount;
			if(fabs(drag.moved_x) > fabs(drag.moved_y))
				amount = drag.moved_x * .005f;
			else
				amount = drag.moved_y * -.0005f;
			fraction(fraction() + amount);
		}
		drag.handled = true;
	}
}

} // namespace wwidget
