#pragma once

#include <string_view>

namespace wwidget {

namespace UnicodeConstants {

constexpr static inline const char

// Widget specific

CheckboxEmpty[]   = u8"☐",
CheckboxChecked[] = u8"☒",

// Button general
CrossCancel[] = u8"🗙",
Trashbin[]    = u8"🗑",

// Files and folders
Folder[]     = u8"🗀",
FolderOpen[] = u8"🗁",

Document[]          = u8"🗎",
DocumentText[]      = u8"🖹",
DocumentImage[]     = u8"🖻",
DocumentTextImage[] = u8"🖺",
DocumentEmpty[]     = u8"🗋";

} // namespace unicode_constants

} // namespace wwidget