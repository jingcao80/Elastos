
#include "elastos/droid/webkit/native/base/EventLog.h"
#include "elastos/droid/webkit/native/base/api/EventLog_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * A simple interface to Android's EventLog to be used by native code.
 */
//@JNINamespace("base::android")

void EventLog::WriteEvent(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 value)
{
    assert(0);
//    Elastos::util::EventLog::WriteEvent(tag, value);
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
