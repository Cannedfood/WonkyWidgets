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
		struct _WidgetMarkerEnable {
			static int enabled;
			_WidgetMarkerEnable() { ++enabled; }
			~_WidgetMarkerEnable() { --enabled; }
		};
	}} // namespace widget::detail

	#ifdef __gnu_linux__
		#define WIDGET_M_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; if(::widget::detail::_WidgetMarkerEnable::enabled > 0) printf("%*s[%p](%s) %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", this, typeid(*this).name(), __PRETTY_FUNCTION__);
		#define WIDGET_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; if(::widget::detail::_WidgetMarkerEnable::enabled > 0) printf("%*s %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", __PRETTY_FUNCTION__);
	#else
		#define WIDGET_M_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; printf("%*s[%p](%s) %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", this, typeid(*this).name(), __FUNCSIG__);
		#define WIDGET_FN_MARKER ::widget::detail::_WidgetMarkerType _widget_marker; if(::widget::detail::_WidgetMarkerEnable::enabled > 0) printf("%*s %s\n", ::widget::detail::_WidgetMarkerType::indention - 1, "", __PRETTY_FUNCTION__);
	#endif

	#define WIDGET_ENABLE_MARKERS ::widget::detail::_WidgetMarkerEnable _widget_marker_enable;

#else // defined(WIDGET_ULTRA_VERBOSE)
	#define WIDGET_M_FN_MARKER
	#define WIDGET_FN_MARKER
	#define WIDGET_ENABLE_MARKERS
#endif // defined(WIDGET_ULTRA_VERBOSE)
