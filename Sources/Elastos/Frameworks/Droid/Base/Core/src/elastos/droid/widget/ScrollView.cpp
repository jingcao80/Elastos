
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/ScrollView.h"
#include "elastos/droid/widget/COverScroller.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/widget/CEdgeEffect.h"
#include "elastos/droid/widget/CScrollViewSavedState.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CViewGroupMarginLayoutParams.h"
#include "elastos/droid/view/FocusFinder.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::CViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::FocusFinder;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ScrollView::ScrollViewSavedState, BaseSavedState, IScrollViewSavedState);
ScrollView::ScrollViewSavedState::ScrollViewSavedState()
    : mScrollPosition(0)
{}

ScrollView::ScrollViewSavedState::~ScrollViewSavedState()
{}

ECode ScrollView::ScrollViewSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mScrollPosition);
    return NOERROR;
}

ECode ScrollView::ScrollViewSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    BaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mScrollPosition);
    return NOERROR;
}

ECode ScrollView::ScrollViewSavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("HorizontalScrollView.SavedState{");
    Int32 hash;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->IdentityHashCode(TO_IINTERFACE(this), &hash);
    sb.Append(StringUtils::ToHexString(hash));
    sb.Append(" scrollPosition=");
    sb.Append(mScrollPosition);
    sb.Append("}");
    return sb.ToString(str);
}

const Int32 ScrollView::ANIMATED_SCROLL_GAP = 250;
const Float ScrollView::MAX_SCROLL_FACTOR = 0.5;
const String ScrollView::TAG("ScrollView");
const Int32 ScrollView::INVALID_POINTER = -1;

CAR_INTERFACE_IMPL(ScrollView, FrameLayout, IScrollView);
ScrollView::ScrollView()
    : mLastScroll(0)
    , mLastMotionY(0)
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
    , mNestedYOffset(0)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));

    mScrollOffset = ArrayOf<Int32>::Alloc(2);
    mScrollConsumed = ArrayOf<Int32>::Alloc(2);
}

ECode ScrollView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::scrollViewStyle);
}

ECode ScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    InitScrollView();

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ScrollView),
            ArraySize(R::styleable::ScrollView));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    Boolean value = FALSE;
    a->GetBoolean(R::styleable::ScrollView_fillViewport, FALSE, &value);

    FAIL_RETURN(SetFillViewport(value));
    a->Recycle();

    return NOERROR;
}

ScrollView::~ScrollView()
{
}

ECode ScrollView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* compatibility)
{
    VALIDATE_NOT_NULL(compatibility);
    *compatibility = TRUE;
    return NOERROR;
}

Float ScrollView::GetTopFadingEdgeStrength()
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        return 0.0;
    }

    Int32 length = 0;
    GetVerticalFadingEdgeLength(&length);
    if (mScrollY < length) {
        return mScrollY / (Float) length;
    }

    return 1.0;
}

Float ScrollView::GetBottomFadingEdgeStrength()
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        return 0.0;
    }

    Int32 length = 0;
    GetVerticalFadingEdgeLength(&length);
    Int32 bottomEdge = 0;
    GetHeight(&bottomEdge);
    bottomEdge -= mPaddingBottom;
    Int32 bottom = 0;
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    child->GetBottom(&bottom);
    Int32 span = bottom - mScrollY - bottomEdge;
    if (span < length) {
        return span / (Float)length;
    }

    return 1.0;
}

ECode ScrollView::GetMaxScrollAmount(
    /* [out] */ Int32* maxAmount)
{
    VALIDATE_NOT_NULL(maxAmount);
    *maxAmount = (Int32)(MAX_SCROLL_FACTOR * (mBottom - mTop));
    return NOERROR;
}

void ScrollView::InitScrollView()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    COverScroller::New(context, (IOverScroller**)&mScroller);
    SetFocusable(TRUE);
    SetDescendantFocusability(ViewGroup::FOCUS_AFTER_DESCENDANTS);
    SetWillNotDraw(FALSE);
    AutoPtr<IViewConfiguration> configuration = ViewConfiguration::Get(mContext);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    configuration->GetScaledOverscrollDistance(&mOverscrollDistance);
    configuration->GetScaledOverflingDistance(&mOverflingDistance);
}

ECode ScrollView::AddView(
    /* [in] */ IView* child)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Logger::E(TAG, "ScrollView can host only one direct child");
        // throw new IllegalStateException("ScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child);
}

ECode ScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Logger::E(TAG, "ScrollView can host only one direct child");
        // throw new IllegalStateException("ScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, index);
}

ECode ScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Logger::E(TAG, "ScrollView can host only one direct child");
        // throw new IllegalStateException("ScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, params);
}

ECode ScrollView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Logger::E(TAG, "ScrollView can host only one direct child");
        // throw new IllegalStateException("ScrollView can host only one direct child");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return FrameLayout::AddView(child, index, params);
}

Boolean ScrollView::CanScroll()
{
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    if (child != NULL) {
        Int32 childHeight = 0, h = 0;
        child->GetHeight(&childHeight);
        GetHeight(&h);
        return h < childHeight + mPaddingTop + mPaddingBottom;
    }
    return FALSE;
}

ECode ScrollView::IsFillViewport(
    /* [out] */ Boolean* isFilled)
{
    VALIDATE_NOT_NULL(isFilled);
    *isFilled = mFillViewport;
    return NOERROR;
}

ECode ScrollView::SetFillViewport(
    /* [in] */ Boolean fillViewport)
{
    if (fillViewport != mFillViewport) {
        mFillViewport = fillViewport;
        RequestLayout();
    }

    return NOERROR;
}

ECode ScrollView::IsSmoothScrollingEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mSmoothScrollingEnabled;
    return NOERROR;
}

ECode ScrollView::SetSmoothScrollingEnabled(
    /* [in] */ Boolean smoothScrollingEnabled)
{
    mSmoothScrollingEnabled = smoothScrollingEnabled;

    return NOERROR;
}

void ScrollView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (!mFillViewport) {
        return;
    }

    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    if (heightMode == MeasureSpec::UNSPECIFIED) {
        return;
    }

    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 height = 0;
        GetMeasuredHeight(&height);
        Int32 childHeight = 0;
        child->GetMeasuredHeight(&childHeight);
        if (childHeight < height) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IFrameLayoutLayoutParams* params = IFrameLayoutLayoutParams::Probe(vglp);

            Int32 childWidthMeasureSpec = GetChildMeasureSpec(widthMeasureSpec,
                    mPaddingLeft + mPaddingRight,
                    ((CFrameLayoutLayoutParams*)params)->mWidth);
            height -= mPaddingTop;
            height -= mPaddingBottom;
            Int32 childHeightMeasureSpec =
                    MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY);

            child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
        }
    }
}

ECode ScrollView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    // Let the focused view and/or our descendants get the key first
    *result = (FrameLayout::DispatchKeyEvent(event, &tmp), tmp) || (ExecuteKeyEvent(event, &tmp), tmp);
    return NOERROR;
}

