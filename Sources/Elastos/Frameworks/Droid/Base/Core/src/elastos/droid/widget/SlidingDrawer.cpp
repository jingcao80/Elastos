
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/SlidingDrawer.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

const String SlidingDrawer::TAG("SlidingDrawer");
const Int32 SlidingDrawer::TAP_THRESHOLD;
const Float SlidingDrawer::MAXIMUM_TAP_VELOCITY;
const Float SlidingDrawer::MAXIMUM_MINOR_VELOCITY;
const Float SlidingDrawer::MAXIMUM_MAJOR_VELOCITY;
const Float SlidingDrawer::MAXIMUM_ACCELERATION;
const Int32 SlidingDrawer::VELOCITY_UNITS;
const Int32 SlidingDrawer::MSG_ANIMATE;
const Int32 SlidingDrawer::ANIMATION_FRAME_DURATION;
const Int32 SlidingDrawer::EXPANDED_FULL_OPEN;
const Int32 SlidingDrawer::COLLAPSED_FULL_CLOSED;

CAR_INTERFACE_IMPL(SlidingDrawer::DrawerToggler, Object, IViewOnClickListener)

SlidingDrawer::SlidingHandler::SlidingHandler(
    /* [in] */ SlidingDrawer* host)
    : mHost(host)
{}

ECode SlidingDrawer::SlidingHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case SlidingDrawer::MSG_ANIMATE:
            mHost->DoAnimation();
            break;
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(SlidingDrawer, ViewGroup, ISlidingDrawer)

SlidingDrawer::SlidingDrawer()
    : mHandleId(0)
    , mContentId(0)
    , mTracking(FALSE)
    , mLocked(FALSE)
    , mVertical(FALSE)
    , mExpanded(FALSE)
    , mBottomOffset(0)
    , mTopOffset(0)
    , mHandleHeight(0)
    , mHandleWidth(0)
    , mAnimatedAcceleration(0.f)
    , mAnimatedVelocity(0.f)
    , mAnimationPosition(0.f)
    , mAnimationLastTime(0)
    , mCurrentAnimationTime(0)
    , mTouchDelta(0)
    , mAnimating(FALSE)
    , mAllowSingleTap(FALSE)
    , mAnimateOnClick(FALSE)
    , mTapThreshold(0)
    , mMaximumTapVelocity(0)
    , mMaximumMinorVelocity(0)
    , mMaximumMajorVelocity(0)
    , mMaximumAcceleration(0)
    , mVelocityUnits(0)
{
    CRect::New((IRect**)&mFrame);
    CRect::New((IRect**)&mInvalidate);
    mHandler = new SlidingHandler(this);
}

