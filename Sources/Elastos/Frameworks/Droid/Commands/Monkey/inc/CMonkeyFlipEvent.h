
#ifndef __CMONKEYFLIPEVENT_H__
#define __CMONKEYFLIPEVENT_H__

#include "_CMonkeyFlipEvent.h"
#include "MonkeyFlipEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyFlipEvent) , public MonkeyFlipEvent
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
        /* [in] */ Boolean isKeyboardOpen);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYFLIPEVENT_H__
