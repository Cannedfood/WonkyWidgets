#pragma once

#include "../Widget.hpp"

namespace wwidget {

class ProgressBar : public Widget {
	float mScale;
	float mProgress;

	float mProgressInterpolated;

protected:
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;

public:
	ProgressBar();
	~ProgressBar() override;

	inline float progress() const noexcept { return mProgress; }
	ProgressBar* progress(float f);

	inline float scale() const noexcept { return mScale; }
	ProgressBar* scale(float f);

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