ECode SlidingDrawer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode SlidingDrawer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SlidingDrawer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyleAttr, 0))
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SlidingDrawer);
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    Int32 orientation;
    a->GetInt32(R::styleable::SlidingDrawer_orientation, ISlidingDrawer::ORIENTATION_VERTICAL, &orientation);
    mVertical = orientation == ISlidingDrawer::ORIENTATION_VERTICAL;
    Float temp;
    a->GetDimension(R::styleable::SlidingDrawer_bottomOffset, 0.0f, &temp);
    mBottomOffset = (Int32)temp;
    a->GetDimension(R::styleable::SlidingDrawer_topOffset, 0.0f, &temp);
    mTopOffset = (Int32)temp;
    a->GetBoolean(R::styleable::SlidingDrawer_allowSingleTap, TRUE, &mAllowSingleTap);
    a->GetBoolean(R::styleable::SlidingDrawer_animateOnClick, TRUE, &mAnimateOnClick);

    Int32 handleId;
    a->GetResourceId(R::styleable::SlidingDrawer_handle, 0, &handleId);
    if (handleId == 0) {
        SLOGGERE(TAG, String("The handle attribute is required and must refer ")
                + "to a valid child.")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("The handle attribute is required and must refer "
        //         + "to a valid child.");
    }

    Int32 contentId;
    a->GetResourceId(R::styleable::SlidingDrawer_content, 0, &contentId);
    if (contentId == 0) {
        SLOGGERE(TAG, String("The content attribute is required and must refer ")
            + "to a valid child.")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("The content attribute is required and must refer "
        //         + "to a valid child.");
    }

    if (handleId == contentId) {
        SLOGGERE(TAG, String("The content and handle attributes must refer ")
            + "to different children.")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mHandleId = handleId;
    mContentId = contentId;

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mTapThreshold = (Int32) (TAP_THRESHOLD * density + 0.5f);
    mMaximumTapVelocity = (Int32) (MAXIMUM_TAP_VELOCITY * density + 0.5f);
    mMaximumMinorVelocity = (Int32) (MAXIMUM_MINOR_VELOCITY * density + 0.5f);
    mMaximumMajorVelocity = (Int32) (MAXIMUM_MAJOR_VELOCITY * density + 0.5f);
    mMaximumAcceleration = (Int32) (MAXIMUM_ACCELERATION * density + 0.5f);
    mVelocityUnits = (Int32) (VELOCITY_UNITS * density + 0.5f);

    a->Recycle();

    SetAlwaysDrawnWithCacheEnabled(FALSE);
    return NOERROR;
}

ECode SlidingDrawer::OnFinishInflate()
{
    FindViewById(mHandleId, (IView**)&mHandle);
    if (mHandle == NULL) {
        SLOGGERD(TAG, String("The handle attribute is must refer to an")
                + " existing child.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<DrawerToggler> toggler = new DrawerToggler(this);
    mHandle->SetOnClickListener(toggler);

    FindViewById(mContentId, (IView**)&mContent);
    if (mContent == NULL) {
        SLOGGERD(TAG, String("The content attribute is must refer to an")
                + " existing child.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContent->SetVisibility(IView::GONE);
    return NOERROR;
}

ECode SlidingDrawer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSpecSize =  MeasureSpec::GetSize(widthMeasureSpec);

    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSpecSize =  MeasureSpec::GetSize(heightMeasureSpec);

    if (widthSpecMode == MeasureSpec::UNSPECIFIED || heightSpecMode == MeasureSpec::UNSPECIFIED) {
        Slogger::D(TAG, "SlidingDrawer cannot have UNSPECIFIED dimensions");
        return NOERROR;
    }

    AutoPtr<IView> handle = mHandle;
    MeasureChild(handle, widthMeasureSpec, heightMeasureSpec);

    if (mVertical) {
        Int32 measuredHeght;
        handle->GetMeasuredHeight(&measuredHeght);
        Int32 height = heightSpecSize - measuredHeght - mTopOffset;
        mContent->Measure(MeasureSpec::MakeMeasureSpec(widthSpecSize, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY));
    } else {
        Int32 measuredWidth;
        handle->GetMeasuredWidth(&measuredWidth);
        Int32 width = widthSpecSize - measuredWidth - mTopOffset;
        mContent->Measure(MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(heightSpecSize, MeasureSpec::EXACTLY));
    }

    SetMeasuredDimension(widthSpecSize, heightSpecSize);
    return NOERROR;
}


ECode SlidingDrawer::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Int64 drawingTime;
    GetDrawingTime(&drawingTime);
    AutoPtr<IView> handle = mHandle;
    Boolean isVertical = mVertical;

    DrawChild(canvas, handle, drawingTime);

    if (mTracking || mAnimating) {
        AutoPtr<IBitmap> cache;
        mContent->GetDrawingCache((IBitmap**)&cache);
        if (cache != NULL) {
            if (isVertical) {
                Int32 bottom;
                handle->GetBottom(&bottom);
                canvas->DrawBitmap(cache, 0, bottom, NULL);
            } else {
                Int32 right;
                handle->GetRight(&right);
                canvas->DrawBitmap(cache, right, 0, NULL);
            }
        } else {
            Int32 rst;
            canvas->Save(&rst);
            Int32 left, top;
            handle->GetLeft(&left);
            handle->GetTop(&top);
            canvas->Translate(isVertical ? 0 : left - mTopOffset,
                    isVertical ? top - mTopOffset : 0);
            DrawChild(canvas, mContent, drawingTime);
            canvas->Restore();
        }
    } else if (mExpanded) {
        DrawChild(canvas, mContent, drawingTime);
    }
    return NOERROR;
}

ECode SlidingDrawer::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    if (mTracking) {
        return NOERROR;
    }

    Int32 width = r - l;
    Int32 height = b - t;

    AutoPtr<IView> handle = mHandle;

    Int32 childWidth;
    handle->GetMeasuredWidth(&childWidth);
    Int32 childHeight;
    handle->GetMeasuredHeight(&childHeight);

    Int32 childLeft;
    Int32 childTop;

    AutoPtr<IView> content = mContent;

    if (mVertical) {
        childLeft = (width - childWidth) / 2;
        childTop = mExpanded ? mTopOffset : height - childHeight + mBottomOffset;

        Int32 measuredWidth, measuredHeght;
        content->GetMeasuredWidth(&measuredWidth);
        content->GetMeasuredHeight(&measuredHeght);
        content->Layout(0, mTopOffset + childHeight, measuredWidth,
                mTopOffset + childHeight + measuredHeght);
    } else {
        childLeft = mExpanded ? mTopOffset : width - childWidth + mBottomOffset;
        childTop = (height - childHeight) / 2;

        Int32 measuredWidth, measuredHeght;
        content->GetMeasuredWidth(&measuredWidth);
        content->GetMeasuredHeight(&measuredHeght);
        content->Layout(mTopOffset + childWidth, 0,
                mTopOffset + childWidth + measuredWidth,
                measuredHeght);
    }

    handle->Layout(childLeft, childTop, childLeft + childWidth, childTop + childHeight);
    handle->GetHeight(&mHandleHeight);
    handle->GetWidth(&mHandleWidth);
    return NOERROR;
}

ECode SlidingDrawer::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mLocked) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 action;
    event->GetAction(&action);

    Float x;
    event->GetX(&x);
    Float y;
    event->GetY(&y);

    AutoPtr<IRect> frame = mFrame;
    AutoPtr<IView> handle = mHandle;

    handle->GetHitRect(frame);
    Boolean isContains;
    if (!mTracking && !(frame->Contains((Int32) x, (Int32) y, &isContains), isContains)) {
        *result = FALSE;
        return NOERROR;
    }

    if (action == IMotionEvent::ACTION_DOWN) {
        mTracking = TRUE;

        handle->SetPressed(TRUE);
        // Must be called before prepareTracking()
        PrepareContent();

        // Must be called after prepareContent()
        if (mOnDrawerScrollListener != NULL) {
            mOnDrawerScrollListener->OnScrollStarted();
        }

        if (mVertical) {
            Int32 top;
            mHandle->GetTop(&top);
            mTouchDelta = (Int32) y - top;
            PrepareTracking(top);
        } else {
            Int32 left;
            mHandle->GetLeft(&left);
            mTouchDelta = (Int32) x - left;
            PrepareTracking(left);
        }
        mVelocityTracker->AddMovement(event);
    }

    *result = TRUE;
    return NOERROR;
}

ECode SlidingDrawer::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mLocked) {
        *result = TRUE;
        return NOERROR;
    }

    if (mTracking) {
        mVelocityTracker->AddMovement(event);
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_MOVE:
            {
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                MoveHandle((Int32) (mVertical ? y : x) - mTouchDelta);
                break;
            }
            case IMotionEvent::ACTION_UP:
            case IMotionEvent::ACTION_CANCEL: {
                AutoPtr<VelocityTracker> velocityTracker = mVelocityTracker;
                velocityTracker->ComputeCurrentVelocity(mVelocityUnits);

                Float yVelocity;
                velocityTracker->GetYVelocity(&yVelocity);
                Float xVelocity;
                velocityTracker->GetXVelocity(&xVelocity);
                Boolean negative;

                Boolean vertical = mVertical;
                if (vertical) {
                    negative = yVelocity < 0;
                    if (xVelocity < 0) {
                        xVelocity = -xVelocity;
                    }
                    if (xVelocity > mMaximumMinorVelocity) {
                        xVelocity = mMaximumMinorVelocity;
                    }
                } else {
                    negative = xVelocity < 0;
                    if (yVelocity < 0) {
                        yVelocity = -yVelocity;
                    }
                    if (yVelocity > mMaximumMinorVelocity) {
                        yVelocity = mMaximumMinorVelocity;
                    }
                }

                Float velocity = (Float) Elastos::Core::Math::Hypot(xVelocity, yVelocity);
                if (negative) {
                    velocity = -velocity;
                }

                Int32 top;
                mHandle->GetTop(&top);
                Int32 left;
                mHandle->GetLeft(&left);

                if (Elastos::Core::Math::Abs(velocity) < mMaximumTapVelocity) {
                    if (vertical ? (mExpanded && top < mTapThreshold + mTopOffset) ||
                            (!mExpanded && top > mBottomOffset + mBottom - mTop -
                                    mHandleHeight - mTapThreshold) :
                            (mExpanded && left < mTapThreshold + mTopOffset) ||
                            (!mExpanded && left > mBottomOffset + mRight - mLeft -
                                    mHandleWidth - mTapThreshold)) {

                        if (mAllowSingleTap) {
                            PlaySoundEffect(SoundEffectConstants::CLICK);

                            if (mExpanded) {
                                AnimateClose(vertical ? top : left);
                            } else {
                                AnimateOpen(vertical ? top : left);
                            }
                        } else {
                            PerformFling(vertical ? top : left, velocity, FALSE);
                        }

                    } else {
                        PerformFling(vertical ? top : left, velocity, FALSE);
                    }
                } else {
                    PerformFling(vertical ? top : left, velocity, FALSE);
                }
            }
            break;
        }
    }

    ViewGroup::OnTouchEvent(event, result);
    if (*result == FALSE)
        *result = mTracking || mAnimating;
    return NOERROR;
}

ECode SlidingDrawer::Toggle()
{
    if (!mExpanded) {
        OpenDrawer();
    } else {
        CloseDrawer();
    }
    Invalidate();
    RequestLayout();
    return NOERROR;
}

ECode SlidingDrawer::AnimateToggle()
{
    if (!mExpanded) {
        AnimateOpen();
    } else {
        AnimateClose();
    }
    return NOERROR;
}

ECode SlidingDrawer::Open()
{
    OpenDrawer();
    Invalidate();
    RequestLayout();

    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    return NOERROR;
}

ECode SlidingDrawer::Close()
{
    CloseDrawer();
    Invalidate();
    RequestLayout();
    return NOERROR;
}

ECode SlidingDrawer::AnimateClose()
{
    PrepareContent();
    AutoPtr<IOnDrawerScrollListener> scrollListener = mOnDrawerScrollListener;
    if (scrollListener != NULL) {
        scrollListener->OnScrollStarted();
    }
    Int32 top, left;
    mHandle->GetTop(&top);
    mHandle->GetLeft(&left);
    AnimateClose(mVertical ? top : left);

    if (scrollListener != NULL) {
        scrollListener->OnScrollEnded();
    }
    return NOERROR;
}

ECode SlidingDrawer::AnimateOpen()
{
    PrepareContent();
    AutoPtr<IOnDrawerScrollListener> scrollListener = mOnDrawerScrollListener;
    if (scrollListener != NULL) {
        scrollListener->OnScrollStarted();
    }
    Int32 top, left;
    mHandle->GetTop(&top);
    mHandle->GetLeft(&left);
    AnimateOpen(mVertical ? top : left);

    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);

    if (scrollListener != NULL) {
        scrollListener->OnScrollEnded();
    }
    return NOERROR;
}

