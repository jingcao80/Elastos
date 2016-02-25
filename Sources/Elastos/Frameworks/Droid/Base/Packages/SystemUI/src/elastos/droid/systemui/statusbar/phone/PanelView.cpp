#include "elastos/droid/systemui/statusbar/phone/PanelView.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/animation/CTimeAnimator.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"

using Elastos::Core::Math;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Animation::EIID_ITimeListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CTimeAnimator;
using Elastos::Droid::Animation::ITimeAnimator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

//======================================================================
//            PanelView::TimeListener
//======================================================================

CAR_INTERFACE_IMPL(PanelView::TimeListener, ITimeListener)

PanelView::TimeListener::TimeListener(
    /* [in] */ PanelView* host)
    : mHost(host)
{
}

ECode PanelView::TimeListener::OnTimeUpdate(
    /* [in] */ ITimeAnimator* animation,
    /* [in] */ Int64 totalTime,
    /* [in] */ Int64 deltaTime)
{
    mHost->AnimationTick(deltaTime);
    return NOERROR;
}

//======================================================================
//            PanelView::StopAnimatorRunnable
//======================================================================
PanelView::StopAnimatorRunnable::StopAnimatorRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{
}

ECode PanelView::StopAnimatorRunnable::Run()
{
    if (mHost->mTimeAnimator != NULL) {
        Boolean isStarted;
        mHost->mTimeAnimator->IsStarted(&isStarted);
        if (isStarted) {
            mHost->mTimeAnimator->End();
            mHost->mRubberbanding = FALSE;
            mHost->mClosing = FALSE;
        }
    }
    return NOERROR;
}

//======================================================================
//            PanelView::ViewOnTouchListener
//======================================================================
//
CAR_INTERFACE_IMPL(PanelView::ViewOnTouchListener, IViewOnTouchListener)

PanelView::ViewOnTouchListener::ViewOnTouchListener(
    /* [in] */ PanelView* host)
    : mHost(host)
{
}

ECode PanelView::ViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    using Elastos::Core::Math;

    VALIDATE_NOT_NULL(result);

    Float rawY, y;
    event->GetY(&y);
    event->GetRawY(&rawY);
    Int32 action;
    event->GetAction(&action);

    // if (DEBUG) LOG("handle.onTouch: a=%s y=%.1f rawY=%.1f off=%.1f",
    //         MotionEvent.actionToString(event.getAction()),
    //         y, rawY, mTouchOffset);

    mHost->GetLocationOnScreen(&mHost->mAbsPos[0], &mHost->mAbsPos[1]);

    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mHost->mTracking = TRUE;
            mHost->mHandleView->SetPressed(TRUE);
            mHost->PostInvalidate(); // catch the press state change
            mHost->mInitialTouchY = y;
            mHost->mVelocityTracker = VelocityTracker::Obtain();
            mHost->TrackMovement(event);
            mHost->mTimeAnimator->Cancel(); // end any outstanding animations
            mHost->mBar->OnTrackingStarted((IPanelView*)(mHost->Probe(EIID_IPanelView)));
            mHost->mTouchOffset = (rawY - mHost->mAbsPos[1]) - mHost->GetExpandedHeight();
            if (mHost->mExpandedHeight == 0) {
                mHost->mJustPeeked = TRUE;
                mHost->RunPeekAnimation();
            }
            break;

        case IMotionEvent::ACTION_MOVE: {
            Float h = rawY - mHost->mAbsPos[1] - mHost->mTouchOffset;
            if (h > mHost->mPeekHeight) {
                if (mHost->mPeekAnimator != NULL) {
                    Boolean isRunning;
                    mHost->mPeekAnimator->IsRunning(&isRunning);
                    if (isRunning)
                           mHost->mPeekAnimator->Cancel();
                }
                mHost->mJustPeeked = FALSE;
            }

            if (!mHost->mJustPeeked) {
                mHost->SetExpandedHeightInternal(h);
                mHost->mBar->PanelExpansionChanged(
                    (IPanelView*)(mHost->Probe(EIID_IPanelView)),
                    mHost->mExpandedFraction);
            }

            mHost->TrackMovement(event);
            }
            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mHost->mFinalTouchY = y;
            mHost->mTracking = FALSE;
            mHost->mHandleView->SetPressed(FALSE);
            mHost->PostInvalidate(); // catch the press state change
            mHost->mBar->OnTrackingStopped((IPanelView*)(mHost->Probe(EIID_IPanelView)));
            mHost->TrackMovement(event);

            Float vel = 0, yVel = 0, xVel = 0;
            Boolean negative = FALSE;

            if (mHost->mVelocityTracker != NULL) {
                // the velocitytracker might be NULL if we got a bad input stream
                mHost->mVelocityTracker->ComputeCurrentVelocity(1000);

                mHost->mVelocityTracker->GetYVelocity(&yVel);
                negative = yVel < 0;

                mHost->mVelocityTracker->GetXVelocity(&xVel);
                if (xVel < 0) {
                    xVel = -xVel;
                }
                if (xVel > mHost->mFlingGestureMaxXVelocityPx) {
                    xVel = mHost->mFlingGestureMaxXVelocityPx; // limit how much we care about the x axis
                }

                vel = (Float)Math::Hypot(yVel, xVel);
                if (vel > mHost->mFlingGestureMaxOutputVelocityPx) {
                    vel = mHost->mFlingGestureMaxOutputVelocityPx;
                }

                mHost->mVelocityTracker->Recycle();
                mHost->mVelocityTracker = NULL;
            }

            // if you've barely moved your finger, we treat the velocity as 0
            // preventing spurious flings due to touch screen jitter
            Float deltaY = Math::Abs(mHost->mFinalTouchY - mHost->mInitialTouchY);
            if (deltaY < mHost->mFlingGestureMinDistPx || vel < mHost->mFlingExpandMinVelocityPx) {
                vel = 0;
            }

            if (negative) {
                vel = -vel;
            }

            // if (DEBUG)
            //     LOG("gesture: dy=%f vel=(%f,%f) vlinear=%f", deltaY, xVel, yVel, vel);

            mHost->Fling(vel, TRUE);

            break;
    }

    *result = TRUE;
    return NOERROR;
}

