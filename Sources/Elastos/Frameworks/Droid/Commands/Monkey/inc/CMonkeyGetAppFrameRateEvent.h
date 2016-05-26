
#ifndef __CMONKEYGETAPPFRAMERATEEVENT_H__
#define __CMONKEYGETAPPFRAMERATEEVENT_H__

#include "_CMonkeyGetAppFrameRateEvent.h"
#include "MonkeyGetAppFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyGetAppFrameRateEvent) , public MonkeyGetAppFrameRateEvent
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetEventType(
        /* [out] */ Int32 *eventType);

    CARAPI IsThrottlable(
        /* [out] */ Boolean *isThrottlable);

    CARAPI InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *am,
        /* [in] */ Int32 verbose,
        /* [out] */ Int32 *result);

    CARAPI constructor(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName,
        /* [in] */ const String& testCaseName);

    CARAPI constructor(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName);

    CARAPI constructor(
        /* [in] */ const String& status);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYGETAPPFRAMERATEEVENT_H__
