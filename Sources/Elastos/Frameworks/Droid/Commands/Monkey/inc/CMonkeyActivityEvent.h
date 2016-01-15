
#ifndef __CMONKEYACTIVITYEVENT_H__
#define __CMONKEYACTIVITYEVENT_H__

#include "_CMonkeyActivityEvent.h"
#include "MonkeyActivityEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyActivityEvent) , public MonkeyActivityEvent
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
        /* [in] */ IComponentName *app);

    CARAPI constructor(
        /* [in] */ IComponentName *app,
        /* [in] */ Int64 arg);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYACTIVITYEVENT_H__