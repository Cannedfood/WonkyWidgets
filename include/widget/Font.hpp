#pragma once

#include <memory.h>
#include <memory>
#include <unordered_map>

namespace widget {

class BitmapFont;
class FontDescription;

/// A font with proper support for just about everything.
class Font {
	template<typename... ARGS>
	using umap = std::unordered_map<ARGS...>;

	template<typename T>
	using wptr = std::weak_ptr<T>;

	struct FontInfo {
		float size, dpix, dpiy;

		bool operator==(FontInfo const& other) const noexcept {
			return memcmp(this, &other, sizeof(FontInfo)) == 0;
		}

		struct hash {
			size_t operator()(FontInfo const& info) const noexcept;
		};
	};

	std::shared_ptr<FontDescription> mFontDescription;
	umap<FontInfo, wptr<BitmapFont>, FontInfo::hash> mCache;
public:
	Font();
	Font(std::string const& path);
	Font(std::shared_ptr<FontDescription> const& fnt);

	void load(std::string const& path);
	void init(std::shared_ptr<FontDescription> const& fnt);

	std::shared_ptr<BitmapFont> get(float size, float dpix = 0, float dpiy = 0);

	std::shared_ptr<FontDescription> const& fontDescription() const noexcept { return mFontDescription; }
};

} // namespace widget
