
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
