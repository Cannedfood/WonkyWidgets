#include "../include/widget/Canvas.hpp"

#include <cmath>
#include <vector>

#define LEN(X) (sizeof(X) / sizeof(X[0]))

namespace widget {

class Canvas::Cache {
public:

};

Canvas::Canvas() {}
Canvas::~Canvas() {}

std::shared_ptr<Bitmap> Canvas::loadTextureNow(std::string const& path) {
	return nullptr;
}

void Canvas::fillRect   (float x, float y, float w, float h, uint32_t color) {
	float points[] = {
		x, y,
		x + w, y,
		x + w, y + h,
		x, y + h
	};
	fillPoly(points, 4, color);
}
void Canvas::outlineRect(float x, float y, float w, float h, uint32_t color) {
	float points[] = {
		x, y,
		x + w, y,
		x + w, y + h,
		x, y + h
	};
	outlinePoly(points, 4, color);
}

void Canvas::fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color) {
	{
		float maximum_radius = std::min(w, h) * .5f;
		if(radius > maximum_radius) {
			radius = maximum_radius;
		}
	}

	float maxx = x + w;
	float maxy = y + h;
	constexpr size_t kSubsteps = 4;
	struct { float x, y; } values[kSubsteps + 1];
	for(size_t i = 0; i < LEN(values); i++) {
		auto& v = values[i];
		v.x = cosf(i * ((M_PI / 2.0) / kSubsteps));
		v.y = sinf(i * ((M_PI / 2.0) / kSubsteps));

		v.x = powf(v.x, 1 / degree);
		v.y = powf(v.y, 1 / degree);
		if(std::isnan(v.x)) v.x = 0;
		if(std::isnan(v.y)) v.y = 0;

		v.x = radius - (v.x * radius);
		v.y = radius - (v.y * radius);

		// printf("%zu %f/%f %f %f\n", i, i / (float)kSubsteps, i * ((M_PI / 2.0) / kSubsteps), v.x, v.y);
	}

	constexpr size_t len = LEN(values) * 2;
	float points[len * 4];
	size_t idx = 0;
	for(size_t i = 0; i < LEN(values); ++i) {
		points[idx++] = x + values[i].x;
		points[idx++] = y + values[i].y;
	}
	for(size_t i = LEN(values) - 1; i < LEN(values); --i) {
		points[idx++] = maxx - values[i].x;
		points[idx++] = y + values[i].y;
	}
	for(size_t i = 0; i < LEN(values); ++i) {
		points[idx++] = maxx - values[i].x;
		points[idx++] = maxy - values[i].y;
	}
	for(size_t i = LEN(values) - 1; i < LEN(values); --i) {
		points[idx++] = x + values[i].x;
		points[idx++] = maxy - values[i].y;
	}
	fillPoly(points, LEN(points) / 2, color);
}

void Canvas::outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color) {
	{
		float maximum_radius = std::min(w, h) * .5f;
		if(radius > maximum_radius) {
			radius = maximum_radius;
		}
	}

	float maxx = x + w;
	float maxy = y + h;
	constexpr size_t kSubsteps = 4;
	struct { float x, y; } values[kSubsteps + 1];
	for(size_t i = 0; i < LEN(values); i++) {
		auto& v = values[i];
		v.x = cosf(i * ((M_PI / 2.0) / kSubsteps));
		v.y = sinf(i * ((M_PI / 2.0) / kSubsteps));

		v.x = powf(v.x, 1 / degree);
		v.y = powf(v.y, 1 / degree);
		if(std::isnan(v.x)) v.x = 0;
		if(std::isnan(v.y)) v.y = 0;

		v.x = radius - (v.x * radius);
		v.y = radius - (v.y * radius);

		// printf("%zu %f/%f %f %f\n", i, i / (float)kSubsteps, i * ((M_PI / 2.0) / kSubsteps), v.x, v.y);
	}

	constexpr size_t len = LEN(values) * 2;
	float points[len * 4];
	size_t idx = 0;
	for(size_t i = 0; i < LEN(values); ++i) {
		points[idx++] = x + values[i].x;
		points[idx++] = y + values[i].y;
	}
	for(size_t i = LEN(values) - 1; i < LEN(values); --i) {
		points[idx++] = maxx - values[i].x;
		points[idx++] = y + values[i].y;
	}
	for(size_t i = 0; i < LEN(values); ++i) {
		points[idx++] = maxx - values[i].x;
		points[idx++] = maxy - values[i].y;
	}
	for(size_t i = LEN(values) - 1; i < LEN(values); --i) {
		points[idx++] = x + values[i].x;
		points[idx++] = maxy - values[i].y;
	}
	outlinePoly(points, LEN(points) / 2, color);
}

} // namespace widget
