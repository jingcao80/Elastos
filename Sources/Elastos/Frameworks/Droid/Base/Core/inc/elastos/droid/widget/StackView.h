//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WIDGET_STACKVIEW_H__
#define __ELASTOS_DROID_WIDGET_STACKVIEW_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/widget/AdapterViewAnimator.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/R.h"

using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IBlurMaskFilter;

namespace Elastos {
namespace Droid {
namespace Widget {

class StackView
    : public AdapterViewAnimator
    , public IStackView
{
public:
    class LayoutParams
        : public ViewGroup::LayoutParams
        , public IStackViewLayoutParams
    {
    public:
        LayoutParams();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IView* view);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI_(void) InvalidateGlobalRegion(
            /* [in] */ IView* v,
            /* [in] */ IRect* r);

        CARAPI_(AutoPtr<IRect>) GetInvalidateRect();

        CARAPI_(void) ResetInvalidateRect();

        CARAPI SetVerticalOffset(
            /* [in] */ Int32 newVerticalOffset);

        CARAPI SetHorizontalOffset(
            /* [in] */ Int32 newHorizontalOffset);

        CARAPI SetOffsets(
            /* [in] */ Int32 newHorizontalOffset,
            /* [in] */ Int32 newVerticalOffset);

    public:
        Int32 mHorizontalOffset;
        Int32 mVerticalOffset;
        AutoPtr<IView> mView;

    private:
        AutoPtr<IRect> mParentRect;
        AutoPtr<IRect> mInvalidateRect;
        AutoPtr<IRectF> mInvalidateRectf;
        AutoPtr<IRect> mGlobalInvalidateRect;
    };

    class StackFrame
        : public FrameLayout
        , public IStackFrame
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context);

        CAR_INTERFACE_DECL()

        CARAPI_(void) SetTransformAnimator(
            /* [in] */ IObjectAnimator* oa);

        CARAPI_(void) SetSliderAnimator(
            /* [in] */ IObjectAnimator* oa);

        CARAPI_(Boolean) CancelTransformAnimator();

        CARAPI_(Boolean) CancelSliderAnimator();

    private:
        AutoPtr<IObjectAnimator> mTransformAnimator;
        AutoPtr<IObjectAnimator> mSliderAnimator;
    };

private:
    class HolographicHelper : public Object
    {
    public:
        HolographicHelper(
            /* [in] */ IContext* context);

        CARAPI_(AutoPtr<IBitmap>) CreateClickOutline(
            /* [in] */ IView* v,
            /* [in] */ Int32 color);

        CARAPI_(AutoPtr<IBitmap>) CreateResOutline(
            /* [in] */ IView* v,
            /* [in] */ Int32 color);

        CARAPI_(AutoPtr<IBitmap>) CreateOutline(
            /* [in] */ IView* v,
            /* [in] */ Int32 type,
            /* [in] */ Int32 color);

        CARAPI_(void) DrawOutline(
            /* [in] */ ICanvas* dest,
            /* [in] */ IBitmap* src);

    private:
        AutoPtr<IPaint> mHolographicPaint;// = new Paint();
        AutoPtr<IPaint> mErasePaint;// = new Paint();
        AutoPtr<IPaint> mBlurPaint;// = new Paint();

        Float mDensity;
        AutoPtr<IBlurMaskFilter> mSmallBlurMaskFilter;
        AutoPtr<IBlurMaskFilter> mLargeBlurMaskFilter;

        AutoPtr<ICanvas> mCanvas; // = new Canvas();
        AutoPtr<ICanvas> mMaskCanvas; // = new Canvas();

        AutoPtr< ArrayOf<Int32> > mTmpXY; //=  new int[2];
        AutoPtr<IMatrix> mIdentityMatrix;//= new Matrix();

        static const Int32 RES_OUT = 0;
        static const Int32 CLICK_FEEDBACK = 1;
    };

    class StackSlider : public Object
    {
    public:
        StackSlider(
            /* [in] */ StackView* host);

        StackSlider(const StackSlider& copy);

        CARAPI_(void) SetView(
            /* [in] */ IView* v);

        CARAPI_(void) SetYProgress(
            /* [in] */ Float r);

        CARAPI_(void) SetXProgress(
            /* [in] */ Float r);

        CARAPI_(void) SetMode(
            /* [in] */ Int32 mode);

        CARAPI_(Float) GetDurationForNeutralPosition();

        CARAPI_(Float) GetDurationForOffscreenPosition();

        CARAPI_(Float) GetDurationForNeutralPosition(
            /* [in] */ Float r);

        CARAPI_(Float) GetDurationForOffscreenPosition(
            /* [in] */ Float r);

        CARAPI_(Float) GetYProgress();

        CARAPI_(Float) GetXProgress();

    private:
        CARAPI_(Float) Cubic(
            /* [in] */ Float r);

        CARAPI_(Float) HighlightAlphaInterpolator(
            /* [in] */ Float r);

        CARAPI_(Float) ViewAlphaInterpolator(
            /* [in] */ Float r);

        CARAPI_(Float) RotationInterpolator(
            /* [in] */ Float r);