ECode ScrollView::ExecuteKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* handled)
{
    VALIDATE_NOT_NULL(handled);
    *handled = FALSE;
    mTempRect->SetEmpty();

    if (!CanScroll()) {
        Int32 keyCode;
        Boolean focused = FALSE;
        if ((IsFocused(&focused)) && (event->GetKeyCode(&keyCode), keyCode) != IKeyEvent::KEYCODE_BACK) {
            AutoPtr<IView> currentFocused;
            FindFocus((IView**)&currentFocused);
            if (currentFocused.Get() == (IView*)this) {
                currentFocused = NULL;
            }

            AutoPtr<FocusFinder> finder = FocusFinder::GetInstance();
            AutoPtr<IView> nextFocused;
            finder->FindNextFocus(this,
                    currentFocused, IView::FOCUS_DOWN, (IView**)&nextFocused);

            if (nextFocused != NULL && nextFocused.Get() != (IView*)this) {
                Boolean isFocus;
                nextFocused->RequestFocus(IView::FOCUS_DOWN, &isFocus);
                *handled = isFocus;
                return NOERROR;
            }
            else {
                return NOERROR;
            }
        }
        return NOERROR;
    }

    Int32 action = 0;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 keyCode = 0;
        event->GetKeyCode(&keyCode);
        Boolean isPressed;
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_UP:
                event->IsAltPressed(&isPressed);
                if (!isPressed) {
                    ArrowScroll(IView::FOCUS_UP, handled);
                }
                else {
                    FullScroll(IView::FOCUS_UP, handled);
                }
                break;
            case IKeyEvent::KEYCODE_DPAD_DOWN:
                event->IsAltPressed(&isPressed);
                if (!isPressed) {
                    ArrowScroll(IView::FOCUS_DOWN, handled);
                }
                else {
                    FullScroll(IView::FOCUS_DOWN, handled);
                }
                break;
            case IKeyEvent::KEYCODE_SPACE:
                event->IsShiftPressed(&isPressed);
                Boolean tmp = FALSE;
                PageScroll(isPressed ? IView::FOCUS_UP : IView::FOCUS_DOWN, &tmp);
                break;
        }
    }

    return NOERROR;
}

Boolean ScrollView::InChild(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Int32 scrollY = mScrollY;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 top, bottom, left, right;
        return !(y < (child->GetTop(&top), top) - scrollY
                || y >= (child->GetBottom(&bottom), bottom) - scrollY
                || x < (child->GetLeft(&left), left)
                || x >= (child->GetRight(&right), right));
    }

    return FALSE;
}

void ScrollView::InitOrResetVelocityTracker()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    else {
        mVelocityTracker->Clear();
    }
}

void ScrollView::InitVelocityTrackerIfNotExists()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
}

void ScrollView::RecycleVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

ECode ScrollView::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    if (disallowIntercept) {
        RecycleVelocityTracker();
    }
    FrameLayout::RequestDisallowInterceptTouchEvent(disallowIntercept);
    return NOERROR;
}

ECode ScrollView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    /*
     * This method JUST determines whether we want to intercept the motion.
     * If we return TRUE, onMotionEvent will be called and we do the actual
     * scrolling there.
     */

    /*
    * Shortcut the most recurring case: the user is in the dragging
    * state and he is moving his finger.  We want to intercept this
    * motion.
    */
    Int32 action;
    ev->GetAction(&action);
    if ((action == IMotionEvent::ACTION_MOVE) && (mIsBeingDragged)) {
        *value = TRUE;
        return NOERROR;
    }

    /*
     * Don't try to intercept touch if we can't scroll anyway.
     */
    Int32 sy = 0;
    Boolean can = FALSE;
    if ((GetScrollY(&sy), sy) == 0 && (CanScrollVertically(1, &can), !can)) {
        *value = FALSE;
        return NOERROR;
    }

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_MOVE: {
            /*
             * mIsBeingDragged == FALSE, otherwise the shortcut would have caught it. Check
             * whether the user has moved far enough from his original down touch.
             */

            /*
            * Locally do absolute value. mLastMotionY is set to the y value
            * of the down event.
            */
            Int32 activePointerId = mActivePointerId;
            if (activePointerId == INVALID_POINTER) {
                // If we don't have a valid id, the touch down wasn't on content.
                break;
            }

            Int32 pointerIndex;
            ev->FindPointerIndex(activePointerId, &pointerIndex);
            if (pointerIndex == -1) {
                Logger::E(TAG, "Invalid pointerId= %d in onInterceptTouchEvent", activePointerId);
                break;
            }

            Float fy;
            ev->GetY(pointerIndex, &fy);
            Int32 y = (Int32) fy;
            Int32 yDiff = Elastos::Core::Math::Abs(y - mLastMotionY);
            Int32 axes = 0;
            if (yDiff > mTouchSlop && ((GetNestedScrollAxes(&axes), axes) & SCROLL_AXIS_VERTICAL) == 0) {
                mIsBeingDragged = TRUE;
                mLastMotionY = y;

                InitVelocityTrackerIfNotExists();
                mVelocityTracker->AddMovement(ev);
                mNestedYOffset = 0;
                if (mScrollStrictSpan == NULL) {
                    AutoPtr<IStrictMode> mode;
                    CStrictMode::AcquireSingleton((IStrictMode**)&mode);
                    mode->EnterCriticalSpan(String("ScrollView-scroll"), (IStrictModeSpan**)&mScrollStrictSpan);
                }
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if (parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }
            break;
        }

        case IMotionEvent::ACTION_DOWN: {
            Float y, x;
            ev->GetY(&y);
            ev->GetX(&x);
            if (!InChild((Int32)x, (Int32)y)) {
                mIsBeingDragged = FALSE;
                RecycleVelocityTracker();
                break;
            }

            /*
             * Remember location of down touch.
             * ACTION_DOWN always refers to pointer index 0.
             */
            mLastMotionY = y;
            ev->GetPointerId(0, &mActivePointerId);

            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(ev);

            /*
            * If being flinged and user touches the screen, initiate drag;
            * otherwise don't.  mScroller.isFinished should be FALSE when
            * being flinged.
            */
            Boolean finished;
            mScroller->IsFinished(&finished);
            mIsBeingDragged = !finished;

            if (mIsBeingDragged && mScrollStrictSpan == NULL) {
                AutoPtr<IStrictMode> mode;
                CStrictMode::AcquireSingleton((IStrictMode**)&mode);
                mode->EnterCriticalSpan(String("ScrollView-scroll"), (IStrictModeSpan**)&mScrollStrictSpan);
            }

            Boolean tmp = FALSE;
            StartNestedScroll(SCROLL_AXIS_VERTICAL, &tmp);
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP: {
            /* Release the drag */
            mIsBeingDragged = FALSE;
            mActivePointerId = INVALID_POINTER;
            RecycleVelocityTracker();
            Boolean tmp = FALSE;
            if ((mScroller->SpringBack(mScrollX, mScrollY, 0, 0, 0, GetScrollRange(), &tmp), tmp)) {
                PostInvalidateOnAnimation();
            }
            StopNestedScroll();
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            break;
    }

    /*
    * The only time we want to intercept motion events is if we are in the
    * drag mode.
    */
    *value = mIsBeingDragged;
    return NOERROR;
}

ECode ScrollView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    InitVelocityTrackerIfNotExists();

    AutoPtr<IMotionEvent> vtev;
    MotionEvent::Obtain(ev, (IMotionEvent**)&vtev);

    Int32 actionMasked = 0;
    ev->GetActionMasked(&actionMasked);

    if (actionMasked == IMotionEvent::ACTION_DOWN) {
        mNestedYOffset = 0;
    }
    vtev->OffsetLocation(0, mNestedYOffset);

    using Elastos::Core::Math;

    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            Int32 count = 0;
            if ((GetChildCount(&count), count) == 0) {
                *value = FALSE;
                return NOERROR;
            }
            Boolean finished = FALSE;
            if ((mIsBeingDragged = (mScroller->IsFinished(&finished), !finished))) {
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if (parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }

            /*
             * If being flinged and user touches, stop the fling. isFinished
             * will be FALSE if being flinged.
             */
            if ((mScroller->IsFinished(&finished), !finished)) {
                mScroller->AbortAnimation();

                if (mFlingStrictSpan != NULL) {
                    mFlingStrictSpan->Finish();
                    mFlingStrictSpan = NULL;
                }
            }

            // Remember where the motion event started
            Float lasty;
            ev->GetY(&lasty);
            mLastMotionY = (Int32)lasty;
            ev->GetPointerId(0, &mActivePointerId);
            Boolean tmp = FALSE;
            StartNestedScroll(SCROLL_AXIS_VERTICAL, &tmp);
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            Int32 activePointerIndex;
            ev->FindPointerIndex(mActivePointerId, &activePointerIndex);
            if (activePointerIndex == -1) {
                Logger::E(TAG, "Invalid pointerId=%d in onTouchEvent", mActivePointerId);
                break;
            }

            Float lasty;
            ev->GetY(activePointerIndex, &lasty);
            Int32 y = (Int32)lasty;
            Int32 deltaY = mLastMotionY - y;

            Boolean result = FALSE;
            if (DispatchNestedPreScroll(0, deltaY, mScrollConsumed, mScrollOffset, &result), result) {
                deltaY -= (*mScrollConsumed)[1];
                vtev->OffsetLocation(0, (*mScrollOffset)[1]);
                mNestedYOffset += (*mScrollOffset)[1];
            }

            if (!mIsBeingDragged && Math::Abs(deltaY) > mTouchSlop) {
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if (parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
                mIsBeingDragged = TRUE;
                if (deltaY > 0) {
                    deltaY -= mTouchSlop;
                }
                else {
                    deltaY += mTouchSlop;
                }
            }

            if (mIsBeingDragged) {
                // Scroll to follow the motion event
                mLastMotionY = y - (*mScrollOffset)[1];

                Int32 oldY = mScrollY;
                Int32 range = GetScrollRange();
                Int32 overscrollMode = 0;
                GetOverScrollMode(&overscrollMode);
                Boolean canOverscroll = overscrollMode == IView::OVER_SCROLL_ALWAYS ||
                    (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS && range > 0);

                // Calling overScrollBy will call onOverScrolled, which
                // calls onScrollChanged if applicable.
                Boolean has = FALSE;
                if (OverScrollBy(0, deltaY, 0, mScrollY, 0, range, 0, mOverscrollDistance, TRUE)
                        && (HasNestedScrollingParent(&has), !has)) {
                    // Break our velocity if we hit a scroll barrier.
                    mVelocityTracker->Clear();
                }

                Int32 scrolledDeltaY = mScrollY - oldY;
                Int32 unconsumedY = deltaY - scrolledDeltaY;
                Boolean tmp = FALSE;
                if (DispatchNestedScroll(0, scrolledDeltaY, 0, unconsumedY, mScrollOffset, &tmp), tmp) {
                    mLastMotionY -= (*mScrollOffset)[1];
                    vtev->OffsetLocation(0, (*mScrollOffset)[1]);
                    mNestedYOffset += (*mScrollOffset)[1];
                }
                else if (canOverscroll) {
                    Int32 pulledToY = oldY + deltaY;
                    if (pulledToY < 0) {
                        Int32 w = 0, h = 0;
                        GetWidth(&w);
                        GetHeight(&h);
                        Float x = 0;
                        ev->GetX(activePointerIndex, &x);
                        mEdgeGlowTop->OnPull((Float) deltaY / h, x / w);
                        Boolean bf = FALSE;
                        mEdgeGlowBottom->IsFinished(&bf);
                        if (!bf) {
                            mEdgeGlowBottom->OnRelease();
                        }
                    }
                    else if (pulledToY > range) {
                        Int32 w = 0, h = 0;
                        GetWidth(&w);
                        GetHeight(&h);
                        Float x = 0;
                        ev->GetX(activePointerIndex, &x);
                        mEdgeGlowBottom->OnPull((Float) deltaY / h, 1.f - x / w);
                        Boolean tf = FALSE;
                        mEdgeGlowTop->IsFinished(&tf);
                        if (!tf) {
                            mEdgeGlowTop->OnRelease();
                        }
                    }

                    if (mEdgeGlowTop != NULL) {
                        Boolean tf, bf;
                        if ((mEdgeGlowTop->IsFinished(&tf), !tf) || (mEdgeGlowBottom->IsFinished(&bf), !bf)) {
                            PostInvalidateOnAnimation();
                        }
                    }
                }
            }
        }
        break;
        case IMotionEvent::ACTION_UP: {
            if (mIsBeingDragged) {
                mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                Float y;
                mVelocityTracker->GetYVelocity(mActivePointerId, &y);
                Int32 initialVelocity = (Int32)y;

                Boolean tmp = FALSE;
                if ((Math::Abs(initialVelocity) > mMinimumVelocity)) {
                    FlingWithNestedDispatch(-initialVelocity);
                }
                else if ((mScroller->SpringBack(mScrollX, mScrollY, 0, 0, 0, GetScrollRange(), &tmp), tmp)) {
                    PostInvalidateOnAnimation();
                }

                mActivePointerId = INVALID_POINTER;
                EndDrag();
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL: {
            Int32 count = 0;
            if (mIsBeingDragged && (GetChildCount(&count), count) > 0) {
                Boolean tmp = FALSE;
                if ((mScroller->SpringBack(mScrollX, mScrollY, 0, 0, 0, GetScrollRange(), &tmp), tmp)) {
                    PostInvalidateOnAnimation();
                }
                mActivePointerId = INVALID_POINTER;
                EndDrag();
            }
        }
            break;
        case IMotionEvent::ACTION_POINTER_DOWN: {
            Int32 index;
            ev->GetActionIndex(&index);
            Float lasty;
            ev->GetY(index, &lasty);
            mLastMotionY = (Int32)lasty;
            ev->GetPointerId(index, &mActivePointerId);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            Int32 index;
            ev->FindPointerIndex(mActivePointerId, &index);
            Float lasty;
            ev->GetY(index, &lasty);
            mLastMotionY = (Int32)lasty;
            break;
    }
    if (mVelocityTracker != NULL) {
        mVelocityTracker->AddMovement(vtev);
    }
    IInputEvent::Probe(vtev)->Recycle();
    *value = TRUE;
    return NOERROR;
}

void ScrollView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    Int32 pointerIndex = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK) >>
            IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
    Int32 pointerId;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        // This was our active pointer going up. Choose a new
        // active pointer and adjust accordingly.
        // TODO: Make this decision more intelligent.
        Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        Float lastY;
        ev->GetY(newPointerIndex, &lastY);
        mLastMotionY = (Int32)lastY;
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
        if (mVelocityTracker != NULL) {
            mVelocityTracker->Clear();
        }
    }
}

