#pragma once

#include "../Canvas.hpp"

namespace widget {

class OpenGL1_Canvas : public Canvas {
public:
	virtual void recommendUpload(std::weak_ptr<Bitmap> bm) {}

	virtual void rect(
		Rect const& area,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint) {}

	virtual void rect(
		Rect const& area,
		Rect const& subarea,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint) {}

	virtual void rect(
		Rect const& area,
		Color const& color) {}

	virtual void rect(
		float corner_radius,
		Rect const& area,
		Color const& color) {}

	virtual void rectline(
		Rect const& area,
		Color const& color) {}

	virtual void rectline(
		float corner_radius,
		Rect const& area,
		Color const& color) {}
};

} // namespace widget
