
#include "elastos/droid/systemui/recents/views/SwipeHelper.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Build.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::Animation::CLinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

const String SwipeHelper::TAG("SwipeHelper");
const Int32 SwipeHelper::X;
const Int32 SwipeHelper::Y;
Float SwipeHelper::ALPHA_FADE_START = 0.15f;
const Float SwipeHelper::ALPHA_FADE_END;
const Boolean SwipeHelper::SLOW_ANIMATIONS = FALSE; // DEBUG;
const Boolean SwipeHelper::CONSTRAIN_SWIPE = TRUE;
const Boolean SwipeHelper::FADE_OUT_DURING_SWIPE = TRUE;
const Boolean SwipeHelper::DISMISS_IF_SWIPED_FAR_ENOUGH = TRUE;

static AutoPtr<ILinearInterpolator> InitLinearInterpolator()
{
    AutoPtr<ILinearInterpolator> li;
    CLinearInterpolator::New((ILinearInterpolator**)&li);
    return li;
}

AutoPtr<ILinearInterpolator> SwipeHelper::sLinearInterpolator = InitLinearInterpolator();

const Int32 SwipeHelper::SNAP_ANIM_LEN = SLOW_ANIMATIONS ? 1000 : 250; // ms

SwipeHelper::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ IView* view,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mCallback(callback)
    , mView(view)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{}

// @Override
ECode SwipeHelper::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mCallback->OnChildDismissed(mView);
    if (FADE_OUT_DURING_SWIPE && mCanAnimViewBeDismissed) {
        mView->SetAlpha(1.f);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(SwipeHelper::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

SwipeHelper::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ SwipeHelper* host,
    /* [in] */ IView* view,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mHost(host)
    , mView(view)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{}

// @Override
ECode SwipeHelper::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && mCanAnimViewBeDismissed) {
        mView->SetAlpha(mHost->GetAlphaForOffset(mView));
    }
    return NOERROR;
}

SwipeHelper::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ IView* view,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mCallback(callback)
    , mView(view)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{}

// @Override
ECode SwipeHelper::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && mCanAnimViewBeDismissed) {
        mView->SetAlpha(1.0f);
    }
    mCallback->OnSnapBackCompleted(mView);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SwipeHelper::AnimatorUpdateListener2, Object, IAnimatorUpdateListener)

SwipeHelper::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ SwipeHelper* host,
    /* [in] */ IView* view,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mHost(host)
    , mView(view)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{}

// @Override
ECode SwipeHelper::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && mCanAnimViewBeDismissed) {
        mView->SetAlpha(mHost->GetAlphaForOffset(mView));
    }
    Float x;
    mView->GetTranslationX(&x);
    mHost->mCallback->OnSwipeChanged(mHost->mCurrView, x);
    return NOERROR;
}

SwipeHelper::SwipeHelper(
    /* [in] */ Int32 swipeDirection,
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ Float densityScale,
    /* [in] */ Float pagingTouchSlop)
    : mCallback(callback)
    , mAllowSwipeTowardsStart(TRUE)
    , mAllowSwipeTowardsEnd(TRUE)
    , SWIPE_ESCAPE_VELOCITY(100.0f)
    , DEFAULT_ESCAPE_ANIMATION_DURATION(75)
    , MAX_ESCAPE_ANIMATION_DURATION(150)
    , MAX_DISMISS_VELOCITY(2000)
    , mMinAlpha(0.0f)
    , mPagingTouchSlop(pagingTouchSlop)
    , mSwipeDirection(swipeDirection)
    , mInitialTouchPos(0.0f)
    , mDragging(FALSE)
    , mCanCurrViewBeDimissed(FALSE)
    , mDensityScale(densityScale)
{
    AutoPtr<IVelocityTrackerHelper> vtHelper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&vtHelper);
    vtHelper->Obtain((IVelocityTracker**)&mVelocityTracker);
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

void SwipeHelper::CancelOngoingDrag()
{
    if (mDragging) {
        if (mCurrView != NULL) {
            mCallback->OnDragCancelled(mCurrView);
            SetTranslation(mCurrView, 0);
            mCallback->OnSnapBackCompleted(mCurrView);
            mCurrView = NULL;
        }
        mDragging = FALSE;
    }
}

void SwipeHelper::ResetTranslation(
    /* [in] */ IView* v)
{
    SetTranslation(v, 0);
}

Float SwipeHelper::GetPos(
    /* [in] */ IMotionEvent* ev)
{
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    return mSwipeDirection == X ? x : y;
}

