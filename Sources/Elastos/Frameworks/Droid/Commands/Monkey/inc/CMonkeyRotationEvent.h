
#ifndef __CMONKEYROTATIONEVENT_H__
#define __CMONKEYROTATIONEVENT_H__

#include "_CMonkeyRotationEvent.h"
#include "MonkeyRotationEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyRotationEvent) , public MonkeyRotationEvent
{
public:
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
        /* [in] */ Int32 degree,
        /* [in] */ Boolean persist);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYROTATIONEVENT_H__