ECode SlidingDrawer::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CSlidingDrawer"));
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode SlidingDrawer::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CSlidingDrawer"));
    return info->SetClassName(seq);
}

ECode SlidingDrawer::SetOnDrawerOpenListener(
    /* [in] */ IOnDrawerOpenListener* onDrawerOpenListener)
{
    mOnDrawerOpenListener = onDrawerOpenListener;
    return NOERROR;
}

ECode SlidingDrawer::SetOnDrawerCloseListener(
    /* [in] */ IOnDrawerCloseListener* onDrawerCloseListener)
{
    mOnDrawerCloseListener = onDrawerCloseListener;
    return NOERROR;
}

ECode SlidingDrawer::SetOnDrawerScrollListener(
    /* [in] */ IOnDrawerScrollListener* onDrawerScrollListener)
{
    mOnDrawerScrollListener = onDrawerScrollListener;
    return NOERROR;
}

ECode SlidingDrawer::GetHandle(
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v)
    *v = mHandle;
    REFCOUNT_ADD(*v)
    return NOERROR;
}

ECode SlidingDrawer::GetContent(
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v)
    *v = mContent;
    REFCOUNT_ADD(*v)
    return NOERROR;
}

ECode SlidingDrawer::Unlock()
{
    mLocked = FALSE;
    return NOERROR;
}

ECode SlidingDrawer::Lock()
{
    mLocked = TRUE;
    return NOERROR;
}

