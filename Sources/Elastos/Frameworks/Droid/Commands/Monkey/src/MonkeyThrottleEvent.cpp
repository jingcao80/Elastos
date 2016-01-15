#include "MonkeyThrottleEvent.h"
#include <unistd.h>

#define msleep(X) usleep(X * 1000)

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyThrottleEvent::MonkeyThrottleEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mThrottle(0)
{}

MonkeyThrottleEvent::MonkeyThrottleEvent(
    /* [in] */ Int64 throttle)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mThrottle(throttle)
{}

CARAPI MonkeyThrottleEvent::Init(
    /* [in] */ Int64 throttle)
{
    mThrottle = throttle;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_THROTTLE);
    return NOERROR;
}

Int32 MonkeyThrottleEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        PFL_EX("Sleeping for %lld milliseconds", mThrottle)
    }

    msleep(mThrottle);
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

