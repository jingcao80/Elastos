
#include "SwipeHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

//=================================================================
// SwipeHelper::DismissAnimatorListenerAdapter
//=================================================================
SwipeHelper::DismissAnimatorListenerAdapter::DismissAnimatorListenerAdapter(
    /* [in] */ SwipeHelper* host)
    : mHost(host)
{}

ECode SwipeHelper::DismissAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->OnChildDismissed(view);
    animView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);

    return NOERROR;
}

//=================================================================
// SwipeHelper::DismissAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_DECL(SwipeHelper::DismissAnimatorUpdateListener, Object, IAnimatorUpdateListener)

SwipeHelper::DismissAnimatorUpdateListener::DismissAnimatorUpdateListener(
    /* [in] */ SwipeHelper* host)
    : mHost(host)
{}

ECode SwipeHelper::DismissAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && canAnimViewBeDismissed) {
        animView->SetAlpha(mHost->GetAlphaForOffset(animView));
    }
    mHost->InvalidateGlobalRegion(animView);

    return NOERROR;
}

//=================================================================
// SwipeHelper::SnapAnimatorListenerAdapter
//=================================================================
SwipeHelper::SnapAnimatorListenerAdapter::SnapAnimatorListenerAdapter(
    /* [in] */ SwipeHelper* host)
    : mHost(host)
{}

ECode SwipeHelper::SnapAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    animView->SetAlpha(mHost->mStartAlpha);
    mHost->mCallback->OnDragCancelled(mHost->mCurrView);

    return NOERROR;
}

//=================================================================
// SwipeHelper::SnapAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_DECL(SwipeHelper::SnapAnimatorUpdateListener, Object, IAnimatorUpdateListener)

SwipeHelper::SnapAnimatorUpdateListener::SnapAnimatorUpdateListener(
    /* [in] */ SwipeHelper* host)
    : mHost(host)
{}

ECode SwipeHelper::SnapAnimatorUpdateListener::SnapAnimatorUpdateListener(
    /* [in] */ IValueAnimator* animation)
{
    if (FADE_OUT_DURING_SWIPE && canAnimViewBeDismissed) {
        animView->SetAlpha(mHost->GetAlphaForOffset(animView));
    }
    mHost->InvalidateGlobalRegion(animView);

    return NOERROR;
}

//=================================================================
// SwipeHelper
//=================================================================
AutoPtr<ILinearInterpolator> CreateInterpolator()
{
    AutoPtr<ILinearInterpolator> obj;
    CLinearInterpolator::New((ILinearInterpolator**)&obj);
    return obj;
}

AutoPtr<IInterface> CreateSWIPEABLE()
{
    AutoPtr<IInterface> obj;
    CObject::New((IInterface**)&obj);
    return obj;
}

const String TAG("SwipeHelper");
const AutoPtr<IInterface> IS_SWIPEABLE = CreateSWIPEABLE();

const Float ALPHA_FADE_END = 0.7f;

const Boolean DEBUG_INVALIDATE = FALSE;
const Boolean CONSTRAIN_SWIPE = TRUE;
const Boolean FADE_OUT_DURING_SWIPE = TRUE;
const Boolean DISMISS_IF_SWIPED_FAR_ENOUGH = TRUE;
const Boolean LOG_SWIPE_DISMISS_VELOCITY = FALSE;

AutoPtr<ILinearInterpolator> sLinearInterpolator = CreateInterpolator();

Int32 SWIPE_ESCAPE_VELOCITY = -1;
Int32 DEFAULT_ESCAPE_ANIMATION_DURATION;
Int32 MAX_ESCAPE_ANIMATION_DURATION;
Int32 MAX_DISMISS_VELOCITY;
Int32 SNAP_ANIM_LEN;
Int32 SWIPE_SCROLL_SLOP;
Float MIN_SWIPE;
Float MIN_VERT;
Float MIN_LOCK;
const Float FACTOR = 1.2f;

const Int32 PROTECTION_PADDING = 50;

CAR_INTERFACE_DECL(SwipeHelper, Object, ISwipeHelper)

