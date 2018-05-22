#include "../../include/wwidget/widget/Form.hpp"

#include "../thirdparty/rapidxml/rapidxml.hpp"
#include "../thirdparty/rapidxml/rapidxml_utils.hpp"

#include <iostream>

#ifdef __GNUC__
#	include <cxxabi.h>
#endif

namespace wwidget {

Form::Form() {}
Form::Form(std::string const& path) :
	Form()
{
	load(path);
}
Form::Form(std::istream& stream) :
	Form()
{
	load(stream);
}
Form::~Form() noexcept {}

Form::Form(Widget* addTo, std::string const& path) :
	Form(path)
{
	addTo->add(this);
}
Form::Form(Widget* addTo, std::istream& stream) :
	Form(stream)
{
	addTo->add(this);
}

void Form::onDraw(Canvas&) {}

Form& Form::factory(std::string const& name, FactoryFn&& fn) {
	mFactories[name] = std::move(fn);
	return *this;
}

Form& Form::factory(std::type_info const& type, FactoryFn&& fn) {
	// Getting the demangled name (Platform dependent)
	#ifdef __GNUC__
		int status;
		char* demangled = abi::__cxa_demangle(type.name(), 0, 0, &status);
		factory(std::string(demangled), std::move(fn));
		free(demangled);
	#elif defined(_WIN32)
		factory(std::string(type.name()), std::move(fn));
	#else
		#error "Not supported for this compiler, please look above ^, implement it and submit a pull request."
	#endif

	return *this;
}

Form& Form::load(std::string const& path) {
	if(mFactories.empty()) addDefaultFactories();

	rapidxml::file<> file;
	try {
		file.load(path.c_str());
	}
	catch(std::runtime_error const& e) {
		throw exceptions::FailedLoadingFile(path);
	}

	try {
		return parse(file.data());
	}
	catch(exceptions::ParsingError& e) {
		e.setFile(path);
		throw;
	}
}

Form& Form::load(std::istream& stream) {
	rapidxml::file<> file;
	file.load(stream);
	return parse(file.data());
}

Form& Form::parse(const char* text) {
	using namespace rapidxml;
	constexpr int options = parse_comment_nodes | parse_non_destructive | parse_fastest;

	xml_document<> doc;
	try {
		doc.parse<options>(const_cast<char*>(text)); // We use the non-destructive mode, const_cast is safe
	}
	catch(rapidxml::parse_error const& e) {
		throw exceptions::ParsingError(e.what(), e.where<char>(), text);
	}

	auto buildRecursive = [=](auto& buildRecursive, Widget* to, xml_node<>* to_data) -> void {
		for(xml_attribute<>* attrib = to_data->first_attribute(); attrib; attrib = attrib->next_attribute()) {
			bool success = to->setAttribute(
				std::string(attrib->name(), attrib->name_size()),
				std::string(attrib->value(), attrib->value_size())
			);
			if(!success) {
				std::cerr <<
					"Unknown attribute '" << std::string(attrib->name(), attrib->name_size()) <<
					"' for '" << std::string(to_data->name(), to_data->name_size()) << "'" << std::endl;
			}
		}

		for(xml_node<>* data = to_data->first_node(); data; data = data->next_sibling()) {
			switch(data->type()) {
			case rapidxml::node_element: {
				if(auto const& factory = mFactories[std::string(data->name(), data->name_size())]) {
					auto w = factory();
					assert(w);

					buildRecursive(buildRecursive, to->add(std::move(w)), data);
				}
				else {
					throw exceptions::ParsingError("Unknown element type " + std::string(data->name(), data->name_size()), data->name(), text);
				}
			} continue;
			case rapidxml::node_cdata:
			case rapidxml::node_data: {
				std::string value = std::string(data->value(), data->value_size());
				bool success = to->setAttribute("content", value);
				if(!success) {
					std::cerr <<
						"Couldn't set content for " <<
						std::string(to_data->name(), to_data->name_size()) <<
						" to '" << value << "'" << std::endl;
				}
			} continue;
			default: continue;
			}
		}
	};

	xml_node<>* form_data = doc.first_node("form");
	if(!form_data) {
		throw exceptions::ParsingError("Expected a '<form>' element at root level");
	}
	buildRecursive(buildRecursive, this, form_data);

	return *this;
}

bool Form::setAttribute(std::string const& name, std::string const& value) {
	if(name == "source" || name == "src") {
		load(value);
		return true;
	}
	return Widget::setAttribute(name, value);
}

} // namespace wwidget
