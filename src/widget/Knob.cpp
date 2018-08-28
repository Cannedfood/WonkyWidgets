#include "../../include/wwidget/widget/Knob.hpp"
#include "../../include/wwidget/Canvas.hpp"

#include <cmath>

/// Constants
static constexpr inline
float knob_angle_center = -M_PI * .5f;
static constexpr inline
float knob_angle_stride = M_PI * .9f;
static constexpr inline
float knob_inner_radius_frac = .9f;

/// Calculated constants
static constexpr inline
float knob_angle_min   = knob_angle_center - knob_angle_stride;
static constexpr inline
float knob_angle_range = knob_angle_stride * 2.f;

namespace wwidget {

Knob::Knob() {
	align(AlignCenter);
}
Knob::~Knob() {}

PreferredSize Knob::onCalcPreferredSize(PreferredSize const& constraint) {
	PreferredSize result = Widget::onCalcPreferredSize(constraint);

	float upscale = 1.f / (knob_inner_radius_frac - .1f);

	result.pref.x = result.pref.y = std::max(std::max(result.pref.x, result.pref.y) * upscale, 20.f);
	result.min.x  = result.min.y  = std::max(std::max(result.min.x, result.min.y)   * upscale, 20.f);
	result.max.x  = result.max.y  = std::numeric_limits<float>::infinity();
	return result;
}

void Knob::onDrawBackground(Canvas& canvas) {}
void Knob::onDraw(Canvas& canvas) {
	Point center       = { width() * .5f, height() * .5f };
	float outer_radius = std::min(center.x, center.y);
	float inner_radius = outer_radius * knob_inner_radius_frac;

	canvas
		.strokeColor(focused() ? rgb(205, 171, 95) : rgb(163, 153, 131))
		.arc(center, outer_radius, knob_angle_min, knob_angle_min + knob_angle_range)
		.stroke();

	canvas
		.strokeColor(rgb(217, 150, 1))
		.arc(center, inner_radius, knob_angle_min, knob_angle_min + knob_angle_range * fraction())
		.lineTo(center)
		.stroke();

	/*
	constexpr
	size_t num_points = 32;
	Point  points[num_points];

	for(unsigned i = 0; i < num_points - 1; i++) {
		float fraction = float(i) / (num_points - 2);
		float angle = knob_angle_min + knob_angle_range * fraction;
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
		float fraction = float(i) / (num_points - 2) * this_fraction;
		float angle = knob_angle_min + knob_angle_range * fraction;
		points[i] = {
			centerx + inner_radius * cosf(angle),
			centery + inner_radius * sinf(angle)
		};
	}
	points[num_points - 1] = { centerx, centery };

	canvas.linestrip(num_points, points, rgb(217, 150, 1));
	*/
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
			double angle = atan2f(drag.position.x - width() * .5f, drag.position.y - height() * .5f) + M_PI * .5f;
			double frac = std::fmod(angle - knob_angle_min, M_PI * 2) / knob_angle_range;
			fraction(1 - frac);
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
