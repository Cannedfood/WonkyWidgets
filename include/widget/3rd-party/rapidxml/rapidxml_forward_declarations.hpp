#pragma once

namespace rapidxml {

template<class Ch> class xml_node;
template<class Ch> class xml_attribute;
template<class Ch> class xml_document;

} // namespace rapidxml

namespace rojo {

enum SerializationMode {
	SERIALIZE_STATE,
	SERIALIZE_ATTRIBUTES,
	SERIALIZE_DEBUG_INFO
};

using xml_node      = rapidxml::xml_node<char>;
using xml_attribute = rapidxml::xml_attribute<char>;
using xml_document  = rapidxml::xml_document<char>;

} // namespace rojo
