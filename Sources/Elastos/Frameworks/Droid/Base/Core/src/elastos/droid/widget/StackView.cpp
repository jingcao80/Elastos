
#include "elastos/droid/widget/StackView.h"
#include <elastos/core/Math.h>
#include "elastos/droid/animation/CPropertyValuesHolderHelper.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CStackViewLayoutParams.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CTableMaskFilterHelper.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include "elastos/droid/graphics/CBitmapFactory.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Math;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Widget::EIID_IFrameLayout;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CStackViewLayoutParams;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CTableMaskFilterHelper;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::CBlurMaskFilter;
using Elastos::Droid::Graphics::CBitmapFactory;

namespace Elastos {
namespace Droid {
namespace Widget {

//76b9f5c3-333f-4d2b-a8b6-2b6ae1adfcac
extern "C" const InterfaceID EIID_StackFrame =
        { 0x76b9f5c3, 0x333f, 0x4d2b, { 0xa8, 0xb6, 0x2b, 0x6a, 0xe1, 0xad, 0xfc, 0xac } };


const String StackView::TAG("StackView");
const Int32 StackView::DEFAULT_ANIMATION_DURATION;
const Int32 StackView::MINIMUM_ANIMATION_DURATION;
const Int32 StackView::STACK_RELAYOUT_DURATION;
const Int32 StackView::ITEMS_SLIDE_UP;
const Int32 StackView::ITEMS_SLIDE_DOWN;
const Int32 StackView::GESTURE_NONE;
const Int32 StackView::GESTURE_SLIDE_UP;
const Int32 StackView::GESTURE_SLIDE_DOWN;
const Int32 StackView::INVALID_POINTER;
const Int32 StackView::NUM_ACTIVE_VIEWS;
const Int32 StackView::FRAME_PADDING;
const Int32 StackView::MIN_TIME_BETWEEN_INTERACTION_AND_AUTOADVANCE;
const Float StackView::PERSPECTIVE_SHIFT_FACTOR_Y;
const Float StackView::PERSPECTIVE_SHIFT_FACTOR_X;
const Float StackView::PERSPECTIVE_SCALE_FACTOR;
const Float StackView::SWIPE_THRESHOLD_RATIO;
const Float StackView::SLIDE_UP_RATIO;
const Int64 StackView::MIN_TIME_BETWEEN_SCROLLS;

AutoPtr<IRect> InitStatic()
{
    AutoPtr<CRect> temp;
    CRect::NewByFriend((CRect**)&temp);
    return (IRect*)temp.Get();
}

AutoPtr<IRect> StackView::sStackInvalidateRect = InitStatic();
AutoPtr<StackView::HolographicHelper> StackView::sHolographicHelper;

StackView::StackView()
    : mPerspectiveShiftX(0)
    , mPerspectiveShiftY(0)
    , mNewPerspectiveShiftX(0)
    , mNewPerspectiveShiftY(0)
    , mInitialY(0)
    , mInitialX(0)
    , mActivePointerId(0)
    , mYVelocity(0)
    , mSwipeGestureType(GESTURE_NONE)
    , mSlideAmount(0)
    , mSwipeThreshold(0)
    , mTouchSlop(0)
    , mMaximumVelocity(0)
    , mResOutColor(0)
    , mClickColor(0)
    , mTransitionIsSetup(FALSE)
    , mClickFeedbackIsValid(FALSE)
    , mFirstLayoutHappened(FALSE)
    , mLastInteractionTime(0)
    , mLastScrollTime(0)
    , mStackMode(0)
    , mFramePadding(0)
{
    CRect::New((IRect**)&mTouchRect);
}

StackView::StackView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mPerspectiveShiftX(0)
    , mPerspectiveShiftY(0)
    , mNewPerspectiveShiftX(0)
    , mNewPerspectiveShiftY(0)
    , mInitialY(0)
    , mInitialX(0)
    , mActivePointerId(0)
    , mYVelocity(0)
    , mSwipeGestureType(GESTURE_NONE)
    , mSlideAmount(0)
    , mSwipeThreshold(0)
    , mTouchSlop(0)
    , mMaximumVelocity(0)
    , mResOutColor(0)
    , mClickColor(0)
    , mTransitionIsSetup(FALSE)
    , mClickFeedbackIsValid(FALSE)
    , mFirstLayoutHappened(FALSE)
    , mLastInteractionTime(0)
    , mLastScrollTime(0)
    , mStackMode(0)
    , mFramePadding(0)
{
    CRect::New((IRect**)&mTouchRect);
    Init(context, attrs, defStyle);
}

ECode StackView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    AdapterViewAnimator::Init(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::StackView),
            ArraySize(R::styleable::StackView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    a->GetColor(R::styleable::StackView_resOutColor, 0, &mResOutColor);
    a->GetColor(R::styleable::StackView_clickColor, 0, &mClickColor);

    a->Recycle();
    InitStackView();
    return NOERROR;
}

ECode StackView::ShowNext()
{
    if (mSwipeGestureType != GESTURE_NONE) return NOERROR;
    if (!mTransitionIsSetup) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(1);
        if (v ) {
            SetupStackSlider(v, StackSlider::NORMAL_MODE);
            mStackSlider->SetYProgress(0);
            mStackSlider->SetXProgress(0);
        }
    }
    AdapterViewAnimator::ShowNext();
    return NOERROR;
}

ECode StackView::ShowPrevious()
{
    if (mSwipeGestureType != GESTURE_NONE) return NOERROR;
    if (!mTransitionIsSetup) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(0);
        if (v) {
            SetupStackSlider(v, StackSlider::NORMAL_MODE);
            mStackSlider->SetYProgress(1);
            mStackSlider->SetXProgress(0);
        }
    }
    AdapterViewAnimator::ShowPrevious();
    return NOERROR;
}

ECode StackView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 source = 0;
    event->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action = 0;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_SCROLL: {
                Float vscroll = 0;
                event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                if (vscroll < 0) {
                    PacedScroll(FALSE);
                    *res = TRUE;
                    return NOERROR;
                } else if (vscroll > 0) {
                    PacedScroll(TRUE);
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    return AdapterViewAnimator::OnGenericMotionEvent(event, res);
}

ECode StackView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 action = 0;
    ev->GetAction(&action);
    switch(action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN:
        {
            if (mActivePointerId == INVALID_POINTER) {
                ev->GetX(&mInitialX);
                ev->GetY(&mInitialY);
                ev->GetPointerId(0, &mActivePointerId);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE:
        {
            Int32 pointerIndex = 0;
            ev->FindPointerIndex(mActivePointerId, &pointerIndex);
            if (pointerIndex == INVALID_POINTER) {
                //Log.d(TAG, "Error: No data for our primary pointer.");
                *res = FALSE;
                return NOERROR;
            }
            Float newY = 0;
            ev->GetY(pointerIndex, &newY);
            Float deltaY = newY - mInitialY;

            BeginGestureIfNeeded(deltaY);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            mActivePointerId = INVALID_POINTER;
            mSwipeGestureType = GESTURE_NONE;
        }
    }

    *res = mSwipeGestureType != GESTURE_NONE;
    return NOERROR;
}

ECode StackView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AdapterViewAnimator::OnTouchEvent(ev, res);

    Int32 action = 0;
    ev->GetAction(&action);
    Int32 pointerIndex = 0;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == INVALID_POINTER) {
        // no data for our primary pointer, this shouldn't happen, log it
        //Log.d(TAG, "Error: No data for our primary pointer.");
        *res = FALSE;
        return NOERROR;
    }

    Float newY = 0, newX = 0;
    ev->GetY(pointerIndex, &newY);
    ev->GetX(pointerIndex, &newX);
    Float deltaY = newY - mInitialY;
    Float deltaX = newX - mInitialX;
    if (!mVelocityTracker) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    mVelocityTracker->AddMovement(ev);

    switch (action & IMotionEvent::ACTION_MASK)
    {
        case IMotionEvent::ACTION_MOVE:
        {
            BeginGestureIfNeeded(deltaY);

            Float rx = deltaX / (mSlideAmount * 1.0f);
            if (mSwipeGestureType == GESTURE_SLIDE_DOWN) {
                Float r = (deltaY - mTouchSlop * 1.0f) / mSlideAmount * 1.0f;
                if (mStackMode == ITEMS_SLIDE_DOWN) r = 1 - r;
                mStackSlider->SetYProgress(1 - r);
                mStackSlider->SetXProgress(rx);
                *res = TRUE;
                return NOERROR;
            } else if (mSwipeGestureType == GESTURE_SLIDE_UP) {
                Float r = -(deltaY + mTouchSlop * 1.0f) / mSlideAmount * 1.0f;
                if (mStackMode == ITEMS_SLIDE_DOWN) r = 1 - r;
                mStackSlider->SetYProgress(r);
                mStackSlider->SetXProgress(rx);
                *res = TRUE;
                return NOERROR;
            }
            break;
        }
        case IMotionEvent::ACTION_UP:
        {
            HandlePointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
        {
            OnSecondaryPointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        {
            mActivePointerId = INVALID_POINTER;
            mSwipeGestureType = GESTURE_NONE;
            break;
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode StackView::Advance()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 timeSinceLastInteraction = millis - mLastInteractionTime;

    if (!mAdapter) return NOERROR;
    Int32 adapterCount = GetCount();
    if (adapterCount == 1 && mLoopViews) return NOERROR;

    if (mSwipeGestureType == GESTURE_NONE &&
            timeSinceLastInteraction > MIN_TIME_BETWEEN_INTERACTION_AND_AUTOADVANCE) {
        ShowNext();
    }
    return NOERROR;
}

ECode StackView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AdapterView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CStringWrapper::New(String("CStackView"), (ICharSequence**)&seq);
    return event->SetClassName(seq);
}

ECode StackView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AdapterView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CStringWrapper::New(String("CStackView"), (ICharSequence**)&seq);
    info->SetClassName(seq);
    info->SetScrollable(GetChildCount() > 1);
    if (IsEnabled()) {
        if (GetDisplayedChild() < GetChildCount() - 1) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
        if (GetDisplayedChild() > 0) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
    }
    return NOERROR;
}

Boolean StackView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (AdapterView::PerformAccessibilityAction(action, arguments)) {
        return TRUE;
    }
    if (!IsEnabled()) {
        return FALSE;
    }
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD:
        {
            if (GetDisplayedChild() < GetChildCount() - 1) {
                ShowNext();
                return TRUE;
            }
        }
        return FALSE;
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD:
        {
            if (GetDisplayedChild() > 0) {
                ShowPrevious();
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}

void StackView::TransformViewForTransition(
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex,
    /* [in] */ IView* view,
    /* [in] */ Boolean animate)
{
    if (animate) {
        AutoPtr<StackFrame> sf = reinterpret_cast<StackFrame*>(view->Probe(EIID_StackFrame));
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        AutoPtr<IViewGroupLayoutParams> lp;
        view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        slp->SetVerticalOffset(0);
        slp->SetHorizontalOffset(0);
    }

    if (fromIndex == -1 && toIndex == GetNumActiveViews() - 1) {
        TransformViewAtIndex(toIndex, view, FALSE);
        view->SetVisibility(IView::VISIBLE);
        view->SetAlpha(1.0f);
    } else if (fromIndex == 0 && toIndex == 1) {
        AutoPtr<StackFrame> sf = reinterpret_cast<StackFrame*>(view->Probe(EIID_StackFrame));
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        view->SetVisibility(IView::VISIBLE);
        Int32 duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition(mYVelocity));
        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        animationSlider->SetView(view);

        if(animate) {
            AutoPtr<IPropertyValuesHolderHelper> helper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
            AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 0.0f;
            AutoPtr<IPropertyValuesHolder> slideInY, slideInX;
            helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&slideInY);
            helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&slideInX);

            AutoPtr<IObjectAnimatorHelper> oaHelp;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
            AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
            pArray->Set(0, slideInX);
            pArray->Set(1, slideInY);
            AutoPtr<IObjectAnimator> slideIn;
            oaHelp->OfPropertyValuesHolder(animationSlider, pArray, (IObjectAnimator**)&slideIn);
            slideIn->SetDuration(duration);

            AutoPtr<ILinearInterpolator> lip;
            CLinearInterpolator::New((ILinearInterpolator**)&lip);
            slideIn->SetInterpolator(lip);
            sf->SetSliderAnimator(slideIn);
            slideIn->Start();
        } else {
            animationSlider->SetYProgress(0.0f);
            animationSlider->SetXProgress(0.0f);
        }
    } else if (fromIndex == 1 && toIndex == 0) {
        AutoPtr<StackFrame> sf = reinterpret_cast<StackFrame*>(view->Probe(EIID_StackFrame));
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        Int32 duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition(mYVelocity));

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        animationSlider->SetView(view);
        if (animate) {
            AutoPtr<IPropertyValuesHolderHelper> helper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
            AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 1.0f;
            AutoPtr<IPropertyValuesHolder> slideOutY, slideOutX;
            helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&slideOutY);
            (*array)[0] = 0.0f;
            helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&slideOutX);

