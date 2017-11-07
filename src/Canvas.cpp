#include "../include/widget/Canvas.hpp"
#include "../include/widget/Error.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "3rd-party/stb_image.h"

#include <cmath>
#include <vector>

#define LEN(X) (sizeof(X) / sizeof(X[0]))

namespace widget {

// TODO: implement caching. See Canvas::loadTextureNow and Canvas::Cache

class Canvas::Cache {
public:

};

Canvas::Canvas() {
	mCache = new Cache;
}
Canvas::~Canvas() {
	delete mCache;
}

std::shared_ptr<Bitmap> Canvas::loadTextureNow(std::string const& path) {
	int x, y, c;
	auto data = std::unique_ptr<uint8_t, void(*)(void*)>(
		stbi_load(path.c_str(), &x, &y, &c, 0),
		stbi_image_free
	);

	if(!data) {
		throw exceptions::FailedLoadingFile(path, stbi_failure_reason());
	}

	return loadTextureNow(data.get(), x, y, c);
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
void Canvas::fillRect(float x, float y, float w, float h, Bitmap* bitmap, uint32_t tint, float srcx, float srcy, float srcw, float srch) {
	if(!bitmap) return;
	float points[] = {
		x, y,
		x + w, y,
		x + w, y + h,
		x, y + h
	};
	float texcoords[] = {
		srcx, srcy,
		srcx + srcw, srcy,
		srcx + srcw, srcy + srch,
		srcx, srcy + srch
	};
	fillPoly(points, texcoords, 4, bitmap, tint);
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

template<size_t kSubsteps = 4>
void getRRectCoords(float (&points)[(kSubsteps + 1) * 2 * 4], float radius, float degree, float x, float y, float w, float h) {
	{
		float maximum_radius = std::min(w, h) * .5f;
		if(radius > maximum_radius) {
			radius = maximum_radius;
		}
	}

	float maxx = x + w;
	float maxy = y + h;
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
}

void Canvas::fillRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color) {
	float points[(4 + 1) * 2 * 4];
	getRRectCoords(points, radius, degree, x, y, w, h);
	fillPoly(points, LEN(points) / 2, color);
}

void Canvas::outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color) {
	float points[(4 + 1) * 2 * 4];
	getRRectCoords(points, radius, degree, x, y, w, h);
	outlinePoly(points, LEN(points) / 2, color);
}
void Canvas::fillRRect(
	float radius, float degree,
	float x, float y, float w, float h,
	Bitmap* bitmap, uint32_t tint,
	float srcx, float srcy, float srcw, float srch)
{
	if(!bitmap) return;
	float points   [(4 + 1) * 2 * 4];
	float texcoords[LEN(points)];
	getRRectCoords(points, radius, degree, x, y, w, h);
	for(size_t i = 0; i < sizeof(points); i += 2) {
		texcoords[i + 0] = ((points[i + 0] - x) / w) * srcw + srcx;
		texcoords[i + 1] = ((points[i + 0] - y) / h) * srch + srcy;
	}
	fillPoly(points, texcoords, LEN(points) / 2, bitmap, tint);
}

} // namespace widget