ECode SlidingDrawer::IsOpened(
    /* [out] */ Boolean* opened)
{
    VALIDATE_NOT_NULL(opened)
    *opened = mExpanded;
    return NOERROR;
}

ECode SlidingDrawer::IsMoving(
    /* [out] */ Boolean* moving)
{
    VALIDATE_NOT_NULL(moving)
    *moving = mTracking || mAnimating;
    return NOERROR;
}

ECode SlidingDrawer::AnimateClose(
    /* [in] */ Int32 position)
{
    PrepareTracking(position);
    PerformFling(position, mMaximumAcceleration, TRUE);
    return NOERROR;
}

ECode SlidingDrawer::AnimateOpen(
    /* [in] */ Int32 position)
{
    PrepareTracking(position);
    PerformFling(position, -mMaximumAcceleration, TRUE);
    return NOERROR;
}

ECode SlidingDrawer::PerformFling(
    /* [in] */ Int32 position,
    /* [in] */ Float velocity,
    /* [in] */ Boolean always)
{
    mAnimationPosition = position;
    mAnimatedVelocity = velocity;

    if (mExpanded) {
        if (always || (velocity > mMaximumMajorVelocity ||
                (position > mTopOffset + (mVertical ? mHandleHeight : mHandleWidth) &&
                        velocity > -mMaximumMajorVelocity))) {
            // We are expanded, but they didn't move sufficiently to cause
            // us to retract.  Animate back to the expanded position.
            mAnimatedAcceleration = mMaximumAcceleration;
            if (velocity < 0) {
                mAnimatedVelocity = 0;
            }
        } else {
            // We are expanded and are now going to animate away.
            mAnimatedAcceleration = -mMaximumAcceleration;
            if (velocity > 0) {
                mAnimatedVelocity = 0;
            }
        }
    } else {
        Int32 height, width;
        GetHeight(&height);
        GetWidth(&width);
        if (!always && (velocity > mMaximumMajorVelocity ||
                (position > (mVertical ? height : width) / 2 &&
                        velocity > -mMaximumMajorVelocity))) {
            // We are collapsed, and they moved enough to allow us to expand.
            mAnimatedAcceleration = mMaximumAcceleration;
            if (velocity < 0) {
                mAnimatedVelocity = 0;
            }
        } else {
            // We are collapsed, but they didn't move sufficiently to cause
            // us to retract.  Animate back to the collapsed position.
            mAnimatedAcceleration = -mMaximumAcceleration;
            if (velocity > 0) {
                mAnimatedVelocity = 0;
            }
        }
    }

    Int64 now = SystemClock::GetUptimeMillis();
    mAnimationLastTime = now;
    mCurrentAnimationTime = now + ANIMATION_FRAME_DURATION;
    mAnimating = TRUE;
    mHandler->RemoveMessages(MSG_ANIMATE);
    Boolean result;
    mHandler->SendEmptyMessageAtTime(MSG_ANIMATE, mCurrentAnimationTime, &result);
    StopTracking();
    return NOERROR;
}

ECode SlidingDrawer::PrepareTracking(
    /* [in] */ Int32 position)
{
    mTracking = TRUE;
    mVelocityTracker = VelocityTracker::Obtain();
    Boolean opening = !mExpanded;
    if (opening) {
        mAnimatedAcceleration = mMaximumAcceleration;
        mAnimatedVelocity = mMaximumMajorVelocity;
        Int32 height, width;
        GetHeight(&height);
        GetWidth(&width);
        mAnimationPosition = mBottomOffset +
                (mVertical ? height - mHandleHeight : width - mHandleWidth);
        MoveHandle((Int32) mAnimationPosition);
        mAnimating = TRUE;
        mHandler->RemoveMessages(MSG_ANIMATE);
        Int64 now = SystemClock::GetUptimeMillis();
        mAnimationLastTime = now;
        mCurrentAnimationTime = now + ANIMATION_FRAME_DURATION;
        mAnimating = TRUE;
    } else {
        if (mAnimating) {
            mAnimating = FALSE;
            mHandler->RemoveMessages(MSG_ANIMATE);
        }
        MoveHandle(position);
    }
    return NOERROR;
}