            AutoPtr<IObjectAnimatorHelper> oaHelp;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
            AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
            pArray->Set(0, slideOutX);
            pArray->Set(1, slideOutY);
            AutoPtr<IObjectAnimator> slideOut;
            oaHelp->OfPropertyValuesHolder(animationSlider, pArray, (IObjectAnimator**)&slideOut);
            slideOut->SetDuration(duration);

            AutoPtr<ILinearInterpolator> lip;
            CLinearInterpolator::New((ILinearInterpolator**)&lip);
            slideOut->SetInterpolator(lip);
            sf->SetSliderAnimator(slideOut);
            slideOut->Start();
        } else {
            animationSlider->SetYProgress(1.0f);
            animationSlider->SetXProgress(0.0f);
        }
    } else if (toIndex == 0) {
        view->SetAlpha(0.0f);
        view->SetVisibility(IView::INVISIBLE);
    } else if ((fromIndex == 0 || fromIndex == 1) && toIndex > 1) {
        view->SetVisibility(IView::VISIBLE);
        view->SetAlpha(1.0f);
        view->SetRotationX(0.0f);
        AutoPtr<IViewGroupLayoutParams> lp;
        view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        slp->SetVerticalOffset(0);
        slp->SetHorizontalOffset(0);
    } else if (fromIndex == -1) {
        view->SetAlpha(1.0f);
        view->SetVisibility(IView::VISIBLE);
    } else if (toIndex == -1) {
        if (animate) {
            AutoPtr<IRunnable> run = new PostRun(view);
            PostDelayed(run, STACK_RELAYOUT_DURATION);
        } else {
            view->SetAlpha(0.0f);
        }
    }

    if (toIndex != -1) {
        TransformViewAtIndex(toIndex, view, animate);
    }
}

