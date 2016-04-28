
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/HorizontalScrollView.h"
#include "elastos/droid/widget/CHorizontalScrollViewSavedState.h"
#include "elastos/droid/widget/ScrollView.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/FocusFinder.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/CViewGroupMarginLayoutParams.h"
#include "elastos/droid/widget/CEdgeEffect.h"
#include "elastos/droid/widget/COverScroller.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::FocusFinder;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IViewBaseSavedState;
using Elastos::Droid::View::CViewGroupMarginLayoutParams;
using Elastos::Droid::Widget::CEdgeEffect;
using Elastos::Droid::Widget::ScrollView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

HorizontalScrollView::SavedState::SavedState()
    : mScrollPosition(0)
    , mIsLayoutRtl(FALSE)
{
}

ECode HorizontalScrollView::SavedState::constructor()
{
    return NOERROR;
}

ECode HorizontalScrollView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode HorizontalScrollView::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mScrollPosition);
    dest->WriteInt32(mIsLayoutRtl ? 1 : 0);
    return NOERROR;
}

ECode HorizontalScrollView::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    BaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mScrollPosition);
    Int32 flag = 0;
    source->ReadInt32(&flag);
    if(flag == 1) {
        mIsLayoutRtl = TRUE;
    }
    else {
        mIsLayoutRtl = FALSE;
    }
    return NOERROR;
}

ECode HorizontalScrollView::SavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    Int32 hash;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->IdentityHashCode(TO_IINTERFACE(this), &hash);
    StringBuilder sb;
    sb.Append("HorizontalScrollView.SavedState{");
    sb.Append(StringUtils::ToHexString(hash));
    sb.Append(" scrollPosition=");
    sb.Append(mScrollPosition);
    sb.Append(" isLayoutRtl=");
    sb.Append(mIsLayoutRtl);
    sb.Append("}");
    return NOERROR;
}

const String HorizontalScrollView::HORIZONTALSCROLLVIEW_NAME("HorizontalScrollView");
const Int32 HorizontalScrollView::ANIMATED_SCROLL_GAP = ScrollView::ANIMATED_SCROLL_GAP;
const Float HorizontalScrollView::MAX_SCROLL_FACTOR = ScrollView::MAX_SCROLL_FACTOR;
const Int32 HorizontalScrollView::INVALID_POINTER = -1;

CAR_INTERFACE_IMPL(HorizontalScrollView, FrameLayout, IHorizontalScrollView);
HorizontalScrollView::HorizontalScrollView()
    : mLastScroll(0)
    , mLastMotionX(0)
    , mIsLayoutDirty(TRUE)
    , mIsBeingDragged(FALSE)
    , mFillViewport(FALSE)
    , mSmoothScrollingEnabled(TRUE)
    , mTouchSlop(0)
    , mMinimumVelocity(0)
    , mMaximumVelocity(0)
    , mOverscrollDistance(0)
    , mOverflingDistance(0)
    , mActivePointerId(INVALID_POINTER)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
}

ECode HorizontalScrollView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode HorizontalScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::horizontalScrollViewStyle);
}

ECode HorizontalScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode HorizontalScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ASSERT_SUCCEEDED(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    InitScrollView();

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::HorizontalScrollView),
            ArraySize(R::styleable::HorizontalScrollView));
    FAIL_RETURN(context->ObtainStyledAttributes(attrs,
            attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    Boolean value;
    a->GetBoolean(R::styleable::HorizontalScrollView_fillViewport, FALSE, &value);

    FAIL_RETURN(SetFillViewport(value));
    a->Recycle();

    return NOERROR;
}

HorizontalScrollView::~HorizontalScrollView()
{
}

Float HorizontalScrollView::GetLeftFadingEdgeStrength()
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        return 0.0;
    }

    Int32 length = 0;
    GetHorizontalFadingEdgeLength(&length);

    if (mScrollX < length) {
        return mScrollX / (Float)length;
    }

    return 1.0;
}

Float HorizontalScrollView::GetRightFadingEdgeStrength()
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        return 0.0;
    }

    Int32 length = 0, w = 0;
    GetHorizontalFadingEdgeLength(&length);
    Int32 rightEdge = (GetWidth(&w), w) - mPaddingRight;
    Int32 right;
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    child->GetRight(&right);
    Int32 span = right - mScrollX - rightEdge;
    if (span < length) {
        return span / (Float)length;
    }

    return 1.0;
}

ECode HorizontalScrollView::GetMaxScrollAmount(
    /* [out] */ Int32* maxScrollAmount)
{
    VALIDATE_NOT_NULL(maxScrollAmount);
    *maxScrollAmount = (Int32)(MAX_SCROLL_FACTOR * (mRight - mLeft));
    return NOERROR;
}

void HorizontalScrollView::InitScrollView()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    COverScroller::New(ctx, (IOverScroller**)&mScroller);
    SetFocusable(TRUE);
    SetDescendantFocusability(ViewGroup::FOCUS_AFTER_DESCENDANTS);
    SetWillNotDraw(FALSE);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration ;
    helper->Get(mContext, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    configuration->GetScaledOverscrollDistance(&mOverscrollDistance);
    configuration->GetScaledOverflingDistance(&mOverflingDistance);
}

ECode HorizontalScrollView::AddView(
    /* [in] */ IView* child)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        //throw new IllegalStateException("HorizontalScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child);
}

ECode HorizontalScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        //throw new IllegalStateException("HorizontalScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, index);
}

ECode HorizontalScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        //throw new IllegalStateException("HorizontalScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, params);
}

ECode HorizontalScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        //throw new IllegalStateException("HorizontalScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, index, params);
}

Boolean HorizontalScrollView::CanScroll()
{
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    if (child != NULL) {
        Int32 childWidth = 0, w = 0;
        child->GetWidth(&childWidth);
        return (GetWidth(&w), w) < childWidth + mPaddingLeft + mPaddingRight;
    }
    return FALSE;
}

ECode HorizontalScrollView::IsFillViewport(
    /* [out] */ Boolean* isFilled)
{
    VALIDATE_NOT_NULL(isFilled);
    *isFilled = mFillViewport;
    return NOERROR;
}

