#pragma once

#include <stdexcept>

namespace wwidget {

class Widget;

namespace exceptions {

class AnyError : public std::exception {
protected:
	std::string mMessage;
public:
	AnyError();
	AnyError(std::string msg);

	const char* what() const noexcept override;
};

class WidgetNotFound : public AnyError {
public:
	WidgetNotFound(Widget const* in, const char* in_name, const char* of_type, const char* name);
};

class InvalidOperation : public AnyError {
public:
	InvalidOperation(std::string const& msg);
};

class Unimplemented : public AnyError {
public:
	Unimplemented();
};

class InvalidPointer : public AnyError {
public:
	InvalidPointer(std::string const& argumentName);
};

class RootNodeSibling : public AnyError {
public:
	RootNodeSibling();
};

class FailedLoadingFile : public AnyError {
public:
	FailedLoadingFile(std::string const& path);
	FailedLoadingFile(std::string const& path, std::string const& reason);
};

class ParsingError : public AnyError {
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
} // namespace wwidget