SwipeHelper::SwipeHelper()
    : mMinAlpha(0.3f)
    , mProtected(FALSE)
    , mChildSwipedFarEnoughFactor(0.4f)
    , mChildSwipedFastEnoughFactor(0.05f)
{}

ECode SwipeHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 swipeDirection,
    /* [in] */ ISwipeHelperCallback* callback,;
    /* [in] */ Float densityScale,
    /* [in] */ Float pagingTouchSlop)
{
    mCallback = callback;
    mSwipeDirection = swipeDirection;
    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    helper->Obtain(&mVelocityTracker);
    mDensityScale = densityScale;
    mPagingTouchSlop = pagingTouchSlop;
    if (SWIPE_ESCAPE_VELOCITY == -1) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetInteger(R::integer::swipe_escape_velocity, &SWIPE_ESCAPE_VELOCITY);
        res->GetInteger(R::integer::escape_animation_duration, &DEFAULT_ESCAPE_ANIMATION_DURATION);
        res->GetInteger(R::integer::max_escape_animation_duration, &MAX_ESCAPE_ANIMATION_DURATION);
        res->GetInteger(R::integer::max_dismiss_velocity, &MAX_DISMISS_VELOCITY);
        res->GetInteger(R::integer::snap_animation_duration, &SNAP_ANIM_LEN);
        res->GetInteger(R.integer.swipe_scroll_slop, &SWIPE_SCROLL_SLOP);
        res->GetDimension(R.dimen.min_swipe, &MIN_SWIPE);
        res->GetDimension(R.dimen.min_vert, &MIN_VERT);
        res->GetDimension(R.dimen.min_lock, &MIN_LOCK);
    }

    return NOERROR;
}

ECode SwipeHelper::SetDensityScale(
    /* [in] */ Float densityScale)
{
    mDensityScale = densityScale;
    return NOERROR;
}

ECode SwipeHelper::SetPagingTouchSlop(
    /* [in] */ Float pagingTouchSlop)
{
    mPagingTouchSlop = pagingTouchSlop;
    return NOERROR;
}

ECode SwipeHelper::SetChildSwipedFarEnoughFactor(
    /* [in] */ Float factor)
{
    mChildSwipedFarEnoughFactor = factor;
    return NOERROR;
}

ECode SwipeHelper::SetChildSwipedFastEnoughFactor(
    /* [in] */ Float factor)
{
    mChildSwipedFastEnoughFactor = factor;
    return NOERROR;
}

Float SwipeHelper::GetVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float velocity;
    mSwipeDirection == ISwipeHelper::X ? vt->GetXVelocity(&velocity) :
                vt->GetYVelocity(&velocity);
    return velocity;
}

AutoPtr<IObjectAnimator> SwipeHelper::CreateTranslationAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos)
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> anim;
    helper->OfFloat(v,
            mSwipeDirection == ISwipeHelper::X ? String("translationX") : String("translationY"),
            newPos, (IObjectAnimator**)&anim);
    return anim;
}

AutoPtr<IObjectAnimator> SwipeHelper::CreateDismissAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos,
    /* [in] */ Int32 duration)
{
    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(v, newPos);
    anim->SetInterpolator(sLinearInterpolator);
    anim->SetDuration(duration);
    return anim;
}

Float SwipeHelper::GetPerpendicularVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float velocity;
    mSwipeDirection == ISwipeHelper::X ? vt->GetYVelocity(&velocity) :
                vt->GetXVelocity(&velocity);
    return velocity;
}