ECode HorizontalScrollView::SetFillViewport(
    /* [in] */ Boolean fillViewport)
{
    if (fillViewport != mFillViewport) {
        mFillViewport = fillViewport;
        RequestLayout();
    }

    return NOERROR;
}

ECode HorizontalScrollView::IsSmoothScrollingEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mSmoothScrollingEnabled;
    return NOERROR;
}

ECode HorizontalScrollView::SetSmoothScrollingEnabled(
    /* [in] */ Boolean smoothScrollingEnabled)
{
    mSmoothScrollingEnabled = smoothScrollingEnabled;
    return NOERROR;
}

void HorizontalScrollView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (!mFillViewport) {
        return;
    }

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    if (widthMode == MeasureSpec::UNSPECIFIED) {
        return;
    }

    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 width = 0;
        GetMeasuredWidth(&width);
        Int32 childWidth = 0;
        child->GetMeasuredHeight(&childWidth);
        if (childWidth < width) {

            ///////////////////////////////////////////////////////////
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IFrameLayoutLayoutParams* params = IFrameLayoutLayoutParams::Probe(vglp);

            Int32 childHeightMeasureSpec = GetChildMeasureSpec(heightMeasureSpec,
                    mPaddingTop + mPaddingBottom,
                    ((FrameLayout::LayoutParams*)params)->mHeight);
            width -= mPaddingLeft;
            width -= mPaddingRight;
            Int32 childWidthMeasureSpec =
                    MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY);

            child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
        }
    }
}

ECode HorizontalScrollView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (FrameLayout::DispatchKeyEvent(event, result), *result) || (ExecuteKeyEvent(event, result), *result);
    return NOERROR;
}

ECode HorizontalScrollView::ExecuteKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    mTempRect->SetEmpty();

    if (!CanScroll()) {
        Boolean focused = FALSE;
        if (IsFocused(&focused), focused) {
            AutoPtr<IView> currentFocused;
            FindFocus((IView**)&currentFocused);
            if (currentFocused.Get() == this) {
                currentFocused = NULL;
            }
            AutoPtr<IView> nextFocused;
            FocusFinder::GetInstance()->FindNextFocus(this,
                currentFocused, IView::FOCUS_RIGHT, (IView**)&nextFocused);

            if (nextFocused != NULL && nextFocused.Get() != this) {
                Boolean isFocus = FALSE;
                nextFocused->RequestFocus(IView::FOCUS_RIGHT, &isFocus);
                *result = isFocus;
                return NOERROR;
            }
            else {
                return NOERROR;
            }
        }
        return NOERROR;
    }
    Boolean handled = FALSE;
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 keyCode = 0;
        event->GetKeyCode(&keyCode);
        Boolean isPressed = FALSE;
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_LEFT:
                event->IsAltPressed(&isPressed);
                if (!isPressed) {
                    ArrowScroll(IView::FOCUS_LEFT, &handled);
                }
                else {
                    FullScroll(IView::FOCUS_LEFT, &handled);
                }
                break;
            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                event->IsAltPressed(&isPressed);
                if (!isPressed) {
                    ArrowScroll(IView::FOCUS_RIGHT, &handled);
                }
                else {
                    FullScroll(IView::FOCUS_RIGHT, &handled);
                }
                break;
            case IKeyEvent::KEYCODE_SPACE:
                event->IsShiftPressed(&isPressed);
                Boolean consumed = FALSE;
                PageScroll(isPressed ? IView::FOCUS_LEFT : IView::FOCUS_RIGHT, &consumed);
                break;
        }
    }

    *result = handled;
    return NOERROR;
}

Boolean HorizontalScrollView::InChild(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Int32 scrollX = mScrollX;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 top, bottom, left, right;
        child->GetTop(&top);
        child->GetBottom(&bottom);
        child->GetLeft(&left);
        child->GetRight(&right);
        return !(y < top
                || y >= bottom
                || x < left - scrollX
                || x >= right - scrollX);
    }

    return FALSE;
}

void HorizontalScrollView::InitOrResetVelocityTracker()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    else {
        mVelocityTracker->Clear();
    }
}

void HorizontalScrollView::InitVelocityTrackerIfNotExists()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
}

void HorizontalScrollView::RecycleVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

ECode HorizontalScrollView::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    if (disallowIntercept) {
        RecycleVelocityTracker();
    }
    FrameLayout::RequestDisallowInterceptTouchEvent(disallowIntercept);
    return NOERROR;
}

