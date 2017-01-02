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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Contains all of the command line switches that are specific to the base/
 * portion of Chromium on Android.
 */
class BaseSwitches
{
public:
    // Block onCreate() of Chrome until a Java debugger is attached.
    static const String WAIT_FOR_JAVA_DEBUGGER;

    // Overrides low-end device detection, disabling low-end device optimizations.
    static const String DISABLE_LOW_END_DEVICE_MODE;

    // Overrides low-end device detection, enabling low-end device optimizations.
    static const String ENABLE_LOW_END_DEVICE_MODE;

    // Adds additional thread idle time information into the trace event output.
    static const String ENABLE_IDLE_TRACING;

private:
    // Prevent instantiation.
    BaseSwitches() {}
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__