ECode SlidingDrawer::MoveHandle(
    /* [in] */ Int32 position)
{
    AutoPtr<IView> handle = mHandle;

    if (mVertical) {
        if (position == EXPANDED_FULL_OPEN) {
            Int32 top;
            handle->GetTop(&top);
            handle->OffsetTopAndBottom(mTopOffset - top);
            Invalidate();
        } else if (position == COLLAPSED_FULL_CLOSED) {
            Int32 top;
            handle->GetTop(&top);
            handle->OffsetTopAndBottom(mBottomOffset + mBottom - mTop -
                    mHandleHeight - top);
            Invalidate();
        } else {
            Int32 top;
            handle->GetTop(&top);
            Int32 deltaY = position - top;
            if (position < mTopOffset) {
                deltaY = mTopOffset - top;
            } else if (deltaY > mBottomOffset + mBottom - mTop - mHandleHeight - top) {
                deltaY = mBottomOffset + mBottom - mTop - mHandleHeight - top;
            }
            handle->OffsetTopAndBottom(deltaY);

            AutoPtr<IRect> frame = mFrame;
            AutoPtr<IRect> region = mInvalidate;

            handle->GetHitRect(frame);
            region->Set(frame);

            Int32 frameLeft, frameTop, frameRight, frameBottom;
            frame->GetLeft(&frameLeft);
            frame->GetTop(&frameTop);
            frame->GetRight(&frameRight);
            frame->GetBottom(&frameBottom);
            region->Union(frameLeft, frameTop - deltaY, frameRight, frameBottom - deltaY);
            Int32 contentHeight;
            mContent->GetHeight(&contentHeight);
            Int32 width;
            GetWidth(&width);
            region->Union(0, frameBottom - deltaY, width,
                    frameBottom - deltaY + contentHeight);

            Invalidate(region);
        }
    } else {
        if (position == EXPANDED_FULL_OPEN) {
            Int32 left;
            handle->GetLeft(&left);
            handle->OffsetLeftAndRight(mTopOffset - left);
            Invalidate();
        } else if (position == COLLAPSED_FULL_CLOSED) {
            Int32 left;
            handle->GetLeft(&left);
            handle->OffsetLeftAndRight(mBottomOffset + mRight - mLeft -
                    mHandleWidth - left);
            Invalidate();
        } else {
            Int32 left;
            handle->GetLeft(&left);
            Int32 deltaX = position - left;
            if (position < mTopOffset) {
                deltaX = mTopOffset - left;
            } else if (deltaX > mBottomOffset + mRight - mLeft - mHandleWidth - left) {
                deltaX = mBottomOffset + mRight - mLeft - mHandleWidth - left;
            }
            handle->OffsetLeftAndRight(deltaX);

            AutoPtr<IRect> frame = mFrame;
            AutoPtr<IRect> region = mInvalidate;

            handle->GetHitRect(frame);
            region->Set(frame);

            Int32 frameLeft, frameTop, frameRight, frameBottom;
            frame->GetLeft(&frameLeft);
            frame->GetTop(&frameTop);
            frame->GetRight(&frameRight);
            frame->GetBottom(&frameBottom);
            region->Union(frameLeft - deltaX, frameTop, frameRight - deltaX, frameBottom);
            Int32 contentWidth;
            mContent->GetWidth(&contentWidth);
            Int32 height;
            GetHeight(&height);
            region->Union(frameRight - deltaX, 0,
                    frameRight - deltaX + contentWidth, height);

            Invalidate(region);
        }
    }
    return NOERROR;
}

