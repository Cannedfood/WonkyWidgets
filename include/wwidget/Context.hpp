#pragma once

#include "Widget.hpp"

namespace wwidget {

class Font;

enum RessourceId {
	URL_ROOT,
	URL_LIBRARIES,
	URL_BINARIES,
	URL_HOME,
	URL_PICTURES,
	URL_DOCUMENTS,
	URL_MUSIC,
	URL_CACHE_DIR,
	URL_TMP_DIR,
	URL_CONFIG_DIR,
};

class Context {
public:
	Context();
	virtual ~Context();

	virtual void defer(std::function<void()>) = 0;

	virtual std::string getRessource(RessourceId res);

	virtual void loadImage(std::function<void(std::shared_ptr<Bitmap>)>, std::string const& url) = 0;
	virtual void loadFont(std::function<void(std::shared_ptr<Font>)>, std::string const& url) = 0;

	virtual bool update() = 0;
	virtual void draw() = 0;
};

} // namespace wwidget