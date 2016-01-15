#include "elastos/droid/systemui/statusbar/policy/DeadZone.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/animation/CObjectAnimator.h"

using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


DeadZone::DebugFlashRunnable::DebugFlashRunnable(
    /* [in] */ DeadZone* host)
    : mHost(host)
{
}

ECode DeadZone::DebugFlashRunnable::Run()
{
    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = 1.f;
    (*params)[1] = 0.f;

    AutoPtr<IObjectAnimator> anim = CObjectAnimator::OfFloat(THIS_PROBE(IInterface), String("flash"), params);
    anim->Start();
    return NOERROR;
}

const Boolean DeadZone::CHATTY;

DeadZone::DeadZone()
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

DeadZone::DeadZone(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
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
    Init(context, attrs);
}

DeadZone::DeadZone(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
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
    Init(context, attrs, defStyle);
}

ECode DeadZone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Init(context, attrs, 0);
    return NOERROR;
}

ECode DeadZone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    View::Init(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(SystemUIR::styleable::DeadZone),
            ArraySize(SystemUIR::styleable::DeadZone));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a));

    a->GetInt32(SystemUIR::styleable::DeadZone_holdTime, 0, &mHold);
    a->GetInt32(SystemUIR::styleable::DeadZone_decayTime, 0, &mDecay);

    a->GetDimensionPixelSize(SystemUIR::styleable::DeadZone_minSize, 0, &mSizeMin);
    a->GetDimensionPixelSize(SystemUIR::styleable::DeadZone_maxSize, 0, &mSizeMax);

    Int32 index;
    a->GetInt32(SystemUIR::styleable::DeadZone_orientation, -1, &index);
    mVertical = (index == IDeadZone::VERTICAL);

    a->Recycle();

//    if (DEBUG)
//        Slog.v(TAG, this + " size=[" + mSizeMin + "-" + mSizeMax + "] hold=" + mHold
//                + (mVertical ? " vertical" : " horizontal"));

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean val;
    res->GetBoolean(SystemUIR::bool_::config_dead_zone_flash, &val);
    SetFlashOnTouchCapture(val);

    return NOERROR;
}

Float DeadZone::Lerp(
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [in] */ Float f)
{
    return (b - a) * f + a;
}

Float DeadZone::GetSize(
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

ECode DeadZone::SetFlashOnTouchCapture(
    /* [in] */ Boolean dbg)
{
    mShouldFlash = dbg;
    mFlashFrac = 0.0f;
    PostInvalidate();
    return NOERROR;
}

Boolean DeadZone::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
//    if (DEBUG) {
//        Slog.v(TAG, this + " onTouch: " + MotionEvent.actionToString(event.getAction()));
//    }

    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_OUTSIDE) {
        Poke(event);
    } else if (action == IMotionEvent::ACTION_DOWN) {
//        if (DEBUG) {
//            Slog.v(TAG, this + " ACTION_DOWN: " + event.getX() + "," + event.getY());
//        }
        Int64 time;
        event->GetEventTime(&time);
        Int32 size = (Int32) GetSize(time);
        Float x, y;
        event->GetX(&x);
        event->GetY(&y);
        if ((mVertical && x < size) || y < size) {
//            if (CHATTY) {
//                Slog.v(TAG, "consuming errant click: (" + event.getX() + "," + event.getY() + ")");
//            }
            if (mShouldFlash) {
                Post(mDebugFlash);
                PostInvalidate();
            }
            return TRUE; // ...but I eated it
        }
    }
    return FALSE;
}

ECode DeadZone::Poke(
    /* [in] */ IMotionEvent* event)
{
    assert(event);
    event->GetEventTime(&mLastPokeTime);
//    if (DEBUG)
//        Slog.v(TAG, "poked! size=" + getSize(mLastPokeTime));
    PostInvalidate();
    return NOERROR;
}

ECode DeadZone::SetFlash(
    /* [in] */ Float f)
{
    mFlashFrac = f;
    PostInvalidate();
    return NOERROR;
}

Float DeadZone::GetFlash()
{
    return mFlashFrac;
}

void DeadZone::OnDraw(
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

//    if (DEBUG && size > mSizeMin) {
//        // crazy aggressive redrawing here, for debugging only
//        PostInvalidateDelayed(100);
//    }
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
