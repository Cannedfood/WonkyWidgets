#pragma once

#include <string>
#include <type_traits>

namespace widget {

class AttributeCollectorInterface {
public:
	virtual void operator()(std::string const& name, bool  b, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float f, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float x, float y, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float x, float y, float w, float h, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, const char* s, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, std::string const& s, bool is_default = false) = 0;
};

namespace detail {

	template<typename Callback>
	class StringAttributeCollectorObject : public AttributeCollectorInterface {
		Callback mCallback;
	public:
		StringAttributeCollectorObject(Callback&& c) : mCallback(std::move(c)) {}
		StringAttributeCollectorObject(Callback const& c) : mCallback(c) {}

		void operator()(std::string const& name, bool  b, bool is_default) override  {
			(*this)(name, b ? "true" : "false", is_default);
		}
		void operator()(std::string const& name, float f, bool is_default) override {
			(*this)(name, std::to_string(f), is_default);
		}
		void operator()(std::string const& name, float x, float y, bool is_default) override {
			(*this)(name, std::to_string(x) + " " + std::to_string(y), is_default);
		}
		void operator()(std::string const& name, float x, float y, float w, float h, bool is_default) override {
			(*this)(name, std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(w) + " " + std::to_string(h), is_default);
		}
		void operator()(std::string const& name, const char* s, bool is_default) override {
			mCallback(name, s, is_default);
		}
		void operator()(std::string const& name, std::string const& s, bool is_default) override {
			mCallback(name, s, is_default);
		}
	};

} // namespace detail

template<typename C>
auto StringAttributeCollector(C&& c) {
	using namespace std;
	return detail::StringAttributeCollectorObject<remove_reference_t<remove_cv_t<C>>>(forward<C>(c));
}

} // namespace widget
