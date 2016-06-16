#ifndef  __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__
#define  __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * OnTouchListener used to shrink the "hit target" of some onscreen buttons.
 *
 * We do this for a few specific buttons which are vulnerable to
 * "false touches" because either (1) they're near the edge of the
 * screen and might be unintentionally touched while holding the
 * device in your hand, (2) they're in the upper corners and might
 * be touched by the user's ear before the prox sensor has a chance to
 * kick in, or (3) they are close to other buttons.
 */
class SmallerHitTargetTouchListener
    : public Object
    , public IViewOnTouchListener
{
public:
    TO_STRING_IMPL("SmallerHitTargetTouchListener")

    CAR_INTERFACE_DECL()

    /**
     * Called when a touch event is dispatched to a view. This allows listeners to
     * get a chance to respond before the target view.
     *
     * @return True if the listener has consumed the event, false otherwise.
     *         (In other words, we return true when the touch is *outside*
     *         the "smaller hit target", which will prevent the actual
     *         button from handling these events.)
     */
    //@Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

private:
    static const String TAG = "SmallerHitTargetTouchListener";

    /**
     * Edge dimensions where a touch does not register an action (in DIP).
     */
    static const Int32 HIT_TARGET_EDGE_IGNORE_DP_X = 30;
    static const Int32 HIT_TARGET_EDGE_IGNORE_DP_Y = 10;
    static const Int32 HIT_TARGET_MIN_SIZE_DP_X = HIT_TARGET_EDGE_IGNORE_DP_X * 3;
    static const Int32 HIT_TARGET_MIN_SIZE_DP_Y = HIT_TARGET_EDGE_IGNORE_DP_Y * 3;

    // True if the most recent DOWN event was a "hit".
    Boolean mDownEventHit;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__