
#include "elastos/droid/phone/SmallerHitTargetTouchListener.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const String SmallerHitTargetTouchListener::TAG("SmallerHitTargetTouchListener");

const Int32 SmallerHitTargetTouchListener::HIT_TARGET_EDGE_IGNORE_DP_X = 30;
const Int32 SmallerHitTargetTouchListener::HIT_TARGET_EDGE_IGNORE_DP_Y = 10;
const Int32 SmallerHitTargetTouchListener::HIT_TARGET_MIN_SIZE_DP_X = HIT_TARGET_EDGE_IGNORE_DP_X * 3;
const Int32 SmallerHitTargetTouchListener::HIT_TARGET_MIN_SIZE_DP_Y = HIT_TARGET_EDGE_IGNORE_DP_Y * 3;

CAR_INTERFACE_IMPL(SmallerHitTargetTouchListener, Object, IViewOnTouchListener)

ECode SmallerHitTargetTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // if (DBG) log("SmallerHitTargetTouchListener: " + v + ", event " + event);
    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        // Note that event.getX() and event.getY() are already
        // translated into the View's coordinates.  (In other words,
        // "0,0" is a touch on the upper-left-most corner of the view.)
        Float x;
        event->GetX(&x);
        Int32 touchX = (Int32)x;

        Float y;
        event->GetY(&y);
        Int32 touchY = (Int32)y;

        Int32 viewWidth;
        v->GetWidth(&viewWidth);
        Int32 viewHeight;
        v->GetHeight(&viewHeight);

        AutoPtr<IResources> resources;
        v->GetResources((IResources**)&resources);
        AutoPtr<IDisplayMetrics> displayMetrics;
        resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
        Float pixelDensity;
        displayMetrics->GetDensity(&pixelDensity);

        Int32 targetMinSizeX = (Int32)(HIT_TARGET_MIN_SIZE_DP_X * pixelDensity);
        Int32 targetMinSizeY = (Int32)(HIT_TARGET_MIN_SIZE_DP_Y * pixelDensity);

        Int32 edgeIgnoreX = (Int32)(HIT_TARGET_EDGE_IGNORE_DP_X * pixelDensity);
        Int32 edgeIgnoreY = (Int32)(HIT_TARGET_EDGE_IGNORE_DP_Y * pixelDensity);

        // If we are dealing with smaller buttons where the dead zone defined by
        // HIT_TARGET_EDGE_IGNORE_DP_[X|Y] is too large.
        if (viewWidth < targetMinSizeX || viewHeight < targetMinSizeY) {
            // This really should not happen given our two use cases (as of this writing)
            // in the call edge button and secondary calling card. However, we leave
            // this is as a precautionary measure.
            Logger::W(TAG, "onTouch: view is too small for SmallerHitTargetTouchListener");
            edgeIgnoreX = 0;
            edgeIgnoreY = 0;
        }

        const Int32 minTouchX = edgeIgnoreX;
        const Int32 maxTouchX = viewWidth - edgeIgnoreX;
        const Int32 minTouchY = edgeIgnoreY;
        const Int32 maxTouchY = viewHeight - edgeIgnoreY;

        if (touchX < minTouchX || touchX > maxTouchX ||
                touchY < minTouchY || touchY > maxTouchY) {
            // Missed!
            // if (DBG) log("  -> MISSED!");
            mDownEventHit = FALSE;
            *result = TRUE;  // Consume this event; don't let the button see it
            return NOERROR;
        }
        else {
            // Hit!
            // if (DBG) log("  -> HIT!");
            mDownEventHit = TRUE;
            *result = FALSE;  // Let this event through to the actual button
            return NOERROR;
        }
    }
    else {
        // This is a MOVE, UP or CANCEL event.
        //
        // We only do the "smaller hit target" check on DOWN events.
        // For the subsequent MOVE/UP/CANCEL events, we let them
        // through to the actual button IFF the previous DOWN event
        // got through to the actual button (i.e. it was a "hit".)
        *result = !mDownEventHit;
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos