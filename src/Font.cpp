#include "../include/widget/Font.hpp"

#include "../include/widget/fonts/BitmapFont.hpp"
#include "../include/widget/fonts/FontDescription.hpp"

#include "../include/widget/Error.hpp"

using namespace std;

namespace widget {

Font::Font() {}
Font::Font(std::string const& path) {
	load(path);
}
Font::Font(std::shared_ptr<FontDescription> const& fnt) :
	mFontDescription(fnt)
{}

void Font::load(std::string const& path) {
	init(std::make_shared<FontDescription>(path));
}
void Font::init(std::shared_ptr<FontDescription> const& fnt) {
	mCache.clear();
	mFontDescription = fnt;
}

shared_ptr<BitmapFont> Font::get(float size, float dpix, float dpiy) {
	if(!mFontDescription) {
		throw exceptions::InvalidOperation("Font is not loaded");
	}

	if(dpix <= 0) {
		dpix = 72;
	}
	if(dpiy <= 0) {
		dpiy = dpix;
	}

	auto info = FontInfo{ size, dpix, dpiy };

	weak_ptr<BitmapFont>&  cacheEntry = mCache[info];
	shared_ptr<BitmapFont> result     = cacheEntry.lock();

	if(!result) {
		result = make_shared<BitmapFont>();
		mFontDescription->render(dpix, dpiy, size, *result);
		cacheEntry = result;
	}

	return result;
}

size_t Font::FontInfo::hash::operator()(FontInfo const& info) const noexcept {
	return (((unsigned&) info.dpix) + ((unsigned&) info.dpix) * ((unsigned&) info.dpiy)) * ((unsigned&) info.size);
}

} // namespace widget
