
#include "CMonkeyTouchEvent.h"


namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyTouchEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType);
    *eventType = MonkeyTouchEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyTouchEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable);
    *isThrottlable = MonkeyTouchEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyTouchEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result);
    *result = MonkeyTouchEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyTouchEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y)
{

    MonkeyTouchEvent::AddPointer(id, x, y);
    return NOERROR;
}

ECode CMonkeyTouchEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size)
{
    MonkeyTouchEvent::AddPointer(id, x, y, pressure, size);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetIntermediateNote(
    /* [in] */ Boolean b)
{
    MonkeyTouchEvent::SetIntermediateNote(b);
    return NOERROR;
}

ECode CMonkeyTouchEvent::GetIntermediateNote(
    /* [out] */ Boolean *b)
{
    VALIDATE_NOT_NULL(b);
    *b = MonkeyTouchEvent::GetIntermediateNote();
    return NOERROR;
}

ECode CMonkeyTouchEvent::GetAction(
    /* [out] */ Int32 *action)
{
    VALIDATE_NOT_NULL(action);
    *action = MonkeyTouchEvent::GetAction();
    return NOERROR;
}

ECode CMonkeyTouchEvent::GetDownTime(
    /* [out] */ Int64 *downtime)
{
    VALIDATE_NOT_NULL(downtime);
    *downtime = MonkeyTouchEvent::GetDownTime();
    return NOERROR;
}

ECode CMonkeyTouchEvent::GetEventTime(
    /* [out] */ Int64 *eventtime)
{
    VALIDATE_NOT_NULL(eventtime);
    *eventtime = MonkeyTouchEvent::GetEventTime();
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetDownTime(
    /* [in] */ Int64 downtime)
{
    MonkeyTouchEvent::SetDownTime(downtime);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetEventTime(
    /* [in] */ Int64 eventtime)
{
    MonkeyTouchEvent::SetEventTime(eventtime);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetMetaState(
    /* [in] */ Int32 metaState)
{
    MonkeyTouchEvent::SetMetaState(metaState);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetPrecision(
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision)
{
    MonkeyTouchEvent::SetPrecision(xPrecision, yPrecision);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetDeviceId(
    /* [in] */ Int32 deviceId)
{
    MonkeyTouchEvent::SetDeviceId(deviceId);
    return NOERROR;
}

ECode CMonkeyTouchEvent::SetEdgeFlags(
    /* [in] */ Int32 edgeFlags)
{
    MonkeyTouchEvent::SetEdgeFlags(edgeFlags);
    return NOERROR;
}

ECode CMonkeyTouchEvent::constructor(
    /* [in] */ Int32 action)
{
    MonkeyTouchEvent::Init(action);
    return NOERROR;
}

PInterface CMonkeyTouchEvent::Probe(
    /* [in] */ REIID riid)
{
    return _CMonkeyTouchEvent::Probe(riid);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos