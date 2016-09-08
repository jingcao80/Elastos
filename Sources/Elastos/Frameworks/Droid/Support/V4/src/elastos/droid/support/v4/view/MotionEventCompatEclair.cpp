
#include "elastos/droid/support/v4/view/MotionEventCompatEclair.h"

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

ECode MotionEventCompatEclair::FindPointerIndex(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerId,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return event->FindPointerIndex(pointerId, index);
}

ECode MotionEventCompatEclair::GetPointerId(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    return event->GetPointerId(pointerIndex, id);
}

ECode MotionEventCompatEclair::GetX(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x)
    return event->GetX(pointerIndex, x);
}

ECode MotionEventCompatEclair::GetY(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y)
    return event->GetY(pointerIndex, y);
}

ECode MotionEventCompatEclair::GetPointerCount(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return event->GetPointerCount(count);
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos
