#include "../include/wwidget/AttributeCollector.hpp"

#include "../include/wwidget/Canvas.hpp"

namespace wwidget {

static
std::string to_string(float f) {
	std::string result = std::to_string(f);
	result.erase(result.find_last_not_of('0'));
	return result;
}

bool StringAttributeCollector::startSection(std::string_view name) { return true; }
void StringAttributeCollector::endSection() {}

void StringAttributeCollector::operator()(
	std::string_view name, bool  b, bool is_default)
{
	(*this)(name, b ? "true" : "false", is_default);
}
void StringAttributeCollector::operator()(
	std::string_view name, float f, bool is_default)
{
	(*this)(name, to_string(f), is_default);
}
void StringAttributeCollector::operator()(
	std::string_view name, float x, float y, bool is_default)
{
	(*this)(name, to_string(x) + " " + to_string(y), is_default);
}
void StringAttributeCollector::operator()(
	std::string_view name, float x, float y, float w, float h, bool is_default)
{
	(*this)(name, to_string(x) + " " + to_string(y) + " " + to_string(w) + " " + to_string(h), is_default);
}
void StringAttributeCollector::operator()(
	std::string_view name, Color const& c, bool is_default)
{
	if(c.a != 1) {
		(*this)(name, to_string(c.r) + " " + to_string(c.g) + " " + to_string(c.b) + " " + to_string(c.a), is_default);
	}
	else {
		(*this)(name, to_string(c.r) + " " + to_string(c.g) + " " + to_string(c.b), is_default);
	}
}
void StringAttributeCollector::operator()(
	std::string_view name, std::string_view s, bool is_default)
{
	(*this)(name, std::string(s), is_default);
}

} // namespace wwidget
