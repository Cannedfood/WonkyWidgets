#pragma once

#include <string>
#include <type_traits>

namespace wwidget {

struct Color;

class AttributeCollectorInterface {
public:
	[[nodiscard]]
	virtual bool startSection(std::string const& name) = 0;
	virtual void endSection() = 0;
	virtual void operator()(std::string const& name, bool  b, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float f, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float x, float y, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, float x, float y, float w, float h, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, Color const& c, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, const char* s, bool is_default = false) = 0;
	virtual void operator()(std::string const& name, std::string const& s, bool is_default = false) = 0;
};

class StringAttributeCollector : public AttributeCollectorInterface {
public:
	bool startSection(std::string const& name) override;
	void endSection() override;

	void operator()(std::string const& name, bool  b, bool is_default = false) override;
	void operator()(std::string const& name, float f, bool is_default = false) override;
	void operator()(std::string const& name, float x, float y, bool is_default = false) override;
	void operator()(std::string const& name, float x, float y, float w, float h, bool is_default = false) override;
	void operator()(std::string const& name, Color const& c, bool is_default = false) override;
	void operator()(std::string const& name, const char* s, bool is_default = false) override;

	void operator()(std::string const& name, std::string const& s, bool is_default = false) override = 0;
};

namespace detail {

	template<typename Callback>
	class StringAttributeCollectorCallback : public StringAttributeCollector {
		Callback mCallback;
	public:
		StringAttributeCollectorCallback(Callback&& c) : mCallback(std::move(c)) {}
		StringAttributeCollectorCallback(Callback const& c) : mCallback(c) {}

		void operator()(std::string const& name, std::string const& s, bool is_default) override {
			mCallback(name, s, is_default);
		}
	};

} // namespace detail

template<typename C>
auto MakeStringAttributeCollector(C&& c) {
	using namespace std;
	return detail::StringAttributeCollectorCallback<remove_reference_t<remove_cv_t<C>>>(forward<C>(c));
}

} // namespace wwidget
