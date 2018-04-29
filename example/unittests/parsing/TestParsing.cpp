#include "../Test.hpp"

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

#include <wwidget/widget/Form.hpp>
#include <wwidget/Error.hpp>

void testParsing() {
	wwidget::Form form;

	for(auto& entry : fs::directory("example/unittests/parsing/succeeds")) {
		test_hint(entry.path().c_str());
		expect_noexcept([&]() {
			form.load(entry.path());
		});
	}
	for(auto& entry : fs::directory("example/unittests/parsing/fails")) {
		test_hint(entry.path().c_str());
		expect_exception(wwidget::exceptions::AnyError, [&]() {
			form.load(entry.path());
		});
	}
}