Float SwipeHelper::GetTranslation(
    /* [in] */ IView* v)
{
    Float x, y;
    v->GetTranslationX(&x);
    v->GetTranslationY(&y);
    return mSwipeDirection == X ? x : y;
}

Float SwipeHelper::GetVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float x, y;
    vt->GetXVelocity(&x);
    vt->GetYVelocity(&y);
    return mSwipeDirection == X ? x : y;
}

AutoPtr<IObjectAnimator> SwipeHelper::CreateTranslationAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos)
{
    AutoPtr<IObjectAnimatorHelper> oaHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
    (*values)[0] = newPos;
    AutoPtr<IObjectAnimator> anim;
    oaHelper->OfFloat(v, mSwipeDirection == X ?
        Elastos::Droid::View::View::TRANSLATION_X : Elastos::Droid::View::View::TRANSLATION_Y,
        values, (IObjectAnimator**)&anim);
    return anim;
}

Float SwipeHelper::GetPerpendicularVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float x, y;
    vt->GetXVelocity(&x);
    vt->GetYVelocity(&y);
    return mSwipeDirection == X ? y : x;
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
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 w, h;
    dm->GetWidthPixels(&w);
    dm->GetHeightPixels(&h);
    return mSwipeDirection == X ? w : h;
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
    Float result = 1.0f;
    Float pos = GetTranslation(view);
    if (pos >= viewSize * ALPHA_FADE_START) {
        result = 1.0f - (pos - viewSize * ALPHA_FADE_START) / fadeSize;
    }
    else if (pos < viewSize * (1.0f - ALPHA_FADE_START)) {
        result = 1.0f + (viewSize * ALPHA_FADE_START + pos) / fadeSize;
    }
    result = Elastos::Core::Math::Min(result, 1.0f);
    result = Elastos::Core::Math::Max(result, 0.0f);
    return Elastos::Core::Math::Max(mMinAlpha, result);
}

/**
 * Determines whether the given view has RTL layout.
 */
// @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
Boolean SwipeHelper::IsLayoutRtl(
    /* [in] */ IView* view)
{
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 direction;
        view->GetLayoutDirection(&direction);
        return IView::LAYOUT_DIRECTION_RTL == direction;
    }
    else {
        return FALSE;
    }
}

Boolean SwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);

    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mDragging = FALSE;
            mCurrView = NULL;
            mCallback->GetChildAtPosition(ev, (IView**)&mCurrView);
            mVelocityTracker->Clear();
            if (mCurrView != NULL) {
                mRtl = IsLayoutRtl(mCurrView);
                mCallback->CanChildBeDismissed(mCurrView, &mCanCurrViewBeDimissed);
                mVelocityTracker->AddMovement(ev);
                mInitialTouchPos = GetPos(ev);
            }
            else {
                mCanCurrViewBeDimissed = FALSE;
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL) {
                mVelocityTracker->AddMovement(ev);
                Float pos = GetPos(ev);
                Float delta = pos - mInitialTouchPos;
                if (Elastos::Core::Math::Abs(delta) > mPagingTouchSlop) {
                    mCallback->OnBeginDrag(mCurrView);
                    mDragging = TRUE;
                    mInitialTouchPos = pos - GetTranslation(mCurrView);
                }
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mDragging = FALSE;
            mCurrView = NULL;
            break;
    }
    return mDragging;
}

/**
 * @param view The view to be dismissed
 * @param velocity The desired pixels/second speed at which the view should move
 */
void SwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    Float newPos;
    if (velocity < 0 || (velocity == 0 && GetTranslation(view) < 0)
        // if we use the Menu to dismiss an item in landscape, animate up
        || (velocity == 0 && GetTranslation(view) == 0 && mSwipeDirection == Y)) {
        newPos = -GetSize(view);
    }
    else {
        newPos = GetSize(view);
    }
    Int32 duration = MAX_ESCAPE_ANIMATION_DURATION;
    if (velocity != 0) {
        duration = Elastos::Core::Math::Min(duration,
            (Int32) (Elastos::Core::Math::Abs(newPos - GetTranslation(view)) *
                    1000.0f / Elastos::Core::Math::Abs(velocity)));
    }
    else {
        duration = DEFAULT_ESCAPE_ANIMATION_DURATION;
    }

    AutoPtr<IValueAnimator> anim = IValueAnimator::Probe(CreateTranslationAnimation(view, newPos));
    IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(sLinearInterpolator));
    anim->SetDuration(duration);
    AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(
            mCallback, view, canAnimViewBeDismissed);
    IAnimator::Probe(anim)->AddListener(adapter);
    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(
            this, view, canAnimViewBeDismissed);
    anim->AddUpdateListener(listener);
    IAnimator::Probe(anim)->Start();
}