ECode HorizontalScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    ev->GetAction(&action);
    if ((action == IMotionEvent::ACTION_MOVE) && (mIsBeingDragged)) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean tmp = FALSE;
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_MOVE: {
            Int32 activePointerId = mActivePointerId;
            if (activePointerId == INVALID_POINTER) {
                // If we don't have a valid id, the touch down wasn't on content.
                break;
            }

            Int32 pointerIndex = 0;
            ev->FindPointerIndex(activePointerId, &pointerIndex);
            if (pointerIndex == -1) {
                /*Log.e(TAG, "Invalid pointerId=" + activePointerId
                    + " in onInterceptTouchEvent");*/
                break;
            }

            Float x = 0;
            ev->GetX(pointerIndex, &x);
            Int32 xDiff = (Int32)Elastos::Core::Math::Abs((Int32)x - mLastMotionX);
            if (xDiff > mTouchSlop) {
                mIsBeingDragged = TRUE;
                mLastMotionX = x;

                InitVelocityTrackerIfNotExists();
                mVelocityTracker->AddMovement(ev);

                if (mParent != NULL) {
                    mParent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }
            break;
        }

        case IMotionEvent::ACTION_DOWN: {
            Float y = 0;
            Float x = 0;
            ev->GetY(&y);
            ev->GetX(&x);
            if (!InChild((Int32)x, (Int32)y)) {
                mIsBeingDragged = FALSE;
                RecycleVelocityTracker();
                break;
            }

            mLastMotionX = x;
            ev->GetPointerId(0, &mActivePointerId);

            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(ev);

            mScroller->IsFinished(&mIsBeingDragged);
            mIsBeingDragged = !mIsBeingDragged;
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            mIsBeingDragged = FALSE;
            mActivePointerId = INVALID_POINTER;
            if (mScroller->SpringBack(mScrollX, mScrollY, 0, GetScrollRange(), 0, 0, &tmp), tmp) {
                PostInvalidateOnAnimation();
            }
            break;

        case IMotionEvent::ACTION_POINTER_DOWN: {
            Int32 index = 0;
            ev->GetActionIndex(&index);
            Float f = 0;
            ev->GetX(index, &f);
            mLastMotionX = (Int32)f;
            ev->GetPointerId(index, &mActivePointerId);
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            Int32 find = 0;
            ev->FindPointerIndex(mActivePointerId, &find);
            Float fx = 0;
            ev->GetX(find, &fx);
            mLastMotionX = (Int32)fx;
            break;
    }

    *result = mIsBeingDragged;
    return NOERROR;
}

ECode HorizontalScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    InitVelocityTrackerIfNotExists();
    mVelocityTracker->AddMovement(ev);

    Int32 action = 0;
    ev->GetAction(&action);

    Int32 count = 0;
    Boolean tmp = FALSE;
    switch(action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN: {
            if((GetChildCount(&count), count) == 0) {
                *result = FALSE;
                return NOERROR;
            }
            Boolean finished = FALSE;
            mScroller->IsFinished(&finished);
            if(mIsBeingDragged != finished) {
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if(parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }

            if(!finished) {
                mScroller->AbortAnimation();
            }
            Float motionX = 0;
            ev->GetX(&motionX);
            mLastMotionX = (Int32)motionX;
            ev->GetPointerId(0, &mActivePointerId);
            break;
        }

        case IMotionEvent::ACTION_MOVE:
        {
            Int32 activePointerIndex = 0;
            ev->FindPointerIndex(mActivePointerId, &activePointerIndex);
            if(activePointerIndex == -1) {
                //log.e(TAG, "invalid pointerID = ")
                break;
            }
            Int32 x = 0;
            Float fx = 0;
            ev->GetX(activePointerIndex, &fx);
            x = (Int32)fx;
            Int32 deltaX = mLastMotionX - x;
            if(!mIsBeingDragged && Elastos::Core::Math::Abs(deltaX) > mTouchSlop) {
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if(parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
                mIsBeingDragged = TRUE;
                if(deltaX > 0) {
                    deltaX -= mTouchSlop;
                }
                else {
                    deltaX += mTouchSlop;
                }

            }
            if(mIsBeingDragged) {
                mLastMotionX = x;

                Int32 oldX = mScrollX;
                Int32 oldY = mScrollY;
                Int32 range = GetScrollRange();
                Int32 overscrollMode = 0;
                GetOverScrollMode(&overscrollMode);
                Boolean canOverscroll = overscrollMode == IView::OVER_SCROLL_ALWAYS
                    || (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS && range > 0) ;

                // Calling overScrollBy will call onOverScrolled, which
                // calls onScrollChanged if applicable.
                if(OverScrollBy(deltaX, 0, mScrollX, 0, range, 0,
                    mOverscrollDistance, 0, TRUE)) {
                    mVelocityTracker->Clear();
                }
                OnScrollChanged(mScrollX, mScrollY, oldX, oldY);

                if(canOverscroll) {
                    Int32 pulledToX = oldX + deltaX;
                    if(pulledToX < 0) {
                        Int32 w = 0, h = 0;
                        GetWidth(&w);
                        GetHeight(&h);
                        Float y = 0;
                        ev->GetY(activePointerIndex, &y);
                        mEdgeGlowLeft->OnPull((Float)deltaX / w, 1.f - y / h);
                        Boolean isFinished = FALSE;
                        mEdgeGlowRight->IsFinished(&isFinished);
                        if(isFinished) {
                            mEdgeGlowRight->OnRelease();
                        }
                    }
                    else if(pulledToX > range) {
                        Int32 w = 0, h = 0;
                        GetWidth(&w);
                        GetHeight(&h);
                        Float y = 0;
                        ev->GetY(activePointerIndex, &y);
                        mEdgeGlowRight->OnPull((Float)deltaX / w, y / h);
                        Boolean isFinished = FALSE;
                        mEdgeGlowLeft->IsFinished(&isFinished);
                        if(isFinished) {
                            mEdgeGlowLeft->OnRelease();
                        }
                    }
                    Boolean leftFin = FALSE;
                    Boolean rightFin = FALSE;
                    mEdgeGlowLeft->IsFinished(&leftFin);
                    mEdgeGlowRight->IsFinished(&rightFin);
                    if(mEdgeGlowLeft != NULL && (!leftFin || !rightFin)) {
                        PostInvalidateOnAnimation();
                    }
                }
            }
        }
        break;

        case IMotionEvent::ACTION_UP:
            if(mIsBeingDragged) {
                AutoPtr<VelocityTracker> velocityTracker = mVelocityTracker;
                velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                Float x;
                velocityTracker->GetXVelocity(mActivePointerId, &x);
                Int32 initialVelocity = (Int32)x;

                if((GetChildCount(&count), count) > 0) {
                    if((Elastos::Core::Math::Abs(initialVelocity) > mMinimumVelocity)) {
                        Fling(-initialVelocity);
                    }
                    else {
                        if(mScroller->SpringBack(mScrollX, mScrollY, 0, GetScrollRange(), 0, 0, &tmp), tmp) {
                            PostInvalidateOnAnimation();
                        }
                    }
                }

                mActivePointerId = INVALID_POINTER;
                mIsBeingDragged = FALSE;
                RecycleVelocityTracker();

                if(mEdgeGlowLeft != NULL) {
                    mEdgeGlowLeft->OnRelease();
                    mEdgeGlowRight->OnRelease();
                }
            }
            break;

        case IMotionEvent::ACTION_CANCEL:
            if(mIsBeingDragged && (GetChildCount(&count), count) > 0) {
                if(mScroller->SpringBack(mScrollX, mScrollY, 0, GetScrollRange(), 0, 0, &tmp), tmp) {
                    PostInvalidateOnAnimation();
                }
                mActivePointerId = INVALID_POINTER;
                mIsBeingDragged = FALSE;
                RecycleVelocityTracker();

                if(mEdgeGlowLeft != NULL) {
                    mEdgeGlowLeft->OnRelease();
                    mEdgeGlowRight->OnRelease();
                }
            }
            break;

        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void HorizontalScrollView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action = 0;
    ev->GetAction(&action);
    Int32 pointerIndex = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK) >>
            IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
    Int32 pointerId = 0;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        Float lastMotionX = 0;
        ev->GetX(newPointerIndex, &lastMotionX);
        mLastMotionX = (Int32)lastMotionX;
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
        if (mVelocityTracker != NULL) {
            mVelocityTracker->Clear();
        }
    }
}

ECode HorizontalScrollView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CMotionEvent> cevent = (CMotionEvent*)event;
    Int32 source = 0;
    cevent->GetSource(&source);
    if((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action = 0;
        event->GetAction(&action);
        switch(action) {
            case IMotionEvent::ACTION_SCROLL:
                if(!mIsBeingDragged) {
                    Float hscroll = 0;
                    Int32 metaState = 0;
                    event->GetMetaState(&metaState);
                    if((metaState & IKeyEvent::META_SHIFT_ON) != 0) {
                        Float fhscroll = 0;
                        event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &fhscroll);
                        hscroll = -fhscroll;
                    }
                    else {
                        event->GetAxisValue(IMotionEvent::AXIS_HSCROLL, &hscroll);
                    }

                    if(hscroll != 0) {
                        Float scrollFactor;
                        GetHorizontalScrollFactor(&scrollFactor);
                        Int32 delta = (Int32)(hscroll * scrollFactor);
                        Int32 range = GetScrollRange();
                        Int32 oldScrollX = mScrollX;
                        Int32 newScrollX = oldScrollX + delta;
                        if(newScrollX < 0) {
                            newScrollX = 0;
                        }
                        else if(newScrollX > range) {
                            newScrollX = range;
                        }
                        if(newScrollX != oldScrollX) {
                            FrameLayout::ScrollTo(newScrollX, mScrollY);
                            *result = TRUE;
                            return NOERROR;
                        }
                    }
                }
        }
    }
    return FrameLayout::OnGenericMotionEvent(event, result);
}

