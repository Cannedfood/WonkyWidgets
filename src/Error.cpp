#include "../include/wwidget/Error.hpp"

#include <string>
#include <sstream>
#include <utility>

namespace wwidget {
namespace exceptions {

AnyError::AnyError() {}
AnyError::AnyError(std::string msg) : mMessage(std::move(msg)) {}
const char* AnyError::what() const noexcept { return mMessage.c_str(); }


WidgetNotFound::WidgetNotFound(Widget const* in, const char* in_name, const char* of_type, const char* name) :
	AnyError(
		"Couldn't Widget::find "+std::string(of_type)+" "+std::string(name)+" in "+std::string(in_name)
	)
{}

InvalidOperation::InvalidOperation(std::string const& msg) :
	AnyError(msg)
{}

Unimplemented::Unimplemented() :
	AnyError("Called an unimplemented function")
{}

InvalidPointer::InvalidPointer(std::string const& argumentName) :
	AnyError("Called invalid pointer: " + argumentName)
{}

RootNodeSibling::RootNodeSibling() :
	AnyError("Tried to add a sibling to a root widget (a widget without a parent)")
{}

FailedLoadingFile::FailedLoadingFile(std::string const& path) :
	AnyError("Failed loading file '" + path + "'")
{}
FailedLoadingFile::FailedLoadingFile(std::string const& path, std::string const& reason) :
	AnyError("Failed loading file '" + path + "': " + reason)
{}

ParsingError::ParsingError(std::string const& msg) :
	AnyError(msg),
	mFile(), mColumn(0), mLine(0),
	mWhatBuffer()
{
	mWhatBuffer = mMessage;
}

ParsingError::ParsingError(std::string const& msg, const char* error_at, const char* text_begin) :
	AnyError(msg),
	mFile(), mColumn(0), mLine(0),
	mWhatBuffer()
{
	errorAt(error_at, text_begin);
}

ParsingError::ParsingError(std::string const& msg, std::string const& file, const char* error_at, const char* text_begin) :
	AnyError(msg),
	mFile(file), mColumn(0), mLine(0),
	mWhatBuffer()
{
	errorAt(error_at, text_begin);
}

void ParsingError::updateWhat() {
	mWhatBuffer.clear();
	if(!mFile.empty()) {
		mWhatBuffer += mFile + ":";
	}
	if(mColumn != 0) {
		mWhatBuffer += std::to_string(mLine) + ':' + std::to_string(mColumn) + ": ";
	}
	mWhatBuffer += mMessage;
}

void ParsingError::errorAt(const char* at, const char* begin_of_file) {
	size_t column = 1;
	size_t line   = 0;
	const char* c = begin_of_file;
	while(c <= at) {
		if(*c == '\n') {
			column = 1;
			++line;
		}
		else {
			++column;
		}
		++c;
	}

	mColumn = column;
	mLine = line;
	updateWhat();
}

void ParsingError::setFile(std::string const& file) {
	mFile = file;
	updateWhat();
}

const char* ParsingError::what() const noexcept {
	return mWhatBuffer.c_str();
}

} // namespace exceptions
} // namespace wwidget
