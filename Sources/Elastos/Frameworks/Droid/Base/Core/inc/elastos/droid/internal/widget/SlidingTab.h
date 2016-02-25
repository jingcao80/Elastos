
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SLIDINGTAB_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SLIDINGTAB_H__

#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::IAnimationAnimationListener;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * A special widget containing two Sliders and a threshold for each.  Moving either slider beyond
 * the threshold will cause the registered OnTriggerListener.onTrigger() to be called with
 * whichHandle being {@link OnTriggerListener#LEFT_HANDLE} or {@link OnTriggerListener#RIGHT_HANDLE}
 * Equivalently, selecting a tab will result in a call to
 * {@link OnTriggerListener#onGrabbedStateChange(View, Int32)} with one of these two states. Releasing
 * the tab will result in whichHandle being {@link OnTriggerListener#NO_HANDLE}.
 *
 */
class SlidingTab
    : public ViewGroup
    , public ISlidingTab
{
private:
    /**
     * Simple container class for all things pertinent to a slider.
     * A slider consists of 3 Views:
     *
     * {@link #tab} is the tab shown on the screen in the default state.
     * {@link #text} is the view revealed as the user slides the tab out.
     * {@link #target} is the target the user must drag the slider past to trigger the slider.
     *
     */
    class Slider : public Object
    {
    public:
        /**
         * Constructor
         *
         * @param parent the container view of this one
         * @param tabId drawable for the tab
         * @param barId drawable for the bar
         * @param targetId drawable for the target
         */
        Slider(
            /* [in] */ IViewGroup* parent,
            /* [in] */ Int32 tabId,
            /* [in] */ Int32 barId,
            /* [in] */ Int32 targetId);

        CARAPI_(void) SetIcon(
            /* [in] */ Int32 iconId);

        CARAPI_(void) SetTabBackgroundResource(
            /* [in] */ Int32 tabId);

        CARAPI_(void) SetBarBackgroundResource(
            /* [in] */ Int32 barId);

        CARAPI_(void) SetHintText(
            /* [in] */ Int32 resId);

        CARAPI_(void) Hide();

        CARAPI_(void) Show(
            /* [in] */ Boolean animate);

        CARAPI_(void) SetState(
            /* [in] */ Int32 state);

        CARAPI_(void) ShowTarget();

        CARAPI_(void) Reset(
            /* [in] */ Boolean animate);

        CARAPI_(void) SetTarget(
            /* [in] */ Int32 targetId);

        /**
         * Layout the given widgets within the parent.
         *
         * @param l the parent's left border
         * @param t the parent's top border
         * @param r the parent's right border
         * @param b the parent's bottom border
         * @param alignment which side to align the widget to
         */
        CARAPI_(void) Layout(
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b,
            /* [in] */ Int32 alignment);

        CARAPI_(void) UpdateDrawableStates();

        /**
         * Ensure all the dependent widgets are measured.
         */
        CARAPI_(void) Measure();

        /**
         * Get the measured tab width. Must be called after {@link Slider#measure()}.
         * @return
         */
        CARAPI_(Int32) GetTabWidth();

        /**
         * Get the measured tab width. Must be called after {@link Slider#measure()}.
         * @return
         */
        CARAPI_(Int32) GetTabHeight();

        /**
         * Start animating the slider. Note we need two animations since a ValueAnimator
         * keeps internal state of the invalidation region which is just the view being animated.
         *
         * @param anim1
         * @param anim2
         */
        CARAPI_(void) StartAnimation(
            /* [in] */ IAnimation* anim1,
            /* [in] */ IAnimation* anim2);

        CARAPI_(void) HideTarget();

    public:
        static const Int32 ALIGN_LEFT;
        static const Int32 ALIGN_RIGHT;
        static const Int32 ALIGN_TOP;
        static const Int32 ALIGN_BOTTOM;
        static const Int32 ALIGN_UNKNOWN;

    /**
     * States for the view.
     */
    private:
        static const Int32 STATE_NORMAL;
        static const Int32 STATE_PRESSED;
        static const Int32 STATE_ACTIVE;

    private:
        AutoPtr<IImageView> mTab;
        AutoPtr<ITextView> mText;
        AutoPtr<IImageView> mTarget;
        Int32 mCurrentState;
        Int32 mAlignment;
        Int32 mAlignment_value;

        friend class SlidingTab;
    };

    class AnimationDoneListener
        : public Object
        , public IAnimationAnimationListener
    {
    public:
        CAR_INTERFACE_DECL();

        AnimationDoneListener(
            /* [in] */ SlidingTab* host);

        virtual CARAPI OnAnimationStart(
            /* [in] */ IAnimation* animation);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimation* animation);

        virtual CARAPI OnAnimationRepeat(
            /* [in] */ IAnimation* animation);

    protected:
        SlidingTab* mHost;
    };

    class StartAnimationListener
        : public Object
        , public IAnimationAnimationListener
    {
    public:
        CAR_INTERFACE_DECL();

        StartAnimationListener(
            /* [in] */ SlidingTab* host,
            /* [in] */ Boolean holdAfter,
            /* [in] */ Int32 dx,
            /* [in] */ Int32 dy);

        virtual CARAPI OnAnimationStart(
            /* [in] */ IAnimation* animation);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimation* animation);

        virtual CARAPI OnAnimationRepeat(
            /* [in] */ IAnimation* animation);

    protected:
        SlidingTab* mHost;

    private:
        Boolean mHoldAfter;
        Int32 mDx;
        Int32 mDy;
    };

public:
    CAR_INTERFACE_DECL();

    SlidingTab();

    /**
     * Constructor used when this widget is created from a layout file.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Reset the tabs to their original state and stop any existing animation.
     * Animate them back into place if animate is TRUE.
     *
     * @param animate
     */
    virtual CARAPI Reset(
        /* [in] */ Boolean animate);

    virtual CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Sets the left handle icon to a given resource.
     *
     * The resource should refer to a Drawable object, or use 0 to remove
     * the icon.
     *
     * @param iconId the resource ID of the icon drawable
     * @param targetId the resource of the target drawable
     * @param barId the resource of the bar drawable (stateful)
     * @param tabId the resource of the
     */
    virtual CARAPI SetLeftTabResources(
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 targetId,
        /* [in] */ Int32 barId,
        /* [in] */ Int32 tabId);

    /**
     * Sets the left handle hint text to a given resource string.
     *
     * @param resId
     */
    virtual CARAPI SetLeftHintText(
        /* [in] */ Int32 resId);

    /**
     * Sets the right handle icon to a given resource.
     *
     * The resource should refer to a Drawable object, or use 0 to remove
     * the icon.
     *
     * @param iconId the resource ID of the icon drawable
     * @param targetId the resource of the target drawable
     * @param barId the resource of the bar drawable (stateful)
     * @param tabId the resource of the
     */
    virtual CARAPI SetRightTabResources(
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 targetId,
        /* [in] */ Int32 barId,
        /* [in] */ Int32 tabId);

    /**
     * Sets the left handle hint text to a given resource string.
     *
     * @param resId
     */
    virtual CARAPI SetRightHintText(
        /* [in] */ Int32 resId);

    virtual CARAPI SetHoldAfterTrigger(
        /* [in] */ Boolean holdLeft,
        /* [in] */ Boolean holdRight);

    /**
     * Registers a callback to be invoked when the user triggers an event.
     *
     * @param listener the OnDialTriggerListener to attach to this view
     */
    virtual CARAPI SetOnTriggerListener(
        /* [in] */ ISlidingTabOnTriggerListener* listener);

protected:
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

private:
    CARAPI_(void) CancelGrab();

    CARAPI_(void) StartAnimating(
        /* [in] */ Boolean holdAfter);

    CARAPI_(void) OnAnimationDone();

    CARAPI_(Boolean) WithinView(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ View* view);

    CARAPI_(Boolean) IsHorizontal();

    CARAPI_(void) ResetView();

    CARAPI_(void) MoveHandle(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Triggers haptic feedback.
     */
    CARAPI_(void) Vibrate(
        /* [in] */ Int64 duration);

    /**
     * Dispatches a trigger event to listener. Ignored if a listener is not set.
     * @param whichHandle the handle that triggered the event.
     */
    CARAPI_(void) DispatchTriggerEvent(
        /* [in] */ Int32 whichHandle);

    /**
     * Sets the current grabbed state, and dispatches a grabbed state change
     * event to our listener.
     */
    CARAPI_(void) SetGrabbedState(
        /* [in] */ Int32 newState);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

    CARAPI AnimationEndInStartAnimating(
        /* [in] */ Boolean holdAfter,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    static CARAPI_(AutoPtr<IAudioAttributes>) InitStatic();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 HORIZONTAL; // as defined in attrs.xml
    static const Int32 VERTICAL;

    // TODO: Make these configurable
    static const Float THRESHOLD;
    static const Int64 VIBRATE_SHORT;
    static const Int64 VIBRATE_LONG;
    static const Int32 TRACKING_MARGIN;
    static const Int32 ANIM_DURATION; // Time for most animations (in ms)
    static const Int32 ANIM_TARGET_TIME; // Time to show targets (in ms)

private:
    Boolean mHoldLeftOnTransition;
    Boolean mHoldRightOnTransition;

    AutoPtr<ISlidingTabOnTriggerListener> mOnTriggerListener;
    Int32 mGrabbedState;
    Boolean mTriggered;
    AutoPtr<IVibrator> mVibrator;
    Float mDensity; // used to scale dimensions for bitmaps.

    /**
     * Either {@link #HORIZONTAL} or {@link #VERTICAL}.
     */
    Int32 mOrientation;

    AutoPtr<Slider> mLeftSlider;
    AutoPtr<Slider> mRightSlider;
    AutoPtr<Slider> mCurrentSlider;
    Boolean mTracking;
    Float mThreshold;
    AutoPtr<Slider> mOtherSlider;
    Boolean mAnimating;
    AutoPtr<IRect> mTmpRect;
    AutoPtr<AnimationDoneListener> mAnimationDoneListener;

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SLIDINGTAB_H__
