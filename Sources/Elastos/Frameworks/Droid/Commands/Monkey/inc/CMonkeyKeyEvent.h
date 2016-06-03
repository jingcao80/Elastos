
#ifndef __CMONKEYKEYEVENT_H__
#define __CMONKEYKEYEVENT_H__

#include "_CMonkeyKeyEvent.h"
#include "MonkeyKeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyKeyEvent) , public MonkeyKeyEvent
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

    CARAPI GetKeyCode(
        /* [out] */ Int32 *keyCode);

    CARAPI GetAction(
        /* [out] */ Int32 *action);

    CARAPI GetDownTime(
        /* [out] */ Int64 *downTime);

    CARAPI GetEventTime(
        /* [out] */ Int64 *eventTime);

    CARAPI SetDownTime(
        /* [in] */ Int64 downTime);

    CARAPI SetEventTime(
        /* [in] */ Int64 eventTime);

    CARAPI constructor(
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 device,
        /* [in] */ Int32 scanCode);

    CARAPI constructor(
        /* [in] */ IKeyEvent *keyEvent);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYKEYEVENT_H__
