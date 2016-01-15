#ifndef __MONKEYTHROTTLEEVENT_H__
#define __MONKEYTHROTTLEEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyThrottleEvent : public MonkeyEvent
{
protected:
    MonkeyThrottleEvent();
    MonkeyThrottleEvent(
        /* [in] */ Int64 throttle);

    CARAPI Init(
        /* [in] */ Int64 throttle);

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

private:
    Int64 mThrottle;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYTHROTTLEEVENT_H__
