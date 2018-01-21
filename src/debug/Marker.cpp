#include "../../include/wwidget/debug/Marker.hpp"

#ifdef WIDGET_ULTRA_VERBOSE

namespace wwidget { namespace detail {
	int _WidgetMarkerType::indention = 0;
	int _WidgetMarkerEnable::enabled = 0;
}} // namespace wwwidget::detail

#endif
