#include "../include/wwidget/AttributeCollector.hpp"

#include "../include/wwidget/Canvas.hpp"

namespace wwidget {

bool StringAttributeCollector::startSection(std::string_view name) { return true; }
void StringAttributeCollector::endSection() {}

void StringAttributeCollector::operator()(std::string_view name, bool             b,     bool             default_val) {
	(*this)(name, std::string_view(b ? "true" : "false"), std::string_view(default_val ? "true" : "false"));
}
void StringAttributeCollector::operator()(std::string_view name, float            f,     float            default_val) {
	(*this)(name, to_string(f), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Flow             f,     Flow             default_val) {
	(*this)(name, to_string(f), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Point            point, Point            default_val) {
	(*this)(name, to_string(point), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Size             size,  Size             default_val) {
	(*this)(name, to_string(size), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Offset           off,   Offset           default_val) {
	(*this)(name, to_string(off), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Rect  const&     rect,  Rect  const&     default_val) {
	(*this)(name, to_string(rect), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Color const&     c,     Color const&     default_val) {
	(*this)(name, to_string(c), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Alignment        a,     Alignment        default_val) {
	(*this)(name, to_string(a), to_string(default_val));
}
void StringAttributeCollector::operator()(std::string_view name, Padding          a,     Padding          default_val) {
	(*this)(name, to_string(a), to_string(default_val));
}

} // namespace wwidget
