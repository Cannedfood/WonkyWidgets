#pragma once

#include "../Widget.hpp"

#include <functional>
#include <memory>
#include <typeinfo>
#include <unordered_map>

#include "../Error.hpp"

namespace widget {

class Form : public Widget {
public:
	using FactoryFn = std::function<std::unique_ptr<Widget>()>;

private:
	std::unordered_map<std::string, FactoryFn> mFactories;

public:
	Form();
	~Form();

	template<typename T>
	Form& factory();
	template<typename T, typename U, typename... ARGS>
	Form& factory();
	template<typename T, typename... ARGS>
	Form& factory(std::string const& name, ARGS&&... args);
	Form& factory(std::string    const& name, FactoryFn&& fn);
	Form& factory(std::type_info const& type, FactoryFn&& fn);

	Form& load(std::string const& path);
	Form& load(std::istream& stream);
	Form& parse(const char* text);
};

template<typename T>
Form& Form::factory() {
	return factory(typeid(T), []() {
		return std::unique_ptr<Widget>(new T);
	});
}

template<typename T, typename U, typename... ARGS>
Form& factory() {
	factory<T>();
	factory<U>();
	factory<ARGS...>();
}

template<typename T, typename... ARGS>
Form& Form::factory(std::string const& name, ARGS&&... args) {
	return factory(name, [=]() {
		return std::unique_ptr<Widget>(new T(std::forward<ARGS>(args)...));
	});
}

} // namespace widget
