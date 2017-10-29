#include "Form.hpp"

#include "../3rd-party/rapidxml/rapidxml.hpp"
#include "../3rd-party/rapidxml/rapidxml_utils.hpp"

#ifdef __GNUC__
#	include <cxxabi.h>
#endif

namespace widget {

Form::Form() {}
Form::~Form() {}

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
	constexpr int options = parse_comment_nodes | parse_non_destructive;

	xml_document<> doc;
	try {
		doc.parse<options>(const_cast<char*>(text)); // We use the non-destructive mode, const_cast is safe
	}
	catch(rapidxml::parse_error const& e) {
		throw exceptions::ParsingError(e.what(), e.where<char>(), text);
	}

	auto buildRecursive = [=](auto& buildRecursive, Widget* to, xml_node<>* to_data) -> void {
		for(xml_attribute<>* attrib = to_data->first_attribute(); attrib; attrib = attrib->next_attribute()) {
			// TODO: set attributes of to from to_data
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
			default: continue;
			}
		}
	};

	xml_node<>* form_data = doc.first_node("form");
	if(!form_data) {
		throw exceptions::ParsingError("Expected a '<form>' element in root of the document");
	}
	buildRecursive(buildRecursive, this, form_data);

	return *this;
}

} // namespace widget
