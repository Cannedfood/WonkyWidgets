#include "../include/wwidget/BasicContext.hpp"

#include "../include/wwidget/Canvas.hpp"

#include "../include/wwidget/Font.hpp"

#include "../include/wwidget/async/Threadpool.hpp"
#include "../include/wwidget/async/Queue.hpp"

#include <GL/gl.h>

#include <unordered_map>

namespace wwidget {

struct BasicContext::Implementation {
	struct {
		std::mutex                                             mutex;
		std::unordered_map<std::string, std::weak_ptr<Bitmap>> images;
		std::unordered_map<std::string, std::weak_ptr<Font>>   fonts;

		auto lock() { return std::unique_lock<std::mutex>(mutex); }
	} cache;

	Threadpool              threadpool;
	TaskQueue               updateTasks;

	std::shared_ptr<Canvas> canvas;
	Widget*                 rootWidget = nullptr;


	std::string defaultFont;

	Implementation() :
		threadpool(std::thread::hardware_concurrency())
	{}
};

BasicContext::BasicContext() :
	mImpl(new Implementation)
{
	mImpl->defaultFont = "/usr/share/fonts/TTF/LiberationMono-Regular.ttf"; // TODO: Font path not cross platform;
}
BasicContext::~BasicContext() {
	delete mImpl;
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
	mImpl->updateTasks.add(std::move(fn));
}

void BasicContext::loadImage(std::function<void(std::shared_ptr<Bitmap>)> fn, std::string const& url) {
	// printf("Started loading image %s\n", url.c_str());

	{ // Check cache
		auto& cache = mImpl->cache;
		auto _  = cache.lock();
		if(auto s = cache.images[url].lock()) {
			// printf("Found %s in cache\n", url.c_str());
			fn(std::move(s));
			return;
		}
	}

	// TODO: do this in a proper thread pool
	// printf("Loading %s in new thread...\n", url.c_str());
	mImpl->threadpool.add(
		[this, url = std::string(url), fn = std::move(fn)]() {
			std::shared_ptr<Bitmap> s;

			try { s = loadImage(url); }
			catch(std::runtime_error& e) {
				fprintf(stderr, "%s\n", e.what());
			}

			defer([fn = std::move(fn), s = std::move(s), url = std::move(url)]() {
				// printf("Invoking callback for %s\n", url.c_str());
				fn(std::move(s));
				// printf("Finished loading %s\n", url.c_str());
			});
		}
	);
}
void BasicContext::loadFont(std::function<void(std::shared_ptr<Font>)> fn, std::string const& url) {
	fn(loadFont(url));
}

std::shared_ptr<Bitmap> BasicContext::loadImage(std::string const& url) {
	auto& cache = mImpl->cache;
	cache.mutex.lock();
	auto& cacheEntry = cache.images[url];
	auto  s          = cacheEntry.lock();
	cache.mutex.unlock();

	if(!s) {
		auto tmp_bmp = std::make_shared<Bitmap>();
		tmp_bmp->load(url);
		s = tmp_bmp;
		{
			auto lock = cache.lock();
			cacheEntry = s;
		}
	}

	return s;
}
std::shared_ptr<Font>   BasicContext::loadFont(std::string const& url) {
	std::string const* pUrl = &url;
	if(url.empty()) {
		pUrl = &mImpl->defaultFont;
	}

	mImpl->cache.mutex.lock();
	auto& cacheEntry = mImpl->cache.fonts[*pUrl];
	auto s = cacheEntry.lock();
	mImpl->cache.mutex.unlock();
	if(!s) {
		cacheEntry = s = std::make_shared<Font>();
		s->load(*pUrl);
	}

	return s;
}

void BasicContext::execute(Widget* from, std::string_view cmd) {
	if(cmd == "remove") {
		from->remove();
	}
}
void BasicContext::execute(Widget* from, std::string_view const* cmds, size_t count) {
	for(size_t i = 0; i < count; i++) {
		execute(from, cmds[i]);
	}
}

bool BasicContext::update() {
	bool a, b;
	unsigned count = 0;
	do {
		a = 0 < mImpl->updateTasks.executeSingleConsumer();
		b = rootWidget() ? rootWidget()->updateLayout() : false;
		++count;
	} while((a || b) && count < 100);

	return count > 1;
}
void BasicContext::draw() {
	if(canvas() && rootWidget()) {
		canvas()->pushViewport(
			rootWidget()->offsetx(),
			rootWidget()->offsety(),
			rootWidget()->width(),
			rootWidget()->height()
			);
		rootWidget()->draw(*mImpl->canvas);
		canvas()->popViewport();
	}
}

void    BasicContext::rootWidget(Widget* w) {
	if(mImpl->rootWidget) {
		mImpl->rootWidget->context(nullptr);
	}
	mImpl->rootWidget = w;
	w->context(this);
}
Widget* BasicContext::rootWidget() {
	return mImpl->rootWidget;
}

void BasicContext::canvas(std::shared_ptr<Canvas> c) noexcept {
	mImpl->canvas = c;
}
std::shared_ptr<Canvas> const& BasicContext::canvas() const noexcept {
	return mImpl->canvas;
}

} // namespace wwidget
