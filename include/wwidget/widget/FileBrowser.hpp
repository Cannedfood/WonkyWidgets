#pragma once

#include "List.hpp"
#include "WrappedList.hpp"
#include "TextField.hpp"
#include "../Error.hpp"

namespace wwidget {

class FileBrowser : public List {
	List        mHeader;
	WrappedList mFilePane;

	TextField   mTextField;
public:
	FileBrowser(std::string const& path = homeDirectory());
	FileBrowser(Widget* parent, std::string const& path = homeDirectory());

	FileBrowser* path(std::string const& path);
	std::string const& path() const noexcept { return mTextField.content(); }

	static std::string homeDirectory();
};

namespace exceptions {

class CantEnterDirectory : public AnyError {
public:
	using AnyError::AnyError;
};

}

} // namespace wwidget
