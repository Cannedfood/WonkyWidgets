#include "../Canvas.hpp"

#include <GL/gl.h>

#include <cmath>

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
public:
	OpenGL1_Canvas(float x, float y, float w, float h) {
		glPushMatrix();
		// glTranslatef(-x, -y, 0);
		// glScalef(1 / w, -1 / h, 1);
		// glTranslatef(0, 1, 0);
		glTranslatef(-1, 1, 0);
		glScalef(2 / w, -2 / h, 1);
		glTranslatef(-x, -y, 0);
		glEnable(GL_BLEND);
	}

	~OpenGL1_Canvas() {
		glDisable(GL_BLEND);
		glPopMatrix();
	}

	void fillRect   (float x, float y, float w, float h, uint32_t color) override {
		glColorU32(color);
		glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
	void outlineRect(float x, float y, float w, float h, uint32_t color) override {
		glColorU32(color);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
	void fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color) override {
		glColorU32(color);
		float maxx = x + w;
		float maxy = y + h;
		glBegin(GL_QUADS);
			// Middle part
			glVertex2f(   x,    y + radius);
			glVertex2f(maxx,    y + radius);
			glVertex2f(maxx, maxy - radius);
			glVertex2f(   x, maxy - radius);

			// Upper part
			glVertex2f(   x + radius, y);
			glVertex2f(maxx - radius, y);
			glVertex2f(maxx - radius, y + radius);
			glVertex2f(   x + radius, y + radius);

			// Lower part
			glVertex2f(   x + radius, maxy - radius);
			glVertex2f(maxx - radius, maxy - radius);
			glVertex2f(maxx - radius, maxy);
			glVertex2f(   x + radius, maxy);
		glEnd();

		constexpr size_t kSubsteps = 4;
		struct { float x, y; } values[1 + kSubsteps + 1];
		values[0] = { radius, radius };
		for(size_t i = 0; i < kSubsteps + 1; i++) {
			auto& v = values[i + 1];
			v.x = cosf(i * ((M_PI / 2.0) / kSubsteps));
			v.y = sinf(i * ((M_PI / 2.0) / kSubsteps));

			v.x = powf(v.x, 1 / degree);
			v.y = powf(v.y, 1 / degree);
			if(std::isnan(v.x)) v.x = 0;
			if(std::isnan(v.y)) v.y = 0;

			v.x = radius - (v.x * radius);
			v.y = radius - (v.y * radius);
		}

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
		glColorU32(color);
		float maxx = x + w;
		float maxy = y + h;
		constexpr size_t kSubsteps = 4;
		struct { float x, y; } values[kSubsteps + 1];
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

		glBegin(GL_LINE_LOOP);
			for(size_t i = 0; i < kSubsteps; i++)
				glVertex2f(x + values[i].x, y + values[i].y);
			for(size_t i = kSubsteps - 1; i < kSubsteps; i--)
				glVertex2f(maxx - values[i].x, y + values[i].y);
			for(size_t i = 0; i < kSubsteps; i++)
				glVertex2f(maxx - values[i].x, maxy - values[i].y);
			for(size_t i = kSubsteps - 1; i < kSubsteps; i--)
				glVertex2f(x + values[i].x, maxy - values[i].y);
		glEnd();
	}
};

} // namespace widget