//======================================================================
//            PanelView
//======================================================================
const String PanelView::TAG("PanelView");

PanelView::PanelView()
    : mInitialTouchY(0)
    , mFinalTouchY(0)
    , mRubberbandingEnabled(TRUE)
    , mSelfExpandVelocityPx(0)
    , mSelfCollapseVelocityPx(0)
    , mFlingExpandMinVelocityPx(0)
    , mFlingCollapseMinVelocityPx(0)
    , mCollapseMinDisplayFraction(0)
    , mExpandMinDisplayFraction(0)
    , mFlingGestureMaxXVelocityPx(0)
    , mFlingGestureMinDistPx(0)
    , mExpandAccelPx(0)
    , mCollapseAccelPx(0)
    , mFlingGestureMaxOutputVelocityPx(0)
    , mCollapseBrakingDistancePx(200)
    , mExpandBrakingDistancePx(150)
    , mBrakingSpeedPx(150)
    , mPeekHeight(0)
    , mTouchOffset(0)
    , mExpandedFraction(0)
    , mExpandedHeight(0)
    , mJustPeeked(FALSE)
    , mClosing(FALSE)
    , mRubberbanding(FALSE)
    , mTracking(FALSE)
    , mVel(0)
    , mAccel(0)
{
    mAbsPos[0] = mAbsPos[1] = 0;

    mStopAnimator = new StopAnimatorRunnable(this);
    mAnimationCallback = new TimeListener(this);

    CTimeAnimator::New((ITimeAnimator**)&mTimeAnimator);
    mTimeAnimator->SetTimeListener(mAnimationCallback);
}

PanelView::PanelView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mInitialTouchY(0)
    , mFinalTouchY(0)
    , mRubberbandingEnabled(TRUE)
    , mSelfExpandVelocityPx(0)
    , mSelfCollapseVelocityPx(0)
    , mFlingExpandMinVelocityPx(0)
    , mFlingCollapseMinVelocityPx(0)
    , mCollapseMinDisplayFraction(0)
    , mExpandMinDisplayFraction(0)
    , mFlingGestureMaxXVelocityPx(0)
    , mFlingGestureMinDistPx(0)
    , mExpandAccelPx(0)
    , mCollapseAccelPx(0)
    , mFlingGestureMaxOutputVelocityPx(0)
    , mCollapseBrakingDistancePx(200)
    , mExpandBrakingDistancePx(150)
    , mBrakingSpeedPx(150)
    , mPeekHeight(0)
    , mTouchOffset(0)
    , mExpandedFraction(0)
    , mExpandedHeight(0)
    , mJustPeeked(FALSE)
    , mClosing(FALSE)
    , mRubberbanding(FALSE)
    , mTracking(FALSE)
    , mVel(0)
    , mAccel(0)
{
    mAbsPos[0] = mAbsPos[1] = 0;

    mStopAnimator = new StopAnimatorRunnable(this);
    mAnimationCallback = new TimeListener(this);

    CTimeAnimator::New((ITimeAnimator**)&mTimeAnimator);
    mTimeAnimator->SetTimeListener(mAnimationCallback);
}

