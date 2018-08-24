#pragma once

#include "../Widget.hpp"

#include <functional>
#include <memory>
#include <typeinfo>
#include <unordered_map>

#include "../Error.hpp"

namespace wwidget {

/// A widget which can load its children from a xml file.
///  You can register your own widgets by using the Form::factory functions, but
///  you have to call Form::addDefaultFactories if you want to add the default widgets then.
class Form : public Widget {
public:
	using FactoryFn = std::function<std::unique_ptr<Widget>()>;

private:
	std::unordered_map<std::string, FactoryFn> mFactories;

protected:
	void onDraw(Canvas&) override;
public:
	Form();
	Form(std::string const& path);
	Form(std::istream& stream);
	Form(Widget* addTo, std::string const& path);
	Form(Widget* addTo, std::istream& stream);
	~Form() noexcept;

	template<typename T>
	Form& factory();
	template<typename T, typename U, typename... ARGS>
	Form& factory();
	template<typename T>
	Form& factory(std::string    const& name);

	Form& factory(std::string    const& name, FactoryFn&& fn);
	Form& factory(std::type_info const& type, FactoryFn&& fn);

	Form& addDefaultFactories();

	Form& load(std::string const& path);
	Form& load(std::istream& stream);
	Form& parse(const char* text);

	bool setAttribute(std::string_view name, Attribute const& value) override;
};

// =============================================================
// == Inline implementation =============================================
// =============================================================

template<typename T>
Form& Form::factory() {
	return factory(typeid(T), []() {
		return std::unique_ptr<Widget>(new T);
	});
}

template<typename T, typename U, typename... ARGS>
Form& Form::factory() {
	factory<T>();
	factory<U>();
	factory<ARGS...>();
	return *this;
}

template<typename T>
Form& Form::factory(std::string const& name) {
	return factory(name, [&]() {
		return std::unique_ptr<Widget>(new T());
	});
}

} // namespace wwidget
