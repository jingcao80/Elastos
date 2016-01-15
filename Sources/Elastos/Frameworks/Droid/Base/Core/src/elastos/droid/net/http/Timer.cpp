
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Timer.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::SystemClock;

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

Timer::Timer()
    : mStart(0)
    , mLast(0)
{
}

ECode Timer::constructor()
{
    mStart = mLast = SystemClock::GetUptimeMillis();
    return NOERROR;
}

Timer::~Timer()
{}

ECode Timer::Mark(
    /* [in] */ const String& message) {
    Int64 now = SystemClock::GetUptimeMillis();
    if (HttpLog::LOGV) {
        HttpLog::V(message + String(" ") + StringUtils::ToString(now - mLast)
            + String(" total ") + StringUtils::ToString(now - mStart));
    }
    mLast = now;

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
