
#include "elastos/droid/webkit/webview/chromium/native/base/EventLog.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/EventLog_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

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

void EventLog::WriteEvent(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 value)
{
    Logger::E("EventLog::WriteEvent", "not implemented");
    assert(0);
//    Elastos::util::EventLog::WriteEvent(tag, value);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
