#pragma once

#include <string>
#include <type_traits>

#include "Attributes.hpp"

namespace wwidget {

struct Color;

class AttributeCollectorInterface {
public:
	[[nodiscard]]
	virtual bool startSection(std::string_view name) = 0;
	virtual void endSection() = 0;
	virtual void operator()(std::string_view name, bool             b,     bool             default_val) = 0;
	virtual void operator()(std::string_view name, float            f,     float            default_val) = 0;
	virtual void operator()(std::string_view name, Flow             f,     Flow             default_val) = 0;
	virtual void operator()(std::string_view name, Point            point, Point            default_val) = 0;
	virtual void operator()(std::string_view name, Size             size,  Size             default_val) = 0;
	virtual void operator()(std::string_view name, Offset           off,   Offset           default_val) = 0;
	virtual void operator()(std::string_view name, Rect  const&     rect,  Rect  const&     default_val) = 0;
	virtual void operator()(std::string_view name, Color const&     c,     Color const&     default_val) = 0;
	virtual void operator()(std::string_view name, Alignment        a,     Alignment        default_val) = 0;
	virtual void operator()(std::string_view name, Padding          a,     Padding          default_val) = 0;
	virtual void operator()(std::string_view name, std::string_view s,     std::string_view default_val) = 0;

	inline void operator()(std::string_view name, const char* s, const char* default_val) {
		(*this)(name, std::string_view(s), std::string_view(default_val));
	}
};

class StringAttributeCollector : public AttributeCollectorInterface {
public:
	bool startSection(std::string_view name) override;
	void endSection() override;

	virtual void operator()(std::string_view name, bool             b,     bool             default_val) override;
	virtual void operator()(std::string_view name, float            f,     float            default_val) override;
	virtual void operator()(std::string_view name, Flow             f,     Flow             default_val) override;
	virtual void operator()(std::string_view name, Point            point, Point            default_val) override;
	virtual void operator()(std::string_view name, Size             size,  Size             default_val) override;
	virtual void operator()(std::string_view name, Offset           off,   Offset           default_val) override;
	virtual void operator()(std::string_view name, Rect  const&     rect,  Rect  const&     default_val) override;
	virtual void operator()(std::string_view name, Color const&     c,     Color const&     default_val) override;
	virtual void operator()(std::string_view name, Alignment        a,     Alignment        default_val) override;
	virtual void operator()(std::string_view name, Padding          a,     Padding          default_val) override;
	virtual void operator()(std::string_view name, std::string_view s,     std::string_view default_val) override = 0;
};

namespace detail {

	template<typename Callback>
	class StringAttributeCollectorCallback final : public StringAttributeCollector {
		Callback mCallback;
	public:
		StringAttributeCollectorCallback(Callback&& c) : mCallback(std::move(c)) {}
		StringAttributeCollectorCallback(Callback const& c) : mCallback(c) {}

		void operator()(std::string_view name, std::string_view s, std::string_view default_val) override {
			mCallback(name, s, default_val);
		}
	};

} // namespace detail

template<typename C>
auto MakeStringAttributeCollector(C&& c) {
	using namespace std;
	return detail::StringAttributeCollectorCallback<remove_reference_t<remove_cv_t<C>>>(forward<C>(c));
}

} // namespace wwidget
