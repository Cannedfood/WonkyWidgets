#pragma once

#include <GL/gl.h>

#include "../Bitmap.hpp"

#include "../Error.hpp"

namespace widget {

class OpenGL1_Bitmap : public Bitmap {
	unsigned mTexture = 0;
public:
	OpenGL1_Bitmap();
	OpenGL1_Bitmap(uint8_t const* data, unsigned w, unsigned h, unsigned components);
	~OpenGL1_Bitmap();

	void load(uint8_t const* data, unsigned w, unsigned h, unsigned components);
	void free();

	unsigned handle();
};

} // namespace widget

// =============================================================
// == Implementation =============================================
// =============================================================

#ifdef WIDGET_OPENGL1_IMPLEMENTATION

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace widget {

OpenGL1_Bitmap::OpenGL1_Bitmap() {}
OpenGL1_Bitmap::OpenGL1_Bitmap(uint8_t const* data, unsigned w, unsigned h, unsigned components) :
	Bitmap()
{
	load(data, w, h, components);
}
OpenGL1_Bitmap::~OpenGL1_Bitmap() {
	free();
}

void OpenGL1_Bitmap::load(uint8_t const* data, unsigned w, unsigned h, unsigned components) {
	free();

	if(!mTexture) {
		glGenTextures(1, &mTexture);
	}

	GLenum internalFormat = GL_INVALID_ENUM;
	GLenum format         = GL_INVALID_ENUM;
	switch(components) {
		case 1: format = internalFormat = GL_ALPHA;  break;
		case 3: format = internalFormat = GL_RGB;  break;
		case 4: format = internalFormat = GL_RGBA; break;
		default: throw exceptions::InvalidOperation("Unsupported number of components: " + std::to_string(components));
	}

	glBindTexture  (GL_TEXTURE_2D, mTexture);
	glTexImage2D   (GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/*
	if(components == 1) {
		GLint values[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, values);
	}
	*/
	Bitmap::size(w, h);
}
void OpenGL1_Bitmap::free() {
	if(mTexture) {
		glDeleteTextures(1, &mTexture);
		mTexture = 0;
	}
}

unsigned OpenGL1_Bitmap::handle() {
	return mTexture;
}

} // namespace widget

#endif // ifdef WIDGET_OPENGL1_IMPLEMENTATION
