#include "../include/wwidget/Applet.hpp"

namespace wwidget {

Applet::Applet() {}
Applet::~Applet() {}

std::string Applet::getRessource(RessourceId res) {
	// TODO: windows compatibility
	switch(res) {
		case URL_ROOT: return "/";
		case URL_LIBRARIES: return "/usr/lib/";
		case URL_BINARIES: return "/usr/bin/";
		case URL_HOME: {
			const char* result = getenv("HOME");
			return result ? result : "";
		}
		case URL_PICTURES:   return getRessource(URL_HOME) + std::string("/Pictures/");
		case URL_DOCUMENTS:  return getRessource(URL_HOME) + std::string("/Documents/");
		case URL_MUSIC:      return getRessource(URL_HOME) + std::string("/Music/");
		case URL_CACHE_DIR:  return getRessource(URL_TMP_DIR);
		case URL_TMP_DIR:    return getRessource(URL_ROOT) + std::string("/tmp/");
		case URL_CONFIG_DIR: return getRessource(URL_HOME) + std::string("/.config/");
		default: return "";
	}
}

} // namespace wwidget
