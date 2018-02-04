#include "../include/wwidget/Events.hpp"

#include <stdexcept>

#include <cstdlib>

extern "C" {
	#include <memory.h>
}

namespace wwidget {

void TextInput::calcUtf8() {
	memset(utf8, 0, sizeof(utf8));
	int result = wctomb(utf8, utf32);
	if(result < 0)
		throw std::runtime_error("Couldn't convert character from wchar to utf8");
}
void TextInput::calcUtf32() {
	wchar_t utf32_ish;
	int len = mbtowc(&utf32_ish, utf8, sizeof(utf8));
	if(len < 0)
		throw std::runtime_error("Couldn't convert character from utf8 to wchar");
	utf32 = utf32_ish;
}

} // namespace wwidget
