#include "Error.hpp"

#include <string>
#include <sstream>
#include <utility>

#include <xlogging>

template<typename... ARGS>
std::string format(const char* fmt, ARGS&&... args) {
	std::stringstream ss;
	stx::write_formatted(ss, fmt, std::forward<ARGS>(args)...);
	return ss.str();
}

namespace widget {
namespace exceptions {

WidgetNotFound::WidgetNotFound(Widget* in, const char* in_name, const char* of_type, const char* name) :
	std::runtime_error(
		format("Couldn't Widget::find %% %% in %% %%", of_type, name, in, in_name)
	)
{}

InvalidOperation::InvalidOperation(std::string const& msg) :
	std::runtime_error(msg)
{}

Unimplemented::Unimplemented() :
	std::runtime_error("Called an unimplemented function")
{}

InvalidPointer::InvalidPointer(std::string const& argumentName) :
	std::runtime_error("Called invalid pointer: " + argumentName)
{}

RootNodeSibling::RootNodeSibling() :
	std::runtime_error("Tried to add a sibling to a root widget (a widget without a parent)")
{}

FailedLoadingFile::FailedLoadingFile(std::string const& path) :
	std::runtime_error("Failed loading file '" + path + "'")
{}
FailedLoadingFile::FailedLoadingFile(std::string const& path, std::string const& reason) :
	std::runtime_error("Failed loading file '" + path + "': " + reason)
{}

} // namespace exceptions
} // namespace widget