void SwipeHelper::SnapChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    AutoPtr<IValueAnimator> anim = IValueAnimator::Probe(CreateTranslationAnimation(view, 0));
    Int32 duration = SNAP_ANIM_LEN;
    anim->SetDuration(duration);
    IAnimator::Probe(anim)->SetInterpolator(
        ITimeInterpolator::Probe(RecentsConfiguration::GetInstance()->mLinearOutSlowInInterpolator));
    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener2(
            this, view, canAnimViewBeDismissed);
    anim->AddUpdateListener(listener);
    AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter2(
            mCallback, view, canAnimViewBeDismissed);
    IAnimator::Probe(anim)->AddListener(adapter);
    IAnimator::Probe(anim)->Start();
}

Boolean SwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (!mDragging) {
        if (!OnInterceptTouchEvent(ev)) {
            return mCanCurrViewBeDimissed;
        }
    }

    mVelocityTracker->AddMovement(ev);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_OUTSIDE:
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL) {
                Float delta = GetPos(ev) - mInitialTouchPos;
                SetSwipeAmount(delta);
                mCallback->OnSwipeChanged(mCurrView, delta);
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            if (mCurrView != NULL) {
                EndSwipe(mVelocityTracker);
            }
            break;
    }
    return TRUE;
}

void SwipeHelper::SetSwipeAmount(
    /* [in] */ Float amount)
{
    // don't let items that can't be dismissed be dragged more than
    // maxScrollDistance
    Boolean canChildBeDismissed;
    if (CONSTRAIN_SWIPE && (!IsValidSwipeDirection(amount) ||
        !(mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed), canChildBeDismissed))) {
        Float size = GetSize(mCurrView);
        Float maxScrollDistance = 0.15f * size;
        if (Elastos::Core::Math::Abs(amount) >= size) {
            amount = amount > 0 ? maxScrollDistance : -maxScrollDistance;
        }
        else {
            amount = maxScrollDistance * (Float) Elastos::Core::Math::Sin((amount/size)*(Elastos::Core::Math::PI/2));
        }
    }
    SetTranslation(mCurrView, amount);
    if (FADE_OUT_DURING_SWIPE && mCanCurrViewBeDimissed) {
        Float alpha = GetAlphaForOffset(mCurrView);
        mCurrView->SetAlpha(alpha);
    }
}

Boolean SwipeHelper::IsValidSwipeDirection(
    /* [in] */ Float amount)
{
    if (mSwipeDirection == X) {
        if (mRtl) {
            return (amount <= 0) ? mAllowSwipeTowardsEnd : mAllowSwipeTowardsStart;
        }
        else {
            return (amount <= 0) ? mAllowSwipeTowardsStart : mAllowSwipeTowardsEnd;
        }
    }

    // Vertical swipes are always valid.
    return TRUE;
}

void SwipeHelper::EndSwipe(
    /* [in] */ IVelocityTracker* velocityTracker)
{
    Float maxVelocity = MAX_DISMISS_VELOCITY * mDensityScale;
    velocityTracker->ComputeCurrentVelocity(1000 /* px/sec */, maxVelocity);
    Float velocity = GetVelocity(velocityTracker);
    Float perpendicularVelocity = GetPerpendicularVelocity(velocityTracker);
    Float escapeVelocity = SWIPE_ESCAPE_VELOCITY * mDensityScale;
    Float translation = GetTranslation(mCurrView);
    // Decide whether to dismiss the current view
    Boolean childSwipedFarEnough = DISMISS_IF_SWIPED_FAR_ENOUGH &&
            Elastos::Core::Math::Abs(translation) > 0.6 * GetSize(mCurrView);
    Boolean childSwipedFastEnough = (Elastos::Core::Math::Abs(velocity) > escapeVelocity) &&
            (Elastos::Core::Math::Abs(velocity) > Elastos::Core::Math::Abs(perpendicularVelocity)) &&
            (velocity > 0) == (translation > 0);

    Boolean canChildBeDismissed;
    mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed);
    Boolean dismissChild = canChildBeDismissed
            && IsValidSwipeDirection(translation)
            && (childSwipedFastEnough || childSwipedFarEnough);

    if (dismissChild) {
        // flingadingy
        DismissChild(mCurrView, childSwipedFastEnough ? velocity : 0.0f);
    }
    else {
        // snappity
        mCallback->OnDragCancelled(mCurrView);
        SnapChild(mCurrView, velocity);
    }
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