ECode ScrollView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* handled)
{
    VALIDATE_NOT_NULL(handled);
    Int32 source = 0;
    IInputEvent::Probe(event)->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action;
        event->GetAction(&action);

        switch (action) {
            case IMotionEvent::ACTION_SCROLL: {
                if (!mIsBeingDragged) {
                    Float vscroll;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                    if (vscroll != 0) {
                        Float scrollFactor;
                        GetVerticalScrollFactor(&scrollFactor);
                        Int32 delta = (Int32) (vscroll * scrollFactor);
                        Int32 range = GetScrollRange();
                        Int32 oldScrollY = mScrollY;
                        Int32 newScrollY = oldScrollY - delta;
                        if (newScrollY < 0) {
                            newScrollY = 0;
                        }
                        else if (newScrollY > range) {
                            newScrollY = range;
                        }
                        if (newScrollY != oldScrollY) {
                            FrameLayout::ScrollTo(mScrollX, newScrollY);
                            *handled = TRUE;
                            return NOERROR;
                        }
                    }
                }
            }
        }
    }
    return FrameLayout::OnGenericMotionEvent(event, handled);
}

void ScrollView::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    // Treat animating scrolls differently; see #computeScroll() for why.
    Boolean finished = FALSE;
    if ((mScroller->IsFinished(&finished), !finished)) {
        const Int32 oldX = mScrollX;
        const Int32 oldY = mScrollY;
        mScrollX = scrollX;
        mScrollY = scrollY;
        InvalidateParentIfNeeded();
        OnScrollChanged(mScrollX, mScrollY, oldX, oldY);
        if (clampedY) {
            Boolean tmp = FALSE;
            mScroller->SpringBack(mScrollX, mScrollY, 0, 0, 0, GetScrollRange(), &tmp);
        }
    }
    else {
        FrameLayout::ScrollTo(scrollX, scrollY);
    }

    AwakenScrollBars();
}

ECode ScrollView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* performed)
{
    VALIDATE_NOT_NULL(performed);
    *performed = FALSE;
    if (FrameLayout::PerformAccessibilityAction(action, arguments, performed), *performed) {
        *performed = TRUE;
        return NOERROR;
    }
    Boolean enabled = FALSE;
    if ((IsEnabled(&enabled), !enabled)) {
        return NOERROR;
    }

    using Elastos::Core::Math;

    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            Int32 height = 0;
            GetHeight(&height);
            Int32 viewportHeight = height - mPaddingBottom - mPaddingTop;
            Int32 targetScrollY = Math::Min(mScrollY + viewportHeight, GetScrollRange());
            if (targetScrollY != mScrollY) {
                SmoothScrollTo(0, targetScrollY);
                *performed = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            Int32 height = 0;
            GetHeight(&height);
            Int32 viewportHeight = height - mPaddingBottom - mPaddingTop;
            Int32 targetScrollY = Math::Max(mScrollY - viewportHeight, 0);
            if (targetScrollY != mScrollY) {
                SmoothScrollTo(0, targetScrollY);
                *performed = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode ScrollView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> csq;
    CString::New(String("CScrollView"), (ICharSequence**)&csq);
    info->SetClassName(csq);
    Boolean enabled = FALSE;
    if (IsEnabled(&enabled), enabled) {
        Int32 scrollRange = GetScrollRange();
        if (scrollRange > 0) {
            info->SetScrollable(TRUE);
            if (mScrollY > 0) {
                info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
            }
            if (mScrollY < scrollRange) {
                info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
            }
        }
    }
    return NOERROR;
}

ECode ScrollView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> csq;
    CString::New(String("CScrollView"), (ICharSequence**)&csq);
    IAccessibilityRecord* _event = IAccessibilityRecord::Probe(event);
    _event->SetClassName(csq);

    Boolean scrollable = GetScrollRange() > 0;
    _event->SetScrollable(scrollable);
    _event->SetScrollX(mScrollX);
    _event->SetScrollY(mScrollY);
    _event->SetMaxScrollX(mScrollX);
    _event->SetMaxScrollY(GetScrollRange());
    return NOERROR;
}

Int32 ScrollView::GetScrollRange()
{
    Int32 scrollRange = 0, count = 0;
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 childHeight = 0, h = 0;
        child->GetHeight(&childHeight);
        GetHeight(&h);
        scrollRange = Elastos::Core::Math::Max(0,
                childHeight- (h - mPaddingBottom - mPaddingTop));
    }
    return scrollRange;
}

AutoPtr<IView> ScrollView::FindFocusableViewInBounds(
    /* [in] */ Boolean topFocus,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IArrayList> focusables;
    ASSERT_SUCCEEDED(GetFocusables(IView::FOCUS_FORWARD, (IArrayList**)&focusables));

    AutoPtr<IView> focusCandidate;

    /*
     * A fully contained focusable is one where its top is below the bound's
     * top, and its bottom is above the bound's bottom. A partially
     * contained focusable is one where some part of it is within the
     * bounds, but it also has some part that is not within bounds.  A fully contained
     * focusable is preferred to a partially contained focusable.
     */
    Boolean foundFullyContainedFocusable = FALSE;

    Int32 size = 0;
    focusables->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        focusables->Get(i, (IInterface**)&obj);
        AutoPtr<IView> view = IView::Probe(obj);

        Int32 viewTop, viewBottom;
        view->GetTop(&viewTop);
        view->GetBottom(&viewBottom);

        if (top < viewBottom && viewTop < bottom) {
            /*
             * the focusable is in the target area, it is a candidate for
             * focusing
             */

            Boolean viewIsFullyContained = (top < viewTop) &&
                    (viewBottom < bottom);

            if (focusCandidate == NULL) {
                /* No candidate, take this one */
                focusCandidate = view;
                foundFullyContainedFocusable = viewIsFullyContained;
            }
            else {
                Int32 t, b;
                Boolean viewIsCloserToBoundary =
                        (topFocus && viewTop < (focusCandidate->GetTop(&t), t))
                        || (!topFocus && viewBottom > (focusCandidate->GetBottom(&b), b));

                if (foundFullyContainedFocusable) {
                    if (viewIsFullyContained && viewIsCloserToBoundary) {
                        /*
                         * We're dealing with only fully contained views, so
                         * it has to be closer to the boundary to beat our
                         * candidate
                         */
                        focusCandidate = view;
                    }
                }
                else {
                    if (viewIsFullyContained) {
                        /* Any fully contained view beats a partially contained view */
                        focusCandidate = view;
                        foundFullyContainedFocusable = TRUE;
                    }
                    else if (viewIsCloserToBoundary) {
                        /*
                         * Partially contained view beats another partially
                         * contained view if it's closer
                         */
                        focusCandidate = view;
                    }
                }
            }
        }
    }

    return focusCandidate;
}

