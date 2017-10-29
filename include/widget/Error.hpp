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

class ParsingError : public std::exception {
	std::string mMessage;
	std::string mFile;
	size_t      mColumn;
	size_t      mLine;

	std::string mWhatBuffer;

	void updateWhat();
public:
	ParsingError(std::string const& msg);
	ParsingError(std::string const& msg, const char* error_at, const char* text_begin);
	ParsingError(std::string const& msg, std::string const& file, const char* error_at, const char* text_begin);
	void errorAt(const char* c, const char* begin_of_file);
	void setFile(std::string const& file);

	const char* what() const noexcept override;
};

} // namespace exceptions
} // namespace widget
