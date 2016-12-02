
#include "elastos/droid/dialer/list/SwipeHelper.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// SwipeHelper::DismissAnimatorListenerAdapter
//=================================================================
SwipeHelper::DismissAnimatorListenerAdapter::DismissAnimatorListenerAdapter(
    /* [in] */ SwipeHelper* host,
    /* [in] */ IView* view,
    /* [in] */ IView* animView)
    : mHost(host)
    , mView(view)
    , mAnimView(animView)
{}

ECode SwipeHelper::DismissAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->OnChildDismissed(mView);
    mAnimView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);

    return NOERROR;
}


//=================================================================
// SwipeHelper::DismissAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_IMPL(SwipeHelper::DismissAnimatorUpdateListener, Object, IAnimatorUpdateListener)

SwipeHelper::DismissAnimatorUpdateListener::DismissAnimatorUpdateListener(
    /* [in] */ SwipeHelper* host,
    /* [in] */ Boolean canBeDismissed,
    /* [in] */ IView* animView)
    : mHost(host)
    , mCanBeDismissed(canBeDismissed)
    , mAnimView(animView)
{}

ECode SwipeHelper::DismissAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && mCanBeDismissed) {
        mAnimView->SetAlpha(mHost->GetAlphaForOffset(mAnimView));
    }
    mHost->InvalidateGlobalRegion(mAnimView);

    return NOERROR;
}


//=================================================================
// SwipeHelper::SnapAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_IMPL(SwipeHelper::SnapAnimatorUpdateListener, Object, IAnimatorUpdateListener)

SwipeHelper::SnapAnimatorUpdateListener::SnapAnimatorUpdateListener(
    /* [in] */ SwipeHelper* host,
    /* [in] */ Boolean canBeDismissed,
    /* [in] */ IView* animView)
    : mHost(host)
    , mCanBeDismissed(canBeDismissed)
    , mAnimView(animView)
{}

ECode SwipeHelper::SnapAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && mCanBeDismissed) {
        mAnimView->SetAlpha(mHost->GetAlphaForOffset(mAnimView));
    }
    mHost->InvalidateGlobalRegion(mAnimView);

    return NOERROR;
}


//=================================================================
// SwipeHelper::SnapAnimatorListenerAdapter
//=================================================================
SwipeHelper::SnapAnimatorListenerAdapter::SnapAnimatorListenerAdapter(
    /* [in] */ SwipeHelper* host,
    /* [in] */ IView* animView)
    : mHost(host)
    , mAnimView(animView)
{}

ECode SwipeHelper::SnapAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mAnimView->SetAlpha(mHost->mStartAlpha);
    mHost->mCallback->OnDragCancelled(mHost->mCurrView);

    return NOERROR;
}


//=================================================================
// SwipeHelper
//=================================================================
const Int32 SwipeHelper::IS_SWIPEABLE_TAG = Elastos::Droid::Dialer::R::id::is_swipeable_tag;
const AutoPtr<Object> SwipeHelper::IS_SWIPEABLE = new Object();
const Int32 SwipeHelper::X;
const Int32 SwipeHelper::Y;
const Float SwipeHelper::ALPHA_FADE_START;
const String SwipeHelper::TAG("SwipeHelper");
const Boolean SwipeHelper::DEBUG_INVALIDATE;
const Boolean SwipeHelper::CONSTRAIN_SWIPE;
const Boolean SwipeHelper::FADE_OUT_DURING_SWIPE;
const Boolean SwipeHelper::DISMISS_IF_SWIPED_FAR_ENOUGH;
const Boolean SwipeHelper::LOG_SWIPE_DISMISS_VELOCITY;

static AutoPtr<ILinearInterpolator> InitInterpolator()
{
    AutoPtr<ILinearInterpolator> interpolator;
    CLinearInterpolator::New((ILinearInterpolator**)&interpolator);
    return interpolator;
}
AutoPtr<ILinearInterpolator> SwipeHelper::sLinearInterpolator = InitInterpolator();
Int32 SwipeHelper::SWIPE_ESCAPE_VELOCITY = -1;
Int32 SwipeHelper::DEFAULT_ESCAPE_ANIMATION_DURATION = 0;
Int32 SwipeHelper::MAX_ESCAPE_ANIMATION_DURATION = 0;
Int32 SwipeHelper::MAX_DISMISS_VELOCITY = 0;
Int32 SwipeHelper::SNAP_ANIM_LEN = 0;
Int32 SwipeHelper::SWIPE_SCROLL_SLOP = 0;
Float SwipeHelper::MIN_SWIPE = 0;
Float SwipeHelper::MIN_VERT = 0;
Float SwipeHelper::MIN_LOCK = 0;
const Float SwipeHelper::ALPHA_FADE_END;
const Float SwipeHelper::FACTOR;
const Int32 SwipeHelper::PROTECTION_PADDING;