ECode PanelView::SetRubberbandingEnabled(
    /* [in] */ Boolean enabled)
{
    mRubberbandingEnabled = enabled;
    return NOERROR;
}

void PanelView::RunPeekAnimation()
{
    // if (DEBUG) LOG("peek to height=%.1f", mPeekHeight);
    Boolean isStarted;
    mTimeAnimator->IsStarted(&isStarted);
    if (isStarted) {
        return;
    }
    if (mPeekAnimator == NULL) {
        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        values->Set(0, mPeekHeight);
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        helper->OfFloat(THIS_PROBE(IInterface), String("expandedHeight"),
            values, (IObjectAnimator**)&mPeekAnimator);
        mPeekAnimator->SetDuration(250);
    }
    mPeekAnimator->Start();
}

void PanelView::PanelView::AnimationTick(
    /* [in] */ Int64 dtms)
{
    Boolean isStarted;
    mTimeAnimator->IsStarted(&isStarted);
    if (!isStarted) {
        // XXX HAX to work around bug in TimeAnimator.end() not resetting its last time
        mTimeAnimator = NULL;
        CTimeAnimator::New((ITimeAnimator**)&mTimeAnimator);
        mTimeAnimator->SetTimeListener(mAnimationCallback);

        if (mPeekAnimator != NULL) mPeekAnimator->Cancel();

        mTimeAnimator->Start();

        mRubberbanding = mRubberbandingEnabled // is it enabled at all?
                && mExpandedHeight > GetFullHeight() // are we past the end?
                && mVel >= -mFlingGestureMinDistPx; // was this not possibly a "close" gesture?
        if (mRubberbanding) {
            mClosing = TRUE;
        }
        else if (mVel == 0) {
            // if the panel is less than halfway open, close it
            mClosing = (mFinalTouchY / GetFullHeight()) < 0.5f;
        }
        else {
            mClosing = mExpandedHeight > 0 && mVel < 0;
        }
    }
    else if (dtms > 0) {
        Float dt = dtms * 0.001f;                  // ms -> s
        // if (DEBUG) LOG("tick: v=%.2fpx/s dt=%.4fs", mVel, dt);
        // if (DEBUG) LOG("tick: before: h=%d", (int) mExpandedHeight);

        Float fh = GetFullHeight();
        Boolean braking = FALSE;
        if (IPanelView::BRAKES) {
            if (mClosing) {
                braking = mExpandedHeight <= mCollapseBrakingDistancePx;
                mAccel = braking ? 10 * mCollapseAccelPx : -mCollapseAccelPx;
            }
            else {
                braking = mExpandedHeight >= (fh-mExpandBrakingDistancePx);
                mAccel = braking ? 10 * (-mExpandAccelPx) : mExpandAccelPx;
            }
        }
        else {
            mAccel = mClosing ? -mCollapseAccelPx : mExpandAccelPx;
        }

        mVel += mAccel * dt;

        if (braking) {
            if (mClosing && mVel > -mBrakingSpeedPx) {
                mVel = -mBrakingSpeedPx;
            }
            else if (!mClosing && mVel < mBrakingSpeedPx) {
                mVel = mBrakingSpeedPx;
            }
        }
        else {
            if (mClosing && mVel > -mFlingCollapseMinVelocityPx) {
                mVel = -mFlingCollapseMinVelocityPx;
            } else if (!mClosing && mVel > mFlingGestureMaxOutputVelocityPx) {
                mVel = mFlingGestureMaxOutputVelocityPx;
            }
        }

        Float h = mExpandedHeight + mVel * dt;

        if (mRubberbanding && h < fh) {
            h = fh;
        }

        // if (DEBUG) LOG("tick: new h=%d closing=%s", (int) h, mClosing?"TRUE":"FALSE");

        SetExpandedHeightInternal(h);

        mBar->PanelExpansionChanged(THIS_PROBE(IPanelView), mExpandedFraction);

        if (mVel == 0
                || (mClosing && mExpandedHeight == 0)
                || ((mRubberbanding || !mClosing) && mExpandedHeight == fh)) {
            Post(mStopAnimator);
        }
    }
}

