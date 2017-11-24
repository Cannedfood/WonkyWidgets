#include "../../include/widget/debug/Marker.hpp"

#ifdef WIDGET_ULTRA_VERBOSE

namespace widget { namespace detail {
	int _WidgetMarkerType::indention = 0;
	int _WidgetMarkerEnable::enabled = 0;
}} // namespace widget::detail

#endif
