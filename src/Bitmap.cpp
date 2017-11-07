#include "../include/widget/Bitmap.hpp"

namespace widget {

Bitmap::Bitmap() {}
Bitmap::~Bitmap() {}

void Bitmap::size(float w, float h) {
	mWidth = w;
	mHeight = h;
}

} // namespace widget
