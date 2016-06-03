
#ifndef __CMONKEYINSTRUMENTATIONEVENT_H__
#define __CMONKEYINSTRUMENTATIONEVENT_H__

#include "_CMonkeyInstrumentationEvent.h"
#include "MonkeyInstrumentationEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyInstrumentationEvent) , public MonkeyInstrumentationEvent
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
        /* [in] */ const String& testCaseName,
        /* [in] */ const String& runnerName);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYINSTRUMENTATIONEVENT_H__
