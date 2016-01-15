#include "elastos/droid/systemui/UniverseBackground.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/Choreographer.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"

using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::Choreographer;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::App::CActivityManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const String UniverseBackground::TAG("UniverseBackground");
const Boolean UniverseBackground::SPEW;
const Boolean UniverseBackground::CHATTY;
const Int32 UniverseBackground::STATE_CLOSED;
const Int32 UniverseBackground::STATE_OPENING;
const Int32 UniverseBackground::STATE_OPEN;

UniverseBackground::AnimationCallback::AnimationCallback(
    /* [in] */ UniverseBackground* host)
{
    mHost = host;
}

ECode UniverseBackground::AnimationCallback::Run()
{
    Int64 frameTimeNanos;
    mHost->mChoreographer->GetFrameTimeNanos(&frameTimeNanos);
    return mHost->DoAnimation(frameTimeNanos);
}

UniverseBackground::UniverseBackground()
    : mSelfExpandVelocityPx(0)
    , mSelfCollapseVelocityPx(0)
    , mFlingExpandMinVelocityPx(0)
    , mFlingCollapseMinVelocityPx(0)
    , mCollapseMinDisplayFraction(0)
    , mExpandMinDisplayFraction(0)
    , mFlingGestureMaxXVelocityPx(0)
    , mExpandAccelPx(0)
    , mCollapseAccelPx(0)
    , mState(STATE_CLOSED)
    , mDragStartX(0)
    , mDragStartY(0)
    , mAverageX(0)
    , mAverageY(0)
    , mExpanded(FALSE)
    , mExpandedVisible(FALSE)
    , mTracking(FALSE)
    , mAnimating(FALSE)
    , mClosing(FALSE)
    , mAnimY(0)
    , mAnimVel(0)
    , mAnimAccel(0)
    , mAnimLastTimeNanos(0)
    , mAnimatingReveal(FALSE)
    , mYDelta(0)
{
    mPositionTmp = ArrayOf<Int32>::Alloc(2);
    mTmpFloats = ArrayOf<Float>::Alloc(9);
}

UniverseBackground::UniverseBackground(
    /* [in] */ IContext* context)
    : FrameLayout(context)
    , mSelfExpandVelocityPx(0)
    , mSelfCollapseVelocityPx(0)
    , mFlingExpandMinVelocityPx(0)
    , mFlingCollapseMinVelocityPx(0)
    , mCollapseMinDisplayFraction(0)
    , mExpandMinDisplayFraction(0)
    , mFlingGestureMaxXVelocityPx(0)
    , mExpandAccelPx(0)
    , mCollapseAccelPx(0)
    , mState(STATE_CLOSED)
    , mDragStartX(0)
    , mDragStartY(0)
    , mAverageX(0)
    , mAverageY(0)
    , mExpanded(FALSE)
    , mExpandedVisible(FALSE)
    , mTracking(FALSE)
    , mAnimating(FALSE)
    , mClosing(FALSE)
    , mAnimY(0)
    , mAnimVel(0)
    , mAnimAccel(0)
    , mAnimLastTimeNanos(0)
    , mAnimatingReveal(FALSE)
    , mYDelta(0)
{
    mPositionTmp = ArrayOf<Int32>::Alloc(2);
    mTmpFloats = ArrayOf<Float>::Alloc(9);

    InitImpl(context);
}

ECode UniverseBackground::Init(
    /* [in] */ IContext* context)
{
    FrameLayout::Init(context);
    return InitImpl(context);
}

ECode UniverseBackground::InitImpl(
    /* [in] */ IContext* context)
{
    SetBackgroundColor(0xff000000);
    mSession = CWindowManagerGlobal::GetWindowSession();
    View::Inflate(context, SystemUIR::layout::universe, NULL, (IView**)&mContent);
    AddView(mContent);
    // mContent.findViewById(R.id.close).setOnClickListener(new View.OnClickListener() {
    //     @Override public void onClick(View v) {
    //         animateCollapse();
    //     }
    // });
    mContent->FindViewById(R::id::bottom, (IView**)&mBottomAnchor);
    mChoreographer = Choreographer::GetInstance();
    LoadDimens();
    return NOERROR;
}

AutoPtr<IWindowManagerLayoutParams> UniverseBackground::GetWindowManagerLayoutParams()
{
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND,
                0
                | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
                | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IPixelFormat::OPAQUE, (IWindowManagerLayoutParams**)&lp);
    // this will allow the window to run in an overlay on devices that support this
    if (CActivityManager::IsHighEndGfx()) {
        Int32 flags;
        lp->GetFlags(&flags);
        flags |= IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
        lp->SetFlags(flags);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(String("UniverseBackground"), (ICharSequence**)&cs);
    lp->SetTitle(cs);
    lp->SetWindowAnimations(0);
    return lp;
}

ECode UniverseBackground::AnimateCollapse()
{
    return AnimateCollapse(1.0f);
}

ECode UniverseBackground::AnimateCollapse(
    /* [in] */ Float velocityMultiplier)
{
    if (SPEW) {
        // Slog.d(TAG, "animateCollapse(): mExpanded=" + mExpanded
        //         + " mExpandedVisible=" + mExpandedVisible
        //         + " mExpanded=" + mExpanded
        //         + " mAnimating=" + mAnimating
        //         + " mAnimY=" + mAnimY
        //         + " mAnimVel=" + mAnimVel);
    }

    mState = STATE_CLOSED;
    if (!mExpandedVisible) {
        return NOERROR;
    }

    Int32 y;
    if (mAnimating) {
        y = (Int32)mAnimY;
    } else {
        y = GetExpandedViewMaxHeight()-1;
    }
    // Let the fling think that we're open so it goes in the right direction
    // and doesn't try to re-open the windowshade.
    mExpanded = TRUE;
    PrepareTracking(y, FALSE);
    return PerformFling(y, -mSelfCollapseVelocityPx*velocityMultiplier, TRUE);
}

ECode UniverseBackground::ConsumeEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 mask;
    event->GetActionMasked(&mask);
    if (mState == STATE_CLOSED) {
        if (mask == IMotionEvent::ACTION_POINTER_DOWN) {
            // Second finger down, time to start opening!
            ComputeAveragePos(event);
            mDragStartX = mAverageX;
            mDragStartY = mAverageY;
            mYDelta = 0;
            mUniverseTransform->Clear();
            SendUniverseTransform();
            SetVisibility(IView::VISIBLE);
            mState = STATE_OPENING;
            PrepareTracking((Int32)mDragStartY, TRUE);
            mVelocityTracker->Clear();
            TrackMovement(event);
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }

    if (mState == STATE_OPENING) {
        if (mask == IMotionEvent::ACTION_UP
                || mask == IMotionEvent::ACTION_CANCEL) {
            mVelocityTracker->ComputeCurrentVelocity(1000);
            ComputeAveragePos(event);

            Float yVel;
            mVelocityTracker->GetYVelocity(&yVel);
            Boolean negative = yVel < 0;

            Float xVel;
            mVelocityTracker->GetXVelocity(&xVel);
            if (xVel < 0) {
                xVel = -xVel;
            }
            if (xVel > mFlingGestureMaxXVelocityPx) {
                xVel = mFlingGestureMaxXVelocityPx; // limit how much we care about the x axis
            }

            Float vel = Elastos::Core::Math::Hypot(yVel, xVel);
            if (negative) {
                vel = -vel;
            }

            if (CHATTY) {
                // Slog.d(TAG, String.format("gesture: vraw=(%f,%f) vnorm=(%f,%f) vlinear=%f",
                //     mVelocityTracker->GetXVelocity(),
                //     mVelocityTracker->GetYVelocity(),
                //     xVel, yVel,
                //     vel));
            }

            PerformFling((Int32)mAverageY, vel, FALSE);
            mState = STATE_OPEN;
            *result = TRUE;
            return NOERROR;
        }

        ComputeAveragePos(event);
        mYDelta = (Int32)(mAverageY - mDragStartY);
        if (mYDelta > GetExpandedViewMaxHeight()) {
            mYDelta = GetExpandedViewMaxHeight();
        }
        UpdateUniverseScale();
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode UniverseBackground::ResetLastAnimTime()
{
    // mAnimLastTimeNanos = System.nanoTime();
    // if (SPEW) {
    //     Throwable t = new Throwable();
    //     t.fillInStackTrace();
    //     Slog.d(TAG, "resetting last anim time=" + mAnimLastTimeNanos, t);
    // }
    return E_NOT_IMPLEMENTED;
}

ECode UniverseBackground::DoAnimation(
    /* [in] */ Int64 frameTimeNanos)
{
    if (mAnimating) {
        // if (SPEW) Slog.d(TAG, "doAnimation dt=" + (frameTimeNanos - mAnimLastTimeNanos));
        // if (SPEW) Slog.d(TAG, "doAnimation before mAnimY=" + mAnimY);
        IncrementAnim(frameTimeNanos);
        if (SPEW) {
            // Slog.d(TAG, "doAnimation after  mAnimY=" + mAnimY);
        }

        if (mAnimY >= GetExpandedViewMaxHeight()-1 && !mClosing) {
            // if (SPEW) Slog.d(TAG, "Animation completed to expanded state.");
            mAnimating = FALSE;
            mYDelta = GetExpandedViewMaxHeight();
            UpdateUniverseScale();
            mExpanded = TRUE;
            mState = STATE_OPEN;
            return NOERROR;
        }

        if (mAnimY <= 0 && mClosing) {
            // if (SPEW) Slog.d(TAG, "Animation completed to collapsed state.");
            mAnimating = FALSE;
            mYDelta = 0;
            UpdateUniverseScale();
            mExpanded = FALSE;
            mState = STATE_CLOSED;
            return NOERROR;
        }

        mYDelta = (Int32)mAnimY;
        UpdateUniverseScale();
        mChoreographer->PostCallback(IChoreographer::CALLBACK_ANIMATION,
                mAnimationCallback, NULL);
    }
    return NOERROR;
}

ECode UniverseBackground::StopTracking()
{
        mTracking = FALSE;
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
        return NOERROR;
}

ECode UniverseBackground::IncrementAnim(
    /* [in] */ Int64 frameTimeNanos)
{
    Int64 deltaNanos = Elastos::Core::Math::Max(frameTimeNanos - mAnimLastTimeNanos, (Int64)0);
    Float t = deltaNanos * 0.000000001f;                  // ns -> s
    Float y = mAnimY;
    Float v = mAnimVel;                                   // px/s
    Float a = mAnimAccel;                                 // px/s/s
    mAnimY = y + (v*t) + (0.5f*a*t*t);                          // px
    mAnimVel = v + (a*t);                                       // px/s
    mAnimLastTimeNanos = frameTimeNanos;                        // ns
    //Slog.d(TAG, "y=" + y + " v=" + v + " a=" + a + " t=" + t + " mAnimY=" + mAnimY
    //        + " mAnimAccel=" + mAnimAccel);
    return NOERROR;
}

ECode UniverseBackground::PrepareTracking(
    /* [in] */ Int32 y,
    /* [in] */ Boolean opening)
{
    if (CHATTY) {
        // Slog.d(TAG, "panel: beginning to track the user's touch, y=" + y + " opening=" + opening);
    }

    mTracking = TRUE;
    mVelocityTracker = VelocityTracker::Obtain();
    if (opening) {
        mAnimAccel = mExpandAccelPx;
        mAnimVel = mFlingExpandMinVelocityPx;
        mAnimY = y;
        mAnimating = TRUE;
        mAnimatingReveal = TRUE;
        ResetLastAnimTime();
        mExpandedVisible = TRUE;
    }
    if (mAnimating) {
        mAnimating = FALSE;
        mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_ANIMATION,
                mAnimationCallback, NULL);
    }
    return NOERROR;
}

ECode UniverseBackground::PerformFling(
    /* [in] */ Int32 y,
    /* [in] */ Float vel,
    /* [in] */ Boolean always)
{
    if (CHATTY) {
        // Slog.d(TAG, "panel: will fling, y=" + y + " vel=" + vel);
    }

    mAnimatingReveal = FALSE;

    mAnimY = y;
    mAnimVel = vel;

    //Slog.d(TAG, "starting with mAnimY=" + mAnimY + " mAnimVel=" + mAnimVel);

    if (mExpanded) {
        if (!always && (
                vel > mFlingCollapseMinVelocityPx
                || (y > (GetExpandedViewMaxHeight() * (1.0 - mCollapseMinDisplayFraction)) &&
                    vel > -mFlingExpandMinVelocityPx))) {
            // We are expanded, but they didn't move sufficiently to cause
            // us to retract.  Animate back to the expanded position.
            mAnimAccel = mExpandAccelPx;
            if (vel < 0) {
                mAnimVel = 0;
            }
        }
        else {
            // We are expanded and are now going to animate away.
            mAnimAccel = -mCollapseAccelPx;
            if (vel > 0) {
                mAnimVel = 0;
            }
        }
    } else {
        if (always || (
                vel > mFlingExpandMinVelocityPx
                || (y > (GetExpandedViewMaxHeight() * (1.0 - mExpandMinDisplayFraction)) &&
                    vel > -mFlingCollapseMinVelocityPx))) {
            // We are collapsed, and they moved enough to allow us to
            // expand.  Animate in the notifications.
            mAnimAccel = mExpandAccelPx;
            if (vel < 0) {
                mAnimVel = 0;
            }
        }
        else {
            // We are collapsed, but they didn't move sufficiently to cause
            // us to retract.  Animate back to the collapsed position.
            mAnimAccel = -mCollapseAccelPx;
            if (vel > 0) {
                mAnimVel = 0;
            }
        }
    }
    //Slog.d(TAG, "mAnimY=" + mAnimY + " mAnimVel=" + mAnimVel
    //        + " mAnimAccel=" + mAnimAccel);

    ResetLastAnimTime();
    mAnimating = true;
    mClosing = mAnimAccel < 0;
    mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_ANIMATION,
            mAnimationCallback, NULL);
    mChoreographer->PostCallback(IChoreographer::CALLBACK_ANIMATION,
            mAnimationCallback, NULL);

    StopTracking();
    return NOERROR;
}

void UniverseBackground::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    LoadDimens();
}

void UniverseBackground::LoadDimens()
{
    AutoPtr<IResources> res;
    GetContext()->GetResources((IResources**)&res);
    res->GetDimension(SystemUIR::dimen::self_expand_velocity, &mSelfExpandVelocityPx);
    res->GetDimension(SystemUIR::dimen::self_collapse_velocity, &mSelfCollapseVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_expand_min_velocity, &mFlingExpandMinVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_collapse_min_velocity, &mFlingCollapseMinVelocityPx);

    res->GetFraction(SystemUIR::dimen::collapse_min_display_fraction, 1, 1, &mCollapseMinDisplayFraction);
    res->GetFraction(SystemUIR::dimen::expand_min_display_fraction, 1, 1, &mExpandMinDisplayFraction);

    res->GetDimension(SystemUIR::dimen::expand_accel, &mExpandAccelPx);
    res->GetDimension(SystemUIR::dimen::collapse_accel, &mCollapseAccelPx);

    res->GetDimension(SystemUIR::dimen::fling_gesture_max_x_velocity, &mFlingGestureMaxXVelocityPx);
}

void UniverseBackground::ComputeAveragePos(
    /* [in] */ IMotionEvent* event)
{
    Int32 num;
    event->GetPointerCount(&num);
    Float x = 0, y = 0;
    for (Int32 i = 0; i < num; i++) {
        Float temp;
        event->GetX(i, &temp);
        x += temp;
        event->GetY(i, &temp);
        y += temp;
    }
    mAverageX = x / num;
    mAverageY = y / num;
}

