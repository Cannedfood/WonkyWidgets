#pragma once

#include <memory.h>
#include <memory>
#include <unordered_map>

namespace wwidget {

class Bitmap;
class BitmapFont;
class FontDescription;

/// A font which can generate and cache bitmap fonts
class Font {
	template<typename... ARGS>
	using umap = std::unordered_map<ARGS...>;

	template<typename T>
	using wptr = weak<T>;

	struct FontInfo {
		float size, dpix, dpiy;

		bool operator==(FontInfo const& other) const noexcept {
			return memcmp(this, &other, sizeof(FontInfo)) == 0;
		}

		struct hash {
			size_t operator()(FontInfo const& info) const noexcept;
		};
	};

	struct Entry {
		shared<BitmapFont> mFont;
		shared<Bitmap>     mFontBitmap;
	};

	shared<FontDescription> mFontDescription;
	umap<FontInfo, wptr<BitmapFont>, FontInfo::hash> mCache;
public:
	Font();
	Font(std::string const& path);
	Font(shared<FontDescription> const& fnt);

	void load(std::string const& path);
	void init(shared<FontDescription> const& fnt);

	shared<BitmapFont> get(float size, float dpix = 0, float dpiy = 0);

	shared<FontDescription> const& fontDescription() const noexcept { return mFontDescription; }
};

} // namespace wwidget