        CARAPI_(Float) GetDuration(
            /* [in] */ Boolean invert,
            /* [in] */ Float r);

    public:
        AutoPtr<IView> mView;
        Float mYProgress;
        Float mXProgress;

        Int32 mMode;

        static const Int32 NORMAL_MODE = 0;
        static const Int32 BEGINNING_OF_STACK_MODE = 1;
        static const Int32 END_OF_STACK_MODE = 2;

    private:
        StackView* mHost;
    };

    class PostRun
        : public Runnable
    {
    public:
        PostRun(
            /* [in] */ IView* v);

        CARAPI Run();

    private:
        AutoPtr<IView> mView;
    };

public:
    StackView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::stackViewStyle,
        /* [in] */ Int32 defStyleRes = 0);

    CARAPI ShowNext();

    CARAPI ShowPrevious();

    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    CARAPI Advance();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

protected:
    virtual CARAPI_(void) TransformViewForTransition(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex,
        /* [in] */ IView* view,
        /* [in] */ Boolean animate);

    //@Override
    CARAPI_(void) ShowOnly(
        /* [in] */ Int32 childIndex,
        /* [in] */ Boolean animate);

    virtual CARAPI_(void) UpdateClickFeedback();

    //@Override
    CARAPI_(void) ShowTapFeedback(
        /* [in] */ IView* child);

    //@Override
    CARAPI_(void) HideTapFeedback(
        /* [in] */ IView* child);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI_(AutoPtr<IFrameLayout>) GetFrameForChild();

    //@Override
    CARAPI_(void) ApplyTransformForChildAtIndex(
        /* [in] */ IView* child,
        /* [in] */ Int32 relativeIndex);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) CreateOrReuseLayoutParams(
        /* [in] */ IView* v);

private:
    CARAPI_(void) InitStackView();

    CARAPI_(void) TransformViewAtIndex(
        /* [in] */ Int32 index,
        /* [in] */ IView* view,
        /* [in] */ Int32 animate);

    CARAPI_(void) SetupStackSlider(
        /* [in] */ IView* view,
        /* [in] */ Int32 mode);

    CARAPI_(void) UpdateChildTransforms();

    CARAPI_(void) OnLayout();

    CARAPI_(void) PacedScroll(
        /* [in] */ Boolean up);

    CARAPI_(void) BeginGestureIfNeeded(
        /* [in] */ Float deltaY);

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) HandlePointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) MeasureChildren();

private:
    static const String TAG;

    static const Int32 DEFAULT_ANIMATION_DURATION = 400;
    static const Int32 MINIMUM_ANIMATION_DURATION = 50;
    static const Int32 STACK_RELAYOUT_DURATION = 100;
    static const Int32 ITEMS_SLIDE_UP = 0;
    static const Int32 ITEMS_SLIDE_DOWN = 1;
    static const Int32 GESTURE_NONE = 0;
    static const Int32 GESTURE_SLIDE_UP = 1;
    static const Int32 GESTURE_SLIDE_DOWN = 2;
    static const Int32 INVALID_POINTER = -1;
    static const Int32 NUM_ACTIVE_VIEWS = 5;
    static const Int32 FRAME_PADDING = 4;
    static const Int32 MIN_TIME_BETWEEN_INTERACTION_AND_AUTOADVANCE = 5000;

    static constexpr Float PERSPECTIVE_SHIFT_FACTOR_Y = 0.1f;
    static constexpr Float PERSPECTIVE_SHIFT_FACTOR_X = 0.1f;
    static constexpr Float PERSPECTIVE_SCALE_FACTOR = 0.0f;
    static constexpr Float SWIPE_THRESHOLD_RATIO = 0.2f;
    static constexpr Float SLIDE_UP_RATIO = 0.7f;

    static const Int64 MIN_TIME_BETWEEN_SCROLLS = 100;

    Float mPerspectiveShiftX;
    Float mPerspectiveShiftY;
    Float mNewPerspectiveShiftX;
    Float mNewPerspectiveShiftY;
    Float mInitialY;
    Float mInitialX;

    Int32 mActivePointerId;
    Int32 mYVelocity;
    Int32 mSwipeGestureType;
    Int32 mSlideAmount;
    Int32 mSwipeThreshold;
    Int32 mTouchSlop;
    Int32 mMaximumVelocity;
    Int32 mResOutColor;
    Int32 mClickColor;

    AutoPtr<IRect> mTouchRect; //=new Rect()

    AutoPtr<VelocityTracker> mVelocityTracker;
    Boolean mTransitionIsSetup; //= FALSE;

    static AutoPtr<HolographicHelper> sHolographicHelper;

    AutoPtr<IImageView> mHighlight;
    AutoPtr<IImageView> mClickFeedback;
    Boolean mClickFeedbackIsValid; //= FALSE;
    AutoPtr<StackSlider> mStackSlider;
    Boolean mFirstLayoutHappened;// = FALSE;

    Int64 mLastInteractionTime;// = 0;
    Int64 mLastScrollTime;
    Int32 mStackMode;
    Int32 mFramePadding;

    static AutoPtr<IRect> sStackInvalidateRect; //=new Rect()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_STACKVIEW_H__
