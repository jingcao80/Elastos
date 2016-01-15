
#ifndef __CMONKEYPOWEREVENT_H__
#define __CMONKEYPOWEREVENT_H__

#include "_CMonkeyPowerEvent.h"
#include "MonkeyPowerEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyPowerEvent) , public MonkeyPowerEvent
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
        /* [in] */ const String& powerLogTag,
        /* [in] */ const String& powerTestResult);

    CARAPI constructor(
        /* [in] */ const String& powerLogTag);

    CARAPI constructor();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYPOWEREVENT_H__