ECode ScrollView::PageScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean down = direction == IView::FOCUS_DOWN;
    Int32 height = 0;
    GetHeight(&height);

    Int32 sy = 0, top = 0;
    if (down) {
        GetScrollY(&sy);
        top = sy + height;
        mTempRect->SetTop(top);
        Int32 count = 0;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetChildAt(count - 1, (IView**)&view);
            Int32 bottom;
            view->GetBottom(&bottom);
            if (top + height > bottom) {
                mTempRect->SetTop(bottom - height);
            }
        }
    }
    else {
        GetScrollY(&sy);
        top = sy - height;
        mTempRect->SetTop(top);
        if (top < 0) {
            mTempRect->SetTop(0);
        }
    }

    mTempRect->GetTop(&top);
    Int32 bottom = top + height;
    mTempRect->SetBottom(bottom);

    *result = ScrollAndFocus(direction, top, bottom);
    return NOERROR;
}

ECode ScrollView::FullScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean down = direction == IView::FOCUS_DOWN;
    Int32 height = 0;
    GetHeight(&height);

    mTempRect->SetTop(0);
    mTempRect->SetBottom(height);

    if (down) {
        Int32 count = 0;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetChildAt(count - 1, (IView**)&view);
            Int32 bottom;
            view->GetBottom(&bottom);
            bottom += mPaddingBottom;
            mTempRect->SetBottom(bottom);
            mTempRect->SetTop(bottom - height);
        }
    }

    Int32 top, bottom;
    mTempRect->GetTop(&top);
    mTempRect->GetBottom(&bottom);
    *result = ScrollAndFocus(direction, top, bottom);
    return NOERROR;
}

Boolean ScrollView::ScrollAndFocus(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom)
{
    Boolean handled = TRUE;

    Int32 height = 0;
    GetHeight(&height);
    Int32 containerTop = 0;
    GetScrollY(&containerTop);
    Int32 containerBottom = containerTop + height;
    Boolean up = direction == IView::FOCUS_UP;

    AutoPtr<IView> newFocused = FindFocusableViewInBounds(up, top, bottom);
    if (newFocused == NULL) {
        newFocused = this;
    }

    if (top >= containerTop && bottom <= containerBottom) {
        handled = FALSE;
    }
    else {
        Int32 delta = up ? (top - containerTop) : (bottom - containerBottom);
        DoScrollY(delta);
    }

    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if (newFocused != currentFocused) {
        newFocused->RequestFocus(direction, &up);
    }

    return handled;
}

ECode ScrollView::ArrowScroll(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if (currentFocused.Get() == (IView*)this) {
        currentFocused = NULL;
    }

    AutoPtr<FocusFinder> finder = FocusFinder::GetInstance();
    AutoPtr<IView> nextFocused;
    finder->FindNextFocus(this, currentFocused, direction, (IView**)&nextFocused);

    Int32 maxJump = 0;
    GetMaxScrollAmount(&maxJump);

    Int32 height = 0;
    if (nextFocused != NULL && IsWithinDeltaOfScreen(nextFocused, maxJump, (GetHeight(&height), height))) {
        nextFocused->GetDrawingRect(mTempRect);
        OffsetDescendantRectToMyCoords(nextFocused, mTempRect);
        Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);
        DoScrollY(scrollDelta);
        Boolean result;
        nextFocused->RequestFocus(direction, &result);
    }
    else {
        // no new focus
        Int32 scrollDelta = maxJump, sy = 0;

        if (direction == IView::FOCUS_UP && (GetScrollY(&sy), sy) < scrollDelta) {
            GetScrollY(&scrollDelta);
        }
        else if (direction == IView::FOCUS_DOWN) {
            Int32 count = 0;
            if ((GetChildCount(&count), count) > 0) {
                Int32 daBottom = 0, h = 0;
                AutoPtr<IView> child;
                GetChildAt(0, (IView**)&child);
                child->GetBottom(&daBottom);

                GetHeight(&h);
                Int32 screenBottom = 0;
                GetScrollY(&screenBottom);
                screenBottom += h - mPaddingBottom;

                if (daBottom - screenBottom < maxJump) {
                    scrollDelta = daBottom - screenBottom;
                }
            }
        }
        if (scrollDelta == 0) {
            return NOERROR;
        }
        DoScrollY(direction == IView::FOCUS_DOWN ? scrollDelta : -scrollDelta);
    }

    Boolean value = FALSE;
    if (currentFocused != NULL && (currentFocused->IsFocused(&value), value)
            && IsOffScreen(currentFocused)) {
        // previously focused item still has focus and is off screen, give
        // it up (take it back to ourselves)
        // (also, need to temporarily force FOCUS_BEFORE_DESCENDANTS so we are
        // sure to
        // get it)
        Int32 descendantFocusability = 0;
        GetDescendantFocusability(&descendantFocusability);  // save
        SetDescendantFocusability(IViewGroup::FOCUS_BEFORE_DESCENDANTS);
        RequestFocus(&value);
        SetDescendantFocusability(descendantFocusability);  // restore
    }
    *result = TRUE;
    return NOERROR;
}

Boolean ScrollView::IsOffScreen(
    /* [in] */ IView* descendant)
{
    Int32 height = 0;
    GetHeight(&height);
    return !IsWithinDeltaOfScreen(descendant, 0, height);
}

Boolean ScrollView::IsWithinDeltaOfScreen(
    /* [in] */ IView* descendant,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 height)
{
    descendant->GetDrawingRect(mTempRect);
    OffsetDescendantRectToMyCoords(descendant, mTempRect);

    Int32 sy = 0;
    GetScrollY(&sy);
    Int32 top, bottom;
    mTempRect->GetTop(&top);
    mTempRect->GetBottom(&bottom);
    return (bottom + delta) >= sy && (top - delta) <= (sy + height);
}

void ScrollView::DoScrollY(
    /* [in] */ Int32 delta)
{
    if (delta != 0) {
        if (mSmoothScrollingEnabled) {
            SmoothScrollBy(0, delta);
        }
        else {
            ScrollBy(0, delta);
        }
    }
}

