#include "../../include/wwidget/widget/FileBrowser.hpp"

// #include "../../include/wwidget/widget/Image.hpp"
// #include "../../include/wwidget/widget/Text.hpp"
#include "../../include/wwidget/widget/Button.hpp"

#include "../../include/wwidget/Canvas.hpp"

#include <limits>

#if __has_include(<filesystem>)
	#include <filesystem>
	namespace fs { using namespace std::filesystem; }
#elif __has_include(<experimental/filesystem>)
	#include <experimental/filesystem>
	namespace fs { using namespace std::experimental::filesystem; }
#else
	#error "Your toolchain has neither std::filesystem nor std::experimental::filesystem"
#endif

namespace fs {
	struct directory {
		path mPath;
		directory(path p) : mPath(p) {}

		directory_iterator begin() { return directory_iterator{mPath}; }
		directory_iterator end() { return {}; }
	};
	struct directory_tree {
		path mPath;
		directory_tree(path p) : mPath(p) {}

		recursive_directory_iterator begin() { return recursive_directory_iterator{mPath}; }
		recursive_directory_iterator end() { return {}; }
	};
} // namespace fs


namespace wwidget {

class FileIcon : public Button {
	fs::path mPath;
	bool mIsFolder = false, mIsLink = false;
public:
	FileIcon(fs::path const& p, const char* display_name = nullptr) :
		mPath(p)
	{
		align(AlignFill);
		mIsLink   = fs::is_symlink(mPath);
		mIsFolder = fs::is_directory(mPath);

		if(!mIsFolder) {
			auto extension = p.extension();
			if(
				extension == ".jpg" ||
				extension == ".png" ||
				extension == ".PNG" ||
				extension == ".jpeg" ||
				extension == ".tga" ||
				extension == ".gif" ||
				extension == ".bmp" ||
				extension == ".ppm" ||
				extension == ".pgm" ||
				extension == ".pnm"
			) {
				image(p);
			}
		}

		text(display_name ? display_name : mPath.filename());

		onClick([this]() {
			try {
				findParent<FileBrowser>()->path(mPath);
			}
			catch(exceptions::CantEnterDirectory& e) {
				// TODO: display an error or something
			}
		});
	}

	PreferredSize onCalcPreferredSize() override {
		// TODO: Don't hard code sizes
		PreferredSize result = calcBoxAroundChildren(16, 16);
		result.min   = {32};
		result.max.x = 256;
		result.max.y = 64;
		result.sanitize();
		return result;
	}

	void onDrawBackground(Canvas& c) override {
		Rect drawRect = {width(), height()};
		if(__builtin_expect(pressed(), 0)) {
			drawRect = { 2, 2, width() - 4, height() - 4 };
		}

		c.rect(drawRect, mIsFolder ? rgb(168, 179, 135) : rgb(135, 175, 179));

		if(focused()) {
			c.box({0, 0, width(), height()}, rgb(215, 150, 0));
		}
	}
	// void onDraw(Canvas& c) override {}
};

FileBrowser::FileBrowser(std::string const& path) :
	mHeader(this),
	mFilePane(this),
	mTextField(mHeader)
{
	this->path(path);
	scrollable(true);
	mHeader.align(AlignFill);
	mHeader.flow(FlowRight);
	mFilePane.flow(FlowDownRight);

	mTextField.onReturn([this]() {
		this->path(mTextField.content());
	});
}
FileBrowser::FileBrowser(Widget* parent, std::string const& path) :
	FileBrowser(path)
{
	parent->add(this);
}

FileBrowser* FileBrowser::path(std::string const& path) {
	if(mTextField.content() == path) return this;

	fs::path dir = path;
	if(!fs::is_directory(dir)) {
		throw exceptions::CantEnterDirectory("Not a directory: " + path);
	}

	mFilePane.clearChildren();

	mFilePane.add<FileIcon>(dir.parent_path(), "..");
	std::vector<fs::path> paths;
	for(auto& entry : fs::directory(dir)) {
		if(entry.path().filename().c_str()[0] != '.')
			paths.emplace_back(entry.path());
	}

	std::sort(paths.begin(), paths.end());

	for(auto& p : paths) {
		mFilePane.add<FileIcon>(p);
	}

	mFilePane.scrollOffset(0);

	mTextField.content(dir);

	return this;
}

std::string FileBrowser::homeDirectory() {
	return getenv("HOME"); // TODO: Make windows compatible
}

} // namespace wwidget
