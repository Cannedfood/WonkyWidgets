#pragma once

#include "OpenGL1_Bitmap.hpp"

#include "../Canvas.hpp"

#include <GL/gl.h>

#include <cmath>
#include <algorithm>
#include <vector>

#include "../Error.hpp"

namespace widget {

class OpenGL1_Canvas : public Canvas {
protected:
	struct Point {
		float x, y;
	};

	std::vector<Point> mOffsets;
	void pushUiMatrix(float x, float y, float w, float h);
	void popUiMatrix();

	OpenGL1_Canvas(float x, float y);

public:
	OpenGL1_Canvas(float x, float y, float w, float h);
	~OpenGL1_Canvas();

	std::shared_ptr<Bitmap> loadTextureNow(uint8_t* data, unsigned w, unsigned h, unsigned components) override;

	void pushArea(float x, float y, float w, float h) override;
	void popArea() override;

	void outlinePoly(float* points, size_t number, uint32_t color) override;
	void fillPoly(float* points, size_t number, uint32_t color) override;
	void fillPoly(float* points, float* texcoords, size_t number, Bitmap* bitmap, uint32_t tint) override;
	void fillRect   (float x, float y, float w, float h, uint32_t color) override;
	void outlineRect(float x, float y, float w, float h, uint32_t color) override;
};

} // namespace widget

// =============================================================
// == Implementation =============================================
// =============================================================

#ifdef WIDGET_OPENGL1_IMPLEMENTATION

namespace widget {

namespace {

inline
void glColorU32(uint32_t v) {
	glColor4f(
		((v >> 16) & 0xFF) / 255.f,
		((v >> 8)  & 0xFF) / 255.f,
		((v >> 0)  & 0xFF) / 255.f,
		((v >> 24) & 0xFF) / 255.f
	);
}

} // namespace

void OpenGL1_Canvas::pushUiMatrix(float x, float y, float w, float h) {
	glPushMatrix();
	glTranslatef(-1, 1, 0);
	glScalef(2 / w, -2 / h, 1);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void OpenGL1_Canvas::popUiMatrix() {
	glPopMatrix();
}
OpenGL1_Canvas::OpenGL1_Canvas(float x, float y) {
	mOffsets = {Point{x - .5f, y - .5f}};
}
OpenGL1_Canvas::OpenGL1_Canvas(float x, float y, float w, float h) :
	OpenGL1_Canvas(x, y)
{
	pushUiMatrix(x, y, w, h);
	glEnable(GL_BLEND);
}

OpenGL1_Canvas::~OpenGL1_Canvas() {
	glDisable(GL_BLEND);
	popUiMatrix();
}

std::shared_ptr<Bitmap> OpenGL1_Canvas::loadTextureNow(uint8_t* data, unsigned w, unsigned h, unsigned components) {
	return std::make_shared<OpenGL1_Bitmap>(data, w, h, components);
}

void OpenGL1_Canvas::pushArea(float x, float y, float w, float h) {
	mOffsets.push_back({mOffsets.back().x + x, mOffsets.back().y + y});
}
void OpenGL1_Canvas::popArea() {
	mOffsets.pop_back();
}

void OpenGL1_Canvas::outlinePoly(float* points, size_t number, uint32_t color) {
	glColorU32(color);
	glBegin(GL_LINE_LOOP);
		for(size_t i = 0; i < number; i++) {
			glVertex2f(points[i * 2 + 0] + mOffsets.back().x, points[i * 2 + 1] + mOffsets.back().y);
		}
	glEnd();
}
void OpenGL1_Canvas::fillPoly(float* points, size_t number, uint32_t color) {
	glColorU32(color);
	glBegin(GL_TRIANGLE_FAN);
		for(size_t i = 0; i < number; i++) {
			glVertex2f(points[i * 2 + 0] + mOffsets.back().x, points[i * 2 + 1] + mOffsets.back().y);
		}
	glEnd();
}
void OpenGL1_Canvas::fillPoly(float* points, float* texcoords, size_t number, Bitmap* bitmap, uint32_t tint) {
	if(!bitmap) return;
	OpenGL1_Bitmap* bm = dynamic_cast<OpenGL1_Bitmap*>(bitmap);
	if(!bm)
		throw exceptions::InvalidPointer("Bitmap supplied to OpenGL1_Canvas is not a OpenGL1_Bitmap");
	glColorU32(tint);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bm->handle());

	float width_inv  = 1 / bm->width();
	float height_inv = 1 / bm->height();
	puts("Draw image:");
	glBegin(GL_TRIANGLE_FAN);
		for (size_t i = 0; i < number; i++) {
			printf("%f %f | tex: %f %f\n",
				points[i * 2 + 0] + mOffsets.back().x,
				points[i * 2 + 1] + mOffsets.back().y,
				texcoords[i * 2 + 0] / bm->width(),
				texcoords[i * 2 + 1] / bm->height()
			);
			glTexCoord2f(
				texcoords[i * 2 + 0] / bm->width(),
				texcoords[i * 2 + 1] / bm->height()
			);
			glVertex2f(points[i * 2 + 0] + mOffsets.back().x, points[i * 2 + 1] + mOffsets.back().y);
		}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OpenGL1_Canvas::fillRect   (float x, float y, float w, float h, uint32_t color) {
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
void OpenGL1_Canvas::outlineRect(float x, float y, float w, float h, uint32_t color) {
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

} // namespace widget

#endif // ifdef WIDGET_OPENGL1_IMPLEMENTATION