ECode HorizontalScrollView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* compatibility)
{
    VALIDATE_NOT_NULL(compatibility);
    *compatibility = TRUE;
    return NOERROR;
}

void HorizontalScrollView::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    Boolean finished = 0;
    if (mScroller->IsFinished(&finished), !finished) {
        const Int32 oldX = mScrollX;
        const Int32 oldY = mScrollY;
        mScrollX = scrollX;
        mScrollY = scrollY;
        InvalidateParentIfNeeded();
        OnScrollChanged(mScrollX, mScrollY, oldX, oldY);
        if (clampedY) {
            Boolean tmp = FALSE;
            mScroller->SpringBack(mScrollX, mScrollY, 0, GetScrollRange(), 0, 0, &tmp);
        }
    }
    else {
        FrameLayout::ScrollTo(scrollX, scrollY);
    }
    AwakenScrollBars();
}

ECode HorizontalScrollView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if(FrameLayout::PerformAccessibilityAction(action, arguments, result), *result) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean enabled = FALSE;
    switch(action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            if(IsEnabled(&enabled), !enabled) {
                return NOERROR;
            }
            Int32 w = 0;
            GetWidth(&w);
            Int32 viewportWidth = w - mPaddingLeft - mPaddingRight;
            Int32 targetScrollX = Elastos::Core::Math::Min(mScrollX + viewportWidth, GetScrollRange());
            if(targetScrollX != mScrollX) {
                SmoothScrollTo(targetScrollX, 0);
                *result = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;

        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            if(IsEnabled(&enabled), !enabled) {
                return NOERROR;
            }
            Int32 w = 0;
            GetWidth(&w);
            Int32 viewportWidth = w - mPaddingLeft - mPaddingRight;
            Int32 targetScrollX = Elastos::Core::Math::Max(0, mScrollX - viewportWidth);
            if(targetScrollX != mScrollX) {
                SmoothScrollTo(targetScrollX, 0);
                *result = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode HorizontalScrollView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(HORIZONTALSCROLLVIEW_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    Int32 scrollRange = GetScrollRange();
    if(scrollRange > 0) {
        info->SetScrollable(TRUE);
        Boolean enabled = FALSE;
        IsEnabled(&enabled);
        if(enabled && mScrollX > 0) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
        if(enabled && mScrollX < scrollRange) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
    }
    return NOERROR;
}

ECode HorizontalScrollView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(HORIZONTALSCROLLVIEW_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    AutoPtr<CAccessibilityEvent> cevent = (CAccessibilityEvent*)event;
    cevent->SetScrollable(GetScrollRange() > 0);
    cevent->SetScrollX(mScrollX);
    cevent->SetScrollY(mScrollY);
    cevent->SetMaxScrollX(GetScrollRange());
    cevent->SetMaxScrollY(mScrollY);
    return NOERROR;
}

Int32 HorizontalScrollView::GetScrollRange()
{
    Int32 scrollRange = 0, count = 0;
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 childWidth, w = 0;
        GetWidth(&w);
        child->GetWidth(&childWidth);
        scrollRange = Elastos::Core::Math::Max(0,
                 childWidth - (w - mPaddingLeft - mPaddingRight));
    }
    return scrollRange;
}

AutoPtr<IView> HorizontalScrollView::FindFocusableViewInMyBounds(
    /* [in] */ Boolean leftFocus,
    /* [in] */ Int32 left,
    /* [in] */ IView* preferredFocusable)
{
    Int32 value = 0;
    GetHorizontalFadingEdgeLength(&value);
    Int32 fadingEdgeLength = value / 2;
    Int32 leftWithoutFadingEdge = left + fadingEdgeLength;
    Int32 rightWithoutFadingEdge = left + (GetWidth(&value), value) - fadingEdgeLength;

    if (preferredFocusable != NULL) {
        Int32 preferredLeft = 0;
        Int32 preferredRight = 0;
        preferredFocusable->GetLeft(&preferredLeft);
        preferredFocusable->GetRight(&preferredRight);
        if ((preferredLeft < rightWithoutFadingEdge)
                && (preferredRight > leftWithoutFadingEdge)) {
            return preferredFocusable;
        }
    }

    return FindFocusableViewInBounds(leftFocus, leftWithoutFadingEdge,
            rightWithoutFadingEdge);
}

AutoPtr<IView> HorizontalScrollView::FindFocusableViewInBounds(
    /* [in] */ Boolean leftFocus,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    AutoPtr<IArrayList> focusables;
    ASSERT_SUCCEEDED(GetFocusables(IView::FOCUS_FORWARD, (IArrayList**)&focusables));

    AutoPtr<IView> focusCandidate;

    Boolean foundFullyContainedFocusable = FALSE;

    Int32 count = 0;
    focusables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        focusables->Get(i, (IInterface**)&obj);
        AutoPtr<IView> view = IView::Probe(obj);
        Int32 viewLeft = 0;
        Int32 viewRight = 0;
        view->GetLeft(&viewLeft);
        view->GetRight(&viewRight);

        if (left < viewRight && viewLeft < right) {
            Boolean viewIsFullyContained = (left < viewLeft) &&
                    (viewRight < right);

            if (focusCandidate == NULL) {
                focusCandidate = view;
                foundFullyContainedFocusable = viewIsFullyContained;
            }
            else {
                Int32 l = 0;
                Int32 r = 0;
                focusCandidate->GetLeft(&l);
                focusCandidate->GetRight(&r);
                Boolean viewIsCloserToBoundary =
                        (leftFocus && viewLeft < l) || (!leftFocus && viewRight > r);

                if (foundFullyContainedFocusable) {
                    if (viewIsFullyContained && viewIsCloserToBoundary) {
                        focusCandidate = view;
                    }
                }
                else {
                    if (viewIsFullyContained) {
                        focusCandidate = view;
                        foundFullyContainedFocusable = TRUE;
                    }
                    else if (viewIsCloserToBoundary) {
                        focusCandidate = view;
                    }
                }
            }
        }
    }

    return focusCandidate;
}

ECode HorizontalScrollView::PageScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isRight = direction == IView::FOCUS_RIGHT;
    Int32 width = 0;
    GetWidth(&width);

    Int32 sx = 0;
    GetScrollX(&sx);
    if (isRight) {
        Int32 left = sx + width;
        mTempRect->SetLeft(left);
        Int32 count = 0;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 right;
            view->GetRight(&right);
            if (left + width > right) {
                mTempRect->SetLeft(right - width);
            }
        }
    }
    else {
        Int32 left = sx - width;
        mTempRect->SetLeft(left);
        if (left < 0) {
            mTempRect->SetLeft(0);
        }
    }

    Int32 left, right;
    mTempRect->GetLeft(&left);
    right = left + width;
    mTempRect->SetRight(right);

    *result = ScrollAndFocus(direction, left, right);
    return NOERROR;
}

