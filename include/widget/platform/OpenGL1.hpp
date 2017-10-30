#include "../Canvas.hpp"

#include <GL/gl.h>

#include <cmath>
#include <algorithm>
#include <vector>

#define LEN(X) (sizeof(X) / sizeof(X[0]))

namespace widget {

class OpenGL1_Canvas : public Canvas {
	inline
	void glColorU32(uint32_t v) {
		glColor4f(
			((v >> 16) & 0xFF) / 255.f,
			((v >> 8)  & 0xFF) / 255.f,
			((v >> 0)  & 0xFF) / 255.f,
			((v >> 24) & 0xFF) / 255.f
		);
	}

	struct Point {
		float x, y;
	};

	std::vector<Point> mOffsets;
public:
	OpenGL1_Canvas(float x, float y, float w, float h) {
		glPushMatrix();
		glTranslatef(-1, 1, 0);
		glScalef(2 / w, -2 / h, 1);

		mOffsets = {Point{x - .5f, y - .5f}};

		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}

	~OpenGL1_Canvas() {
		glDisable(GL_BLEND);
		glPopMatrix();
	}

	void pushArea(float x, float y, float w, float h) override {
		mOffsets.push_back({mOffsets.back().x + x, mOffsets.back().y + y});
	}
	void popArea() override {
		mOffsets.pop_back();
	}

	void fillRect   (float x, float y, float w, float h, uint32_t color) override {
		x += mOffsets.back().x;
		y += mOffsets.back().y;
		glColorU32(color);
		glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
	void outlineRect(float x, float y, float w, float h, uint32_t color) override {
		x += mOffsets.back().x;
		y += mOffsets.back().y;
		glColorU32(color);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
	void fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color) override {
		x += mOffsets.back().x;
		y += mOffsets.back().y;
		{
			float maximum_radius = std::min(w, h) * .5f;
			if(radius > maximum_radius) {
				radius = maximum_radius;
			}
		}

		glColorU32(color);
		float maxx = x + w;
		float maxy = y + h;
		glBegin(GL_QUADS);
			// Middle part
			glVertex2f(   x,    y + radius); glVertex2f(maxx,    y + radius);
			glVertex2f(maxx, maxy - radius); glVertex2f(   x, maxy - radius);

			// Upper part
			glVertex2f(   x + radius,          y); glVertex2f(maxx - radius,          y);
			glVertex2f(maxx - radius, y + radius); glVertex2f(   x + radius, y + radius);

			// Lower part
			glVertex2f(   x + radius, maxy - radius); glVertex2f(maxx - radius, maxy - radius);
			glVertex2f(maxx - radius,          maxy); glVertex2f(   x + radius,          maxy);
		glEnd();

		constexpr size_t kSubsteps = 4;
		struct { float x, y; } values[kSubsteps + 1 + 1];
		for(size_t i = 0; i < kSubsteps + 1; i++) {
			auto& v = values[i];
			v.x = cosf(i * ((M_PI / 2.0) / kSubsteps));
			v.y = sinf(i * ((M_PI / 2.0) / kSubsteps));

			v.x = powf(v.x, 1 / degree);
			v.y = powf(v.y, 1 / degree);
			if(std::isnan(v.x)) v.x = 0;
			if(std::isnan(v.y)) v.y = 0;

			v.x = radius - (v.x * radius);
			v.y = radius - (v.y * radius);
		}
		values[LEN(values) - 1] = { radius, radius };

		// Upper left
		glBegin(GL_TRIANGLE_FAN);
			for(auto& v : values)
				glVertex2f(x + v.x, y + v.y);
		glEnd();
		// Upper right
		glBegin(GL_TRIANGLE_FAN);
			for(auto& v : values)
				glVertex2f(maxx - v.x, y + v.y);
		glEnd();
		// Lower left
		glBegin(GL_TRIANGLE_FAN);
			for(auto& v : values)
				glVertex2f(x + v.x, maxy - v.y);
		glEnd();
		// Lower right
		glBegin(GL_TRIANGLE_FAN);
			for(auto& v : values)
				glVertex2f(maxx - v.x, maxy - v.y);
		glEnd();
	}

	void outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color) override {
		x += mOffsets.back().x;
		y += mOffsets.back().y;
		{
			float maximum_radius = std::min(w, h) * .5f;
			if(radius > maximum_radius) {
				radius = maximum_radius;
			}
		}

		glColorU32(color);
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

		glBegin(GL_LINE_LOOP);
			for(size_t i = 0; i < LEN(values); ++i)
				glVertex2f(x + values[i].x, y + values[i].y);
			for(size_t i = LEN(values) - 1; i < LEN(values); --i)
				glVertex2f(maxx - values[i].x, y + values[i].y);
			for(size_t i = 0; i < LEN(values); ++i)
				glVertex2f(maxx - values[i].x, maxy - values[i].y);
			for(size_t i = LEN(values) - 1; i < LEN(values); --i) {
				glVertex2f(x + values[i].x, maxy - values[i].y);
				// printf("%f %f | %f %f | %f %f\n", values[i].x, values[i].y, x, maxy, x + values[i].x, maxy - values[i].y);
			}
		glEnd();
	}
};

} // namespace widget
