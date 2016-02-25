
#include "elastos/droid/systemui/statusbar/policy/CDeadZone.h"
#include "../../R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CDeadZone::DebugFlashRunnable::DebugFlashRunnable(
    /* [in] */ CDeadZone* host)
    : mHost(host)
{
}

ECode CDeadZone::DebugFlashRunnable::Run()
{
    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = 1.f;
    (*params)[1] = 0.f;

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> anim;
    helper->OfFloat(THIS_PROBE(IInterface), String("flash"), params, (IObjectAnimator**)&anim);
    IViewPropertyAnimator::Probe(anim)->Start();
    return NOERROR;
}

const Boolean CDeadZone::CHATTY = TRUE;
CAR_INTERFACE_IMPL(CDeadZone, View, IDeadZone);
CDeadZone::CDeadZone()
    : mShouldFlash(FALSE)
    , mFlashFrac(0.0f)
    , mSizeMax(0)
    , mSizeMin(0)
    , mHold(0)
    , mDecay(0)
    , mVertical(FALSE)
    , mLastPokeTime(0)
{
    mDebugFlash = new DebugFlashRunnable(this);
}

ECode CDeadZone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CDeadZone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    View::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::DeadZone),
            ArraySize(R::styleable::DeadZone));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a));

    a->GetInt32(R::styleable::DeadZone_holdTime, 0, &mHold);
    a->GetInt32(R::styleable::DeadZone_decayTime, 0, &mDecay);

    a->GetDimensionPixelSize(R::styleable::DeadZone_minSize, 0, &mSizeMin);
    a->GetDimensionPixelSize(R::styleable::DeadZone_maxSize, 0, &mSizeMax);

    Int32 index;
    a->GetInt32(R::styleable::DeadZone_orientation, -1, &index);
    mVertical = (index == IDeadZone::VERTICAL);

    a->Recycle();

    if (DEBUG) {
       Slogger::V(TAG, "%p size=[%d-%d] hold=%d%s",
               this, mSizeMin, mSizeMax, mHold, (mVertical ? " vertical" : " horizontal"));
    }

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean val;
    res->GetBoolean(R::bool_::config_dead_zone_flash, &val);
    SetFlashOnTouchCapture(val);

    return NOERROR;
}

Float CDeadZone::Lerp(
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [in] */ Float f)
{
    return (b - a) * f + a;
}

Float CDeadZone::GetSize(
    /* [in] */ Int64 now)
{
    if (mSizeMax == 0)
        return 0;
    Int64 dt = (now - mLastPokeTime);
    if (dt > mHold + mDecay)
        return mSizeMin;
    if (dt < mHold)
        return mSizeMax;
    return (Int32) Lerp(mSizeMax, mSizeMin, (Float) (dt - mHold) / mDecay);
}

ECode CDeadZone::SetFlashOnTouchCapture(
    /* [in] */ Boolean dbg)
{
    mShouldFlash = dbg;
    mFlashFrac = 0.0f;
    PostInvalidate();
    return NOERROR;
}

ECode CDeadZone::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    event->GetAction(&action);

    if (DEBUG) {
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        String str;
        helper->ActionToString(action, &str);
        Slogger::V(TAG, "%p onTouch: %s", this, str.string());
    }

    if (action == IMotionEvent::ACTION_OUTSIDE) {
        Poke(event);
    }
    else if (action == IMotionEvent::ACTION_DOWN) {
        Float x, y;
        event->GetX(&x);
        event->GetY(&y);
        if (DEBUG) {
            Slogger::V(TAG, "%p ACTION_DOWN: %ld,%ld", this, x, y);
        }
        Int64 time;
        IInputEvent::Probe(event)->GetEventTime(&time);
        Int32 size = (Int32) GetSize(time);
        if ((mVertical && x < size) || y < size) {
            if (CHATTY) {
                Slogger::V(TAG, "consuming errant click: (%ld,%ld)", x, y);
            }
            if (mShouldFlash) {
                Boolean tmp = FALSE;
                Post(mDebugFlash, &tmp);
                PostInvalidate();
            }
            *result = TRUE; // ...but I eated it
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDeadZone::Poke(
    /* [in] */ IMotionEvent* event)
{
    IInputEvent::Probe(event)->GetEventTime(&mLastPokeTime);
    if (DEBUG) {
       Slogger::V(TAG, "poked! size=%ld", GetSize(mLastPokeTime));
    }
    if (mShouldFlash) {
        PostInvalidate();
    }
    return NOERROR;
}

ECode CDeadZone::SetFlash(
    /* [in] */ Float f)
{
    mFlashFrac = f;
    PostInvalidate();
    return NOERROR;
}

ECode CDeadZone::GetFlash(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mFlashFrac;
    return NOERROR;
}

void CDeadZone::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!mShouldFlash || mFlashFrac <= 0.0f) {
        return;
    }

    Int32 w,h;
    canvas->GetWidth(&w);
    canvas->GetHeight(&h);
    Int64 now = SystemClock::GetUptimeMillis();
    Int32 size = (Int32) GetSize(now);
    Boolean isNotEmpty;
    canvas->ClipRect(0, 0, mVertical ? size : w, mVertical ? h : size, &isNotEmpty);
    Float frac = /* DEBUG ? (mFlashFrac - 0.5f) + 0.5f :  */ mFlashFrac;
    canvas->DrawARGB((Int32) (frac * 0xFF), 0xDD, 0xEE, 0xAA);

    if (DEBUG && size > mSizeMin) {
        // crazy aggressive redrawing here, for debugging only
        PostInvalidateDelayed(100);
    }
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
