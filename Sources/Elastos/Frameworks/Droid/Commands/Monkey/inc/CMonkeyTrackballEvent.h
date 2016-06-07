
#ifndef __CMONKEYTRACKBALLEVENT_H__
#define __CMONKEYTRACKBALLEVENT_H__

#include "_CMonkeyTrackballEvent.h"
#include "MonkeyTrackballEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyTrackballEvent) , public MonkeyTrackballEvent
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

    CARAPI AddPointer(
        /* [in] */ Int32 id,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI AddPointer(
        /* [in] */ Int32 id,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float pressure,
        /* [in] */ Float size);

    CARAPI SetIntermediateNote(
        /* [in] */ Boolean b);

    CARAPI GetIntermediateNote(
        /* [out] */ Boolean *b);

    CARAPI GetAction(
        /* [out] */ Int32 *action);

    CARAPI GetDownTime(
        /* [out] */ Int64 *downtime);

    CARAPI GetEventTime(
        /* [out] */ Int64 *eventtime);

    CARAPI SetDownTime(
        /* [in] */ Int64 downtime);

    CARAPI SetEventTime(
        /* [in] */ Int64 eventtime);

    CARAPI SetMetaState(
        /* [in] */ Int32 metaState);

    CARAPI SetPrecision(
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision);

    CARAPI SetDeviceId(
        /* [in] */ Int32 deviceId);

    CARAPI SetEdgeFlags(
        /* [in] */ Int32 edgeFlags);

    CARAPI constructor(
        /* [in] */ Int32 action);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


#endif // __CMONKEYTRACKBALLEVENT_H__
