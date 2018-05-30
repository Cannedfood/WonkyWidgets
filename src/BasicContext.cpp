#include "../include/wwidget/BasicContext.hpp"

#include "../include/wwidget/Canvas.hpp"

#include "../include/wwidget/Font.hpp"

#include "../include/wwidget/async/Threadpool.hpp"

#include <GL/gl.h>

#include <unordered_map>

namespace wwidget {

struct BasicContext::Cache {
	std::mutex                                             mutex;
	std::unordered_map<std::string, std::weak_ptr<Bitmap>> images;
	std::unordered_map<std::string, std::weak_ptr<Font>>   fonts;
	Threadpool                                             threadpool;

	auto lock() { return std::unique_lock<std::mutex>(mutex); }

	Cache() :
		threadpool(std::thread::hardware_concurrency())
	{}
};

BasicContext::BasicContext() :
	mCache(new Cache),
	mDefaultFont("/usr/share/fonts/TTF/LiberationMono-Regular.ttf") // TODO: Font path not cross platform
{
	context(this);
}
BasicContext::~BasicContext() {
	delete mCache;
}

void BasicContext::cleanCache() {
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

void BasicContext::defer(std::function<void()> fn) {
	mUpdateTasks.add(std::move(fn));
}

void BasicContext::loadImage(std::function<void(std::shared_ptr<Bitmap>)> fn, std::string const& url) {
	// printf("Started loading image %s\n", url.c_str());

	{ // Check cache
		auto _  = mCache->lock();
		if(auto s = mCache->images[url].lock()) {
			// printf("Found %s in cache\n", url.c_str());
			fn(std::move(s));
			return;
		}
	}

	// TODO: do this in a proper thread pool
	// printf("Loading %s in new thread...\n", url.c_str());
	mCache->threadpool.add(
		[this, url = std::string(url), fn = std::move(fn)]() {
			mCache->mutex.lock();
			auto& cacheEntry = mCache->images[url];
			auto  s          = cacheEntry.lock();
			mCache->mutex.unlock();

			if(!s) {
				// printf("Loading %s...\n", url.c_str());
				try {
					auto tmp_bmp = std::make_shared<Bitmap>();
					tmp_bmp->load(url);
					s = tmp_bmp;
					{
						auto lock = mCache->lock();
						cacheEntry = s;
					}
				}
				catch(std::runtime_error& e) {
					fprintf(stderr, "%s\n", e.what());
				}
			}
			// else {
			// 	printf("Image %s found in cache\n", url.c_str());
			// }

			defer([fn = std::move(fn), s = std::move(s), url = std::move(url)]() {
				// printf("Invoking callback for %s\n", url.c_str());
				fn(std::move(s));
				// printf("Finished loading %s\n", url.c_str());
			});
	}
	);
}

void BasicContext::loadFont(std::function<void(std::shared_ptr<Font>)> fn, std::string const& url) {
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

bool BasicContext::update() {
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
void BasicContext::draw() {
	if(mCanvas) {
		mCanvas->pushViewport(offsetx(), offsety(), width(), height());
		Widget::draw(*mCanvas);
		mCanvas->popViewport();
	}
}

} // namespace wwidget