void StackView::ShowOnly(
    /* [in] */ Int32 childIndex,
    /* [in] */ Boolean animate)
{
    AdapterViewAnimator::ShowOnly(childIndex, animate);

    for (Int32 i = mCurrentWindowEnd; i >= mCurrentWindowStart; i--) {
        Int32 index = Modulo(i, GetWindowSize());
        if (mViewsMap.Find(index) != mViewsMap.End()) {
            AutoPtr<ViewAndMetaData> vm = mViewsMap.Find(index)->mSecond;
            if (vm) {
                AutoPtr<IView> v =vm->mView;
                if (v) v->BringToFront();
            }
        }
    }
    if (mHighlight) {
        mHighlight->BringToFront();
    }
    mTransitionIsSetup = FALSE;
    mClickFeedbackIsValid = FALSE;
}

void StackView::UpdateClickFeedback()
{
    if (!mClickFeedbackIsValid) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(1);
        if (v) {
            AutoPtr<IBitmap> map = sHolographicHelper->CreateClickOutline(v, mClickColor);
            mClickFeedback->SetImageBitmap(map);
            Float x = 0, y = 0;
            v->GetTranslationX(&x);
            v->GetTranslationY(&y);
            mClickFeedback->SetTranslationX(x);
            mClickFeedback->SetTranslationY(y);
        }
        mClickFeedbackIsValid = TRUE;
    }
}

void StackView::ShowTapFeedback(
    /* [in] */ IView* child)
{
    UpdateClickFeedback();
    mClickFeedback->SetVisibility(IView::VISIBLE);
    mClickFeedback->BringToFront();
    Invalidate();
}

void StackView::HideTapFeedback(
    /* [in] */ IView* child)
{
    mClickFeedback->SetVisibility(IView::INVISIBLE);
    Invalidate();
}

void StackView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    CheckForAndHandleDataChanged();

    Int32 childCount = GetChildCount();
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child = GetChildAt(i);

        Int32 childRight = 0;
        child->GetMeasuredWidth(&childRight);
        childRight += mPaddingLeft;
        Int32 childBottom = 0;
        child->GetMeasuredHeight(&childBottom);
        childBottom += mPaddingTop;
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        assert(slp != NULL);
        Int32 hos = 0, vos = 0;
        slp->GetHorizontalOffset(&hos);
        slp->GetVerticalOffset(&vos);
        child->Layout(mPaddingLeft + hos, mPaddingTop + vos,
                childRight + hos, childBottom + vos);

    }
    OnLayout();
}

void StackView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSpecSize = MeasureSpec::GetSize(heightMeasureSpec);
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean haveChildRefSize = (mReferenceChildWidth != -1 && mReferenceChildHeight != -1);

    Float factorY = 1/(1 - PERSPECTIVE_SHIFT_FACTOR_Y);
    if (heightSpecMode == MeasureSpec::UNSPECIFIED) {
        heightSpecSize = haveChildRefSize ? Elastos::Core::Math::Round(mReferenceChildHeight * (1 + factorY)) +
                mPaddingTop + mPaddingBottom : 0;
    } else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 height = Elastos::Core::Math::Round(mReferenceChildHeight * (1 + factorY)) + mPaddingTop + mPaddingBottom;
            if (height <= heightSpecSize) {
                heightSpecSize = height;
            } else {
                heightSpecSize |= IView::MEASURED_STATE_TOO_SMALL;

            }
        } else {
            heightSpecSize = 0;
        }
    }

    Float factorX = 1/(1 - PERSPECTIVE_SHIFT_FACTOR_X);
    if (widthSpecMode == MeasureSpec::UNSPECIFIED) {
        widthSpecSize = haveChildRefSize ? Elastos::Core::Math::Round(mReferenceChildWidth * (1 + factorX)) +
                mPaddingLeft + mPaddingRight : 0;
    } else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 width = mReferenceChildWidth + mPaddingLeft + mPaddingRight;
            if (width <= widthSpecSize) {
                widthSpecSize = width;
            } else {
                widthSpecSize |= IView::MEASURED_STATE_TOO_SMALL;
            }
        } else {
            widthSpecSize = 0;
        }
    }
    SetMeasuredDimension(widthSpecSize, heightSpecSize);
    MeasureChildren();
}

void StackView::InitStackView()
{
    ConfigureViewAnimator(NUM_ACTIVE_VIEWS, 1);
    SetStaticTransformationsEnabled(TRUE);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(GetContext(), (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    mActivePointerId = INVALID_POINTER;
    CImageView::New(GetContext(), (IImageView**)&mHighlight);
    AutoPtr<IStackViewLayoutParams> hLP;
    CStackViewLayoutParams::New((IStackView*)this->Probe(EIID_IStackView) ,mHighlight, (IStackViewLayoutParams**)&hLP);
    mHighlight->SetLayoutParams(hLP);
    AddViewInLayout(mHighlight, -1, hLP);

    CImageView::New(GetContext(), (IImageView**)&mClickFeedback);
    AutoPtr<IStackViewLayoutParams> cLP;
    CStackViewLayoutParams::New((IStackView*)this->Probe(EIID_IStackView), mClickFeedback, (IStackViewLayoutParams**)&cLP);
    mHighlight->SetLayoutParams(cLP);
    AddViewInLayout(mClickFeedback, -1, cLP);
    mClickFeedback->SetVisibility(IView::INVISIBLE);

    mStackSlider = new StackSlider(this);

    if (!sHolographicHelper) {
        sHolographicHelper = new HolographicHelper(mContext);
    }

    SetClipChildren(FALSE);
    SetClipToPadding(FALSE);

    mStackMode = ITEMS_SLIDE_DOWN;
    mWhichChild = -1;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density = 0;
    dm->GetDensity(&density);

    mFramePadding = (Int32)Elastos::Core::Math::Ceil(density * FRAME_PADDING);
}

void StackView::TransformViewAtIndex(
        /* [in] */ Int32 index,
        /* [in] */ IView* view,
        /* [in] */ Int32 animate)
{
    Float maxPerspectiveShiftY = mPerspectiveShiftY;
    Float maxPerspectiveShiftX = mPerspectiveShiftX;

    if (mStackMode == ITEMS_SLIDE_DOWN) {
        index = mMaxNumActiveViews - index - 1;
        if (index == mMaxNumActiveViews - 1) index--;
    } else {
        index--;
        if (index < 0) index++;
    }

    Float r = (index * 1.0f) / (mMaxNumActiveViews - 2);

    Float scale = 1 - PERSPECTIVE_SCALE_FACTOR * (1 - r);

    Float perspectiveTranslationY = r * maxPerspectiveShiftY;
    Float scaleShiftCorrectionY = (scale - 1) * (GetMeasuredHeight() * (1 - PERSPECTIVE_SHIFT_FACTOR_Y) / 2.0f);
    Float transY = perspectiveTranslationY + scaleShiftCorrectionY;

    Float perspectiveTranslationX = (1 - r) * maxPerspectiveShiftX;
    Float scaleShiftCorrectionX =  (1 - scale) * (GetMeasuredHeight() * (1 - PERSPECTIVE_SHIFT_FACTOR_X) / 2.0f);
    Float transX = perspectiveTranslationX + scaleShiftCorrectionX;

    if (view->Probe(EIID_StackFrame)) {
        AutoPtr<StackFrame> sf = reinterpret_cast<StackFrame*>(view->Probe(EIID_StackFrame));
        assert(sf != NULL);
        sf->CancelTransformAnimator();
    }

    if (animate) {
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        AutoPtr<IPropertyValuesHolder> translationX, translationY, scalePropX, scalePropY;
        (*array)[0] = transX;
        helper->OfFloat(String("translationX"), array, (IPropertyValuesHolder**)&translationX);
        (*array)[0] = transY;
        helper->OfFloat(String("translationY"), array, (IPropertyValuesHolder**)&translationY);
        (*array)[0] = scale;
        helper->OfFloat(String("scaleX"), array, (IPropertyValuesHolder**)&scalePropX);
        helper->OfFloat(String("scaleY"), array, (IPropertyValuesHolder**)&scalePropY);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(4);
        pArray->Set(0, scalePropX);
        pArray->Set(1, scalePropY);
        pArray->Set(2, translationY);
        pArray->Set(3, translationX);
        AutoPtr<IObjectAnimator> oa;
        oaHelp->OfPropertyValuesHolder((IInterface*)view->Probe(EIID_IInterface), pArray, (IObjectAnimator**)&oa);
        oa->SetDuration(STACK_RELAYOUT_DURATION);
        AutoPtr<StackFrame> sf = reinterpret_cast<StackFrame*>(view->Probe(EIID_StackFrame));
        if (sf) {
            sf->SetTransformAnimator(oa);
        }
        oa->Start();
    } else {
        view->SetTranslationX(transX);
        view->SetTranslationY(transY);
        view->SetScaleX(scale);
        view->SetScaleY(scale);
    }
}

void StackView::SetupStackSlider(
    /* [in] */ IView* view,
    /* [in] */ Int32 mode)
{
    mStackSlider->SetMode(mode);
    if (view) {
        mHighlight->SetImageBitmap(sHolographicHelper->CreateResOutline(view, mResOutColor));
        Float rotation = 0, ty = 0, tx = 0;
        view->GetRotation(&rotation);
        mHighlight->SetRotation(rotation);
        view->GetTranslationY(&ty);
        mHighlight->SetTranslationY(ty);
        view->GetTranslationX(&tx);
        mHighlight->SetTranslationX(tx);
        mHighlight->BringToFront();
        view->BringToFront();
        mStackSlider->SetView(view);

        view->SetVisibility(IView::VISIBLE);
    }
}

void StackView::UpdateChildTransforms()
{
    for (Int32 i = 0; i < GetNumActiveViews(); i++) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(i);
        if (v) {
            TransformViewAtIndex(i, v, FALSE);
        }
    }
}

AutoPtr<IFrameLayout> StackView::GetFrameForChild()
{
    AutoPtr<StackFrame> fl = new StackFrame(mContext);
    fl->SetPadding(mFramePadding, mFramePadding, mFramePadding, mFramePadding);
    return fl;
}

void StackView::ApplyTransformForChildAtIndex(
    /* [in] */ IView* child,
    /* [in] */ Int32 relativeIndex)
{
    //do nothing
}

void StackView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Boolean expandClipRegion = FALSE;
    Boolean res = FALSE;
    canvas->GetClipBounds(sStackInvalidateRect, &res);
    Int32 childCount = GetChildCount();
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child = GetChildAt(i);
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        assert(slp != NULL);
        Int32 horizontalOffset = 0, verticalOffset = 0, visible = 0;
        slp->GetHorizontalOffset(&horizontalOffset);
        slp->GetVerticalOffset(&verticalOffset);
        child->GetVisibility(&visible);
        Float alpha = 0.0f;
        child->GetAlpha(&alpha);
        if ((horizontalOffset == 0 && verticalOffset == 0) || alpha == 0.0f || visible != IView::VISIBLE) {
            slp->ResetInvalidateRect();
        }
        AutoPtr<IRect> childInvalidateRect;
        slp->GetInvalidateRect((IRect**)&childInvalidateRect);
        childInvalidateRect->IsEmpty(&res);
        if (!res) {
            expandClipRegion = TRUE;
            sStackInvalidateRect->Union(childInvalidateRect);
        }
    }

    if (expandClipRegion) {
        Int32 result = 0;
        canvas->Save(ICanvas::CLIP_SAVE_FLAG, &result);
        canvas->ClipRect(sStackInvalidateRect, RegionOp_UNION, &res);
        AdapterViewAnimator::DispatchDraw(canvas);
        canvas->Restore();
    } else {
        AdapterViewAnimator::DispatchDraw(canvas);
    }
}