void PanelView::LoadDimens()
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetDimension(SystemUIR::dimen::self_expand_velocity, &mSelfExpandVelocityPx);
    res->GetDimension(SystemUIR::dimen::self_collapse_velocity, &mSelfCollapseVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_expand_min_velocity, &mFlingExpandMinVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_collapse_min_velocity, &mFlingCollapseMinVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_gesture_min_dist, &mFlingGestureMinDistPx);

    res->GetFraction(SystemUIR::dimen::collapse_min_display_fraction, 1, 1, &mCollapseMinDisplayFraction);
    res->GetFraction(SystemUIR::dimen::expand_min_display_fraction, 1, 1, &mExpandMinDisplayFraction);

    res->GetDimension(SystemUIR::dimen::expand_accel, &mExpandAccelPx);
    res->GetDimension(SystemUIR::dimen::collapse_accel, &mCollapseAccelPx);
    res->GetDimension(SystemUIR::dimen::fling_gesture_max_x_velocity, &mFlingGestureMaxXVelocityPx);
    res->GetDimension(SystemUIR::dimen::fling_gesture_max_output_velocity, &mFlingGestureMaxOutputVelocityPx);
    res->GetDimension(SystemUIR::dimen::peek_height, &mPeekHeight);

    mPeekHeight += GetPaddingBottom(); // our window might have a dropshadow
    if (mHandleView != NULL) {
        Int32 top;
        mHandleView->GetPaddingTop(&top); // the handle might have a topshadow
        mPeekHeight -= top;
    }
}

void PanelView::TrackMovement(
    /* [in] */ IMotionEvent* event)
{
    Float rx, ry, x, y;
    event->GetRawX(&rx);
    event->GetRawY(&ry);
    event->GetX(&x);
    event->GetY(&y);
    // Add movement to velocity tracker using raw screen X and Y coordinates instead
    // of window coordinates because the window frame may be moving at the same time.
    Float deltaX = rx - x;
    Float deltaY = ry - y;
    event->OffsetLocation(deltaX, deltaY);
    if (mVelocityTracker != NULL)
        mVelocityTracker->AddMovement(event);
    event->OffsetLocation(-deltaX, -deltaY);
}

// Pass all touches along to the handle, allowing the user to
// drag the panel closed from its interior
//@Override
Boolean PanelView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean result;
    mHandleView->DispatchTouchEvent(event, &result);
    return result;
}

//@Override
ECode PanelView::OnFinishInflate()
{
       FrameLayout::OnFinishInflate();
       //TODO mHandleView = FindViewById(R.id.handle);

    LoadDimens();

    // if (DEBUG) LOG("handle view: " + mHandleView);
    if (mHandleView != NULL) {
        AutoPtr<IViewOnTouchListener> listener = new ViewOnTouchListener(this);
        mHandleView->SetOnTouchListener(listener);
    }
    return NOERROR;
}

ECode PanelView::Fling(
    /* [in] */ Float vel,
    /* [in] */ Boolean always)
{
    // if (DEBUG) LOG("fling: vel=%.3f, this=%s", vel, this);
    mVel = vel;

    if (always || mVel != 0) {
        AnimationTick(0); // begin the animation
    }
    return NOERROR;
}

//@Override
ECode PanelView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    AutoPtr<IResources> res = GetResources();
    res->GetResourceName(GetId(), &mViewName);
    return NOERROR;
}

String PanelView::GetName()
{
    return mViewName;
}

//@Override
void PanelView::OnViewAdded(
    /* [in] */ IView* child)
{
    // if (DEBUG) LOG("onViewAdded: " + child);
}

AutoPtr<IView> PanelView::GetHandle()
{
    return mHandleView;
}

