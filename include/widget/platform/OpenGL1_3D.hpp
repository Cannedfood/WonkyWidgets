#pragma once

#include "OpenGL1.hpp"

namespace widget {

class OpenGL1_3D_Canvas : public OpenGL1_Canvas {
	float mDepthScale  = 1;
	float mDepthOffset = -1;
	float getDepth() const {
		return mOffsets.size() * mDepthScale + mDepthOffset;
	}
public:
	OpenGL1_3D_Canvas(float depth, float x, float y, float w, float h);
	~OpenGL1_3D_Canvas();

	void outlinePoly(float* points, size_t number, uint32_t color) override;
	void fillPoly(float* points, size_t number, uint32_t color) override;
	void fillRect   (float x, float y, float w, float h, uint32_t color) override;
	void outlineRect(float x, float y, float w, float h, uint32_t color) override;
};

} // namespace widget

// =============================================================
// == Implementation =============================================
// =============================================================

#ifdef WIDGET_OPENGL1_IMPLEMENTATION

namespace widget {

OpenGL1_3D_Canvas::OpenGL1_3D_Canvas(float depth_scale, float x, float y, float w, float h) :
	mDepthScale(depth_scale),
	mDepthOffset(-depth_scale * mOffsets.size())
{
	// TODO: FOV and perspective matrix
	pushUiMatrix(x, y, w, h);
	glEnable(GL_BLEND);
}

OpenGL1_3D_Canvas::~OpenGL1_3D_Canvas() {
	glDisable(GL_BLEND);
	popUiMatrix();
}

void OpenGL1_3D_Canvas::outlinePoly(float* points, size_t number, uint32_t color) {
	float depth = getDepth();
	glColorU32(color);
	glBegin(GL_LINE_LOOP);
		for(size_t i = 0; i < number; i++) {
			glVertex3f(points[i * 2 + 0] + mOffsets.back().x, points[i * 2 + 1] + mOffsets.back().y, depth);
		}
	glEnd();
}
void OpenGL1_3D_Canvas::fillPoly(float* points, size_t number, uint32_t color) {
	float depth = getDepth();
	glColorU32(color);
	glBegin(GL_TRIANGLE_FAN);
		for(size_t i = 0; i < number; i++) {
			glVertex3f(points[i * 2 + 0] + mOffsets.back().x, points[i * 2 + 1] + mOffsets.back().y, depth);
		}
	glEnd();
}

void OpenGL1_3D_Canvas::fillRect   (float x, float y, float w, float h, uint32_t color) {
	float depth = getDepth();
	x += mOffsets.back().x;
	y += mOffsets.back().y;
	glColorU32(color);
	glBegin(GL_QUADS);
		glVertex3f(x, y, depth);
		glVertex3f(x + w, y, depth);
		glVertex3f(x + w, y + h, depth);
		glVertex3f(x, y + h, depth);
	glEnd();
}
void OpenGL1_3D_Canvas::outlineRect(float x, float y, float w, float h, uint32_t color) {
	float depth = getDepth();
	x += mOffsets.back().x;
	y += mOffsets.back().y;
	glColorU32(color);
	glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, depth);
		glVertex3f(x + w, y, depth);
		glVertex3f(x + w, y + h, depth);
		glVertex3f(x, y + h, depth);
	glEnd();
}

} // namespace widget

#endif // ifdef WIDGET_OPENGL1_IMPLEMENTATION
