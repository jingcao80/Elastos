#include "elastos/droid/systemui/CSwipeHelper.h"
#include <elastos/core/Math.h>
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Core::Math;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::Animation::CLinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_INTERFACE_IMPL(CSwipeHelper::AnimatorUpdateListenerOne, IAnimatorUpdateListener)


//===============================================================================
//          CSwipeHelper::WatchLongPressRunnable
//===============================================================================
CSwipeHelper::WatchLongPressRunnable::WatchLongPressRunnable(
    /* [in] */ CSwipeHelper* host)
    : mHost(host)
{
}

ECode CSwipeHelper::WatchLongPressRunnable::Run()
{
    if (mHost->mCurrView != NULL && !mHost->mLongPressSent) {
        Boolean result;
        mHost->mLongPressSent = TRUE;
        AutoPtr<IAccessibilityEventSource> ae = IAccessibilityEventSource::Probe(mHost->mCurrView.Get());
        assert(ae != NULL);
        ae->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        mHost->mLongPressListener->OnLongClick(mHost->mCurrView, &result);
    }
    return NOERROR;
}

CSwipeHelper::AnimatorListenerAdapterOne::AnimatorListenerAdapterOne(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IView* animView,
    /* [in] */ IView* view)
    : mHost(host)
    , mAnimView(animView)
    , mView(view)
{
}

ECode CSwipeHelper::AnimatorListenerAdapterOne::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->OnChildDismissed(mView);
    mAnimView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    return NOERROR;
}

CSwipeHelper::AnimatorUpdateListenerOne::AnimatorUpdateListenerOne(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IView* animView,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mHost(host)
    , mAnimView(animView)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{
}

ECode CSwipeHelper::AnimatorUpdateListenerOne::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (CSwipeHelper::FADE_OUT_DURING_SWIPE && mCanAnimViewBeDismissed) {
        mAnimView->SetAlpha(mHost->GetAlphaForOffset(mAnimView));
    }
    return mHost->InvalidateGlobalRegion(mAnimView);
}

//===============================================================================
//          CSwipeHelper
//===============================================================================
AutoPtr<ILinearInterpolator> InitLinearInterpolator()
{
    AutoPtr<ILinearInterpolator> li;
    CLinearInterpolator::New((ILinearInterpolator**)&li);
    return li;
}

const String CSwipeHelper::TAG("com.android.systemui.SwipeHelper");
const Boolean CSwipeHelper::DEBUG = FALSE;
const Boolean CSwipeHelper::DEBUG_INVALIDATE = FALSE;
const Boolean CSwipeHelper::SLOW_ANIMATIONS = FALSE; // DEBUG;
const Boolean CSwipeHelper::CONSTRAIN_SWIPE = TRUE;
const Boolean CSwipeHelper::FADE_OUT_DURING_SWIPE = TRUE;
const Boolean CSwipeHelper::DISMISS_IF_SWIPED_FAR_ENOUGH = TRUE;

AutoPtr<ILinearInterpolator> CSwipeHelper::sLinearInterpolator = InitLinearInterpolator();
const Int32 CSwipeHelper::SNAP_ANIM_LEN = 150;//SLOW_ANIMATIONS ? 1000 : 150; // ms

CSwipeHelper::CSwipeHelper()
    : SWIPE_ESCAPE_VELOCITY(100.f)
    , DEFAULT_ESCAPE_ANIMATION_DURATION(200)
    , MAX_ESCAPE_ANIMATION_DURATION(400)
    , MAX_DISMISS_VELOCITY(2000)
    , mMinAlpha(0.0f)
    , mCallback(NULL)
    , mPagingTouchSlop(0.0f)
    , mSwipeDirection(0)
    , mInitialTouchPos(0)
    , mDragging(FALSE)
    , mCanCurrViewBeDimissed(FALSE)
    , mDensityScale(FALSE)
    , mLongPressSent(FALSE)
    , mLongPressTimeout(0)
{}

