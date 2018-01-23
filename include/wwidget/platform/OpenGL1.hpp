#pragma once

#include "../Canvas.hpp"
#include "../async/Queue.hpp"

#include <GL/gl.h>
#include <cmath>

namespace wwidget {

class OpenGL1_Canvas : public Canvas {
	TaskQueue mQueue;

	void bindBitmap(std::shared_ptr<Bitmap> const& b) {
		auto& proxy = Canvas::proxyRef(b);
		GLuint handle = static_cast<GLuint>(reinterpret_cast<size_t>(proxy.get()));
		if(handle) { // Already uploaded?
			glBindTexture(GL_TEXTURE_2D, handle);
		}
		else { // Upload texture
			GLenum format;
			GLenum internalFormat;
			switch (b->format()) {
				case Bitmap::INVALID: throw std::runtime_error("Don't draw unloaded bitmaps!"); break;
				case Bitmap::ALPHA: internalFormat = GL_ALPHA; format = GL_ALPHA; break;
				case Bitmap::RGB: internalFormat = GL_RGB; format = GL_RGB; break;
				case Bitmap::RGBA: internalFormat = GL_RGBA; format = GL_RGBA; break;
			}

			GLuint tex = 0;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, b->width(), b->height(), 0, format, GL_UNSIGNED_BYTE, b->data());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			proxy = std::shared_ptr<void>(
				reinterpret_cast<void*>(static_cast<size_t>(tex)),
				[this](void* t) { // On destroy:
					// Add to queue of pending destructions
					mQueue.add([tex = (GLuint)(size_t)t]() {
						glDeleteTextures(1, &tex);
					});
				}
			);
		}
	}

public:
	void pushViewport(float x, float y, float w, float h) override {
		mQueue.executeSingleConsumer();
		glLoadIdentity();
		glTranslatef(-1, 1, 0);
		glScaled(2 / w, -2 / h, 1);
		// glTranslated(-x, -y, 0);
		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void popViewport() override {
		glPopMatrix();
		mQueue.executeSingleConsumer();
	}

	void pushClipRect(float x, float y, float w, float h) override {
		glPushMatrix();
		glTranslatef(x, y, 0);
	}
	void popClipRect() override {
		glPopMatrix();
	}

	void recommendUpload(std::weak_ptr<Bitmap> bm) override {
		mQueue.add([=]() {
			if(auto sbm = bm.lock()) {
				bindBitmap(sbm);
			}
		});
	}

	void rect(
		Rect const& area,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override
	{
		rect(area, {0, 0, (float)bm->width(), (float)bm->height()}, bm, tint);
	}

	void rect(
		Rect const& area,
		Rect const& texarea,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override
	{
		glEnable(GL_TEXTURE_2D);
		bindBitmap(bm);
		glColor4fv(tint.rgba);
		glBegin(GL_QUADS);
		glTexCoord2f(texarea.x0 / bm->width(),  texarea.y1 / bm->height());
		glVertex2f(area.x0, area.y1);
		glTexCoord2f(texarea.x1 / bm->width(), texarea.y1 / bm->height());
		glVertex2f(area.x1, area.y1);
		glTexCoord2f(texarea.x1 / bm->width(), texarea.y0 / bm->height());
		glVertex2f(area.x1, area.y0);
		glTexCoord2f(texarea.x0 / bm->width(),  texarea.y0 / bm->height());
		glVertex2f(area.x0, area.y0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	void rect(
		Rect const& area,
		Color const& color) override
	{
		glColor4fv(color.rgba);
		glBegin(GL_QUADS);
		glVertex2f(area.x0, area.y1);
		glVertex2f(area.x1, area.y1);
		glVertex2f(area.x1, area.y0);
		glVertex2f(area.x0, area.y0);
		glEnd();
	}

	void rect(
		float corner_radius,
		Rect const& area,
		Color const& color) override
	{
		// float max_radius = std::min(area.w / 2.f, area.h / 2.f);
		// if(corner_radius < 0 || corner_radius > max_radius)
		// 	corner_radius = max_radius;
		// TODO: corners
		rect(area, color);
	}

	void rects(
		size_t num,
		Rect const* areas,
		Rect const* texareas,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override
	{
		bindBitmap(bm);
		glEnable(GL_TEXTURE_2D);
		glColor4fv(tint.rgba);
		glBegin(GL_QUADS);
		for (size_t i = 0; i < num; i++) {
			auto& area = areas[i];
			auto& tarea = texareas[i];

			glTexCoord2f(tarea.x0 / bm->width(),  tarea.y1 / bm->height());
			glVertex2f(area.x0, area.y1);
			glTexCoord2f(tarea.x1 / bm->width(), tarea.y1 / bm->height());
			glVertex2f(area.x1, area.y1);
			glTexCoord2f(tarea.x1 / bm->width(), tarea.y0 / bm->height());
			glVertex2f(area.x1, area.y0);
			glTexCoord2f(tarea.x0 / bm->width(),  tarea.y0 / bm->height());
			glVertex2f(area.x0, area.y0);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	void box(
		Rect const& area,
		Color const& color) override
	{
		glColor4fv(color.rgba);
		glBegin(GL_LINE_LOOP);
		glVertex2f(area.x0, area.y1);
		glVertex2f(area.x1, area.y1);
		glVertex2f(area.x1, area.y0);
		glVertex2f(area.x0, area.y0);
		glEnd();
	}

	void box(
		float corner_radius,
		Rect const& area,
		Color const& color) override
	{
		// TODO: corners
		box(area, color);
	}

	void polygon( // Solid color
		size_t num, Point const* points, Color const& color) override
	{
		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_TRIANGLE_FAN);
		for (size_t i = 0; i < num; i++) {
			glVertex2fv(points[i].xy);
		}
		glEnd();
	}
	void polygon( // Vertex color
		size_t num, Point const* points, Color const* color) override
	{
		glBegin(GL_TRIANGLE_FAN);
		for (size_t i = 0; i < num; i++) {
			glColor4fv(color[i].rgba);
			glVertex2fv(points[i].xy);
		}
		glEnd();
	}
	void polygon( // Solid color texture
		size_t num, Point const* points,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()) override
	{
		bindBitmap(bm);
		glEnable(GL_TEXTURE_2D);
		glColor4fv(tint.rgba);
		glBegin(GL_TRIANGLE_FAN);
		for (size_t i = 0; i < num; i++) {
			// TODO: Texcoords
			glVertex2fv(points[i].xy);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	void polygon( // Solid color texture w/ texcoords
		size_t num, Point const* points, Point const* texcoords,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()) override
	{
		bindBitmap(bm);
		glEnable(GL_TEXTURE_2D);
		glColor4fv(tint.rgba);
		glBegin(GL_TRIANGLE_FAN);
		for (size_t i = 0; i < num; i++) {
			glTexCoord2fv(texcoords[i].xy);
			glVertex2fv(points[i].xy);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	void linestrip(
		size_t num, Point const* points, Color const& color) override
	{
		glColor4fv(color.rgba);
		glBegin(GL_LINE_STRIP);
		for(size_t i = 0; i < num; i++) {
			glVertex2fv(points[i].xy);
		}
		glEnd();
	}
	void lineloop(
		size_t num, Point const* points, Color const& color) override
	{
		glColor4fv(color.rgba);
		glBegin(GL_LINE_LOOP);
		for(size_t i = 0; i < num; i++) {
			glVertex2fv(points[i].xy);
		}
		glEnd();
	}
};

} // namespace wwidget
