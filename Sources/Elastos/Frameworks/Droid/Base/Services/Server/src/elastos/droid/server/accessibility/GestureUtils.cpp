
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/accessibility/GestureUtils.h"
#include <elastos/core/Math.h>
#include <elastos/droid/utility/MathUtils.h>

using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

Boolean GestureUtils::IsTap(
    /* [in] */ IMotionEvent* down,
    /* [in] */ IMotionEvent* up,
    /* [in] */ Int32 tapTimeSlop,
    /* [in] */ Int32 tapDistanceSlop,
    /* [in] */ Int32 actionIndex)
{
    return EventsWithinTimeAndDistanceSlop(down, up, tapTimeSlop,
            tapDistanceSlop, actionIndex);
}

Boolean GestureUtils::IsMultiTap(
    /* [in] */ IMotionEvent* firstUp,
    /* [in] */ IMotionEvent* secondUp,
    /* [in] */ Int32 multiTapTimeSlop,
    /* [in] */ Int32 multiTapDistanceSlop,
    /* [in] */ Int32 actionIndex)
{
    return EventsWithinTimeAndDistanceSlop(firstUp, secondUp, multiTapTimeSlop,
            multiTapDistanceSlop, actionIndex);
}

Boolean GestureUtils::EventsWithinTimeAndDistanceSlop(
    /* [in] */ IMotionEvent* first,
    /* [in] */ IMotionEvent* second,
    /* [in] */ Int32 timeout,
    /* [in] */ Int32 distance,
    /* [in] */ Int32 actionIndex)
{
    if (IsTimedOut(first, second, timeout)) {
        return FALSE;
    }
    Double deltaMove = ComputeDistance(first, second, actionIndex);
    if (deltaMove >= distance) {
        return FALSE;
    }
    return TRUE;
}

Double GestureUtils::ComputeDistance(
    /* [in] */ IMotionEvent* first,
    /* [in] */ IMotionEvent* second,
    /* [in] */ Int32 pointerIndex)
{
    assert(first != NULL && second != NULL);

    Float x1, y1, x2, y2;
    first->GetX(pointerIndex, &x1);
    first->GetY(pointerIndex, &y1);
    second->GetX(pointerIndex, &x2);
    second->GetY(pointerIndex, &y2);

    return (Double)MathUtils::Dist(x1, y1, x2, y2);
}

Boolean GestureUtils::IsTimedOut(
    /* [in] */ IMotionEvent* firstUp,
    /* [in] */ IMotionEvent* secondUp,
    /* [in] */ Int32 timeout)
{
    assert(firstUp != NULL && secondUp != NULL);

    Int64 eventTime1, eventTime2;
    IInputEvent::Probe(secondUp)->GetEventTime(&eventTime1);
    IInputEvent::Probe(firstUp)->GetEventTime(&eventTime2);
    Int64 deltaTime = eventTime1 - eventTime2;
    return (deltaTime >= timeout);
}

Boolean GestureUtils::IsSamePointerContext(
    /* [in] */ IMotionEvent* first,
    /* [in] */ IMotionEvent* second)
{
    assert(first != NULL && second != NULL);

    Int32 idBits1, idBits2;
    first->GetPointerIdBits(&idBits1);
    second->GetPointerIdBits(&idBits2);
    if (idBits1 == idBits2) {
        Int32 index1, index2;
        first->GetActionIndex(&index1);
        second->GetActionIndex(&index2);
        Int32 id1, id2;
        first->GetPointerId(index1, &id1);
        second->GetPointerId(index2, &id2);
        return id1 == id2;
    }
    return FALSE;
}

Boolean GestureUtils::IsDraggingGesture(
    /* [in] */ Float firstPtrDownX,
    /* [in] */ Float firstPtrDownY,
    /* [in] */ Float secondPtrDownX,
    /* [in] */ Float secondPtrDownY,
    /* [in] */ Float firstPtrX,
    /* [in] */ Float firstPtrY,
    /* [in] */ Float secondPtrX,
    /* [in] */ Float secondPtrY,
    /* [in] */ Float maxDraggingAngleCos)
{
    // Check if the pointers are moving in the same direction.
    Float firstDeltaX = firstPtrX - firstPtrDownX;
    Float firstDeltaY = firstPtrY - firstPtrDownY;

    if (firstDeltaX == 0 && firstDeltaY == 0) {
        return TRUE;
    }

    using Elastos::Core::Math;

    Float firstMagnitude =
        (Float)Math::Sqrt(firstDeltaX * firstDeltaX + firstDeltaY * firstDeltaY);
    Float firstXNormalized =
        (firstMagnitude > 0) ? firstDeltaX / firstMagnitude : firstDeltaX;
    Float firstYNormalized =
        (firstMagnitude > 0) ? firstDeltaY / firstMagnitude : firstDeltaY;

    Float secondDeltaX = secondPtrX - secondPtrDownX;
    Float secondDeltaY = secondPtrY - secondPtrDownY;

    if (secondDeltaX == 0 && secondDeltaY == 0) {
        return TRUE;
    }

    Float secondMagnitude =
        (Float)Math::Sqrt(secondDeltaX * secondDeltaX + secondDeltaY * secondDeltaY);
    Float secondXNormalized =
        (secondMagnitude > 0) ? secondDeltaX / secondMagnitude : secondDeltaX;
    Float secondYNormalized =
        (secondMagnitude > 0) ? secondDeltaY / secondMagnitude : secondDeltaY;

    Float angleCos =
        firstXNormalized * secondXNormalized + firstYNormalized * secondYNormalized;

    if (angleCos < maxDraggingAngleCos) {
        return FALSE;
    }

    return TRUE;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