ECode CSwipeHelper::constructor(
    /* [in] */ Int32 swipeDirection,
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ Float densityScale,
    /* [in] */ Float pagingTouchSlop)
{
    mCallback = callback;
    CHandler::New((IHandler**)&mHandler);
    mSwipeDirection = swipeDirection;
    mVelocityTracker = VelocityTracker::Obtain();
    mDensityScale = densityScale;
    mPagingTouchSlop = pagingTouchSlop;

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    Int32 timeOUt;
    helper->GetLongPressTimeout(&timeOUt);
    mLongPressTimeout = (Int64) (timeOUt * 1.5f); // extra long-press!
    return NOERROR;
}

ECode CSwipeHelper::SetLongPressListener(
    /* [in] */ IViewOnLongClickListener* listener)
{
    mLongPressListener = listener;
    return NOERROR;
}

ECode CSwipeHelper::SetDensityScale(
    /* [in] */ Float densityScale)
{
    mDensityScale = densityScale;
    return NOERROR;
}

ECode CSwipeHelper::SetPagingTouchSlop(
    /* [in] */ Float pagingTouchSlop)
{
    mPagingTouchSlop = pagingTouchSlop;
    return NOERROR;
}

Float CSwipeHelper::GetPos(
    /* [in] */ IMotionEvent* ev)
{
    assert(ev != NULL);
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    return mSwipeDirection == ISwipeHelper::X ? x : y;
}

Float CSwipeHelper::GetTranslation(
    /* [in] */ IView* v)
{
    Float tx, ty;
    v->GetTranslationX(&tx);
    v->GetTranslationY(&ty);
    return mSwipeDirection == ISwipeHelper::X ? tx : ty;
}

Float CSwipeHelper::GetVelocity(
    /* [in] */ VelocityTracker* vt)
{
    Float x, y;
    vt->GetXVelocity(&x);
    vt->GetYVelocity(&y);
    return mSwipeDirection == ISwipeHelper::X ? x : y;
}

AutoPtr<IObjectAnimator> CSwipeHelper::CreateTranslationAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos)
{
    AutoPtr<IObjectAnimator> anim;
    String info = mSwipeDirection == ISwipeHelper::X ?
        String("translationX") : String("translationY");

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(1);
    floats->Set(0, newPos);
    helper->OfFloat(v, info, floats, (IObjectAnimator**)&anim);
    return anim;
}

Float CSwipeHelper::GetPerpendicularVelocity(
    /* [in] */ VelocityTracker* vt)
{
    Float x, y;
    vt->GetYVelocity(&y);
    vt->GetXVelocity(&x);
    return mSwipeDirection == ISwipeHelper::X ? y : x;
}

void CSwipeHelper::SetTranslation(
    /* [in] */ IView* v,
    /* [in] */ Float translate)
{
    if (mSwipeDirection == ISwipeHelper::X) {
        v->SetTranslationX(translate);
    }
    else {
        v->SetTranslationY(translate);
    }
}

Float CSwipeHelper::GetSize(
    /* [in] */ IView* v)
{
    Int32 mw, mh;
    v->GetMeasuredWidth(&mw);
    v->GetMeasuredHeight(&mh);
    return mSwipeDirection == ISwipeHelper::X ? mw : mh;
}

ECode CSwipeHelper::SetMinAlpha(
    /* [in] */ Float minAlpha)
{
    mMinAlpha = minAlpha;
    return NOERROR;
}

Float CSwipeHelper::GetAlphaForOffset(
    /* [in] */ IView* view)
{
    using Elastos::Core::Math;
    Float viewSize = GetSize(view);
    Float fadeSize = ISwipeHelper::ALPHA_FADE_END * viewSize;
    Float result = 1.0f;
    Float pos = GetTranslation(view);
    if (pos >= viewSize * ISwipeHelper::ALPHA_FADE_START) {
        result = 1.0f - (pos - viewSize * ISwipeHelper::ALPHA_FADE_START) / fadeSize;
    } else if (pos < viewSize * (1.0f - ISwipeHelper::ALPHA_FADE_START)) {
        result = 1.0f + (viewSize * ISwipeHelper::ALPHA_FADE_START + pos) / fadeSize;
    }
    return Math::Max(mMinAlpha, result);
}

