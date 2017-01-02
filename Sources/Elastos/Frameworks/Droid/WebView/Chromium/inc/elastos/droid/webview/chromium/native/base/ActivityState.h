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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_ACTIVITYSTATE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_ACTIVITYSTATE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * A set of states that represent the last state change of an Activity.
 */
class ActivityState
{
public:
    /**
     * Represents Activity#onCreate().
     */
    static const Int32 CREATED = 1;

    /**
     * Represents Activity#onStart().
     */
    static const Int32 STARTED = 2;

    /**
     * Represents Activity#onResume().
     */
    static const Int32 RESUMED = 3;

    /**
     * Represents Activity#onPause().
     */
    static const Int32 PAUSED = 4;

    /**
     * Represents Activity#onStop().
     */
    static const Int32 STOPPED = 5;

    /**
     * Represents Activity#onDestroy().  This is also used when the state of an Activity is unknown.
     */
    static const Int32 DESTROYED = 6;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_ACTIVITYSTATE_H__