// Rubberbands the panel to hold its contents.
//@Override
void PanelView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */  Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    // if (DEBUG) LOG("onMeasure(%d, %d) -> (%d, %d)",
    //         widthMeasureSpec, heightMeasureSpec, getMeasuredWidth(), getMeasuredHeight());

    // Did one of our children change size?
    Int32 newHeight = GetMeasuredHeight();
    if (newHeight != mFullHeight) {
        mFullHeight = newHeight;
        Boolean isStarted = FALSE;
        if (!mTracking && !mRubberbanding)
            mTimeAnimator->IsStarted(&isStarted);
        // If the user isn't actively poking us, let's rubberband to the content
        if (!mTracking && !mRubberbanding && !isStarted
                && mExpandedHeight > 0 && mExpandedHeight != mFullHeight) {
            mExpandedHeight = mFullHeight;
        }
    }

    heightMeasureSpec = MeasureSpec::MakeMeasureSpec(
                (Int32) mExpandedHeight, MeasureSpec::AT_MOST); // MeasureSpec.getMode(heightMeasureSpec));
    SetMeasuredDimension(widthMeasureSpec, heightMeasureSpec);
}


ECode PanelView::SetExpandedHeight(
    /* [in] */ Float height)
{
    // if (DEBUG) LOG("setExpandedHeight(%.1f)", height);
    mRubberbanding = FALSE;
    Boolean isRunning;
    mTimeAnimator->IsRunning(&isRunning);
    if (isRunning) {
        Post(mStopAnimator);
    }

    SetExpandedHeightInternal(height);
    mBar->PanelExpansionChanged(THIS_PROBE(IPanelView), mExpandedFraction);
    return NOERROR;
}

//@Override
void PanelView::OnLayout (
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    // if (DEBUG) LOG("onLayout: changed=%s, bottom=%d eh=%d fh=%d", changed?"T":"f", bottom, (int)mExpandedHeight, mFullHeight);
    FrameLayout::OnLayout(changed, left, top, right, bottom);
}

ECode PanelView::SetExpandedHeightInternal(
    /* [in] */ Float h)
{
    using Elastos::Core::Math;
    Float fh = GetFullHeight();
    if (fh == 0) {
        // Hmm, full height hasn't been computed yet
    }

    if (h < 0) h = 0;
    if (!(mRubberbandingEnabled && (mTracking || mRubberbanding)) && h > fh) h = fh;
    mExpandedHeight = h;

    // if (DEBUG) LOG("setExpansion: height=%.1f fh=%.1f tracking=%s rubber=%s", h, fh, mTracking?"T":"f", mRubberbanding?"T":"f");

    RequestLayout();
//        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) getLayoutParams();
//        lp.height = (int) mExpandedHeight;
//        setLayoutParams(lp);

    mExpandedFraction = Math::Min(1.0f, (fh == 0) ? 0 : h / fh);
    return NOERROR;
}

Float PanelView::GetFullHeight()
{
    if (mFullHeight <= 0) {
        // if (DEBUG) LOG("Forcing measure() since fullHeight=" + mFullHeight);
        Measure(MeasureSpec::MakeMeasureSpec(IViewGroupLayoutParams::WRAP_CONTENT, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(IViewGroupLayoutParams::WRAP_CONTENT, MeasureSpec::EXACTLY));
    }
    return mFullHeight;
}

ECode PanelView::SetExpandedFraction(
    /* [in] */ Float frac)
{
    return SetExpandedHeight(GetFullHeight() * frac);
}

Float PanelView::GetExpandedHeight()
{
    return mExpandedHeight;
}

Float PanelView::GetExpandedFraction()
{
    return mExpandedFraction;
}

Boolean PanelView::IsFullyExpanded()
{
    return mExpandedHeight >= GetFullHeight();
}

Boolean PanelView::IsFullyCollapsed()
{
    return mExpandedHeight <= 0;
}

Boolean PanelView::IsCollapsing()
{
    return mClosing;
}

ECode PanelView::SetBar(
    /* [in] */ IPanelBar* panelBar)
{
    mBar = panelBar;
    return NOERROR;
}

ECode PanelView::Collapse()
{
    // TODO: abort animation or ongoing touch
    // if (DEBUG) LOG("collapse: " + this);
    if (!IsFullyCollapsed()) {
        mTimeAnimator->Cancel();
        mClosing = TRUE;
        // collapse() should never be a rubberband, even if an animation is already running
        mRubberbanding = FALSE;
        Fling(-mSelfCollapseVelocityPx, /*always=*/ TRUE);
    }
    return NOERROR;
}

ECode PanelView::Expand()
{
    // if (DEBUG) LOG("expand: " + this);
    if (IsFullyCollapsed()) {
        mBar->StartOpeningPanel(THIS_PROBE(IPanelView));
        Fling(mSelfExpandVelocityPx, /*always=*/ TRUE);
    }
    // else if (DEBUG) {
    //     if (DEBUG) LOG("skipping expansion: is expanded");
    // }
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