// invalidate the view's own bounds all the way up the view hierarchy
ECode CSwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view)
{
    AutoPtr<IRectF> rectf;
    Int32 l, t, r, b;
    view->GetLeft(&l);
    view->GetTop(&t);
    view->GetRight(&r);
    view->GetBottom(&b);
    CRectF::New(l, t, r, b, (IRectF**)&rectf);
    return InvalidateGlobalRegion(view, rectf);
}

// invalidate a rectangle relative to the view's coordinate system all the way up the view
// hierarchy
ECode CSwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view,
    /* [in] */ IRectF* childBounds)
{
    //childBounds.offset(view.getTranslationX(), view.getTranslationY());
    // if (DEBUG_INVALIDATE)
    //     Log.v(TAG, "-------------");

    using Elastos::Core::Math;
    AutoPtr<IViewParent> parent;
    AutoPtr<IView> tmpView = view;
    Boolean result;
    Float l, t, r, b;

    while (tmpView->GetParent((IViewParent**)&parent) && IView::Probe(parent) != NULL) {
        tmpView = IView::Probe(parent);
        parent = NULL;

        AutoPtr<IMatrix> matrix;
        tmpView->GetMatrix((IMatrix**)&matrix);
        matrix->MapRect(childBounds, &result);
        childBounds->Get(&l, &t, &r, &b);
        tmpView->Invalidate(
            (Int32) Math::Floor(l),
            (Int32) Math::Floor(t),
            (Int32) Math::Ceil(r),
            (Int32) Math::Ceil(b));
        // if (DEBUG_INVALIDATE) {
        //     Log.v(TAG, "INVALIDATE(" + (Int32) Math.floor(childBounds.left)
        //             + "," + (Int32) Math.floor(childBounds.top)
        //             + "," + (Int32) Math.ceil(childBounds.right)
        //             + "," + (Int32) Math.ceil(childBounds.bottom));
        // }
    }
    return NOERROR;
}

ECode CSwipeHelper::RemoveLongPressCallback()
{
    if (mWatchLongPress != NULL) {
        mHandler->RemoveCallbacks(mWatchLongPress);
        mWatchLongPress = NULL;
    }
    return NOERROR;
}

ECode CSwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    using Elastos::Core::Math;

    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mDragging = FALSE;
            mLongPressSent = FALSE;
            mCurrView = NULL;
            mCallback->GetChildAtPosition(ev, (IView**)&mCurrView);
            mVelocityTracker->Clear();
            if (mCurrView != NULL) {
                mCurrAnimView = NULL;
                mCallback->GetChildContentView(mCurrView, (IView**)&mCurrAnimView);
                mCallback->CanChildBeDismissed(mCurrView, &mCanCurrViewBeDimissed);
                mVelocityTracker->AddMovement(ev);
                mInitialTouchPos = GetPos(ev);

                if (mLongPressListener != NULL) {
                    if (mWatchLongPress == NULL) {
                        mWatchLongPress = new WatchLongPressRunnable(this);
                    }

                    Boolean result;
                    mHandler->PostDelayed(mWatchLongPress, mLongPressTimeout, &result);
                }

            }
            break;

        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL && !mLongPressSent) {
                mVelocityTracker->AddMovement(ev);
                Float pos = GetPos(ev);
                Float delta = pos - mInitialTouchPos;
                if (Math::Abs(delta) > mPagingTouchSlop) {
                    mCallback->OnBeginDrag(mCurrView);
                    mDragging = TRUE;
                    mInitialTouchPos = GetPos(ev) - GetTranslation(mCurrAnimView);

                    RemoveLongPressCallback();
                }
            }

            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mDragging = FALSE;
            mCurrView = NULL;
            mCurrAnimView = NULL;
            mLongPressSent = FALSE;
            RemoveLongPressCallback();
            break;
    }

    *result = mDragging;
    return NOERROR;
}