ECode HorizontalScrollView::FullScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isRight = direction == IView::FOCUS_RIGHT;
    Int32 width = 0;
    GetWidth(&width);

    mTempRect->SetLeft(0);
    mTempRect->SetRight(width);

    if (isRight) {
        Int32 count = 0;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 right;
            view->GetRight(&right);
            mTempRect->SetRight(right);
            mTempRect->SetLeft(right - width);
        }
    }

    Int32 left, right;
    mTempRect->GetLeft(&left);
    mTempRect->GetRight(&right);
    *result = ScrollAndFocus(direction, left, right);
    return NOERROR;
}

Boolean HorizontalScrollView::ScrollAndFocus(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    Boolean handled = TRUE;
    Int32 width = 0;
    GetWidth(&width);
    Int32 containerLeft = 0;
    GetScrollX(&containerLeft);
    Int32 containerRight = containerLeft + width;
    Boolean goLeft = direction == IView::FOCUS_LEFT;

    AutoPtr<IView> newFocused = FindFocusableViewInBounds(goLeft, left, right);
    if(newFocused == NULL) {
        newFocused = this;
    }

    if(left >= containerLeft && right <= containerRight) {
        handled = FALSE;
    }
    else {
        Int32 delta = goLeft ? (left - containerLeft) : (right - containerRight);
        DoScrollX(delta);
    }

    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if(newFocused != currentFocused) {
        Boolean res = FALSE;
        newFocused->RequestFocus(direction, &res);
    }
    return handled;
}

ECode HorizontalScrollView::ArrowScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if(currentFocused.Get() == this) currentFocused = NULL;
    AutoPtr<IView> nextFocused;
    FocusFinder::GetInstance()->FindNextFocus(this,
        currentFocused, direction, (IView**)&nextFocused);
    Int32 maxJump = 0;
    GetMaxScrollAmount(&maxJump);

    if(nextFocused.Get() && IsWithinDeltaOfScreen(nextFocused, maxJump)) {
        nextFocused->GetDrawingRect(mTempRect);
        OffsetDescendantRectToMyCoords(nextFocused, mTempRect);
        Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);
        DoScrollX(scrollDelta);
        Boolean res = FALSE;
        nextFocused->RequestFocus(direction, &res);
    }
    else {
        Int32 scrollDelta = maxJump;
        Int32 count = 0;
        Int32 sx = 0;
        GetScrollX(&sx);
        if(direction == IView::FOCUS_LEFT && sx < scrollDelta) {
            scrollDelta = sx;
        }
        else if(direction == IView::FOCUS_RIGHT && (GetChildCount(&count), count) > 0) {
            Int32 daRight = 0, w = 0;
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            child->GetRight(&daRight);
            Int32 screenRight = sx + (GetWidth(&w), w);

            if(daRight - screenRight < maxJump) {
                scrollDelta = daRight - screenRight;
            }
        }
        if(scrollDelta == 0) {
            *result = FALSE;
            return NOERROR;
        }
        DoScrollX(direction == IView::FOCUS_RIGHT ? scrollDelta : -scrollDelta);
    }

    if (currentFocused) {
        Boolean isFocused = FALSE;
        currentFocused->IsFocused(&isFocused);
        if (isFocused && IsOffScreen(currentFocused)) {
            Int32 descendantFocusability = 0;
            GetDescendantFocusability(&descendantFocusability);
            SetDescendantFocusability(IViewGroup::FOCUS_BEFORE_DESCENDANTS);
            RequestFocus(&isFocused);
            SetDescendantFocusability(descendantFocusability);
        }
    }

    *result = TRUE;
    return NOERROR;
}

Boolean HorizontalScrollView::IsOffScreen(
    /* [in] */ IView* descendant)
{
    return !IsWithinDeltaOfScreen(descendant, 0);
}

