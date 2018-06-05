#pragma once

#include <string>
#include <type_traits>

namespace wwidget {

struct Color;

class AttributeCollectorInterface {
public:
	[[nodiscard]]
	virtual bool startSection(std::string_view name) = 0;
	virtual void endSection() = 0;
	virtual void operator()(std::string_view name, bool  b, bool is_default = false) = 0;
	virtual void operator()(std::string_view name, float f, bool is_default = false) = 0;
	virtual void operator()(std::string_view name, float x, float y, bool is_default = false) = 0;
	virtual void operator()(std::string_view name, float x, float y, float w, float h, bool is_default = false) = 0;
	virtual void operator()(std::string_view name, Color const& c, bool is_default = false) = 0;
	virtual void operator()(std::string_view name, std::string_view s, bool is_default = false) = 0;

	inline void operator()(std::string_view name, const char* s, bool is_default = false) {
		(*this)(name, std::string_view(s), is_default);
	}
};

class StringAttributeCollector : public AttributeCollectorInterface {
public:
	bool startSection(std::string_view name) override;
	void endSection() override;

	void operator()(std::string_view name, bool  b, bool is_default = false) override;
	void operator()(std::string_view name, float f, bool is_default = false) override;
	void operator()(std::string_view name, float x, float y, bool is_default = false) override;
	void operator()(std::string_view name, float x, float y, float w, float h, bool is_default = false) override;
	void operator()(std::string_view name, Color const& c, bool is_default = false) override;
	void operator()(std::string_view name, std::string_view s, bool is_default = false) override = 0;
};

namespace detail {

	template<typename Callback>
	class StringAttributeCollectorCallback : public StringAttributeCollector {
		Callback mCallback;
	public:
		StringAttributeCollectorCallback(Callback&& c) : mCallback(std::move(c)) {}
		StringAttributeCollectorCallback(Callback const& c) : mCallback(c) {}

		void operator()(std::string_view name, std::string_view s, bool is_default) override {
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
