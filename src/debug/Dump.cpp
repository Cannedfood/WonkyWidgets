#include "../../include/widget/debug/Dump.hpp"

#include "../../include/widget/widget/List.hpp"
#include "../../include/widget/widget/Label.hpp"

#include "../../include/widget/Attribute.hpp"


namespace widget {

constexpr static const char kDumpListName[] = "89dca0c5587e8179_dbg_Dump";

static
std::unique_ptr<Widget> _CreateDumpRecursively(Widget& of, int indent) {
	auto dump = std::make_unique<List>();
	dump->name(kDumpListName);
	auto collector = StringAttributeCollector([&](std::string const& name, std::string const& value, bool is_default) {
		dump->add<Label>()->content(
			std::string(indent, ' ') + name + ": " + value + (is_default ? " [default]" : "")
		);
	});
	of.getAttributes(collector);
	of.eachChild([&](Widget* child) {
		if(child->name() != kDumpListName) { // Prevent recursing into dump
			dump->add(_CreateDumpRecursively(*child, indent + 1));
		}
	});
	return dump;
}

std::unique_ptr<Widget> CreateDump(Widget& of) {
	auto dump = _CreateDumpRecursively(of, 0);
	dump->setAttribute("scrollable", "true");
	return dump;
}

Widget* ShowDump (Widget& showIn, Widget& dumpOf) {
	while(auto* old = showIn.search(kDumpListName))
		old->remove();
	return showIn.add(CreateDump(dumpOf));
}

static
void _PrintDumpRecursive(std::ostream& to, Widget& dumpOf, int level) {
	// Print attributes
	{
		auto indent = [&]() { for(int i = 0; i < level; i++) to << "\t"; };

		auto collector = StringAttributeCollector([&](std::string const& name, std::string const& value, bool is_default) {
			indent();
			to << name << ": " << value;
			if(is_default)
				to << " [default]";
			to << std::endl;
		});
		dumpOf.getAttributes(collector);
	}

	// Print children
	level += 1;
	dumpOf.eachChild([&](Widget* child) {
			to << std::endl;
			_PrintDumpRecursive(to, *child, level);
	});
}

void PrintDump(std::ostream& to, Widget& dumpOf) {
	_PrintDumpRecursive(to, dumpOf, 0);
}

} // namespace widget
