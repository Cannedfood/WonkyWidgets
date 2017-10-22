#pragma once

#include <stdexcept>

namespace widget {

class Widget;

namespace exceptions {

class WidgetNotFound : public std::runtime_error {
public:
	WidgetNotFound(Widget* in, const char* in_name, const char* of_type, const char* name);
};

class InvalidOperation : public std::runtime_error {
public:
	InvalidOperation(std::string const& msg);
};

class Unimplemented : public std::runtime_error {
public:
	Unimplemented();
};

class InvalidPointer : public std::runtime_error {
public:
	InvalidPointer(std::string const& argumentName);
};

class RootNodeSibling : public std::runtime_error {
public:
	RootNodeSibling();
};

class FailedLoadingFile : public std::runtime_error {
public:
	FailedLoadingFile(std::string const& path);
	FailedLoadingFile(std::string const& path, std::string const& reason);
};

} // namespace exceptions
} // namespace widget
