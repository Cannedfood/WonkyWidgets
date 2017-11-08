#pragma once

#ifdef WIDGET_ULTRA_VERBOSE

#include <cstdio>
#include <typeinfo>
namespace widget { namespace detail {
	struct _WidgetMarkerType {
		static int indention;
		_WidgetMarkerType() { ++indention; }
		~_WidgetMarkerType() { --indention; }
	};
}} // namespace widget::detail

#ifdef __gnu_linux__
	#define WIDGET_M_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; printf("%*s[%p](%s) %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", this, typeid(*this).name(), __PRETTY_FUNCTION__);
#else
	#define WIDGET_M_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; printf("%*s[%p](%s) %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", this, typeid(*this).name(), __FUNCSIG__);
#endif

#else // defined(WIDGET_ULTRA_VERBOSE)

#define WIDGET_M_FN_MARKER

#endif // defined(WIDGET_ULTRA_VERBOSE)
