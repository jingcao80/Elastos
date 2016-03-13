
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CMotionEventHelper, Singleton, IMotionEventHelper);

CAR_SINGLETON_IMPL(CMotionEventHelper);

ECode CMotionEventHelper::Obtain(
    /* [out] */ IMotionEvent** o)
{
    AutoPtr<MotionEvent> event = MotionEvent::Obtain();
    *o = (IMotionEvent*)event.Get();
    REFCOUNT_ADD(*o);
    return NOERROR;
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ ArrayOf<IPointerProperties*>* pointerProperties,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 buttonState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 source,
    /* [in] */ Int32 flags,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(
            downTime, eventTime, action, pointerCount,
            pointerProperties, pointerCoords,
            metaState, buttonState, xPrecision, yPrecision,
            deviceId, edgeFlags, source, flags, event);
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ ArrayOf<Int32>* pointerIds,
    /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 source,
    /* [in] */ Int32 flags,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(
            downTime, eventTime, action, pointerCount, pointerIds,
            pointerCoords, metaState, xPrecision, yPrecision,
            deviceId, edgeFlags, source, flags, event);
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(
            downTime, eventTime, action, x, y, pressure, size, metaState,
            xPrecision, yPrecision, deviceId, edgeFlags, event);
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size,
    /* [in] */ Int32 metaState,
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 edgeFlags,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(
            downTime, eventTime, action, pointerCount, x, y, pressure, size,
            metaState, xPrecision, yPrecision, deviceId, edgeFlags, event);
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 metaState,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(
            downTime, eventTime, action, x, y, metaState, event);
}

ECode CMotionEventHelper::Obtain(
    /* [in] */ IMotionEvent* o,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::Obtain(o, event);
}

ECode CMotionEventHelper::ObtainNoHistory(
    /* [in] */ IMotionEvent* o,
    /* [out] */ IMotionEvent** event)
{
    return MotionEvent::ObtainNoHistory(o, event);
}

ECode CMotionEventHelper::ActionToString(
    /* [in] */ Int32 action,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = MotionEvent::ActionToString(action);
    return NOERROR;
}

ECode CMotionEventHelper::AxisToString(
    /* [in] */ Int32 axis,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = MotionEvent::AxisToString(axis);
    return NOERROR;
}

ECode CMotionEventHelper::AxisFromString(
    /* [in] */ const String& symbolicName,
    /* [out] */ Int32* axis)
{
    VALIDATE_NOT_NULL(axis);

    return MotionEvent::AxisFromString(symbolicName, axis);
}

ECode CMotionEventHelper::ButtonStateToString(
    /* [in] */ Int32 buttonState,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = MotionEvent::ButtonStateToString(buttonState);
    return NOERROR;
}

ECode CMotionEventHelper::ToolTypeToString(
    /* [in] */ Int32 toolType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = MotionEvent::ToolTypeToString(toolType);
    return NOERROR;
}

ECode CMotionEventHelper::CreateFromParcelBody(
    /* [in] */ IParcel* in,
    /* [out] */ IMotionEvent** event)
{
    VALIDATE_NOT_NULL(event);

    return MotionEvent::CreateFromParcelBody(in, event);
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