Boolean HorizontalScrollView::IsWithinDeltaOfScreen(
    /* [in] */ IView* descendant,
    /* [in] */ Int32 delta)
{
    descendant->GetDrawingRect(mTempRect);
    OffsetDescendantRectToMyCoords(descendant, mTempRect);

    Int32 w = 0, sx = 0;
    GetWidth(&w);
    GetScrollX(&sx);
    Int32 left, right;
    mTempRect->GetLeft(&left);
    mTempRect->GetRight(&right);
    return (right + delta) >= sx && (left - delta) <= (sx + w);
}

void HorizontalScrollView::DoScrollX(
    /* [in] */ Int32 delta)
{
    if (delta != 0) {
        if (mSmoothScrollingEnabled) {
            SmoothScrollBy(delta, 0);
        }
        else {
            ScrollBy(delta, 0);
        }
    }
}

ECode HorizontalScrollView::SmoothScrollBy(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    Int32 count = 0;
    if((GetChildCount(&count), count) == 0) {
        return NOERROR;
    }

    Int64 duration = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&duration);
    duration = duration - mLastScroll;
    if(duration > ANIMATED_SCROLL_GAP) {
        Int32 width = 0;
        GetWidth(&width);
        width = width - mPaddingRight - mPaddingLeft;
        Int32 right = 0;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        child->GetWidth(&right);
        Int32 maxX= Elastos::Core::Math::Max(0, right - width);
        Int32 scrollX = mScrollX;
        dx = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(scrollX + dx, maxX)) - scrollX;

        mScroller->StartScroll(scrollX, mScrollY, dx, 0);
        PostInvalidateOnAnimation();
    }
    else {
        Boolean finished = FALSE;
        if(mScroller->IsFinished(&finished), !finished) {
            mScroller->AbortAnimation();
        }
        ScrollBy(dx, dy);
    }
    AnimationUtils::CurrentAnimationTimeMillis(&mLastScroll);
    return NOERROR;
}

ECode HorizontalScrollView::SmoothScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return SmoothScrollBy(x - mScrollX, y - mScrollY);
}

Int32 HorizontalScrollView::ComputeHorizontalScrollRange()
{
    Int32 count = 0;
    GetChildCount(&count);
    Int32 contentWidth = 0;
    GetWidth(&contentWidth);
    contentWidth = contentWidth - mPaddingLeft - mPaddingRight;
    if (count == 0) {
        return contentWidth;
    }

    Int32 scrollRange = 0;
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    child->GetRight(&scrollRange);
    Int32 scrollX = mScrollX;
    Int32 overscrollRight = Elastos::Core::Math::Max(0, scrollRange - contentWidth);
    if (scrollX < 0) {
        scrollRange -= scrollX;
    }
    else if (scrollX > overscrollRight) {
        scrollRange += scrollX - overscrollRight;
    }

    return scrollRange;
}

Int32 HorizontalScrollView::ComputeHorizontalScrollOffset()
{
    return Elastos::Core::Math::Max(0, FrameLayout::ComputeHorizontalScrollOffset());
}

void HorizontalScrollView::MeasureChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 parentHeightMeasureSpec)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 childWidthMeasureSpec = 0;
    Int32 childHeightMeasureSpec = 0;

    Int32 height = 0;
    lp->GetHeight(&height);
    childHeightMeasureSpec = GetChildMeasureSpec(parentHeightMeasureSpec,
            mPaddingTop + mPaddingBottom, height);

    childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void HorizontalScrollView::MeasureChildWithMargins(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 widthUsed,
    /* [in] */ Int32 parentHeightMeasureSpec,
    /* [in] */ Int32 heightUsed)
{
    AutoPtr<IViewGroupLayoutParams> vglp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    IViewGroupLayoutParams* params = IViewGroupLayoutParams::Probe(vglp);
    CViewGroupMarginLayoutParams* lp = (CViewGroupMarginLayoutParams*)params;

    Int32 childHeightMeasureSpec = GetChildMeasureSpec(parentHeightMeasureSpec,
            mPaddingTop + mPaddingBottom + lp->mTopMargin + lp->mBottomMargin
            + heightUsed, lp->mHeight);
    Int32 childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(
            lp->mLeftMargin + lp->mRightMargin, MeasureSpec::UNSPECIFIED);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

ECode HorizontalScrollView::ComputeScroll()
{
    Boolean result = FALSE;
    if(mScroller->ComputeScrollOffset(&result), result) {
        Int32 oldX = mScrollX;
        Int32 oldY = mScrollY;
        Int32 x = 0;
        mScroller->GetCurrX(&x);
        Int32 y = 0;
        mScroller->GetCurrY(&y);

        if(oldX != x || oldY != y) {
            Int32 range = GetScrollRange();
            Int32 overscrollMode = 0;
            GetOverScrollMode(&overscrollMode);
            Boolean canOverscroll = overscrollMode == IView::OVER_SCROLL_ALWAYS
                || (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS && range >0);

            OverScrollBy(x - oldX, y - oldY, oldX, oldY, range, 0,
                mOverflingDistance, 0, FALSE);
            OnScrollChanged(mScrollX, mScrollY, oldX, oldY);

            if(canOverscroll) {
                if(x < 0 && oldX >= 0) {
                    Float currVelocity = 0;
                    mScroller->GetCurrVelocity(&currVelocity);
                    mEdgeGlowLeft->OnAbsorb((Int32) currVelocity);
                }
                else if(x > range && oldX <= range) {
                    Float currVelocity = 0;
                    mScroller->GetCurrVelocity(&currVelocity);
                    mEdgeGlowRight->OnAbsorb((Int32) currVelocity);
                }
            }
        }
        if(!AwakenScrollBars()) {
            PostInvalidateOnAnimation();
        }
    }
    return NOERROR;
}

void HorizontalScrollView::ScrollToChild(
    /* [in] */ IView* child)
{
    child->GetDrawingRect(mTempRect);

    OffsetDescendantRectToMyCoords(child, mTempRect);

    Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);

    if (scrollDelta != 0) {
        ScrollBy(scrollDelta, 0);
    }
}