void StackView::OnLayout()
{
    if (!mFirstLayoutHappened) {
        mFirstLayoutHappened = TRUE;
        UpdateChildTransforms();
    }

    Int32 newSlideAmount = Elastos::Core::Math::Round(SLIDE_UP_RATIO * GetMeasuredHeight());
    if (mSlideAmount != newSlideAmount) {
        mSlideAmount = newSlideAmount;
        mSwipeThreshold = Elastos::Core::Math::Round(SWIPE_THRESHOLD_RATIO * newSlideAmount);
    }

    if (Elastos::Core::Math::Compare(mPerspectiveShiftY, mNewPerspectiveShiftY) != 0 ||
            Elastos::Core::Math::Compare(mPerspectiveShiftX, mNewPerspectiveShiftX) != 0) {

        mPerspectiveShiftY = mNewPerspectiveShiftY;
        mPerspectiveShiftX = mNewPerspectiveShiftX;
        UpdateChildTransforms();
    }
}

void StackView::PacedScroll(
    /* [in] */ Boolean up)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 timeSinceLastScroll = millis - mLastScrollTime;
    if (timeSinceLastScroll > MIN_TIME_BETWEEN_SCROLLS) {
        if (up) {
            ShowPrevious();
        } else {
            ShowNext();
        }
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mLastScrollTime);
    }
}

void StackView::BeginGestureIfNeeded(
    /* [in] */ Float deltaY)
{
    if ((Int32)Elastos::Core::Math::Abs(deltaY) > mTouchSlop && mSwipeGestureType == GESTURE_NONE) {
        Int32 swipeGestureType = deltaY < 0 ? GESTURE_SLIDE_UP : GESTURE_SLIDE_DOWN;
        CancelLongPress();
        RequestDisallowInterceptTouchEvent(TRUE);

        if (!mAdapter) return;
        Int32 adapterCount = GetCount();

        Int32 activeIndex = 0;
        if (mStackMode == ITEMS_SLIDE_UP) {
            activeIndex = (swipeGestureType == GESTURE_SLIDE_DOWN) ? 0 : 1;
        } else {
            activeIndex = (swipeGestureType == GESTURE_SLIDE_DOWN) ? 1 : 0;
        }

        Boolean endOfStack = mLoopViews && adapterCount == 1 &&
            ((mStackMode == ITEMS_SLIDE_UP && swipeGestureType == GESTURE_SLIDE_UP) ||
             (mStackMode == ITEMS_SLIDE_DOWN && swipeGestureType == GESTURE_SLIDE_DOWN));
        Boolean beginningOfStack = mLoopViews && adapterCount == 1 &&
            ((mStackMode == ITEMS_SLIDE_DOWN && swipeGestureType == GESTURE_SLIDE_UP) ||
             (mStackMode == ITEMS_SLIDE_UP && swipeGestureType == GESTURE_SLIDE_DOWN));

        Int32 stackMode = 0;
        if (mLoopViews && !beginningOfStack && !endOfStack) {
            stackMode = StackSlider::NORMAL_MODE;
        } else if (mCurrentWindowStartUnbounded + activeIndex == -1 || beginningOfStack) {
            activeIndex++;
            stackMode = StackSlider::BEGINNING_OF_STACK_MODE;
        } else if (mCurrentWindowStartUnbounded + activeIndex == adapterCount - 1 || endOfStack) {
            stackMode = StackSlider::END_OF_STACK_MODE;
        } else {
            stackMode = StackSlider::NORMAL_MODE;
        }

        mTransitionIsSetup = stackMode == StackSlider::NORMAL_MODE;

        AutoPtr<IView> v = GetViewAtRelativeIndex(activeIndex);
        if (!v) return;

        SetupStackSlider(v, stackMode);

        mSwipeGestureType = swipeGestureType;
        CancelHandleClick();
    }
}

void StackView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 activePointerIndex = 0 , pointerId = 0;
    ev->GetActionIndex(&activePointerIndex);
    ev->GetPointerId(activePointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {

        Int32 activeViewIndex = (mSwipeGestureType == GESTURE_SLIDE_DOWN) ? 0 : 1;

        AutoPtr<IView> v = GetViewAtRelativeIndex(activeViewIndex);
        if (!v) return;
        Int32 pointerCount = 0;
        ev->GetPointerCount(&pointerCount);
        for (Int32 index = 0; index < pointerCount; index++) {
            if (index != activePointerIndex) {

                Float x = 0, y = 0;
                ev->GetX(index, &x);
                ev->GetY(index, &y);
                Int32 left = 0, top = 0, right = 0, bottom = 0;
                v->GetLeft(&left);
                v->GetTop(&top);
                v->GetRight(&right);
                v->GetBottom(&bottom);
                mTouchRect->Set(left, top, right, bottom);
                Boolean res = FALSE;
                mTouchRect->Contains(Elastos::Core::Math::Round(x), Elastos::Core::Math::Round(y), &res);
                if (res) {
                    Float oldX = 0, oldY =0;
                    ev->GetX(activePointerIndex, &oldX);
                    ev->GetY(activePointerIndex, &oldY);

                    mInitialY += (y - oldY);
                    mInitialX += (x - oldX);

                    ev->GetPointerId(index, &mActivePointerId);
                    if (mVelocityTracker) {
                        mVelocityTracker->Clear();
                    }

                    return;
                }
            }
        }

        HandlePointerUp(ev);
    }
}