void UniverseBackground::SendUniverseTransform()
{
    if (GetWindowToken() != NULL) {
        AutoPtr<IMatrix> matrix;
        mUniverseTransform->GetMatrix((IMatrix**)&matrix);
        matrix->GetValues(mTmpFloats);
        // try {
        Float alpha;
        mUniverseTransform->GetAlpha(&alpha);
        mSession->SetUniverseTransform(GetWindowToken(), alpha,
                (*mTmpFloats)[IMatrix::MTRANS_X], (*mTmpFloats)[IMatrix::MTRANS_Y],
                (*mTmpFloats)[IMatrix::MSCALE_X], (*mTmpFloats)[IMatrix::MSKEW_Y],
                (*mTmpFloats)[IMatrix::MSKEW_X], (*mTmpFloats)[IMatrix::MSCALE_Y]);
        // } catch (RemoteException e) {
        // }
    }
}

Int32 UniverseBackground::GetExpandedViewMaxHeight()
{
    Int32 top;
    mBottomAnchor->GetTop(&top);
    return top;
}

void UniverseBackground::UpdateUniverseScale()
{
    if (mYDelta > 0) {
        Int32 w = GetWidth();
        Int32 h = GetHeight();
        Float scale = (h-mYDelta+.5f) / (Float)h;
        AutoPtr<IMatrix> matrix;
        mUniverseTransform->GetMatrix((IMatrix**)&matrix);
        matrix->SetScale(scale, scale, w/2, h);
        // if (CHATTY) Log.i(TAG, "w=" + w + " h=" + h + " scale=" + scale
        //         + ": " + mUniverseTransform);
        SendUniverseTransform();
        if (GetVisibility() != IView::VISIBLE) {
            SetVisibility(IView::VISIBLE);
        }
    } else {
        // if (CHATTY) Log.i(TAG, "mYDelta=" + mYDelta);
        mUniverseTransform->Clear();
        SendUniverseTransform();
        if (GetVisibility() == IView::VISIBLE) {
            SetVisibility(IView::GONE);
        }
    }
}

void UniverseBackground::TrackMovement(
    /* [in] */ IMotionEvent* event)
{
    mVelocityTracker->AddMovement(event);
}

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
