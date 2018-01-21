#include "../include/wwidget/Events.hpp"

#include "3rd-party/utf8.hpp"

extern "C" {
	#include <memory.h>
}

namespace wwidget {

void TextInput::calcUtf8() {
	memset(utf8, 0, sizeof(utf8));
	stx::utf32to8(this->utf32, this->utf8);
}
void TextInput::calcUtf32() {
	this->utf32 = stx::utf8to32(this->utf8);
}

} // namespace wwidget
