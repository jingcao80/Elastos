
#ifndef __CMONKEYNOOPEVENT_H__
#define __CMONKEYNOOPEVENT_H__

#include "_CMonkeyNoopEvent.h"
#include "MonkeyNoopEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyNoopEvent) , public MonkeyNoopEvent
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

    CARAPI constructor();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYNOOPEVENT_H__