ECode ScrollView::SmoothScrollBy(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) {
        // Nothing to do.
        return NOERROR;
    }

    using Elastos::Core::Math;

    Int64 duration = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&duration);
    duration -= mLastScroll;
    if (duration > ANIMATED_SCROLL_GAP) {
        Int32 height = 0;
        GetHeight(&height);
        height = height - mPaddingBottom - mPaddingTop;
        Int32 bottom;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        child->GetHeight(&bottom);
        Int32 maxY = Math::Max(0, bottom - height);
        Int32 scrollY = mScrollY;
        dy = Math::Max(0, Math::Min(scrollY + dy, maxY)) - scrollY;

        mScroller->StartScroll(mScrollX, scrollY, 0, dy);
        PostInvalidateOnAnimation();
    }
    else {
        Boolean finished = FALSE;
        if ((mScroller->IsFinished(&finished), !finished)) {
            mScroller->AbortAnimation();

            if (mFlingStrictSpan != NULL) {
                mFlingStrictSpan->Finish();
                mFlingStrictSpan = NULL;
            }
        }
        ScrollBy(dx, dy);
    }
    AnimationUtils::CurrentAnimationTimeMillis(&mLastScroll);

    return NOERROR;
}

ECode ScrollView::SmoothScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return SmoothScrollBy(x - mScrollX, y - mScrollY);
}

Int32 ScrollView::ComputeVerticalScrollRange()
{
    Int32 count = 0, h = 0;
    GetChildCount(&count);
    GetHeight(&h);
    Int32 contentHeight = h - mPaddingBottom - mPaddingTop;
    if (count == 0) {
        return contentHeight;
    }

    Int32 scrollRange = 0;
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    child->GetBottom(&scrollRange);
    Int32 scrollY = mScrollY;
    Int32 overscrollBottom = Elastos::Core::Math::Max(0, scrollRange - contentHeight);
    if (scrollY < 0) {
        scrollRange -= scrollY;
    }
    else if (scrollY > overscrollBottom) {
        scrollRange += scrollY - overscrollBottom;
    }

    return scrollRange;
}

Int32 ScrollView::ComputeVerticalScrollOffset()
{
    return Elastos::Core::Math::Max(0, FrameLayout::ComputeVerticalScrollOffset());
}

void ScrollView::MeasureChild(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 parentHeightMeasureSpec)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 childWidthMeasureSpec;
    Int32 childHeightMeasureSpec;

    Int32 width;
    lp->GetWidth(&width);
    childWidthMeasureSpec = GetChildMeasureSpec(parentWidthMeasureSpec,
            mPaddingLeft + mPaddingRight, width);

    childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void ScrollView::MeasureChildWithMargins(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 widthUsed,
    /* [in] */ Int32 parentHeightMeasureSpec,
    /* [in] */ Int32 heightUsed)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    CViewGroupMarginLayoutParams* lp = (CViewGroupMarginLayoutParams*)IViewGroupMarginLayoutParams::Probe(params);

    Int32 childWidthMeasureSpec = GetChildMeasureSpec(parentWidthMeasureSpec,
            mPaddingLeft + mPaddingRight + lp->mLeftMargin + lp->mRightMargin
            + widthUsed, lp->mWidth);
    Int32 childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(
            lp->mTopMargin + lp->mBottomMargin, MeasureSpec::UNSPECIFIED);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

ECode ScrollView::ComputeScroll()
{
    Boolean value = FALSE;
    if (mScroller->ComputeScrollOffset(&value), value) {
        // This is called at drawing time by ViewGroup.  We don't want to
        // re-show the scrollbars at this point, which scrollTo will do,
        // so we replicate most of scrollTo here.
        //
        //         It's a little odd to call onScrollChanged from inside the drawing.
        //
        //         It is, except when you remember that computeScroll() is used to
        //         animate scrolling. So unless we want to defer the onScrollChanged()
        //         until the end of the animated scrolling, we don't really have a
        //         choice here.
        //
        //         I agree.  The alternative, which I think would be worse, is to post
        //         something and tell the subclasses later.  This is bad because there
        //         will be a window where mScrollX/Y is different from what the app
        //         thinks it is.
        //
        Int32 oldX = mScrollX;
        Int32 oldY = mScrollY;
        Int32 x = 0;
        mScroller->GetCurrX(&x);
        Int32 y = 0;
        mScroller->GetCurrY(&y);

        if (oldX != x || oldY != y) {

            Int32 range = GetScrollRange();
            Int32 overscrollMode = 0;
            GetOverScrollMode(&overscrollMode);
            Boolean canOverscroll = overscrollMode == IView::OVER_SCROLL_ALWAYS ||
                (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS && range > 0);

            OverScrollBy(x - oldX, y - oldY, oldX, oldY, 0, range,
                0, mOverflingDistance, FALSE);
            OnScrollChanged(mScrollX, mScrollY, oldX, oldY);

            if (canOverscroll) {
                if (y < 0 && oldY >= 0) {
                    Float velocity = 0;
                    mScroller->GetCurrVelocity(&velocity);
                    mEdgeGlowTop->OnAbsorb((Int32) velocity);
                }
                else if (y > range && oldY <= range) {
                    Float velocity = 0;
                    mScroller->GetCurrVelocity(&velocity);
                    mEdgeGlowBottom->OnAbsorb((Int32) velocity);
                }
            }
        }

        if (!AwakenScrollBars()) {
            // Keep on drawing until the animation has finished.
            PostInvalidateOnAnimation();
        }
    }
    else {
        if (mFlingStrictSpan != NULL) {
            mFlingStrictSpan->Finish();
            mFlingStrictSpan = NULL;
        }
    }


    return NOERROR;
}

void ScrollView::ScrollToChild(
    /* [in] */ IView* child)
{
    child->GetDrawingRect(mTempRect);

    /* Offset from child's local coordinates to ScrollView coordinates */
    OffsetDescendantRectToMyCoords(child, mTempRect);

    Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);

    if (scrollDelta != 0) {
        ScrollBy(0, scrollDelta);
    }
}

