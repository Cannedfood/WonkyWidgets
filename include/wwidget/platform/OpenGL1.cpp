#include "OpenGL1.hpp"

namespace wwidget {

void OpenGL1_Canvas::bindBitmap(std::shared_ptr<Bitmap> const& b) {
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

void OpenGL1_Canvas::pushViewport(float x, float y, float w, float h) {
	mQueue.executeSingleConsumer();
	glLoadIdentity();
	glTranslatef(-1, 1, 0);
	glScaled(2 / w, -2 / h, 1);
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGL1_Canvas::popViewport() {
	glPopMatrix();
	mQueue.executeSingleConsumer();
}

void OpenGL1_Canvas::pushClipRect(float x, float y, float w, float h) {
	glPushMatrix();
	glTranslatef(x, y, 0);
}
void OpenGL1_Canvas::popClipRect() {
	glPopMatrix();
}

void OpenGL1_Canvas::recommendUpload(std::weak_ptr<Bitmap> bm) {
	mQueue.add([=]() {
		if(auto sbm = bm.lock()) {
			bindBitmap(sbm);
		}
	});
}

void OpenGL1_Canvas::rect(
	Rect const& area,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	rect(area, {0, 0, (float)bm->width(), (float)bm->height()}, bm, tint);
}

void OpenGL1_Canvas::rect(
	Rect const& area,
	Rect const& texarea,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	glEnable(GL_TEXTURE_2D);
	bindBitmap(bm);
	glColor4fv(tint.rgba);
	glBegin(GL_QUADS);
	glTexCoord2f(texarea.min.x / bm->width(),  texarea.max.y / bm->height());
	glVertex2f(area.min.x, area.max.y);
	glTexCoord2f(texarea.max.x / bm->width(), texarea.max.y / bm->height());
	glVertex2f(area.max.x, area.max.y);
	glTexCoord2f(texarea.max.x / bm->width(), texarea.min.y / bm->height());
	glVertex2f(area.max.x, area.min.y);
	glTexCoord2f(texarea.min.x / bm->width(),  texarea.min.y / bm->height());
	glVertex2f(area.min.x, area.min.y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OpenGL1_Canvas::rect(
	Rect const& area,
	Color const& color)
{
	glColor4fv(color.rgba);
	glBegin(GL_QUADS);
	glVertex2f(area.min.x, area.max.y);
	glVertex2f(area.max.x, area.max.y);
	glVertex2f(area.max.x, area.min.y);
	glVertex2f(area.min.x, area.min.y);
	glEnd();
}

void OpenGL1_Canvas::rect(
	float corner_radius,
	Rect const& area,
	Color const& color)
{
	// float max_radius = std::min(area.w / 2.f, area.h / 2.f);
	// if(corner_radius < 0 || corner_radius > max_radius)
	// 	corner_radius = max_radius;
	// TODO: corners
	rect(area, color);
}

void OpenGL1_Canvas::rects(
	size_t num,
	Rect const* areas,
	Rect const* texareas,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	bindBitmap(bm);
	glEnable(GL_TEXTURE_2D);
	glColor4fv(tint.rgba);
	glBegin(GL_QUADS);
	for (size_t i = 0; i < num; i++) {
		auto& area = areas[i];
		auto& tarea = texareas[i];
		glTexCoord2f(tarea.min.x / bm->width(),  tarea.max.y / bm->height());
		glVertex2f(area.min.x, area.max.y);
		glTexCoord2f(tarea.max.x / bm->width(), tarea.max.y / bm->height());
		glVertex2f(area.max.x, area.max.y);
		glTexCoord2f(tarea.max.x / bm->width(), tarea.min.y / bm->height());
		glVertex2f(area.max.x, area.min.y);
		glTexCoord2f(tarea.min.x / bm->width(),  tarea.min.y / bm->height());
		glVertex2f(area.min.x, area.min.y);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OpenGL1_Canvas::box(
	Rect const& area,
	Color const& color)
{
	glColor4fv(color.rgba);
	glBegin(GL_LINE_LOOP);
	glVertex2f(area.min.x + .5f, area.max.y + .5f);
	glVertex2f(area.max.x + .5f, area.max.y + .5f);
	glVertex2f(area.max.x + .5f, area.min.y + .5f);
	glVertex2f(area.min.x + .5f, area.min.y + .5f);
	glEnd();
}

void OpenGL1_Canvas::box(
	float corner_radius,
	Rect const& area,
	Color const& color)
{
	// TODO: corners
	box(area, color);
}

void OpenGL1_Canvas::polygon( // Solid color
	size_t num, Point const* points, Color const& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_FAN);
	for (size_t i = 0; i < num; i++) {
		glVertex2fv(points[i].xy);
	}
	glEnd();
}
void OpenGL1_Canvas::polygon( // Vertex color
	size_t num, Point const* points, Color const* color)
{
	glBegin(GL_TRIANGLE_FAN);
	for (size_t i = 0; i < num; i++) {
		glColor4fv(color[i].rgba);
		glVertex2fv(points[i].xy);
	}
	glEnd();
}
void OpenGL1_Canvas::polygon( // Solid color texture w/ texcoords
	size_t num, Point const* points, Point const* texcoords,
	std::shared_ptr<Bitmap> const& bm, Color const& tint)
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
void OpenGL1_Canvas::linestrip(
	size_t num, Point const* points, Color const& color)
{
	glColor4fv(color.rgba);
	glBegin(GL_LINE_STRIP);
	for(size_t i = 0; i < num; i++) {
		glVertex2f(points[i].x + .5f, points[i].y + .5f);
	}
	glEnd();
}
void OpenGL1_Canvas::lineloop(
	size_t num, Point const* points, Color const& color)
{
	glColor4fv(color.rgba);
	glBegin(GL_LINE_LOOP);
	for(size_t i = 0; i < num; i++) {
		glVertex2f(points[i].x + .5f, points[i].y + .5f);
	}
	glEnd();
}

} // namespace wwidget
