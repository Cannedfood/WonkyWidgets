#pragma once

namespace widget {

class Bitmap {
	float mWidth, mHeight;
protected:
	void size(float w, float h);
public:
	Bitmap();
	virtual ~Bitmap();

	inline float width() const { return mWidth; }
	inline float height() const { return mHeight; }
};

} // namespace widget
