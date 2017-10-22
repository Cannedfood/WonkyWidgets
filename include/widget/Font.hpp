#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <vector>

namespace widget {

class Texture;

struct FontPage {
	enum Format {
		BITMAP,
		PIXMAP,
		RGB
	};

	struct Glyph {
		unsigned minx;
		unsigned maxx;
		unsigned width;
		unsigned height;
		unsigned advance_x;
	};

	std::shared_ptr<Texture>   texture;
	std::unique_ptr<uint8_t[]> data;
	unsigned                   width;
	unsigned                   height;
	Format                     format;
	std::vector<Glyph>         glyphs;
};

struct FontGlyph {
	std::weak_ptr<FontPage> page;
	struct { float x, y; }  min;
};

class Font {
	using TextureGenerator = std::function<std::shared_ptr<Texture>(std::shared_ptr<FontPage> const& p)>;

	struct Data;

	Data* mData;

	float mDpiX, mDpiY;
	TextureGenerator mTextureGenerator;

	void reloadAllPages();
	void generateFontPage(FontPage* page, float size, uint32_t index);
public:
	Font();
	Font(std::string const& path, int index = 0);
	~Font();

	void setDpi(float x, float y = -1);
	void setTextureGenerator(TextureGenerator&& gen);

	void load(std::string const& path, int index = 0);
	void load(void const* data, size_t length, int index = 0);
	void free();

	std::shared_ptr<FontPage> getFontPage(float size, uint32_t with_character);

	void render(std::string const& s, uint8_t* data, unsigned w, unsigned h);
};

class CachedString {
	struct Quads {
		std::vector<float> positions;
		std::vector<float> sizes;
		std::vector<float> texture;
		std::vector<float> texture_sizes;
	};

	std::shared_ptr<Font> mFont;
	std::map<std::shared_ptr<FontPage>, Quads> mPages;

	std::string mString;
public:
	CachedString();
};

} // namespace widget