void SwipeHelper::SetTranslation(
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

Float SwipeHelper::GetSize(
    /* [in] */ IView* v)
{
    Float size;
    mSwipeDirection == X ? v->GetMeasuredWidth(&size) :
                v->GetMeasuredHeight(&size);
    return size;
}

ECode SwipeHelper::SetMinAlpha(
    /* [in] */ Float minAlpha)
{
    mMinAlpha = minAlpha;
    return NOERROR;
}

Float SwipeHelper::GetAlphaForOffset(
    /* [in] */ IView* view)
{
    Float viewSize = GetSize(view);
    const Float fadeSize = ALPHA_FADE_END * viewSize;
    Float result = mStartAlpha;
    Float pos;
    view->GetTranslationX(&pos);
    if (pos >= viewSize * ALPHA_FADE_START) {
        result = mStartAlpha - (pos - viewSize * ALPHA_FADE_START) / fadeSize;
    }
    else if (pos < viewSize * (mStartAlpha - ALPHA_FADE_START)) {
        result = mStartAlpha + (viewSize * ALPHA_FADE_START + pos) / fadeSize;
    }
    return Math::Max(mMinAlpha, result);
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
        matrix->MapRect(childBounds);

        Float left, top, right, bottom;
        childBounds->GetLeft(&left);
        childBounds->GetTop(&top);
        childBounds->GetRight(&right);
        childBounds->GetBottom(&bottom);
        view->Invalidate((Int32) Math::Floor(left), (Int32) Math::Floor(top),
                (Int32) Math::Ceil(right), (Int32) Math::Ceil(bottom));
        if (DEBUG_INVALIDATE) {
            Logger::V(TAG, "INVALIDATE(%d,%d,%d,%d", (Int32) Math::Floor(left),
                    (Int32) Math::Floor(top), (Int32) Math::Ceil(right),
                    (Int32) Math::Ceil(bottom));
        }
    }
}

ECode SwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            ev->GetY(&mLastY);
            mDragging = FALSE;
            mCallback->GetChildAtPosition(ev, &mCurrView);
            mVelocityTracker->Clear();
            if (mCurrView != NULL) {
                mCallback->GetChildContentView(mCurrView, &mCurrAnimView);
                mCurrAnimView->GetAlpha(&mStartAlpha);
                mCallback->CanChildBeDismissed(mCurrView, &mCanCurrViewBeDimissed);
                mVelocityTracker->AddMovement(ev);
                ev->GetX(&mInitialTouchPosX);
                ev->GetY(&mInitialTouchPosY);
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != null) {
                // Check the movement direction.
                if (mLastY >= 0 && !mDragging) {
                    Float currY;
                    ev->GetY(&currY);
                    Float currX;
                    ev->GetX(&currX);
                    Float deltaY = Math::Abs(currY - mInitialTouchPosY);
                    Float deltaX = Math::Abs(currX - mInitialTouchPosX);
                    if (deltaY > SWIPE_SCROLL_SLOP && deltaY > (FACTOR * deltaX)) {
                        ev->GetY(&mLastY);
                        mCallback->OnScroll();
                        *result = FALSE;
                        return NOERROR;
                    }
                }
                mVelocityTracker->AddMovement(ev);
                Float pos;
                ev->GetX(&pos);
                Float delta = pos - mInitialTouchPosX;
                if (Math::Abs(delta) > mPagingTouchSlop) {
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
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mDragging = FALSE;
            mCurrView = NULL;
            mCurrAnimView = NULL;
            mLastY = -1;
            break;
    }
    *result = mDragging;
    return NOERROR;
}

void SwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->GanChildBeDismissed(view, &canAnimViewBeDismissed);
    Float newPos = DeterminePos(animView, velocity);
    Int32 duration = DetermineDuration(animView, newPos, velocity);

    animView->SetLayerType(View.LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<IObjectAnimator> anim = createDismissAnimation(animView, newPos, duration);
    anim->AddListener((IAnimatorListener*)new DismissAnimatorListenerAdapter(this));
    anim->AddUpdateListener((IAnimatorUpdateListener*)new DismissAnimatorUpdateListener(this));
    anim->Start();
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
        duration = Math::Min(duration,
                    (Int32) (Math::Abs(newPos - x) * 1000f / Math::abs(velocity)));
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
    if (velocity < 0 || (velocity == 0 && animView->GetTranslationX(&x), x < 0)
            // if we use the Menu to dismiss an item in landscape, animate up
            || (velocity == 0 && animView.getTranslationX(&x), x == 0
            && mSwipeDirection == ISwipeHelper::Y)) {
        newPos = -GetSize(animView);
    }
    else {
        newPos = GetSize(animView);
    }
    return newPos;
}

