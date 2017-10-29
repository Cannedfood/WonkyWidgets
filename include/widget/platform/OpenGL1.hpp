#include "../Canvas.hpp"

#include <GL/gl.h>

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

	virtual void fillRect   (float x, float y, float w, float h, uint32_t color) override {
		glColorU32(color);
		glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
	virtual void outlineRect(float x, float y, float w, float h, uint32_t color) override {
		glColorU32(color);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}
};

} // namespace widget
