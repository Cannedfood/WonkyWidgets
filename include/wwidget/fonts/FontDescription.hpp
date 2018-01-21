#pragma once

#include <string>

#include "BitmapFont.hpp"

namespace wwidget {

class FontDescription {
	struct ImplData;
	ImplData* mImpl; // Using pImpl-idiom to enable easy backend switching
public:
	FontDescription();
	FontDescription(std::string const& path);
	FontDescription(std::istream& data);
	~FontDescription();

	void load(std::string const& path);
	void load(std::istream& data);
	void load(void* data, size_t length);
	void free();

	/// If cache is enabled it uses a cache file depending on the pointer and the dpix, dpiy, and size
	void render(
		float dpix, float dpiy, float size,
		BitmapFont& to,
		bool cache = true
	);

	void render(
		std::string const& string,
		float dpix, float dpiy, float size,
		std::vector<uint8_t>& bitmap, unsigned w, unsigned h,
		float* posx, float* posy
	);
};

} // namespace wwidget
