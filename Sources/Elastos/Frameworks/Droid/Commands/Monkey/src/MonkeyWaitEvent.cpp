#include "MonkeyWaitEvent.h"
#include <unistd.h>
#define msleep(X) usleep(X * 1000)

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyWaitEvent::MonkeyWaitEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
{
}

MonkeyWaitEvent::MonkeyWaitEvent(
    /* [in] */ Int64 waitTime)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mWaitTime(waitTime)
{
}

ECode MonkeyWaitEvent::Init(
    /* [in] */ Int64 waitTime)
{
    mWaitTime = waitTime;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_THROTTLE);
    return NOERROR;
}

Int32 MonkeyWaitEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 1) {
        PFL_EX("Wait Event For %lld  milliseconds", mWaitTime)
    }

    msleep(mWaitTime);
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