ECode SwipeHelper::SnapChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    AutoPtr<IObjectAnimator> anim = createTranslationAnimation(animView, 0);
    Int32 duration = SNAP_ANIM_LEN;
    anim->SetDuration(duration);
    anim->AddUpdateListener((IAnimatorUpdateListener*)new SnapAnimatorUpdateListener(this));
    anim->AddListener((IAnimatorListener*)new SnapAnimatorListenerAdapter(this));
    anim->Start();

    return NOERROR;
}

ECode SwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(reuslt);

    if (!mDragging || mProtected) {
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
                Float x, y;
                ev->GetX(&x);
                ev->GetY(&y);
                Float deltaX = x - mInitialTouchPosX;
                Float deltaY = Math::Abs(y - mInitialTouchPosY);
                // If the user has gone vertical and not gone horizontalish AT
                // LEAST minBeforeLock, switch to scroll. Otherwise, cancel
                // the swipe.
                if (!mDragging && deltaY > MIN_VERT && (Math::Abs(deltaX)) < MIN_LOCK
                        && deltaY > (FACTOR * Math::Abs(deltaX))) {
                    mCallback->OnScroll();
                    *result = FALSE;
                    return NOERROR;
                }
                Float minDistance = MIN_SWIPE;
                if (Math::Abs(deltaX) < minDistance) {
                    // Don't start the drag until at least X distance has
                    // occurred.
                    *result = TRUE;
                    return NOERROR;
                }
                // don't let items that can't be dismissed be dragged more
                // than maxScrollDistance
                Boolean canChildBeDismissed;
                if (CONSTRAIN_SWIPE &&
                        mCallback.canChildBeDismissed(mCurrView, &canChildBeDismissed),
                        !canChildBeDismissed) {
                    Float size = getSize(mCurrAnimView);
                    Float maxScrollDistance = 0.15f * size;
                    if (Math::Abs(deltaX) >= size) {
                        deltaX = deltaX > 0 ? maxScrollDistance : -maxScrollDistance;
                    }
                    else {
                        deltaX = maxScrollDistance
                                * (Float) Math::Sin((deltaX / size) * (Math.PI / 2));
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
                Float translation = Math::Abs(x);
                Float currAnimViewSize = GetSize(mCurrAnimView);
                // Long swipe = translation of {@link #mChildSwipedFarEnoughFactor} * width
                Boolean childSwipedFarEnough = DISMISS_IF_SWIPED_FAR_ENOUGH
                        && translation > mChildSwipedFarEnoughFactor * currAnimViewSize;
                // Fast swipe = > escapeVelocity and translation of
                // {@link #mChildSwipedFastEnoughFactor} * width
                Boolean childSwipedFastEnough = (Math::Abs(velocity) > escapeVelocity)
                        && (Math.abs(velocity) > Math::Abs(perpendicularVelocity))
                        && (velocity > 0) == (x > 0)
                        && translation > mChildSwipedFastEnoughFactor * currAnimViewSize;
                if (LOG_SWIPE_DISMISS_VELOCITY) {
                    Logger::V(TAG, "Swipe/Dismiss: %f/%f/%f, x: %f/%f", velocity, escapeVelocity,
                            perpendicularVelocity, translation, currAnimViewSize);
                }

                Boolean canChildBeDismissed;
                Boolean dismissChild = mCallback->CanChildBeDismissed(
                        mCurrView, &canChildBeDismissed), canChildBeDismissed
                        && (childSwipedFastEnough || childSwipedFarEnough);

                if (dismissChild) {
                    DismissChild(mCurrView, childSwipedFastEnough ? velocity : 0f);
                }
                else {
                    SnapChild(mCurrView, velocity);
                }
            }
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void SwipeHelper::SetSwipeable(
    /* [in] */ IView* view,
    /* [in] */ Boolean swipeable)
{
    view->SetTag(IS_SWIPEABLE_TAG, swipeable ? IS_SWIPEABLE : null);
}

Boolean SwipeHelper::IsSwipeable(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag(IS_SWIPEABLE_TAG, (IInterface**)&tag);
    return IObject::Probe(IS_SWIPEABLE).Equals(tag);
}

} // List
} // Dialer
} // Apps
} // Elastos