Boolean HorizontalScrollView::ScrollToChildRect(
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
{
    Int32 delta = ComputeScrollDeltaToGetChildRectOnScreen(rect);
    Boolean scroll = delta != 0;
    if (scroll) {
        if (immediate) {
            ScrollBy(delta, 0);
        }
        else {
            SmoothScrollBy(delta, 0);
        }
    }
    return scroll;
}

Int32 HorizontalScrollView::ComputeScrollDeltaToGetChildRectOnScreen(
    /* [in] */ IRect* rect)
{
    Int32 count = 0;
    if((GetChildCount(&count), count) == 0) return 0;

    Int32 width = 0;
    GetWidth(&width);
    Int32 screenLeft = 0;
    GetScrollX(&screenLeft);
    Int32 screenRight = screenLeft + width;

    Int32 fadingEdge = 0;
    GetHorizontalFadingEdgeLength(&fadingEdge);

    CRect* cRect = (CRect*)rect;
    if(cRect->mLeft > 0) {
        screenLeft += fadingEdge;
    }
    Int32 childWidth= 0;
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    child->GetWidth(&childWidth);
    if(cRect->mRight < childWidth) {
        screenRight -= fadingEdge;
    }
    Int32 scrollXDelta = 0;
    if(cRect->mRight > screenRight && cRect->mLeft > screenLeft) {
        Int32 cWidth = 0;
        cRect->GetWidth(&cWidth);
        if(cWidth > width) {
            scrollXDelta += (cRect->mLeft - screenLeft);
        }
        else {
            scrollXDelta += (cRect->mRight - screenRight);
        }

        Int32 right = 0;
        child->GetRight(&right);
        Int32 distanceToRight = right - screenRight;
        scrollXDelta = Elastos::Core::Math::Min(scrollXDelta, distanceToRight);
    }
    else if(cRect->mLeft < screenLeft && cRect->mRight < screenRight) {
        Int32 cWidth = 0;
        cRect->GetWidth(&cWidth);
        if(cWidth > width) {
            scrollXDelta -= (screenRight - cRect->mRight);
        }
        else {
            scrollXDelta -= (screenLeft - cRect->mLeft);
        }
        Int32 sx = 0;
        GetScrollX(&sx);
        scrollXDelta = Elastos::Core::Math::Max(scrollXDelta, -sx);
    }
    return scrollXDelta;
}

ECode HorizontalScrollView::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (!mIsLayoutDirty) {
        ScrollToChild(focused);
    }
    else {
         mChildToScrollTo = focused;
    }

    return FrameLayout::RequestChildFocus(child, focused);
}

Boolean HorizontalScrollView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    if (direction == IView::FOCUS_FORWARD) {
        direction = IView::FOCUS_RIGHT;
    }
    else if (direction == IView::FOCUS_BACKWARD) {
        direction = IView::FOCUS_LEFT;
    }

    AutoPtr<IView> nextFocus;
    if(previouslyFocusedRect) {
        FocusFinder::GetInstance()->FindNextFocusFromRect(
                    this, previouslyFocusedRect, direction, (IView**)&nextFocus);
    }
    else {
        FocusFinder::GetInstance()->FindNextFocus(
                    this, NULL, direction, (IView**)&nextFocus);
    }

    if (nextFocus == NULL) {
        return FALSE;
    }

    if (IsOffScreen(nextFocus)) {
        return FALSE;
    }

    Boolean result = FALSE;
    ASSERT_SUCCEEDED(nextFocus->RequestFocus(direction, previouslyFocusedRect, &result));

    return result;
}

ECode HorizontalScrollView::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 left = 0;
    Int32 scrollX = 0;
    Int32 top = 0;
    Int32 scrollY = 0;
    child->GetLeft(&left);
    child->GetScrollX(&scrollX);
    child->GetTop(&top);
    child->GetScrollY(&scrollY);
    rectangle->Offset(left - scrollX, top - scrollY);

    *result = ScrollToChildRect(rectangle, immediate);
    return NOERROR;
}

ECode HorizontalScrollView::RequestLayout()
{
    mIsLayoutDirty = TRUE;
    return FrameLayout::RequestLayout();
}

ECode HorizontalScrollView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 childWidth = 0;
    Int32 childMargins = 0, count = 0;


    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        child->GetMeasuredWidth(&childWidth);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<FrameLayout::LayoutParams> childParams = (FrameLayout::LayoutParams*) IFrameLayoutLayoutParams::Probe(params);
        childMargins = childParams->mLeftMargin + childParams->mRightMargin;
    }

    Int32 available = r - l - GetPaddingLeftWithForeground() -
            GetPaddingRightWithForeground() - childMargins;

    Boolean forceLeftGravity = (childWidth > available);

    LayoutChildren(l, t, r, b, forceLeftGravity);

    mIsLayoutDirty = FALSE;
    // Give a child focus if it needs it
    if (mChildToScrollTo != NULL && IsViewDescendantOf(mChildToScrollTo, this)) {
        ScrollToChild(mChildToScrollTo);
    }
    mChildToScrollTo = NULL;

    Boolean tmp = 0;
    if (IsLaidOut(&tmp), !tmp) {
        Int32 scrollRange = Elastos::Core::Math::Max(0, childWidth - (r - l - mPaddingLeft - mPaddingRight));
        IsLayoutRtl(&tmp);
        if (mSavedState != NULL) {
            if (tmp == mSavedState->mIsLayoutRtl) {
                mScrollX = mSavedState->mScrollPosition;
            }
            else {
                mScrollX = scrollRange - mSavedState->mScrollPosition;
            }
            mSavedState = NULL;
        }
        else {
            if (tmp) {
                mScrollX = scrollRange - mScrollX;
            } // mScrollX default value is "0" for LTR
        }
        // Don't forget to clamp
        if (mScrollX > scrollRange) {
            mScrollX = scrollRange;
        }
        else if (mScrollX < 0) {
            mScrollX = 0;
        }
    }

    // Calling this with the present values causes it to re-claim them
    ScrollTo(mScrollX, mScrollY);
    return NOERROR;
}

void HorizontalScrollView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);

    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if (NULL == currentFocused || this == currentFocused.Get()) {
        return;
    }

    Int32 maxJump = mRight - mLeft;

    if (IsWithinDeltaOfScreen(currentFocused, maxJump)) {
        currentFocused->GetDrawingRect(mTempRect);
        OffsetDescendantRectToMyCoords(currentFocused, mTempRect);
        Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);
        DoScrollX(scrollDelta);
    }
}