Boolean ScrollView::ScrollToChildRect(
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
{
    Int32 delta = ComputeScrollDeltaToGetChildRectOnScreen(rect);
    Boolean scroll = delta != 0;
    if (scroll) {
        if (immediate) {
            ScrollBy(0, delta);
        }
        else {
            SmoothScrollBy(0, delta);
        }
    }

    return scroll;
}

/**
 * Compute the amount to scroll in the Y direction in order to get
 * a rectangle completely on the screen (or, if taller than the screen,
 * at least the first screen size chunk of it).
 *
 * @param rect The rect.
 * @return The scroll delta.
 */
Int32 ScrollView::ComputeScrollDeltaToGetChildRectOnScreen(
    /* [in] */ IRect* rect)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) == 0) return 0;

    Int32 height = 0;
    GetHeight(&height);
    Int32 screenTop = 0;
    GetScrollY(&screenTop);
    Int32 screenBottom = screenTop + height;

    Int32 fadingEdge = 0;
    GetVerticalFadingEdgeLength(&fadingEdge);

    Int32 l, t, r, b, h;
    rect->Get(&l, &t, &r, &b);
    rect->GetHeight(&h);

    // leave room for top fading edge as long as rect isn't at very top
    if (t > 0) {
        screenTop += fadingEdge;
    }

    // leave room for bottom fading edge as long as rect isn't at very bottom
    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    Int32 childHeight;
    child->GetHeight(&childHeight);
    if (b < childHeight) {
        screenBottom -= fadingEdge;
    }

    Int32 scrollYDelta = 0;

    using Elastos::Core::Math;
    if (b > screenBottom && t > screenTop) {
        // need to move down to get it in view: move down just enough so
        // that the entire rectangle is in view (or at least the first
        // screen size chunk).

        if (h > height) {
            // just enough to get screen size chunk on
            scrollYDelta += (t - screenTop);
        }
        else {
            // get entire rect at bottom of screen
            scrollYDelta += (b - screenBottom);
        }

        // make sure we aren't scrolling beyond the end of our content
        Int32 bottom;
        child->GetBottom(&bottom);
        Int32 distanceToBottom = bottom - screenBottom;
        scrollYDelta = Math::Min(scrollYDelta, distanceToBottom);

    }
    else if (t < screenTop && b < screenBottom) {
        // need to move up to get it in view: move up just enough so that
        // entire rectangle is in view (or at least the first screen
        // size chunk of it).

        if (h > height) {
            // screen size chunk
            scrollYDelta -= (screenBottom - b);
        }
        else {
            // entire rect at top
            scrollYDelta -= (screenTop - t);
        }

        // make sure we aren't scrolling any further than the top our content
        Int32 sy = 0;
        GetScrollY(&sy);
        scrollYDelta = Math::Max(scrollYDelta, -sy);
    }
    return scrollYDelta;
}

ECode ScrollView::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (!mIsLayoutDirty) {
        ScrollToChild(focused);
    }
    else {
        // The child may not be laid out yet, we can't compute the scroll yet
        mChildToScrollTo = focused;
    }
    return FrameLayout::RequestChildFocus(child, focused);
}

/**
 * When looking for focus in children of a scroll view, need to be a little
 * more careful not to give focus to something that is scrolled off screen.
 *
 * This is more expensive than the default {@link android.view.ViewGroup}
 * implementation, otherwise this behavior might have been made the default.
 */
Boolean ScrollView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    // convert from forward / backward notation to up / down / left / right
    // (ugh).
    if (direction == IView::FOCUS_FORWARD) {
        direction = IView::FOCUS_DOWN;
    }
    else if (direction == IView::FOCUS_BACKWARD) {
        direction = IView::FOCUS_UP;
    }

    AutoPtr<IView> nextFocus;
    AutoPtr<FocusFinder> finder = FocusFinder::GetInstance();
    if (previouslyFocusedRect == NULL) {
        finder->FindNextFocusFromRect(this,
                NULL, direction, (IView**)&nextFocus);
    }
    else {
        finder->FindNextFocusFromRect(this,
                previouslyFocusedRect, direction, (IView**)&nextFocus);
    }

    if (nextFocus == NULL) {
        return FALSE;
    }

    if (IsOffScreen(nextFocus)) {
        return FALSE;
    }

    Boolean result;
    ASSERT_SUCCEEDED(nextFocus->RequestFocus(
            direction, previouslyFocusedRect, &result));

    return result;
}

ECode ScrollView::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // offset into coordinate space of this scroll view
    Int32 left, scrollX, top, scrollY;
    child->GetLeft(&left);
    child->GetScrollX(&scrollX);
    child->GetTop(&top);
    child->GetScrollY(&scrollY);
    rectangle->Offset(left - scrollX, top - scrollY);

    *result = ScrollToChildRect(rectangle, immediate);
    return NOERROR;
}

ECode ScrollView::RequestLayout()
{
    mIsLayoutDirty = TRUE;
    return FrameLayout::RequestLayout();
}

ECode ScrollView::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();

    if (mScrollStrictSpan != NULL) {
        mScrollStrictSpan->Finish();
        mScrollStrictSpan = NULL;
    }
    if (mFlingStrictSpan != NULL) {
        mFlingStrictSpan->Finish();
        mFlingStrictSpan = NULL;
    }
    return NOERROR;
}

ECode ScrollView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FrameLayout::OnLayout(changed, left, top, right, bottom);
    mIsLayoutDirty = FALSE;
    // Give a child focus if it needs it
    if (mChildToScrollTo != NULL &&
            IsViewDescendantOf(mChildToScrollTo, this)) {
        ScrollToChild(mChildToScrollTo);
    }
    mChildToScrollTo = NULL;

    Boolean result = FALSE;
    if (IsLaidOut(&result), !result) {
        if (mSavedState != NULL) {
            mScrollY = mSavedState->mScrollPosition;
            mSavedState = NULL;
        } // mScrollY default value is "0"

        Int32 childHeight = 0;
        Int32 count = 0;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> child;
            GetChildAt(0, (IView**)&child);
            child->GetMeasuredHeight(&childHeight);
        }

        const Int32 scrollRange = Elastos::Core::Math::Max(0,
                childHeight - (bottom - top - mPaddingBottom - mPaddingTop));

        // Don't forget to clamp
        if (mScrollY > scrollRange) {
            mScrollY = scrollRange;
        }
        else if (mScrollY < 0) {
            mScrollY = 0;
        }
    }

    // Calling this with the present values causes it to re-clam them
    ScrollTo(mScrollX, mScrollY);
    return NOERROR;
}

void ScrollView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);

    AutoPtr<IView> currentFocused;
    FindFocus((IView**)&currentFocused);
    if (NULL == currentFocused || (IView*)this == currentFocused.Get()) {
        return;
    }

    // If the currently-focused view was visible on the screen when the
    // screen was at the old height, then scroll the screen to make that
    // view visible with the new screen height.
    if (IsWithinDeltaOfScreen(currentFocused, 0, oldh)) {
        currentFocused->GetDrawingRect(mTempRect);
        OffsetDescendantRectToMyCoords(currentFocused, mTempRect);
        Int32 scrollDelta = ComputeScrollDeltaToGetChildRectOnScreen(mTempRect);
        DoScrollY(scrollDelta);
    }
}

Boolean ScrollView::IsViewDescendantOf(
    /* [in] */ IView* child,
    /* [in] */ IView* parent)
{
    if (child == parent) {
        return TRUE;
    }

    AutoPtr<IViewParent> theParent;
    child->GetParent((IViewParent**)&theParent);
    IViewGroup* vg = IViewGroup::Probe(theParent);
    if (vg != NULL) {
        return IsViewDescendantOf(IView::Probe(vg), IView::Probe(parent));
    }
    return FALSE;
}