ECode SlidingDrawer::PrepareContent()
{
    if (mAnimating) {
        return NOERROR;
    }

    // Something changed in the content, we need to honor the layout request
    // before creating the cached bitmap
    AutoPtr<IView> content = mContent;
    Boolean isLayoutRequested;
    content->IsLayoutRequested(&isLayoutRequested);
    if (isLayoutRequested) {
        if (mVertical) {
            Int32 childHeight = mHandleHeight;
            Int32 height = mBottom - mTop - childHeight - mTopOffset;
            content->Measure(MeasureSpec::MakeMeasureSpec(mRight - mLeft, MeasureSpec::EXACTLY),
                    MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY));
            Int32 measuredWidth, measuredHeght;
            content->GetMeasuredWidth(&measuredWidth);
            content->GetMeasuredHeight(&measuredHeght);
            content->Layout(0, mTopOffset + childHeight, measuredWidth,
                    mTopOffset + childHeight + measuredHeght);
        } else {
            Int32 childWidth;
            mHandle->GetWidth(&childWidth);
            Int32 width = mRight - mLeft - childWidth - mTopOffset;
            content->Measure(MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY),
                    MeasureSpec::MakeMeasureSpec(mBottom - mTop, MeasureSpec::EXACTLY));
            Int32 measuredWidth, measuredHeght;
            content->GetMeasuredWidth(&measuredWidth);
            content->GetMeasuredHeight(&measuredHeght);
            content->Layout(childWidth + mTopOffset, 0,
                    mTopOffset + childWidth + measuredWidth, measuredHeght);
        }
    }
    // Try only once... we should really loop but it's not a big deal
    // if the draw was cancelled, it will only be temporary anyway
    AutoPtr<IViewTreeObserver> vto;
    content->GetViewTreeObserver((IViewTreeObserver**)&vto);
    Boolean isPreDraw;
    vto->DispatchOnPreDraw(&isPreDraw);
    Boolean attached;
    content->IsHardwareAccelerated(&attached);

    if (!attached) content->BuildDrawingCache();

    content->SetVisibility(IView::GONE);
    return NOERROR;
}

ECode SlidingDrawer::StopTracking()
{
    mHandle->SetPressed(FALSE);
    mTracking = FALSE;

    if (mOnDrawerScrollListener != NULL) {
        mOnDrawerScrollListener->OnScrollEnded();
    }

    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
    return NOERROR;
}

ECode SlidingDrawer::DoAnimation()
{
    if (mAnimating) {
        IncrementAnimation();
        Int32 width, height;
        GetWidth(&width);
        GetHeight(&height);
        if (mAnimationPosition >= mBottomOffset + (mVertical ? height : width) - 1) {
            mAnimating = FALSE;
            CloseDrawer();
        } else if (mAnimationPosition < mTopOffset) {
            mAnimating = FALSE;
            OpenDrawer();
        } else {
            MoveHandle((int) mAnimationPosition);
            mCurrentAnimationTime += ANIMATION_FRAME_DURATION;
            Boolean result;
            mHandler->SendEmptyMessageAtTime(MSG_ANIMATE,
                mCurrentAnimationTime, &result);
        }
    }
    return NOERROR;
}

ECode SlidingDrawer::IncrementAnimation()
{
    Int64 now = SystemClock::GetUptimeMillis();
    Float t = (now - mAnimationLastTime) / 1000.0f;                   // ms -> s
    Float position = mAnimationPosition;
    Float v = mAnimatedVelocity;                                // px/s
    Float a = mAnimatedAcceleration;                            // px/s/s
    mAnimationPosition = position + (v * t) + (0.5f * a * t * t);     // px
    mAnimatedVelocity = v + (a * t);                                  // px/s
    mAnimationLastTime = now;
    return NOERROR;
}


ECode SlidingDrawer::CloseDrawer()
{
    MoveHandle(COLLAPSED_FULL_CLOSED);
    mContent->SetVisibility(IView::GONE);
    mContent->DestroyDrawingCache();

    if (!mExpanded) {
        return NOERROR;
    }

    mExpanded = FALSE;
    if (mOnDrawerCloseListener != NULL) {
        mOnDrawerCloseListener->OnDrawerClosed();
    }
    return NOERROR;
}

ECode SlidingDrawer::OpenDrawer()
{
    MoveHandle(EXPANDED_FULL_OPEN);
    mContent->SetVisibility(IView::VISIBLE);

    if (mExpanded) {
        return NOERROR;
    }

    mExpanded = TRUE;

    if (mOnDrawerOpenListener != NULL) {
        mOnDrawerOpenListener->OnDrawerOpened();
    }
    return NOERROR;
}

ECode SlidingDrawer::HandleMessage()
{
    DoAnimation();
    return NOERROR;
}

SlidingDrawer::DrawerToggler::DrawerToggler(
    /* [in] */ SlidingDrawer* host) : mHost(host)
{}

ECode SlidingDrawer::DrawerToggler::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mLocked) {
        return NOERROR;
    }
    // mAllowSingleTap isn't relevant here; you're *always*
    // allowed to open/close the drawer by clicking with the
    // trackball.

    if (mHost->mAnimateOnClick) {
        mHost->AnimateToggle();
    } else {
        mHost->Toggle();
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
