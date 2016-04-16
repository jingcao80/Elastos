
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_WINDOWOPENDISPOSITION_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_WINDOWOPENDISPOSITION_H_

#include "elastos/droid/ext/frameworkext.h"

// package org.chromium.ui;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

class WindowOpenDisposition
{
public:
    static const Int32 UNKNOWN = 0;
    static const Int32 SUPPRESS_OPEN = 1;
    static const Int32 CURRENT_TAB = 2;
    static const Int32 SINGLETON_TAB = 3;
    static const Int32 NEW_FOREGROUND_TAB = 4;
    static const Int32 NEW_BACKGROUND_TAB = 5;
    static const Int32 NEW_POPUP = 6;
    static const Int32 NEW_WINDOW = 7;
    static const Int32 SAVE_TO_DISK = 8;
    static const Int32 OFF_THE_RECORD = 9;
    static const Int32 IGNORE_ACTION = 10;
    static const Int32 WINDOW_OPEN_DISPOSITION_LAST = 10;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_WINDOWOPENDISPOSITION_H_

