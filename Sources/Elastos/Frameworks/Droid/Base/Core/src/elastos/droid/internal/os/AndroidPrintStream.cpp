
#include "elastos/droid/internal/os/AndroidPrintStream.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

AndroidPrintStream::AndroidPrintStream(
    /* [in] */ Int32 priority,
    /* [in] */ const String& tag)
    : mPriority(priority)
    , mTag(tag)
{
    if (tag == NULL) {
        // throw new NullPointerException("tag");
        assert(0);
    }
}

void AndroidPrintStream::Log(
    /* [in] */ const String& line)
{
    Logger::Println(mPriority, mTag, line);
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