ECode CSwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    using Elastos::Core::Math;

    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    Float newPos;

    if (velocity < 0
            || (velocity == 0 && GetTranslation(animView) < 0)
            // if we use the Menu to dismiss an item in landscape, animate up
            || (velocity == 0 && GetTranslation(animView) == 0 && mSwipeDirection == Y)) {
        newPos = -GetSize(animView);
    } else {
        newPos = GetSize(animView);
    }
    Int32 duration = MAX_ESCAPE_ANIMATION_DURATION;
    if (velocity != 0) {
        duration = Math::Min(duration,
            (Int32) (Math::Abs(newPos - GetTranslation(animView)) * 1000.0f / Math::Abs(velocity)));
    } else {
        duration = DEFAULT_ESCAPE_ANIMATION_DURATION;
    }

    animView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(animView, newPos);
    if (anim) {
        anim->SetInterpolator(sLinearInterpolator);
        anim->SetDuration(duration);

        AutoPtr<AnimatorListenerAdapterOne> adapter = new AnimatorListenerAdapterOne(this, animView, view);
        anim->AddListener(adapter);
        AutoPtr<AnimatorUpdateListenerOne> listener = new AnimatorUpdateListenerOne(this, animView, canAnimViewBeDismissed);
        anim->AddUpdateListener(listener);

        anim->Start();
    }
    return NOERROR;
}

ECode CSwipeHelper::SnapChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);

    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(animView, 0);
    if (anim) {
        Int32 duration = SNAP_ANIM_LEN;
        anim->SetDuration(duration);
        AutoPtr<AnimatorUpdateListenerOne> listener = new AnimatorUpdateListenerOne(this, animView, canAnimViewBeDismissed);
        anim->AddUpdateListener(listener);
        anim->Start();
    }

    return NOERROR;
}

ECode CSwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    using Elastos::Core::Math;

    VALIDATE_NOT_NULL(result);

    if (mLongPressSent) {
        *result = TRUE;
        return NOERROR;
    }

    if (!mDragging) {
        // We are not doing anything, make sure the long press callback
        // is not still ticking like a bomb waiting to go off.
        RemoveLongPressCallback();
        *result = FALSE;
        return NOERROR;
    }

    mVelocityTracker->AddMovement(ev);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_OUTSIDE:
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL) {
                Float delta = GetPos(ev) - mInitialTouchPos;
                // don't let items that can't be dismissed be dragged more than
                // maxScrollDistance
                Boolean canChildBeDismissed;
                mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed);
                if (CONSTRAIN_SWIPE && !canChildBeDismissed) {
                    Float size = GetSize(mCurrAnimView);
                    Float maxScrollDistance = 0.15f * size;
                    if (Math::Abs(delta) >= size) {
                        delta = delta > 0 ? maxScrollDistance : -maxScrollDistance;
                    } else {
                        delta = maxScrollDistance * (Float) Math::Sin((delta/size)*(Math::DOUBLE_PI/2));
                    }
                }
                SetTranslation(mCurrAnimView, delta);
                if (FADE_OUT_DURING_SWIPE && mCanCurrViewBeDimissed) {
                    mCurrAnimView->SetAlpha(GetAlphaForOffset(mCurrAnimView));
                }
                InvalidateGlobalRegion(mCurrView);
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
                Boolean childSwipedFarEnough = DISMISS_IF_SWIPED_FAR_ENOUGH &&
                        Math::Abs(GetTranslation(mCurrAnimView)) > 0.4 * GetSize(mCurrAnimView);
                Boolean childSwipedFastEnough = (Math::Abs(velocity) > escapeVelocity) &&
                        (Math::Abs(velocity) > Math::Abs(perpendicularVelocity)) &&
                        (velocity > 0) == (GetTranslation(mCurrAnimView) > 0);

                Boolean dismissChild;
                mCallback->CanChildBeDismissed(mCurrView, &dismissChild);
                dismissChild = (dismissChild && (childSwipedFastEnough || childSwipedFarEnough));

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
            break;
    }

    *result = TRUE;
    return NOERROR;
}


}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
