#include "../../include/wwidget/widget/FileBrowser.hpp"

#include "../../include/wwidget/widget/Image.hpp"
#include "../../include/wwidget/widget/Text.hpp"
#include "../../include/wwidget/widget/Button.hpp"
#include "../../include/wwidget/widget/ContextMenu.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/UnicodeConstants.hpp"

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

class ContextDialogue : public List {
public:
	void add(std::string value, std::function<void()> on_click) {
		List::add<Button>()
			->onClick(on_click)
			->text(value);
	}
};

class FileIcon : public Button {
	fs::path mPath;
	bool mIsFolder = false, mIsLink = false;

	List mContent;
public:
	FileIcon(fs::path const& p, const char* display_name = nullptr) :
		mPath(p),
		mContent(this)
	{
		padding(1);

		mContent.align(AlignFill);
		mContent.flow(FlowDown);

		align(AlignFill);
		mIsLink   = fs::is_symlink(mPath);
		mIsFolder = fs::is_directory(mPath);

		if(!mIsFolder) {
			auto extension = p.extension();
			if(
				extension == ".jpg" ||
				extension == ".JPG" ||
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
				Image* img = mContent.add<Image>();
				img->maxSize({64});
				img->source(p)
				   ->align(AlignCenter);
			}
			else {
				mContent.add<Text>(UnicodeConstants::Document)
					->font("icon")
					->fontSize(64)
					->align(AlignCenter);
			}
		}
		else {
			mContent.add<Text>(UnicodeConstants::Folder)
				->font("icon")
				->fontSize(64)
				->align(AlignCenter);
		}

		mContent.add<Text>(display_name ? display_name : mPath.filename())
			->set(
				Alignment(AlignCenter),
				Padding(5, 0)
			);
	}

	void on(Click const& click) override {
		if(!click.down()) return;
		click.handled = true;

		if(click.button == 0) {
			if(mIsFolder) {
				defer([this, p = click.position]() {
					try {
						findParent<FileBrowser>()->path(mPath);
					}
					catch(std::filesystem::filesystem_error& e) {
						ContextMenu::Create(findRoot())
							->add<Text>(e.what());
					}
				});
			}
			else {
				system(("xdg-open '" + std::string(mPath) + "'").c_str());
			}
		}
		else {
			ContextMenu* ctxt = ContextMenu::Create(this, click.position);
			ctxt->add<Button>("Option 1");
			ctxt->add<Button>("Option 2");
			ctxt->add<Button>("Option 3");
		}
	}

	PreferredSize onCalcPreferredSize() override {
		auto size = Button::onCalcPreferredSize();
		size.pref.x = std::max(size.pref.x, size.pref.y);
		size.sanitize();
		return size;
	}

	void onDrawBackground(Canvas& c) override {
		Rect drawRect = {width(), height()};
		if(__builtin_expect(pressed(), 0)) {
			drawRect = { 2, 2, width() - 4, height() - 4 };
		}

		if(mIsFolder)
			c.fillColor(rgb(168, 179, 135));
		else
			c.fillColor(rgb(135, 175, 179));

		c.rect(drawRect, 5)
		 .fill();

		if(focused()) {
			c.strokeColor(rgb(215, 150, 0))
			 .rect(drawRect)
			 .stroke();
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
	scrollable(false);
	mHeader.align(AlignFill);
	mHeader.flow(FlowRight);
	mFilePane.flow(FlowDownRight);
	mFilePane.scrollable(true);

	mTextField.onReturn([this]() {
		this->path(mTextField.content());
	});
}
FileBrowser::FileBrowser(Widget* parent, std::string const& path) :
	FileBrowser(path)
{
	parent->add(this);
}

FileBrowser* FileBrowser::path(std::string const& path_s) {
	if(mTextField.content() == path_s) return this;

	fs::path path = path_s;
	if(!fs::is_directory(path)) {
		throw exceptions::CantEnterDirectory("Not a directory: " + std::string(path));
	}

	std::vector<fs::path> paths;
	for(auto& entry : fs::directory(path)) {
		if(entry.path().filename().c_str()[0] != '.')
			paths.emplace_back(entry.path());
	}

	std::sort(paths.begin(), paths.end());

	mFilePane.clearChildren();
	mFilePane.add<FileIcon>(path.parent_path(), "..");
	for(auto& p : paths) {
		mFilePane.add<FileIcon>(p);
	}
	mFilePane.scrollOffset(0);

	mTextField.content(path);

	return this;
}

std::string FileBrowser::homeDirectory() {
	return getenv("HOME"); // TODO: Make windows compatible
}

} // namespace wwidget
