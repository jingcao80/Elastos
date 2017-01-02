//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