SwipeHelper::SwipeHelper(
    /* [in] */ IContext* context,
    /* [in] */ Int32 swipeDirection,
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ Float densityScale,
    /* [in] */ Float pagingTouchSlop)
    : mMinAlpha(0.3)
    , mPagingTouchSlop(0.0)
    , mSwipeDirection(0)
    , mInitialTouchPosX(0.0)
    , mDragging(FALSE)
    , mCanCurrViewBeDimissed(FALSE)
    , mDensityScale(0.0)
    , mLastY(0.0)
    , mInitialTouchPosY(0.0)
    , mStartAlpha(0.0)
    , mProtected(FALSE)
    , mChildSwipedFarEnoughFactor(0.4)
    , mChildSwipedFastEnoughFactor(0.05)
{
    mCallback = callback;
    mSwipeDirection = swipeDirection;
    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    mDensityScale = densityScale;
    mPagingTouchSlop = pagingTouchSlop;
    if (SWIPE_ESCAPE_VELOCITY == -1) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::swipe_escape_velocity, &SWIPE_ESCAPE_VELOCITY);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::escape_animation_duration, &DEFAULT_ESCAPE_ANIMATION_DURATION);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::max_escape_animation_duration, &MAX_ESCAPE_ANIMATION_DURATION);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::max_dismiss_velocity, &MAX_DISMISS_VELOCITY);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::snap_animation_duration, &SNAP_ANIM_LEN);
        res->GetInteger(Elastos::Droid::Dialer::R::integer::swipe_scroll_slop, &SWIPE_SCROLL_SLOP);
        res->GetDimension(Elastos::Droid::Dialer::R::dimen::min_swipe, &MIN_SWIPE);
        res->GetDimension(Elastos::Droid::Dialer::R::dimen::min_vert, &MIN_VERT);
        res->GetDimension(Elastos::Droid::Dialer::R::dimen::min_lock, &MIN_LOCK);
    }
}

void SwipeHelper::SetDensityScale(
    /* [in] */ Float densityScale)
{
    mDensityScale = densityScale;
}

void SwipeHelper::SetPagingTouchSlop(
    /* [in] */ Float pagingTouchSlop)
{
    mPagingTouchSlop = pagingTouchSlop;
}

void SwipeHelper::SetChildSwipedFarEnoughFactor(
    /* [in] */ Float factor)
{
    mChildSwipedFarEnoughFactor = factor;
}

void SwipeHelper::SetChildSwipedFastEnoughFactor(
    /* [in] */ Float factor)
{
    mChildSwipedFastEnoughFactor = factor;
}

Float SwipeHelper::GetVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float velocity;
    mSwipeDirection == X ? vt->GetXVelocity(&velocity) : vt->GetYVelocity(&velocity);
    return velocity;
}

AutoPtr<IObjectAnimator> SwipeHelper::CreateTranslationAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos)
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > attrs = ArrayOf<Float>::Alloc(1);
    (*attrs)[0] = newPos;
    AutoPtr<IObjectAnimator> anim;
    helper->OfFloat(v,
            mSwipeDirection == X ? String("translationX") : String("translationY"),
            attrs, (IObjectAnimator**)&anim);
    return anim;
}

AutoPtr<IObjectAnimator> SwipeHelper::CreateDismissAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos,
    /* [in] */ Int32 duration)
{
    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(v, newPos);
    AutoPtr<IAnimator> a = IAnimator::Probe(anim);
    a->SetInterpolator(ITimeInterpolator::Probe(sLinearInterpolator));
    a->SetDuration(duration);
    return anim;
}

Float SwipeHelper::GetPerpendicularVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float velocity;
    mSwipeDirection == X ? vt->GetYVelocity(&velocity) : vt->GetXVelocity(&velocity);
    return velocity;
}

void SwipeHelper::SetTranslation(
    /* [in] */ IView* v,
    /* [in] */ Float translate)
{
    if (mSwipeDirection == X) {
        v->SetTranslationX(translate);
    }
    else {
        v->SetTranslationY(translate);
    }
}

Float SwipeHelper::GetSize(
    /* [in] */ IView* v)
{
    Int32 size;
    mSwipeDirection == X ? v->GetMeasuredWidth(&size) : v->GetMeasuredHeight(&size);
    return (Float)size;
}

void SwipeHelper::SetMinAlpha(
    /* [in] */ Float minAlpha)
{
    mMinAlpha = minAlpha;
}

Float SwipeHelper::GetAlphaForOffset(
    /* [in] */ IView* view)
{
    Float viewSize = GetSize(view);
    Float fadeSize = ALPHA_FADE_END * viewSize;
    Float result = mStartAlpha;
    Float pos;
    view->GetTranslationX(&pos);
    if (pos >= viewSize * ALPHA_FADE_START) {
        result = mStartAlpha - (pos - viewSize * ALPHA_FADE_START) / fadeSize;
    }
    else if (pos < viewSize * (mStartAlpha - ALPHA_FADE_START)) {
        result = mStartAlpha + (viewSize * ALPHA_FADE_START + pos) / fadeSize;
    }
    return Elastos::Core::Math::Max(mMinAlpha, result);
}

void SwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view)
{
    Int32 left, top, right, bottom;
    view->GetLeft(&left);
    view->GetTop(&top);
    view->GetRight(&right);
    view->GetBottom(&bottom);
    AutoPtr<IRectF> rectF;
    CRectF::New(left, top, right, bottom, (IRectF**)&rectF);
    InvalidateGlobalRegion(view, rectF);
}

void SwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view,
    /* [in] */ IRectF* childBounds)
{
    // childBounds.offset(view.getTranslationX(), view.getTranslationY());
    if (DEBUG_INVALIDATE)
        Logger::V(TAG, "-------------");

    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    while (parent != NULL && IView::Probe(parent) != NULL) {
        view = IView::Probe(parent);
        AutoPtr<IMatrix> matrix;
        view->GetMatrix((IMatrix**)&matrix);
        Boolean result;
        matrix->MapRect(childBounds, &result);

        Float left, top, right, bottom;
        childBounds->GetLeft(&left);
        childBounds->GetTop(&top);
        childBounds->GetRight(&right);
        childBounds->GetBottom(&bottom);
        view->Invalidate((Int32)Elastos::Core::Math::Floor(left), (Int32)Elastos::Core::Math::Floor(top),
                (Int32)Elastos::Core::Math::Ceil(right), (Int32)Elastos::Core::Math::Ceil(bottom));
        if (DEBUG_INVALIDATE) {
            Logger::V(TAG, "INVALIDATE(%d,%d,%d,%d", (Int32)Elastos::Core::Math::Floor(left),
                    (Int32)Elastos::Core::Math::Floor(top), (Int32)Elastos::Core::Math::Ceil(right),
                    (Int32)Elastos::Core::Math::Ceil(bottom));
        }
    }
}

Boolean SwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            ev->GetY(&mLastY);
            mDragging = FALSE;
            AutoPtr<IView> temp;
            mCallback->GetChildAtPosition(ev, (IView**)&temp);
            mCurrView = temp;
            mVelocityTracker->Clear();
            if (mCurrView != NULL) {
                AutoPtr<IView> temp1;
                mCallback->GetChildContentView(mCurrView, (IView**)&temp1);
                mCurrAnimView = temp1;
                mCurrAnimView->GetAlpha(&mStartAlpha);
                mCallback->CanChildBeDismissed(mCurrView, &mCanCurrViewBeDimissed);
                mVelocityTracker->AddMovement(ev);
                ev->GetX(&mInitialTouchPosX);
                ev->GetY(&mInitialTouchPosY);
            }
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            if (mCurrView != NULL) {
                // Check the movement direction.
                if (mLastY >= 0 && !mDragging) {
                    Float currY;
                    ev->GetY(&currY);
                    Float currX;
                    ev->GetX(&currX);
                    Float deltaY = Elastos::Core::Math::Abs(currY - mInitialTouchPosY);
                    Float deltaX = Elastos::Core::Math::Abs(currX - mInitialTouchPosX);
                    if (deltaY > SWIPE_SCROLL_SLOP && deltaY > (FACTOR * deltaX)) {
                        ev->GetY(&mLastY);
                        mCallback->OnScroll();
                        return FALSE;
                    }
                }
                mVelocityTracker->AddMovement(ev);
                Float pos;
                ev->GetX(&pos);
                Float delta = pos - mInitialTouchPosX;
                if (Elastos::Core::Math::Abs(delta) > mPagingTouchSlop) {
                    AutoPtr<IView> view;
                    mCallback->GetChildContentView(mCurrView, (IView**)&view);
                    mCallback->OnBeginDrag(view);
                    mDragging = TRUE;
                    Float x;
                    ev->GetX(&x);
                    Float tx;
                    mCurrAnimView->GetTranslationX(&tx);
                    mInitialTouchPosX = x - tx;
                    ev->GetY(&mInitialTouchPosY);
                }
            }
            ev->GetY(&mLastY);
            break;
        }
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mDragging = FALSE;
            mCurrView = NULL;
            mCurrAnimView = NULL;
            mLastY = -1;
            break;
    }
    return mDragging;
}

void SwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    Float newPos = DeterminePos(animView, velocity);
    Int32 duration = DetermineDuration(animView, newPos, velocity);

    animView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<IObjectAnimator> anim = CreateDismissAnimation(animView, newPos, duration);
    AutoPtr<IAnimatorListener> listener = (IAnimatorListener*)new DismissAnimatorListenerAdapter(this, view, animView);
    IAnimator::Probe(anim)->AddListener(listener);
    AutoPtr<IAnimatorUpdateListener> updateListener =
            (IAnimatorUpdateListener*)new DismissAnimatorUpdateListener(this, canAnimViewBeDismissed, animView);
    IValueAnimator::Probe(anim)->AddUpdateListener(updateListener);
    IAnimator::Probe(anim)->Start();
}

Int32 SwipeHelper::DetermineDuration(
    /* [in] */ IView* animView,
    /* [in] */ Float newPos,
    /* [in] */ Float velocity)
{
    Int32 duration = MAX_ESCAPE_ANIMATION_DURATION;
    if (velocity != 0) {
        Float x;
        animView->GetTranslationX(&x);
        duration = Elastos::Core::Math::Min(duration,
                (Int32)(Elastos::Core::Math::Abs(newPos - x) * 1000.0f / Elastos::Core::Math::Abs(velocity)));
    }
    else {
        duration = DEFAULT_ESCAPE_ANIMATION_DURATION;
    }
    return duration;
}

Float SwipeHelper::DeterminePos(
    /* [in] */ IView* animView,
    /* [in] */ Float velocity)
{
    Float newPos = 0;
    Float x;
    if (velocity < 0 || (velocity == 0 && (animView->GetTranslationX(&x), x < 0))
            // if we use the Menu to dismiss an item in landscape, animate up
            || (velocity == 0 && (animView->GetTranslationX(&x), x == 0) && mSwipeDirection == Y)) {
        newPos = -GetSize(animView);
    }
    else {
        newPos = GetSize(animView);
    }
    return newPos;
}

void SwipeHelper::SnapChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(animView, 0);
    Int32 duration = SNAP_ANIM_LEN;
    AutoPtr<IAnimator> a = IAnimator::Probe(anim);
    a->SetDuration(duration);
    AutoPtr<IAnimatorUpdateListener> updateListener = new SnapAnimatorUpdateListener(this, canAnimViewBeDismissed, animView);
    IValueAnimator::Probe(anim)->AddUpdateListener(updateListener);
    AutoPtr<IAnimatorListener> snapListener = new SnapAnimatorListenerAdapter(this, animView);
    a->AddListener(snapListener);
    a->Start();
}

