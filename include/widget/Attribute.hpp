#pragma once

#include <string>
#include <type_traits>

namespace widget {

class AttributeCollectorInterface {
public:
	virtual void operator()(std::string const& name, bool  b) = 0;
	virtual void operator()(std::string const& name, float f) = 0;
	virtual void operator()(std::string const& name, float x, float y) = 0;
	virtual void operator()(std::string const& name, float x, float y, float w, float h) = 0;
	virtual void operator()(std::string const& name, const char* s) = 0;
	virtual void operator()(std::string const& name, std::string const& s) = 0;
};

namespace detail {

	template<typename Callback>
	class StringAttributeCollectorObject : public AttributeCollectorInterface {
		Callback mCallback;
	public:
		StringAttributeCollectorObject(Callback&& c) : mCallback(std::move(c)) {}
		StringAttributeCollectorObject(Callback const& c) : mCallback(c) {}

		void operator()(std::string const& name, bool  b) override  {
			(*this)(name, b ? "true" : "false");
		}
		void operator()(std::string const& name, float f) override {
			(*this)(name, std::to_string(f));
		}
		void operator()(std::string const& name, float x, float y) override {
			(*this)(name, std::to_string(x) + " " + std::to_string(y));
		}
		void operator()(std::string const& name, float x, float y, float w, float h) override {
			(*this)(name, std::to_string(x) + " " + std::to_string(y));
		}
		void operator()(std::string const& name, const char* s) override {
			mCallback(name, s);
		}
		void operator()(std::string const& name, std::string const& s) override {
			mCallback(name, s);
		}
	};

} // namespace detail

template<typename C>
auto StringAttributeCollector(C&& c) {
	using namespace std;
	return detail::StringAttributeCollectorObject<remove_reference_t<remove_cv_t<C>>>(forward<C>(c));
}

} // namespace widget