void StackView::HandlePointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 pointerIndex = 0;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    Float newY = 0;
    ev->GetY(pointerIndex, &newY);
    Int32 deltaY = (Int32) (newY - mInitialY);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastInteractionTime);

    if (mVelocityTracker) {
        mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
        Float y;
        mVelocityTracker->GetYVelocity(mActivePointerId, &y);
        mYVelocity = (Int32)y;
    }

    if (mVelocityTracker) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }

    if (deltaY > mSwipeThreshold && mSwipeGestureType == GESTURE_SLIDE_DOWN
            && mStackSlider->mMode == StackSlider::NORMAL_MODE) {
        mSwipeGestureType = GESTURE_NONE;

        if (mStackMode == ITEMS_SLIDE_UP) {
            ShowPrevious();
        } else {
            ShowNext();
        }
        mHighlight->BringToFront();
    } else if (deltaY < -mSwipeThreshold && mSwipeGestureType == GESTURE_SLIDE_UP
            && mStackSlider->mMode == StackSlider::NORMAL_MODE) {
        mSwipeGestureType = GESTURE_NONE;

        if (mStackMode == ITEMS_SLIDE_UP) {
            ShowNext();
        } else {
            ShowPrevious();
        }

        mHighlight->BringToFront();
    } else if (mSwipeGestureType == GESTURE_SLIDE_UP ) {
        Int32 duration = 0;
        Float finalYProgress = (mStackMode == ITEMS_SLIDE_DOWN) ? 1 : 0;
        if (mStackMode == ITEMS_SLIDE_UP || mStackSlider->mMode != StackSlider::NORMAL_MODE) {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition());
        } else {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition());
        }

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalYProgress;
        AutoPtr<IPropertyValuesHolder> snapBackY;
        helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&snapBackY);
        (*array)[0] = 0.0f;
        AutoPtr<IPropertyValuesHolder> snapBackX;
        helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&snapBackX);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
        pArray->Set(0, snapBackX);
        pArray->Set(1, snapBackY);
        AutoPtr<IObjectAnimator> pa;
        oaHelp->OfPropertyValuesHolder(animationSlider, pArray, (IObjectAnimator**)&pa);
        pa->SetDuration(duration);
        AutoPtr<ILinearInterpolator> lip;
        CLinearInterpolator::New((ILinearInterpolator**)&lip);
        pa->SetInterpolator(lip);
        pa->Start();
    } else if (mSwipeGestureType == GESTURE_SLIDE_DOWN) {
        Float finalYProgress = (mStackMode == ITEMS_SLIDE_DOWN) ? 0 : 1;
        Int32 duration = 0;
        if (mStackMode == ITEMS_SLIDE_DOWN || mStackSlider->mMode != StackSlider::NORMAL_MODE) {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition());
        } else {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition());
        }

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalYProgress;
        AutoPtr<IPropertyValuesHolder> snapBackY;
        helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&snapBackY);
        (*array)[0] = 0.0f;
        AutoPtr<IPropertyValuesHolder> snapBackX;
        helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&snapBackX);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
        pArray->Set(0, snapBackX);
        pArray->Set(1, snapBackY);
        AutoPtr<IObjectAnimator> pa;
        oaHelp->OfPropertyValuesHolder(animationSlider, pArray, (IObjectAnimator**)&pa);
        pa->SetDuration(duration);
        pa->Start();
    }

    mActivePointerId = INVALID_POINTER;
    mSwipeGestureType = GESTURE_NONE;
}

AutoPtr<IViewGroupLayoutParams> StackView::CreateOrReuseLayoutParams(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> currentLp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&currentLp);
    AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(currentLp);
    if (slp) {
        slp->SetHorizontalOffset(0);
        slp->SetVerticalOffset(0);
        slp->SetWidth(0);
    } else {
        CStackViewLayoutParams::New((IStackView*)this->Probe(EIID_IStackView), v, (IStackViewLayoutParams**)&slp);
    }
    return slp;
}

void StackView::MeasureChildren()
{
    Int32 count = GetChildCount();

    Int32 measuredWidth = GetMeasuredWidth();
    Int32 measuredHeight = GetMeasuredHeight();

    Int32 childWidth = Elastos::Core::Math::Round(measuredWidth*(1-PERSPECTIVE_SHIFT_FACTOR_X)) - mPaddingLeft - mPaddingRight;
    Int32 childHeight = Elastos::Core::Math::Round(measuredHeight*(1-PERSPECTIVE_SHIFT_FACTOR_Y)) - mPaddingTop - mPaddingBottom;

    Int32 maxWidth = 0;
    Int32 maxHeight = 0;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = GetChildAt(i);
        child->Measure(MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::AT_MOST),
            MeasureSpec::MakeMeasureSpec(childHeight, MeasureSpec::AT_MOST));

        if (child != mHighlight && child != mClickFeedback) {
            Int32 childMeasuredWidth = 0;
            child->GetMeasuredWidth(&childMeasuredWidth);
            Int32 childMeasuredHeight = 0;
            child->GetMeasuredHeight(&childMeasuredHeight);
            if (childMeasuredWidth > maxWidth) {
                maxWidth = childMeasuredWidth;
            }
            if (childMeasuredHeight > maxHeight) {
                maxHeight = childMeasuredHeight;
            }
        }
    }

    mNewPerspectiveShiftX = PERSPECTIVE_SHIFT_FACTOR_X * measuredWidth;
    mNewPerspectiveShiftY = PERSPECTIVE_SHIFT_FACTOR_Y * measuredHeight;

    if (maxWidth > 0 && count > 0 && maxWidth < childWidth) {
        mNewPerspectiveShiftX = measuredWidth - maxWidth;
    }

    if (maxHeight > 0 && count > 0 && maxHeight < childHeight) {
        mNewPerspectiveShiftY = measuredHeight - maxHeight;
    }
}

///////////////////////////////////////////////////////////////////////
//                  StackView::_StackFrame
///////////////////////////////////////////////////////////////////////

StackView::_StackFrame::_StackFrame(
    /* [in] */ IContext* context)
    : FrameLayout(context)
{}

void StackView::_StackFrame::SetTransformAnimator(
    /* [in] */ IObjectAnimator* oa)
{
    mTransformAnimator = oa;
}

void StackView::_StackFrame::SetSliderAnimator(
    /* [in] */ IObjectAnimator* oa)
{
    mSliderAnimator = oa;
}

Boolean StackView::_StackFrame::CancelTransformAnimator()
{
    if (mTransformAnimator) {
        AutoPtr<IObjectAnimator> oa = mTransformAnimator;
        if (oa) {
            oa->Cancel();
            return TRUE;
        }
    }
    return FALSE;
}

Boolean StackView::_StackFrame::CancelSliderAnimator()
{
    if (mSliderAnimator) {
        AutoPtr<IObjectAnimator> oa = mSliderAnimator;
        if (oa) {
            oa->Cancel();
            return TRUE;
        }
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////
//                  StackView::StackFrame
///////////////////////////////////////////////////////////////////////
StackView::StackFrame::StackFrame(
    /* [in] */ IContext* context)
    : _StackFrame(context)
{}

IVIEW_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IVIEWGROUP_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IVIEWPARENT_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IVIEWMANAGER_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IDRAWABLECALLBACK_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IKEYEVENTCALLBACK_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)
IFRAMELAYOUT_METHODS_IMPL(StackView::StackFrame, StackView::_StackFrame)

PInterface StackView::StackFrame::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IFrameLayout*)this;
    } else if (riid == EIID_IFrameLayout) {
        return (IFrameLayout*)this;
    } else if (riid == EIID_IViewGroup) {
        return (IViewGroup*)(IFrameLayout*)this;
    } else if (riid == EIID_IView) {
        return (IView*)(IViewGroup*)(IFrameLayout*)this;
    } else if (riid == EIID_IViewParent) {
        return (IViewParent*)this;
    } else if (riid == EIID_IViewManager) {
        return (IViewManager*)this;
    } else if (riid == EIID_IDrawableCallback) {
        return (IDrawableCallback*)this;
    } else if (riid == EIID_IKeyEventCallback) {
        return (IKeyEventCallback*)this;
    } else if (riid == EIID_IAccessibilityEventSource) {
        return (IAccessibilityEventSource*)this;
    } else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    } else if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)(_StackFrame*)this);
    } else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)(_StackFrame*)this);
    } else if (riid == EIID_StackFrame) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}

