
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_EVENTLOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_EVENTLOG_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * A simple interface to Android's EventLog to be used by native code.
 */
//@JNINamespace("base::android")
class EventLog
{
public:
    static CARAPI_(void*) ElaEventLogCallback_Init();

    //@CalledByNative
    static CARAPI_(void) WriteEvent(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 value);
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_EVENTLOG_H__
