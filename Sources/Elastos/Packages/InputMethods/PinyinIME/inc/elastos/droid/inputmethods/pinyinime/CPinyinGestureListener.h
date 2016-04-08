
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINGESTURELISTENER_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINGESTURELISTENER_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CPinyinGestureListener.h"
#include "elastos/droid/view/GestureDetector.h"

using Elastos::Droid::Inputmethods::PinyinIME::IPinyinIME;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CarClass(CPinyinGestureListener), public Elastos::Droid::View::GestureDetector::SimpleOnGestureListener
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CPinyinGestureListener();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* Object,
        /* [out] */ InterfaceID* iID);

    CARAPI constructor(
        /* [in] */ Boolean reponseGestures,
        /* [in] */ IPinyinIME* ime);

    CARAPI OnDown(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    CARAPI OnScroll(
        /* [in] */ IMotionEvent* e1,
        /* [in] */ IMotionEvent* e2,
        /* [in] */ Float distanceX,
        /* [in] */ Float distanceY,
        /* [out] */ Boolean* res);

    CARAPI OnFling(
        /* [in] */ IMotionEvent* me1,
        /* [in] */ IMotionEvent* me2,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* res);

    CARAPI_(void) OnDirectionGesture(
        /* [in] */ Int32 gravity);

    CARAPI OnDoubleTap(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    CARAPI OnDoubleTapEvent(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    CARAPI OnSingleTapConfirmed(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    CARAPI OnShowPress(
        /* [in] */ IMotionEvent* e);

    CARAPI OnSingleTapUp(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    CARAPI OnLongPress(
        /* [in] */ IMotionEvent* e);

private:
    /**
     * When user presses and drags, the minimum x-distance to make a
     * response to the drag event.
     */
    static const Int32 MIN_X_FOR_DRAG;

    /**
     * When user presses and drags, the minimum y-distance to make a
     * response to the drag event.
     */
    static const Int32 MIN_Y_FOR_DRAG;

    /**
     * Velocity threshold for a screen-move gesture. If the minimum
     * x-velocity is less than it, no gesture.
     */
    static const Float VELOCITY_THRESHOLD_X1;

    /**
     * Velocity threshold for a screen-move gesture. If the maximum
     * x-velocity is less than it, no gesture.
     */
    static const Float VELOCITY_THRESHOLD_X2;

    /**
     * Velocity threshold for a screen-move gesture. If the minimum
     * y-velocity is less than it, no gesture.
     */
    static const Float VELOCITY_THRESHOLD_Y1;

    /**
     * Velocity threshold for a screen-move gesture. If the maximum
     * y-velocity is less than it, no gesture.
     */
    static const Float VELOCITY_THRESHOLD_Y2;

    /** If it FALSE, we will not response detected gestures. */
    Boolean mReponseGestures;

    /** The minimum X velocity observed in the gesture. */
    Float mMinVelocityX;

    /** The minimum Y velocity observed in the gesture. */
    Float mMinVelocityY;

    /** The first down time for the series of touch events for an action. */
    Int64 mTimeDown;

    /** The last time when onScroll() is called. */
    Int64 mTimeLastOnScroll;

    /** This flag used to indicate that this gesture is not a gesture. */
    Boolean mNotGesture;

    /** This flag used to indicate that this gesture has been recognized. */
    Boolean mGestureRecognized;

    AutoPtr<IPinyinIME> mIme;
};


} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINGESTURELISTENER_H__