UInt32 StackView::StackFrame::AddRef()
{
    return _StackFrame::AddRef();
}

UInt32 StackView::StackFrame::Release()
{
    return _StackFrame::Release();
}

CARAPI StackView::StackFrame::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pObject == (IFrameLayout*)this) {
        *pIID = EIID_IFrameLayout;
    } else if (pObject == (IViewGroup*)this) {
        *pIID = EIID_IViewGroup;
    } else if (pObject == (IViewParent*)this) {
        *pIID = EIID_IViewParent;
    } else if (pObject == (IViewManager*)this) {
        *pIID = EIID_IViewManager;
    } else if (pObject == (IDrawableCallback*)this) {
        *pIID = EIID_IDrawableCallback;
    } else if (pObject == (IKeyEventCallback*)this) {
        *pIID = EIID_IKeyEventCallback;
    } else if (pObject == (IAccessibilityEventSource*)this) {
        *pIID = EIID_IAccessibilityEventSource;
    } else if (pObject == reinterpret_cast<PInterface>((View*)this)) {
        *pIID = EIID_View;
    } else if (pObject == reinterpret_cast<PInterface>((ViewGroup*)this)) {
        *pIID = EIID_ViewGroup;
    } else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode StackView::StackFrame::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
//                  StackView::HolographicHelper
///////////////////////////////////////////////////////////////////////
const Int32 StackView::HolographicHelper::RES_OUT;
const Int32 StackView::HolographicHelper::CLICK_FEEDBACK;

StackView::HolographicHelper::HolographicHelper(
    /* [in] */ IContext* context)
{
    CPaint::New((IPaint**)&mHolographicPaint);
    CPaint::New((IPaint**)&mErasePaint);
    CPaint::New((IPaint**)&mBlurPaint);

    CCanvas::New((ICanvas**)&mCanvas);
    CCanvas::New((ICanvas**)&mMaskCanvas);
    mTmpXY = ArrayOf<Int32>::Alloc(2);
    CMatrix::New((IMatrix**)&mIdentityMatrix);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensity);

    mHolographicPaint->SetFilterBitmap(TRUE);
    AutoPtr<ITableMaskFilterHelper> helper;
    CTableMaskFilterHelper::AcquireSingleton((ITableMaskFilterHelper**)&helper);
    AutoPtr<ITableMaskFilter> tmf;
    helper->CreateClipTable(0, 30, (ITableMaskFilter**)&tmf);
    mHolographicPaint->SetMaskFilter(tmf);
    AutoPtr<IPorterDuffXfermode> pd;
    CPorterDuffXfermode::New(PorterDuffMode_DST_OUT, (IPorterDuffXfermode**)&pd);
    mErasePaint->SetXfermode(pd);
    mErasePaint->SetFilterBitmap(TRUE);

    CBlurMaskFilter::New(2 * mDensity, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&mSmallBlurMaskFilter);
    CBlurMaskFilter::New(4 * mDensity, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&mLargeBlurMaskFilter);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateClickOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 color)
{
    return CreateOutline(v, CLICK_FEEDBACK, color);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateResOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 color)
{
    return CreateOutline(v, RES_OUT, color);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 type,
    /* [in] */ Int32 color)
{
    mHolographicPaint->SetColor(color);
    if (type == RES_OUT) {
        mBlurPaint->SetMaskFilter(mSmallBlurMaskFilter);
    } else if (type == CLICK_FEEDBACK) {
        mBlurPaint->SetMaskFilter(mLargeBlurMaskFilter);
    }

    Int32 width = 0, height = 0;
    v->GetMeasuredWidth(&width);
    v->GetMeasuredHeight(&height);
    if (width == 0 || height == 0) {
        return NULL;
    }

    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IResources> res;
    v->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    AutoPtr<IBitmap> bitmap;
    factory->CreateBitmap(dm, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    mCanvas->SetBitmap(bitmap);

    Float rotationX = 0, rotation = 0, translationY = 0, translationX = 0;
    v->GetRotationX(&rotationX);
    v->GetRotation(&rotation);
    v->GetTranslationY(&translationY);
    v->GetTranslationX(&translationX);
    v->SetRotationX(0);
    v->SetRotation(0);
    v->SetTranslationY(0);
    v->SetTranslationX(0);
    v->Draw(mCanvas);
    v->SetRotationX(rotationX);
    v->SetRotation(rotation);
    v->SetTranslationY(translationY);
    v->SetTranslationX(translationX);

    DrawOutline(mCanvas, bitmap);
    mCanvas->SetBitmap(NULL);
    return bitmap;
}

void StackView::HolographicHelper::DrawOutline(
    /* [in] */ ICanvas* dest,
    /* [in] */ IBitmap* src)
{
    AutoPtr<IBitmap> mask;
    src->ExtractAlpha(mBlurPaint, mTmpXY, (IBitmap**)&mask);
    mMaskCanvas->SetBitmap(mask);
    mMaskCanvas->DrawBitmap(src, -(*mTmpXY)[0], -(*mTmpXY)[1], mErasePaint);
    dest->DrawColor(0, PorterDuffMode_CLEAR);
    dest->SetMatrix(mIdentityMatrix);
    dest->DrawBitmap(mask, (*mTmpXY)[0], (*mTmpXY)[1], mHolographicPaint);
    mMaskCanvas->SetBitmap(NULL);
    mask->Recycle();
}

///////////////////////////////////////////////////////////////////////
//                  StackView::StackSlider
///////////////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(StackView::StackSlider, IInterface)

const Int32 StackView::StackSlider::NORMAL_MODE;
const Int32 StackView::StackSlider::BEGINNING_OF_STACK_MODE;
const Int32 StackView::StackSlider::END_OF_STACK_MODE;

StackView::StackSlider::StackSlider(
    /* [in] */ StackView* host)
    : mHost(host)
{
}

StackView::StackSlider::StackSlider(const StackSlider& copy)
{
    mView = copy.mView;
    mYProgress = copy.mYProgress;
    mXProgress = copy.mXProgress;
    mMode = copy.mMode;
}

void StackView::StackSlider::SetView(
        /* [in] */ IView* v)
{
    mView = v;
}

void StackView::StackSlider::SetYProgress(
    /* [in] */ Float r)
{
    r = Elastos::Core::Math::Min(1.0f, r);
    r = Elastos::Core::Math::Max(0.0f, r);

    mYProgress = r;
    if (!mView) return;

    AutoPtr<IViewGroupLayoutParams> viewLp;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);
    AutoPtr<IViewGroupLayoutParams> highlightLp;
    mHost->mHighlight->GetLayoutParams((IViewGroupLayoutParams**)&highlightLp);

    Int32 stackDirection = (mHost->mStackMode == ITEMS_SLIDE_UP) ? 1 : -1;

    if (Elastos::Core::Math::Compare(0.0f, mYProgress) != 0 && Elastos::Core::Math::Compare(1.0f, mYProgress) != 0) {
        Int32 type = 0;
        mView->GetLayerType(&type);
        if (type == IView::LAYER_TYPE_NONE) {
            mView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
        }
    } else {
        Int32 type = 0;
        mView->GetLayerType(&type);
        if (type != IView::LAYER_TYPE_NONE) {
            mView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
        }
    }

    switch (mMode) {
        case NORMAL_MODE:
        {
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(-r * stackDirection * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(-r * stackDirection * mHost->mSlideAmount));
            mHost->mHighlight->SetAlpha(HighlightAlphaInterpolator(r));

            Float alpha = ViewAlphaInterpolator(1 - r);

            Float vah = 0;
            mView->GetAlpha(&vah);
            Int32 visible = 0;
            mView->GetVisibility(&visible);
            if (vah == 0 && alpha != 0 && visible != IView::VISIBLE) {
                mView->SetVisibility(IView::VISIBLE);
            } else if (alpha == 0 && vah != 0 && visible == IView::VISIBLE) {
                mView->SetVisibility(IView::INVISIBLE);
            }

            mView->SetAlpha(alpha);
            mView->SetRotationX(stackDirection * 90.0f * RotationInterpolator(r));
            mHost->mHighlight->SetRotationX(stackDirection * 90.0f * RotationInterpolator(r));
            break;
        }

        case END_OF_STACK_MODE:
        {
            r = r * 0.2f;
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(-stackDirection * r * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(-stackDirection * r * mHost->mSlideAmount));
            mHost->mHighlight->SetAlpha(HighlightAlphaInterpolator(r));
            break;
        }
        case BEGINNING_OF_STACK_MODE:
        {
            r = (1-r) * 0.2f;
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(stackDirection * r * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(stackDirection * r * mHost->mSlideAmount));
            mHost->mHighlight->SetAlpha(HighlightAlphaInterpolator(r));
            break;
        }
    }
}

