#include "../include/widget/BasicApplet.hpp"

#include "../include/widget/Canvas.hpp"

#include "../include/widget/Font.hpp"

#include <GL/gl.h>

#include <unordered_map>


namespace widget {

struct BasicApplet::Cache {
	std::unordered_map<std::string, std::weak_ptr<Bitmap>> images;
	std::unordered_map<std::string, std::weak_ptr<Font>>   fonts;
};

BasicApplet::BasicApplet() :
	mCache(new Cache),
	mDefaultFont("/usr/share/fonts/TTF/LiberationMono-Regular.ttf") // TODO: Font path not cross platform
{}
BasicApplet::~BasicApplet() {
	delete mCache;
}

void BasicApplet::cleanCache() {
	/*
	for(auto& [url, res] : mCache->images) {
		if(res.unique())
			res.reset();
	}
	for(auto& [url, res] : mCache->fonts) {
		if(res.unique())
			res.reset();
	}
	*/
}

void BasicApplet::defer(std::function<void()> fn) {
	mUpdateTasks.add(std::move(fn));
}

void BasicApplet::loadImage(std::function<void(std::shared_ptr<Bitmap>)> fn, std::string const& url) {
	auto& cacheEntry = mCache->images[url];
	auto s = cacheEntry.lock();
	if(!s) {
		cacheEntry = s = std::make_shared<Bitmap>();
		s->load(url);
	}
	fn(std::move(s));
}

void BasicApplet::loadFont(std::function<void(std::shared_ptr<Font>)> fn, std::string const& url) {
	std::string const* pUrl = &url;
	if(url.empty()) {
		pUrl = &mDefaultFont;
	}

	auto& cacheEntry = mCache->fonts[*pUrl];
	auto s = cacheEntry.lock();
	if(!s) {
		cacheEntry = s = std::make_shared<Font>();
		s->load(*pUrl);
	}

	fn(std::move(s));
}

bool BasicApplet::update() {
	bool a = Widget::updateLayout();
	bool b = 0 < mUpdateTasks.executeSingleConsumer();
	if(!(a || b)) return false;
	for(size_t c = 0; c < 100; c++) {
		bool a = Widget::updateLayout();
		bool b = 0 < mUpdateTasks.executeSingleConsumer();
		if(!(a || b)) break;
	}
	return true;
}
void BasicApplet::draw() {
	if(mCanvas) {
		mCanvas->pushViewport(offsetx(), offsety(), width(), height());
		Widget::draw(*mCanvas);
		mCanvas->popViewport();
	}
}

void BasicApplet::onAdd(Widget* child) {
	child->applet(this);
}
void BasicApplet::onRemove(Widget* child) {
	child->applet(nullptr);
}

} // namespace widget
