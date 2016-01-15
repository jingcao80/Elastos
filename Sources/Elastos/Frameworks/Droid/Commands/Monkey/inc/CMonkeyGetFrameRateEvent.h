
#ifndef __CMONKEYGETFRAMERATEEVENT_H__
#define __CMONKEYGETFRAMERATEEVENT_H__

#include "_CMonkeyGetFrameRateEvent.h"
#include "MonkeyGetFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyGetFrameRateEvent) , public MonkeyGetFrameRateEvent
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
        /* [in] */ const String& status,
        /* [in] */ const String& testCaseName);

    CARAPI constructor(
        /* [in] */ const String& status);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYGETFRAMERATEEVENT_H__
