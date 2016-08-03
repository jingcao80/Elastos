
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_GLOWPADVIEW_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_GLOWPADVIEW_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/widget/multiwaveview/PointCloud.h"
#include "elastos/droid/incallui/widget/multiwaveview/TargetDrawable.h"
#include "elastos/droid/incallui/widget/multiwaveview/Tweener.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/view/View.h>
#include <elastos/utility/ArrayList.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Utility::ArrayList;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

/**
 * This is a copy of com.android.internal.widget.multiwaveview.GlowPadView with minor changes
 * to remove dependencies on private api's.
 *
 * Incoporated the scaling functionality.
 *
 * A re-usable widget containing a center, outer ring and wave animation.
 */
class GlowPadView
    : public Elastos::Droid::View::View
    , public IGlowPadView
{
private:
    class AnimationBundle
        : public ArrayList
    {
    public:
        CARAPI_(void) Start();

        CARAPI_(void) Cancel();

        CARAPI_(void) Stop();

        CARAPI_(void) SetSuspended(
            /* [in] */ Boolean suspend);

        CARAPI ToString(
            /* [ou] */ String* str);

    private:
        Boolean mSuspended;
    };

    class ResetListener
        : public AnimatorListenerAdapter
    {
    public:
        ResetListener(
            /* [in] */ GlowPadView* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    private:
        GlowPadView* mHost;
    };

    class ResetListenerWithPing
        : public AnimatorListenerAdapter
    {
    public:
        ResetListenerWithPing(
            /* [in] */ GlowPadView* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    private:
        GlowPadView* mHost;
    };

    class UpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        UpdateListener(
            /* [in] */ GlowPadView* host)
            : mHost(host)
        {}

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        GlowPadView* mHost;
    };

    class TargetUpdateListener
        : public AnimatorListenerAdapter
    {
    public:
        TargetUpdateListener(
            /* [in] */ GlowPadView* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    private:
        GlowPadView* mHost;
    };

    class WaveAnimationListener
        : public AnimatorListenerAdapter
    {
    public:
        WaveAnimationListener(
            /* [in] */ GlowPadView* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    private:
        GlowPadView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    GlowPadView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) SuspendAnimations();

    CARAPI_(void) ResumeAnimations();

    /**
     * Loads an array of drawables from the given resourceId.
     *
     * @param resourceId
     */
    CARAPI_(void) SetTargetResources(
        /* [in] */ Int32 resourceId);

    CARAPI_(Int32) GetTargetResourceId();

    /**
     * Sets teh handle drawable to the drawable specified by the resource ID.
     * @param resourceId
     */
    CARAPI_(void) SetHandleDrawable(
        /* [in] */ Int32 resourceId);

    /**
     * Sets the resource id specifying the target descriptions for accessibility.
     *
     * @param resourceId The resource id.
     */
    CARAPI_(void) SetTargetDescriptionsResourceId(
        /* [in] */ Int32 resourceId);

    /**
     * Gets the resource id specifying the target descriptions for accessibility.
     *
     * @return The resource id.
     */
    CARAPI_(Int32) GetTargetDescriptionsResourceId();

    /**
     * Sets the resource id specifying the target direction descriptions for accessibility.
     *
     * @param resourceId The resource id.
     */
    CARAPI_(void) SetDirectionDescriptionsResourceId(
        /* [in] */ Int32 resourceId);

    /**
     * Gets the resource id specifying the target direction descriptions.
     *
     * @return The resource id.
     */
    CARAPI_(Int32) GetDirectionDescriptionsResourceId();

    /**
     * Enable or disable vibrate on touch.
     *
     * @param enabled
     */
    CARAPI_(void) SetVibrateEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Starts wave animation.
     *
     */
    CARAPI_(void) Ping();

    /**
     * Resets the widget to default state and cancels all animation. If animate is 'true', will
     * animate objects into place. Otherwise, objects will snap back to place.
     *
     * @param animate
     */
    CARAPI_(void) Reset(
        /* [in] */ Boolean animate);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI_(void) SetOnTriggerListener(
        /* [in] */ IGlowPadViewOnTriggerListener* listener);

    CARAPI_(Int32) GetResourceIdForTarget(
        /* [in] */ Int32 index);

    CARAPI_(void) SetEnableTarget(
        /* [in] */ Int32 resourceId,
        /* [in] */ Boolean enabled);

    /**
     * Gets the position of a target in the array that matches the given resource.
     * @param resourceId
     * @return the index or -1 if not found
     */
    CARAPI_(Int32) GetTargetPosition(
        /* [in] */ Int32 resourceId);

    /**
     * Searches the given package for a resource to use to replace the Drawable on the
     * target with the given resource id
     * @param component of the .apk that contains the resource
     * @param name of the metadata in the .apk
     * @param existingResId the resource id of the target to search for
     * @return true if found in the given package and replaced at least one target Drawables
     */
    CARAPI_(Boolean) ReplaceTargetDrawablesIfPresent(
        /* [in] */ IComponentName* component,
        /* [in] */ const String& name,
        /* [in] */ Int32 existingResId);

protected:
    // @Override
    CARAPI_(Int32) GetSuggestedMinimumWidth();

    // @Override
    CARAPI_(Int32) GetSuggestedMinimumHeight();

    /**
     * This gets the suggested width accounting for the ring's scale factor.
     */
    CARAPI_(Int32) GetScaledSuggestedMinimumWidth();

    /**
     * This gets the suggested height accounting for the ring's scale factor.
     */
    CARAPI_(Int32) GetScaledSuggestedMinimumHeight();

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Int32) GetResourceId(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 id);

    CARAPI_(void) Dump();

    CARAPI_(Int32) ResolveMeasured(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 desired);

    CARAPI_(void) SwitchToState(
        /* [in] */ Int32 state,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) ShowGlow(
        /* [in] */ Int32 duration,
        /* [in] */ Int32 delay,
        /* [in] */ Float finalAlpha,
        /* [in] */ IAnimatorListener* finishListener);

    CARAPI_(void) HideGlow(
        /* [in] */ Int32 duration,
        /* [in] */ Int32 delay,
        /* [in] */ Float finalAlpha,
        /* [in] */ IAnimatorListener* finishListener);

    CARAPI_(void) DeactivateTargets();

    /**
     * Dispatches a trigger event to listener. Ignored if a listener is not set.
     * @param whichTarget the target that was triggered.
     */
    CARAPI_(void) DispatchTriggerEvent(
        /* [in] */ Int32 whichTarget);

    CARAPI_(void) DispatchOnFinishFinalAnimation();

    CARAPI_(void) DoFinish();

    CARAPI_(void) HighlightSelected(
        /* [in] */ Int32 activeTarget);

    CARAPI_(void) HideUnselected(
        /* [in] */ Int32 active);

    CARAPI_(void) HideTargets(
        /* [in] */ Boolean animate,
        /* [in] */ Boolean expanded);

    CARAPI_(void) ShowTargets(
        /* [in] */ Boolean animate);

    CARAPI_(void) Vibrate();

    CARAPI_(AutoPtr<IArrayList>) LoadDrawableArray(
        /* [in] */ Int32 resourceId);

    CARAPI_(void) InternalSetTargetResources(
        /* [in] */ Int32 resourceId);

    CARAPI_(void) StopAndHideWaveAnimation();

    CARAPI_(void) StartWaveAnimation();

    CARAPI_(void) StartBackgroundAnimation(
        /* [in] */ Int32 duration,
        /* [in] */ Float alpha);

    CARAPI_(void) UpdateGlowPosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) HandleDown(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) HandleUp(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) HandleCancel(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) HandleMove(
        /* [in] */ IMotionEvent* event);

    /**
     * Sets the current grabbed state, and dispatches a grabbed state change
     * event to our listener.
     */
    CARAPI_(void) SetGrabbedState(
        /* [in] */ Int32 newState);

    CARAPI_(Boolean) TrySwitchToFirstTouchState(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) AssignDefaultsIfNeeded();

    CARAPI_(void) ComputeInsets(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Given the desired width and height of the ring and the allocated width and height, compute
     * how much we need to scale the ring.
     */
    CARAPI_(Float) ComputeScaleFactor(
        /* [in] */ Int32 desiredWidth,
        /* [in] */ Int32 desiredHeight,
        /* [in] */ Int32 actualWidth,
        /* [in] */ Int32 actualHeight);

    CARAPI_(Float) GetRingWidth();

    CARAPI_(Float) GetRingHeight();

    CARAPI_(void) UpdateTargetPositions(
        /* [in] */ Float centerX,
        /* [in] */ Float centerY);

    CARAPI_(void) UpdatePointCloudPosition(
        /* [in] */ Float centerX,
        /* [in] */ Float centerY);

    CARAPI_(Float) Square(
        /* [in] */ Float d);

    CARAPI_(Float) Dist2(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI_(Float) GetScaledGlowRadiusSquared();

    CARAPI_(void) AnnounceTargets();

    CARAPI_(String) GetTargetDescription(
        /* [in] */ Int32 index);

    CARAPI_(String) GetDirectionDescription(
        /* [in] */ Int32 index);

    CARAPI_(AutoPtr<IArrayList>) LoadDescriptions(
        /* [in] */ Int32 resourceId);

    CARAPI_(Boolean) ReplaceTargetDrawables(
        /* [in] */ IResources* res,
        /* [in] */ Int32 existingResourceId,
        /* [in] */ Int32 newResourceId);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // Wave state machine
    static const Int32 STATE_IDLE;
    static const Int32 STATE_START;
    static const Int32 STATE_FIRST_TOUCH;
    static const Int32 STATE_TRACKING;
    static const Int32 STATE_SNAP;
    static const Int32 STATE_FINISH;

    // Animation properties.
    static const Float SNAP_MARGIN_DEFAULT; // distance to ring before we snap to it

    // Tuneable parameters for animation
    static const Int32 WAVE_ANIMATION_DURATION;
    static const Int32 RETURN_TO_HOME_DELAY;
    static const Int32 RETURN_TO_HOME_DURATION;
    static const Int32 HIDE_ANIMATION_DELAY;
    static const Int32 HIDE_ANIMATION_DURATION;
    static const Int32 SHOW_ANIMATION_DURATION;
    static const Int32 SHOW_ANIMATION_DELAY;
    static const Int32 INITIAL_SHOW_HANDLE_DURATION;
    static const Int32 REVEAL_GLOW_DELAY;
    static const Int32 REVEAL_GLOW_DURATION;

    static const Float TAP_RADIUS_SCALE_ACCESSIBILITY_ENABLED;
    static const Float TARGET_SCALE_EXPANDED;
    static const Float TARGET_SCALE_COLLAPSED;
    static const Float RING_SCALE_EXPANDED;
    static const Float RING_SCALE_COLLAPSED;

    AutoPtr<IArrayList> mTargetDrawables;
    AutoPtr<AnimationBundle> mWaveAnimations;
    AutoPtr<AnimationBundle> mTargetAnimations;
    AutoPtr<AnimationBundle> mGlowAnimations;
    AutoPtr<IArrayList> mTargetDescriptions;
    AutoPtr<IArrayList> mDirectionDescriptions;
    AutoPtr<IGlowPadViewOnTriggerListener> mOnTriggerListener;
    AutoPtr<TargetDrawable> mHandleDrawable;
    AutoPtr<TargetDrawable> mOuterRing;
    AutoPtr<IVibrator> mVibrator;

    Int32 mFeedbackCount;
    Int32 mVibrationDuration;
    Int32 mGrabbedState;
    Int32 mActiveTarget;
    Float mGlowRadius;
    Float mWaveCenterX;
    Float mWaveCenterY;
    Int32 mMaxTargetHeight;
    Int32 mMaxTargetWidth;
    Float mRingScaleFactor;
    Boolean mAllowScaling;

    Float mOuterRadius;
    Float mSnapMargin;
    Boolean mDragging;
    Int32 mNewTargetResources;

    AutoPtr<IAnimatorListener> mResetListener;

    AutoPtr<IAnimatorListener> mResetListenerWithPing;

    AutoPtr<IAnimatorUpdateListener> mUpdateListener;

    Boolean mAnimatingTargets;
    AutoPtr<IAnimatorListener> mTargetUpdateListener;
    Int32 mTargetResourceId;
    Int32 mTargetDescriptionsResourceId;
    Int32 mDirectionDescriptionsResourceId;
    Boolean mAlwaysTrackFinger;
    Int32 mHorizontalInset;
    Int32 mVerticalInset;
    Int32 mGravity;
    Boolean mInitialLayout;
    AutoPtr<Tweener> mBackgroundAnimator;
    AutoPtr<PointCloud> mPointCloud;
    Float mInnerRadius;
    Int32 mPointerId;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_GLOWPADVIEW_H__