void StackView::StackSlider::SetXProgress(
    /* [in] */ Float r)
{
    r = Elastos::Core::Math::Min(2.0f, r);
    r = Elastos::Core::Math::Max(-2.0f, r);

    mXProgress = r;

    if (!mView) return;

    AutoPtr<IViewGroupLayoutParams> viewLp;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);
    AutoPtr<IViewGroupLayoutParams> highlightLp;
    mHost->mHighlight->GetLayoutParams((IViewGroupLayoutParams**)&highlightLp);

    r *= 0.2f;
    AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
    AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
    assert(slp && shlp);
    slp->SetHorizontalOffset(Elastos::Core::Math::Round(r * mHost->mSlideAmount));
    shlp->SetHorizontalOffset(Elastos::Core::Math::Round(r * mHost->mSlideAmount));
}

void StackView::StackSlider::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
}

Float StackView::StackSlider::GetDurationForNeutralPosition()
{
    return GetDuration(FALSE, 0);
}

Float StackView::StackSlider::GetDurationForOffscreenPosition()
{
     return GetDuration(TRUE, 0);
}

Float StackView::StackSlider::GetDurationForNeutralPosition(
    /* [in] */ Float r)
{
    return GetDuration(FALSE, r);
}

Float StackView::StackSlider::GetDurationForOffscreenPosition(
    /* [in] */ Float r)
{
    return GetDuration(TRUE, r);
}

Float StackView::StackSlider::GetYProgress()
{
    return mYProgress;
}

Float StackView::StackSlider::GetXProgress()
{
    return mXProgress;
}

Float StackView::StackSlider::Cubic(
    /* [in] */ Float r)
{
    return (Float) (Elastos::Core::Math::Pow(2 * r - 1, 3) + 1) / 2.0f;
}

Float StackView::StackSlider::HighlightAlphaInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.4f;
    if (r < pivot) {
        return 0.85f * Cubic(r / pivot);
    } else {
        return 0.85f * Cubic(1 - (r - pivot) / (1 - pivot));
    }
}

Float StackView::StackSlider::ViewAlphaInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.3f;
    if (r > pivot) {
        return (r - pivot) / (1 - pivot);
    } else {
        return 0;
    }
}

Float StackView::StackSlider::RotationInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.2f;
    if (r < pivot) {
        return 0;
    } else {
        return (r - pivot) / (1 - pivot);
    }
}

Float StackView::StackSlider::GetDuration(
    /* [in] */ Boolean invert,
    /* [in] */ Float r)
{
    if (mView) {
        AutoPtr<IViewGroupLayoutParams> viewLp;
        mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);

        Int32 horizontalOffset = 0, verticalOffset = 0;
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
        assert(slp);
        slp->GetHorizontalOffset(&horizontalOffset);
        slp->GetVerticalOffset(&verticalOffset);
        Float d = (Float) Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(horizontalOffset, 2) + Elastos::Core::Math::Pow(verticalOffset, 2));
        Float maxd = (Float) Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(mHost->mSlideAmount, 2)
            + Elastos::Core::Math::Pow(0.4f * mHost->mSlideAmount, 2));

        if (r == 0) {
            return (invert ? (1 - d / maxd) : d / maxd) * DEFAULT_ANIMATION_DURATION;
        } else {
            Float duration = invert ? d / Elastos::Core::Math::Abs(r) :
                    (maxd - d) / Elastos::Core::Math::Abs(r);
            if (duration < MINIMUM_ANIMATION_DURATION || duration > DEFAULT_ANIMATION_DURATION) {
                return GetDuration(invert, 0);
            } else {
                return duration;
            }
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//                  StackView::PostRun
///////////////////////////////////////////////////////////////////////
StackView::PostRun::PostRun(
    /* [in] */ IView* v)
    : mView(v)
{
}

ECode StackView::PostRun::Run()
{
    mView->SetAlpha(0);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos














