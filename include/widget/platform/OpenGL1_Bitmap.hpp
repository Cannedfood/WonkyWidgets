#pragma once

#include <GL/gl.h>

#include "../Bitmap.hpp"

#include "../Error.hpp"

namespace widget {

class OpenGL1_Bitmap : public Bitmap {
	unsigned mTexture;
public:
	OpenGL1_Bitmap();
	OpenGL1_Bitmap(uint8_t* data, unsigned w, unsigned h, unsigned components);
	~OpenGL1_Bitmap();

	void load(uint8_t* data, unsigned w, unsigned h, unsigned components);
	void free();

	unsigned handle();
};

} // namespace widget

// =============================================================
// == Implementation =============================================
// =============================================================

#ifdef WIDGET_OPENGL1_IMPLEMENTATION

namespace widget {

OpenGL1_Bitmap::OpenGL1_Bitmap() :
	mTexture(0)
{}
OpenGL1_Bitmap::OpenGL1_Bitmap(uint8_t* data, unsigned w, unsigned h, unsigned components) :
	Bitmap()
{
	load(data, w, h, components);
}
OpenGL1_Bitmap::~OpenGL1_Bitmap() {
	free();
}

void OpenGL1_Bitmap::load(uint8_t* data, unsigned w, unsigned h, unsigned components) {
	if(!mTexture) {
		glGenTextures(1, &mTexture);
	}

	GLenum format;
	GLenum internalFormat;
	switch (components) {
		case 1:
			format = GL_ALPHA;
			internalFormat = GL_ALPHA;
		break;
		case 2:
			format = GL_RG;
			internalFormat = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			internalFormat = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = GL_RGBA;
			break;
		default: throw exceptions::InvalidOperation("Unsupported number of components: " + std::to_string(components));
	}

	glTexImage2D(mTexture, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
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