Boolean HorizontalScrollView::IsViewDescendantOf(
    /* [in] */ IView* child,
    /* [in] */ IView* parent)
{
    if (child == parent) {
        return TRUE;
    }

    AutoPtr<IViewParent> theParent;
    child->GetParent((IViewParent**)&theParent);
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(theParent);
    AutoPtr<IView> viewParent = IView::Probe(theParent);
    return (viewGroup != NULL) && IsViewDescendantOf(viewParent, parent);
}

ECode HorizontalScrollView::Fling(
    /* [in] */ Int32 velocityX)
{
    Int32 count = 0;
    if((GetChildCount(&count), count) > 0) {
        Int32 width = 0;
        GetWidth(&width);
        width = width - mPaddingRight - mPaddingLeft;
        Int32 right = 0;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        child->GetWidth(&right);

        mScroller->Fling(mScrollX, mScrollY, velocityX, 0, 0,
            Elastos::Core::Math::Max(0, right - width), 0, 0, width/2, 0);
        Boolean movingRight = velocityX > 0;
        AutoPtr<IView> currentFocused;
        FindFocus((IView**)&currentFocused);
        Int32 finalX = 0;
        mScroller->GetFinalX(&finalX);
        AutoPtr<IView> newFocused = FindFocusableViewInMyBounds(movingRight,
            finalX, currentFocused);

        if(!newFocused) {
            newFocused = this;
        }

        if(newFocused.Get() != currentFocused.Get()) {
            Boolean res = FALSE;
            newFocused->RequestFocus(movingRight ? IView::FOCUS_RIGHT
                : IView::FOCUS_LEFT, &res);
        }
        PostInvalidateOnAnimation();
    }
    return NOERROR;
}

ECode HorizontalScrollView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count = 0;
    if ((GetChildCount(&count)) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 width = 0, value = 0;
        Int32 height = 0;
        child->GetWidth(&width);
        child->GetHeight(&height);
        x = Clamp(x, (GetWidth(&value), value) - mPaddingRight - mPaddingLeft, width);
        y = Clamp(y, (GetHeight(&value), value) - mPaddingBottom - mPaddingTop, height);
        if (x != mScrollX || y != mScrollY) {
            FAIL_RETURN(FrameLayout::ScrollTo(x, y));
        }
    }
    return NOERROR;
}

ECode HorizontalScrollView::SetOverScrollMode(
    /* [in] */ Int32 overScrollMode)
{
    if (overScrollMode != IView::OVER_SCROLL_NEVER) {
        if (mEdgeGlowLeft == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            CEdgeEffect::New(ctx, (IEdgeEffect**)&mEdgeGlowLeft);
            mEdgeGlowRight = NULL;
            CEdgeEffect::New(ctx, (IEdgeEffect**)&mEdgeGlowRight);
        }
    }
    else {
        mEdgeGlowLeft = NULL;
        mEdgeGlowRight = NULL;
    }
    return FrameLayout::SetOverScrollMode(overScrollMode);
}

ECode HorizontalScrollView::Draw(
    /* [in] */ ICanvas* canvas)
{
    FAIL_RETURN(FrameLayout::Draw(canvas));

    if (mEdgeGlowLeft != NULL) {
        Int32 scrollX = mScrollX;
        Boolean res = FALSE;
        mEdgeGlowLeft->IsFinished(&res);
        if (!res) {
            Int32 restoreCount = 0, w = 0;
            canvas->Save(&restoreCount);
            Int32 height = 0;
            GetHeight(&height);
            height = height - mPaddingTop - mPaddingBottom;;

            canvas->Rotate(270);
            canvas->Translate(-height + mPaddingTop, Elastos::Core::Math::Min(0, scrollX));
            mEdgeGlowLeft->SetSize(height, (GetWidth(&w), w));
            Boolean isDraw = FALSE;
            mEdgeGlowLeft->Draw(canvas, &isDraw);
            if (isDraw) {
                PostInvalidateOnAnimation();
            }
            canvas->RestoreToCount(restoreCount);
        }
        Boolean isFinished = FALSE;
        mEdgeGlowLeft->IsFinished(&isFinished);
        if (!isFinished) {
            Int32 restoreCount = 0;
            canvas->Save(&restoreCount);
            Int32 width = 0, h = 0;;
            GetWidth(&width);
            Int32 height = (GetHeight(&h), h) - mPaddingTop - mPaddingBottom;

            canvas->Rotate(90);
            canvas->Translate(-mPaddingTop, Elastos::Core::Math::Max(GetScrollRange(), scrollX) + width);
            mEdgeGlowRight->SetSize(height, width);
            Boolean isDraw = FALSE;
            mEdgeGlowLeft->Draw(canvas, &isDraw);
            if (isDraw) {
                PostInvalidateOnAnimation();
            }
            canvas->RestoreToCount(restoreCount);
        }
    }
    return NOERROR;
}

Int32 HorizontalScrollView::Clamp(
    /* [in] */ Int32 n,
    /* [in] */ Int32 my,
    /* [in] */ Int32 child)
{
    if (my >= child || n < 0) {
        return 0;
    }
    if ((my + n) > child) {
        return child - my;
    }
    return n;
}

void HorizontalScrollView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion = 0;
    info->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion <= Build::VERSION_CODES::JELLY_BEAN_MR2) {
        // Some old apps reused IDs in ways they shouldn't have.
        // Don't break them, but they don't get scroll state restoration.
        FrameLayout::OnRestoreInstanceState(state);
        return;
    }
    AutoPtr<SavedState> ss = (SavedState*) IViewBaseSavedState::Probe(state);
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    FrameLayout::OnRestoreInstanceState(p);
    mSavedState = ss;
    RequestLayout();
}

AutoPtr<IParcelable> HorizontalScrollView::OnSaveInstanceState()
{
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion = 0;
    info->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion <= Build::VERSION_CODES::JELLY_BEAN_MR2) {
        // Some old apps reused IDs in ways they shouldn't have.
        // Don't break them, but they don't get scroll state restoration.
        return FrameLayout::OnSaveInstanceState();
    }
    AutoPtr<IParcelable> superState = FrameLayout::OnSaveInstanceState();
    AutoPtr<IParcelable> _ss;
    CHorizontalScrollViewSavedState::New(superState, (IParcelable**)&_ss);
    SavedState* ss = (SavedState*)_ss.Get();
    ss->mScrollPosition = mScrollX;
    IsLayoutRtl(&ss->mIsLayoutRtl);
    return ss;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