ECode ScrollView::Fling(
    /* [in] */ Int32 velocityY)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Int32 height = 0;
        GetHeight(&height);
        height = height - mPaddingBottom - mPaddingTop;
        Int32 bottom = 0;
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        child->GetHeight(&bottom);

        mScroller->Fling(mScrollX, mScrollY, 0, velocityY, 0, 0, 0,
                Elastos::Core::Math::Max(0, bottom - height), 0, height / 2);

        if (mFlingStrictSpan == NULL) {
            AutoPtr<IStrictMode> mode;
            CStrictMode::AcquireSingleton((IStrictMode**)&mode);
            mode->EnterCriticalSpan(String("ScrollView-fling"), (IStrictModeSpan**)&mFlingStrictSpan);
        }

        PostInvalidateOnAnimation();
    }
    return NOERROR;
}

void ScrollView::FlingWithNestedDispatch(
    /* [in] */ Int32 velocityY)
{
    Boolean canFling = (mScrollY > 0 || velocityY > 0) &&
            (mScrollY < GetScrollRange() || velocityY < 0);
    Boolean result = FALSE;
    if (DispatchNestedPreFling(0, velocityY, &result), !result) {
        DispatchNestedFling(0, velocityY, canFling, &result);
        if (canFling) {
            Fling(velocityY);
        }
    }
}

void ScrollView::EndDrag()
{
    mIsBeingDragged = FALSE;

    RecycleVelocityTracker();

    if (mEdgeGlowTop != NULL) {
        mEdgeGlowTop->OnRelease();
        mEdgeGlowBottom->OnRelease();
    }

    if (mScrollStrictSpan != NULL) {
        mScrollStrictSpan->Finish();
        mScrollStrictSpan = NULL;
    }
}

ECode ScrollView::ScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count = 0;
    // we rely on the fact the View.scrollBy calls scrollTo.
    if ((GetChildCount(&count), count) > 0) {
        AutoPtr<IView> child;
        GetChildAt(0, (IView**)&child);
        Int32 width = 0, height = 0, sh = 0, sw = 0;
        child->GetWidth(&width);
        child->GetHeight(&height);
        GetWidth(&sw);
        GetHeight(&sh);
        x = Clamp(x, sw - mPaddingRight - mPaddingLeft, width);
        y = Clamp(y, sh - mPaddingBottom - mPaddingTop, height);
        if (x != mScrollX || y != mScrollY) {
            FAIL_RETURN(FrameLayout::ScrollTo(x, y));
        }
    }
    return NOERROR;
}

ECode ScrollView::SetOverScrollMode(
    /* [in] */ Int32 overScrollMode)
{
    if (overScrollMode != IView::OVER_SCROLL_NEVER) {
        if (mEdgeGlowTop == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowTop);
            CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowBottom);
        }
    }
    else {
        mEdgeGlowTop = NULL;
        mEdgeGlowBottom = NULL;
    }
    return FrameLayout::SetOverScrollMode(overScrollMode);
}

ECode ScrollView::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (nestedScrollAxes & SCROLL_AXIS_VERTICAL) != 0;
    return NOERROR;
}

ECode ScrollView::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes)
{
    FrameLayout::OnNestedScrollAccepted(child, target, axes);
    Boolean tmp = FALSE;
    StartNestedScroll(SCROLL_AXIS_VERTICAL, &tmp);
    return NOERROR;
}

ECode ScrollView::OnStopNestedScroll(
    /* [in] */ IView* target)
{
    return FrameLayout::OnStopNestedScroll(target);
}

ECode ScrollView::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    const Int32 oldScrollY = mScrollY;
    ScrollBy(0, dyUnconsumed);
    const Int32 myConsumed = mScrollY - oldScrollY;
    const Int32 myUnconsumed = dyUnconsumed - myConsumed;
    Boolean tmp = FALSE;
    DispatchNestedScroll(0, myConsumed, 0, myUnconsumed, NULL, &tmp);
    return NOERROR;
}

ECode ScrollView::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!consumed) {
        FlingWithNestedDispatch((Int32) velocityY);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ScrollView::Draw(
    /* [in] */ ICanvas* canvas)
{
    FAIL_RETURN(FrameLayout::Draw(canvas));

    Boolean result;

    using Elastos::Core::Math;

    if (mEdgeGlowTop != NULL) {
        Int32 scrollY = mScrollY;
        mEdgeGlowTop->IsFinished(&result);
        if (!result) {
            Int32 restoreCount = 0, height = 0;
            canvas->Save(&restoreCount);
            Int32 width = 0;
            GetWidth(&width);
            width = width - mPaddingLeft - mPaddingRight;

            canvas->Translate(mPaddingLeft, Math::Min(0, scrollY));
            mEdgeGlowTop->SetSize(width, (GetHeight(&height), height));
            if (mEdgeGlowTop->Draw(canvas, &result), result) {
                PostInvalidateOnAnimation();
            }
            canvas->RestoreToCount(restoreCount);
        }

        mEdgeGlowBottom->IsFinished(&result);
        if (!result) {
            Int32 restoreCount = 0;
            canvas->Save(&restoreCount);
            Int32 width = 0;
            GetWidth(&width);
            width = width - mPaddingLeft - mPaddingRight;
            Int32 height = 0;
            GetHeight(&height);

            canvas->Translate(-width + mPaddingLeft, Math::Max(GetScrollRange(), scrollY) + height);
            canvas->Rotate(180, width, 0);
            mEdgeGlowBottom->SetSize(width, height);

            if (mEdgeGlowBottom->Draw(canvas, &result), result) {
                PostInvalidateOnAnimation();
            }
            canvas->RestoreToCount(restoreCount);
        }
    }

    return NOERROR;
}

Int32 ScrollView::Clamp(
    /* [in] */ Int32 n,
    /* [in] */ Int32 my,
    /* [in] */ Int32 child)
{
    if (my >= child || n < 0) {
        /* my >= child is this case:
         *                    |--------------- me ---------------|
         *     |------ child ------|
         * or
         *     |--------------- me ---------------|
         *            |------ child ------|
         * or
         *     |--------------- me ---------------|
         *                                  |------ child ------|
         *
         * n < 0 is this case:
         *     |------ me ------|
         *                    |-------- child --------|
         *     |-- mScrollX --|
         */
        return 0;
    }
    if ((my + n) > child) {
        /* this case:
         *                    |------ me ------|
         *     |------ child ------|
         *     |-- mScrollX --|
         */
        return child - my;
    }
    return n;
}

void ScrollView::OnRestoreInstanceState(
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
    AutoPtr<ScrollViewSavedState> ss = (ScrollViewSavedState*) state;
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    FrameLayout::OnRestoreInstanceState(p);
    mSavedState = ss;
    RequestLayout();
}

AutoPtr<IParcelable> ScrollView::OnSaveInstanceState()
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
    AutoPtr<IParcelable> ss;
    CScrollViewSavedState::New(superState, (IParcelable**)&ss);
    ((CScrollViewSavedState*)ss.Get())->mScrollPosition = mScrollY;
    return ss;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