Boolean SwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (!mDragging || mProtected) {
        return FALSE;
    }
    mVelocityTracker->AddMovement(ev);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_OUTSIDE:
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL) {
                Float x, y;
                ev->GetX(&x);
                ev->GetY(&y);
                Float deltaX = x - mInitialTouchPosX;
                Float deltaY = Elastos::Core::Math::Abs(y - mInitialTouchPosY);
                // If the user has gone vertical and not gone horizontalish AT
                // LEAST minBeforeLock, switch to scroll. Otherwise, cancel
                // the swipe.
                if (!mDragging && deltaY > MIN_VERT && (Elastos::Core::Math::Abs(deltaX)) < MIN_LOCK
                        && deltaY > (FACTOR * Elastos::Core::Math::Abs(deltaX))) {
                    mCallback->OnScroll();
                    return FALSE;
                }
                Float minDistance = MIN_SWIPE;
                if (Elastos::Core::Math::Abs(deltaX) < minDistance) {
                    // Don't start the drag until at least X distance has
                    // occurred.
                    return TRUE;
                }
                // don't let items that can't be dismissed be dragged more
                // than maxScrollDistance
                Boolean canChildBeDismissed;
                if (CONSTRAIN_SWIPE &&
                        (mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed), !canChildBeDismissed)) {
                    Float size = GetSize(mCurrAnimView);
                    Float maxScrollDistance = 0.15f * size;
                    if (Elastos::Core::Math::Abs(deltaX) >= size) {
                        deltaX = deltaX > 0 ? maxScrollDistance : -maxScrollDistance;
                    }
                    else {
                        deltaX = maxScrollDistance
                                * (Float) Elastos::Core::Math::Sin((deltaX / size) * (Elastos::Core::Math::PI / 2));
                    }
                }
                SetTranslation(mCurrAnimView, deltaX);
                if (FADE_OUT_DURING_SWIPE && mCanCurrViewBeDimissed) {
                    mCurrAnimView->SetAlpha(GetAlphaForOffset(mCurrAnimView));
                }
                AutoPtr<IView> view;
                mCallback->GetChildContentView(mCurrView, (IView**)&view);
                InvalidateGlobalRegion(view);
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            if (mCurrView != NULL) {
                Float maxVelocity = MAX_DISMISS_VELOCITY * mDensityScale;
                mVelocityTracker->ComputeCurrentVelocity(1000 /* px/sec */, maxVelocity);
                Float escapeVelocity = SWIPE_ESCAPE_VELOCITY * mDensityScale;
                Float velocity = GetVelocity(mVelocityTracker);
                Float perpendicularVelocity = GetPerpendicularVelocity(mVelocityTracker);

                // Decide whether to dismiss the current view
                // Tweak constants below as required to prevent erroneous
                // swipe/dismiss
                Float x;
                mCurrAnimView->GetTranslationX(&x);
                Float translation = Elastos::Core::Math::Abs(x);
                Float currAnimViewSize = GetSize(mCurrAnimView);
                // Long swipe = translation of {@link #mChildSwipedFarEnoughFactor} * width
                Boolean childSwipedFarEnough = DISMISS_IF_SWIPED_FAR_ENOUGH
                        && translation > mChildSwipedFarEnoughFactor * currAnimViewSize;
                // Fast swipe = > escapeVelocity and translation of
                // {@link #mChildSwipedFastEnoughFactor} * width
                Boolean childSwipedFastEnough = (Elastos::Core::Math::Abs(velocity) > escapeVelocity)
                            && (Elastos::Core::Math::Abs(velocity) > Elastos::Core::Math::Abs(perpendicularVelocity))
                            && (velocity > 0) == ((mCurrAnimView->GetTranslationX(&x), x > 0))
                            && translation > mChildSwipedFastEnoughFactor * currAnimViewSize;
                if (LOG_SWIPE_DISMISS_VELOCITY) {
                    Logger::V(TAG, "Swipe/Dismiss: %f/%f/%f, x: %f/%f", velocity, escapeVelocity,
                            perpendicularVelocity, translation, currAnimViewSize);
                }

                Boolean canChildBeDismissed;
                mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed);
                Boolean dismissChild = canChildBeDismissed && (childSwipedFastEnough || childSwipedFarEnough);

                if (dismissChild) {
                    DismissChild(mCurrView, childSwipedFastEnough ? velocity : 0.0f);
                }
                else {
                    SnapChild(mCurrView, velocity);
                }
            }
            break;
    }
    return TRUE;
}

void SwipeHelper::SetSwipeable(
    /* [in] */ IView* view,
    /* [in] */ Boolean swipeable)
{
    view->SetTag(IS_SWIPEABLE_TAG, swipeable ? (IObject*)IS_SWIPEABLE.Get() : NULL);
}

Boolean SwipeHelper::IsSwipeable(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag(IS_SWIPEABLE_TAG, (IInterface**)&tag);
    return Object::Equals(tag, IS_SWIPEABLE);
}

} // List
} // Dialer
} // Droid
} // Elastos
