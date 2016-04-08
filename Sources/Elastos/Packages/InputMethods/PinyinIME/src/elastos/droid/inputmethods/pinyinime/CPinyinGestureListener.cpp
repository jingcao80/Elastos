
#include "CPinyinGestureListener.h"
#include <elastos/core/Math.h>

using Elastos::Core::Math;
using Elastos::Droid::Inputmethods::PinyinIME::ICandidatesContainer;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

const Int32 CPinyinGestureListener::MIN_X_FOR_DRAG = 60;
const Int32 CPinyinGestureListener::MIN_Y_FOR_DRAG = 40;
const Float CPinyinGestureListener::VELOCITY_THRESHOLD_X1 = 0.3f;
const Float CPinyinGestureListener::VELOCITY_THRESHOLD_X2 = 0.7f;
const Float CPinyinGestureListener::VELOCITY_THRESHOLD_Y1 = 0.2f;
const Float CPinyinGestureListener::VELOCITY_THRESHOLD_Y2 = 0.45f;

CAR_OBJECT_IMPL(CPinyinGestureListener);

CPinyinGestureListener::CPinyinGestureListener()
    : mReponseGestures(FALSE)
    , mMinVelocityX(Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mMinVelocityY(Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mTimeDown(0)
    , mTimeLastOnScroll(0)
    , mNotGesture(FALSE)
    , mGestureRecognized(FALSE)
{}

PInterface CPinyinGestureListener::Probe(
    /* [in] */ REIID riid)
{
    return _CPinyinGestureListener::Probe(riid);
}

ECode CPinyinGestureListener::GetInterfaceID(
    /* [in] */ IInterface* Object,
    /* [out] */ InterfaceID* iID)
{
    VALIDATE_NOT_NULL(iID);
    return _CPinyinGestureListener::GetInterfaceID(Object, iID);
}

UInt32 CPinyinGestureListener::AddRef()
{
    return _CPinyinGestureListener::AddRef();
}

UInt32 CPinyinGestureListener::Release()
{
    return _CPinyinGestureListener::Release();
}

ECode CPinyinGestureListener::constructor(
    /* [in] */ Boolean reponseGestures,
    /* [in] */ IPinyinIME* ime)
{
    mReponseGestures = reponseGestures;
    mIme = ime;
    return NOERROR;
}

ECode CPinyinGestureListener::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    assert(e != NULL);
    mMinVelocityX = Elastos::Core::Math::INT32_MAX_VALUE;
    mMinVelocityY = Elastos::Core::Math::INT32_MAX_VALUE;
    e->GetEventTime(&mTimeDown);
    mTimeLastOnScroll = mTimeDown;
    mNotGesture = FALSE;
    mGestureRecognized = FALSE;
    *res = FALSE;
    return NOERROR;
}

ECode CPinyinGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mNotGesture) {
        *res = FALSE;
        return NOERROR;
    }

    if (mGestureRecognized) {
        *res = TRUE;
        return NOERROR;
    }

    assert(e1 != NULL && e2 != NULL);
    Float x1 = 0.f, y1 = 0.f, x2 = 0.f, y2 = 0.f;
    e1->GetX(&x1);
    e1->GetY(&y1);
    e2->GetX(&x2);
    e2->GetY(&y2);

    if (Elastos::Core::Math::Abs(x1 - x2) < MIN_X_FOR_DRAG && Elastos::Core::Math::Abs(y1 - y2) < MIN_Y_FOR_DRAG) {
        *res = FALSE;
        return NOERROR;
    }

    Int64 timeNow = 0;
    e2->GetEventTime(&timeNow);
    Int64 spanTotal = timeNow - mTimeDown;
    Int64 spanThis = timeNow - mTimeLastOnScroll;
    if (0 == spanTotal) spanTotal = 1;
    if (0 == spanThis) spanThis = 1;

    Float vXTotal = (x2 - x1) / spanTotal;
    Float vYTotal = (y2 - y1) / spanTotal;

    // The distances are from the current point to the previous one.
    Float vXThis = -distanceX / spanThis;
    Float vYThis = -distanceY / spanThis;

    Float kX = vXTotal * vXThis;
    Float kY = vYTotal * vYThis;
    Float k1 = kX + kY;
    Float k2 = Elastos::Core::Math::Abs(kX) + Elastos::Core::Math::Abs(kY);

    if (k1 / k2 < 0.8) {
        mNotGesture = TRUE;
        *res = FALSE;
        return NOERROR;
    }

    Float absVXTotal = Elastos::Core::Math::Abs(vXTotal);
    Float absVYTotal = Elastos::Core::Math::Abs(vYTotal);
    if (absVXTotal < mMinVelocityX) {
        mMinVelocityX = absVXTotal;
    }
    if (absVYTotal < mMinVelocityY) {
        mMinVelocityY = absVYTotal;
    }

    if (mMinVelocityX < VELOCITY_THRESHOLD_X1
            && mMinVelocityY < VELOCITY_THRESHOLD_Y1) {
        mNotGesture = TRUE;
        *res = FALSE;
        return NOERROR;
    }

    if (vXTotal > VELOCITY_THRESHOLD_X2
            && absVYTotal < VELOCITY_THRESHOLD_Y2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::RIGHT);
        mGestureRecognized = TRUE;
    } else if (vXTotal < -VELOCITY_THRESHOLD_X2
            && absVYTotal < VELOCITY_THRESHOLD_Y2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::LEFT);
        mGestureRecognized = TRUE;
    } else if (vYTotal > VELOCITY_THRESHOLD_Y2
            && absVXTotal < VELOCITY_THRESHOLD_X2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::BOTTOM);
        mGestureRecognized = TRUE;
    } else if (vYTotal < -VELOCITY_THRESHOLD_Y2
            && absVXTotal < VELOCITY_THRESHOLD_X2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::TOP);
        mGestureRecognized = TRUE;
    }

    mTimeLastOnScroll = timeNow;
    *res = mGestureRecognized;
    return NOERROR;
}

ECode CPinyinGestureListener::OnFling(
    /* [in] */ IMotionEvent* me1,
    /* [in] */ IMotionEvent* me2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mGestureRecognized;
    return NOERROR;
}

void CPinyinGestureListener::OnDirectionGesture(
    /* [in] */ Int32 gravity)
{
    if (IGravity::NO_GRAVITY == gravity) {
        return;
    }

    if (IGravity::LEFT == gravity || IGravity::RIGHT == gravity) {
        Boolean shown = FALSE;
        AutoPtr<ICandidatesContainer> container;
        mIme->GetCandidatesContainer((ICandidatesContainer**)&container);
        assert(container != NULL);
        if (container->IsShown(&shown), shown) {
            Boolean result;
            if (IGravity::LEFT == gravity) {
                container->PageForward(TRUE, TRUE, &result);
            } else {
                container->PageBackward(TRUE, TRUE, &result);
            }
            return;
        }
    }
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
