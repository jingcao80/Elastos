
#ifndef __ELASTOS_DROID_WIDGET_STACKVIEW_H__
#define __ELASTOS_DROID_WIDGET_STACKVIEW_H__

#include <R.h>
#include "elastos/droid/ext/frameworkext.h"
#include <widget/AdapterViewAnimator.h>



#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Widget::IFrameLayout;
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
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Widget {

extern "C" const InterfaceID EIID_StackFrame;

class StackView : public AdapterViewAnimator
{
private:

    class _StackFrame
        : public FrameLayout
        , public ElRefBase
    {
    public:
        _StackFrame(
            /* [in] */ IContext* context);

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

    class StackFrame
        : public _StackFrame
        , public IFrameLayout
        , public IViewParent
        , public IViewManager
        , public IDrawableCallback
        , public IKeyEventCallback
        , public IAccessibilityEventSource
        , public IWeakReferenceSource
    {
    public:
        StackFrame(
            /* [in] */ IContext* context);

        IVIEW_METHODS_DECL()
        IVIEWGROUP_METHODS_DECL()
        IVIEWPARENT_METHODS_DECL()
        IVIEWMANAGER_METHODS_DECL()
        IDRAWABLECALLBACK_METHODS_DECL()
        IKEYEVENTCALLBACK_METHODS_DECL()
        IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
        IFRAMELAYOUT_METHODS_DECL()

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI GetWeakReference(
            /* [out] */ IWeakReference** weakReference);
    };

    class HolographicHelper
        : public ElRefBase
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

    class StackSlider
        : public ElRefBase
        , public IInterface
    {
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

    public:
        CAR_INTERFACE_DECL()

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
    StackView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyle = R::attr::stackViewStyle);

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

    CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

protected:
    StackView();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyle = R::attr::stackViewStyle);

    CARAPI_(void) TransformViewForTransition(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex,
        /* [in] */ IView* view,
        /* [in] */ Boolean animate);

    CARAPI_(void) ShowOnly(
        /* [in] */ Int32 childIndex,
        /* [in] */ Boolean animate);

    CARAPI_(void) UpdateClickFeedback();

    CARAPI_(void) ShowTapFeedback(
        /* [in] */ IView* child);

    CARAPI_(void) HideTapFeedback(
        /* [in] */ IView* child);

    CARAPI_(void) OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(AutoPtr<IFrameLayout>) GetFrameForChild();

    CARAPI_(void) ApplyTransformForChildAtIndex(
        /* [in] */ IView* child,
        /* [in] */ Int32 relativeIndex);

    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

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

    static const Float PERSPECTIVE_SHIFT_FACTOR_Y = 0.1f;
    static const Float PERSPECTIVE_SHIFT_FACTOR_X = 0.1f;
    static const Float PERSPECTIVE_SCALE_FACTOR = 0.0f;
    static const Float SWIPE_THRESHOLD_RATIO = 0.2f;
    static const Float SLIDE_UP_RATIO = 0.7f;

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
