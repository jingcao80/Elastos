
#ifndef __CMONKEYTHROTTLEEVENT_H__
#define __CMONKEYTHROTTLEEVENT_H__

#include "_CMonkeyThrottleEvent.h"
#include "MonkeyThrottleEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyThrottleEvent) , public MonkeyThrottleEvent
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetEventType(
        /* [out] */ Int32 *eventType);

    CARAPI IsThrottlable(
        /* [out] */ Boolean *isThrottlable);

    CARAPI InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose,
        /* [out] */ Int32 *result);

    CARAPI constructor(
        /* [in] */ Int64 throttle);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYTHROTTLEEVENT_H__
