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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_WAVEVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_WAVEVIEW_H__

#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

using Elastos::Droid::View::View;

/**
 * A special widget containing a center and outer ring. Moving the center ring to the outer ring
 * causes an event that can be caught by implementing OnTriggerListener.
 */
class WaveView
    : public View
{
private:
    class InnerListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ WaveView* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        WaveView* mHost;
    };

    class InnerLockTimerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerLockTimerRunnable(
            /* [in] */ WaveView* host);

        CARAPI Run();

    private:
        WaveView* mHost;
    };

    class InnerAddWaveRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAddWaveRunnable(
            /* [in] */ WaveView* host);

        CARAPI Run();

    private:
        WaveView* mHost;
    };

public:

    WaveView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Registers a callback to be invoked when the user triggers an event.
     *
     * @param listener the OnDialTriggerListener to attach to this view
     */
    virtual CARAPI SetOnTriggerListener(
        /* [in] */ IWaveViewOnTriggerListener* listener);

    virtual CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    virtual CARAPI Reset();

protected:
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);


    CARAPI_(Int32) GetSuggestedMinimumWidth();


    CARAPI_(Int32) GetSuggestedMinimumHeight();


    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);


    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) InitDrawables();

    CARAPI_(void) WaveUpdateFrame(
        /* [in] */ Float mouseX,
        /* [in] */ Float mouseY,
        /* [in] */ Boolean fingerDown);

    CARAPI_(AutoPtr<IBitmapDrawable>) CreateDrawable(
        /* [in] */ Int32 resId);

private:
    /**
     * Tries to transition to start attempt state.
     *
     * @param event A motion event.
     */
    CARAPI_(void) TryTransitionToStartAttemptState(
        /* [in] */ IMotionEvent* event);

    /**
     * @return The radius in which the handle is grabbed scaled based on
     *     whether accessibility is enabled.
     */
    CARAPI_(Float) GetScaledGrabHandleRadius();

    /**
     * Announces the unlock handle if accessibility is enabled.
     */
    CARAPI_(void) AnnounceUnlockHandle();

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

    static CARAPI_(AutoPtr<IAudioAttributes>) MiddleInitVibrationAttributes();

private:
    static const String TAG;
    static const Boolean DBG = FALSE;
    static const Int32 WAVE_COUNT = 20; // default wave count
    static const Int64 VIBRATE_SHORT = 20;  // msec
    static const Int64 VIBRATE_LONG = 20;  // msec

    // Lock state machine states
    static const Int32 STATE_RESET_LOCK = 0;
    static const Int32 STATE_READY = 1;
    static const Int32 STATE_START_ATTEMPT = 2;
    static const Int32 STATE_ATTEMPTING = 3;
    static const Int32 STATE_UNLOCK_ATTEMPT = 4;
    static const Int32 STATE_UNLOCK_SUCCESS = 5;

    // Animation properties.
    static const Int64 DURATION = 300; // duration of transitional animations
    static const Int64 FINAL_DURATION = 200; // duration of final animations when unlocking
    static const Int64 RING_DELAY = 1300; // when to start fading animated rings
    static const Int64 FINAL_DELAY = 200; // delay for unlock success animation
    static const Int64 SHORT_DELAY = 100; // for starting one animation after another.
    static const Int64 WAVE_DURATION = 2000; // amount of time for way to expand/decay
    static const Int64 RESET_TIMEOUT = 3000; // elapsed time of inactivity before we reset
    static const Int64 DELAY_INCREMENT = 15; // increment per wave while tracking motion
    static const Int64 DELAY_INCREMENT2 = 12; // increment per wave while not tracking
    static const Int64 WAVE_DELAY = WAVE_DURATION / WAVE_COUNT; // initial propagation delay

    /**
     * The scale by which to multiply the unlock handle width to compute the radius
     * in which it can be grabbed when accessibility is disabled.
     */
    static constexpr Float GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_DISABLED = 0.5f;

    /**
     * The scale by which to multiply the unlock handle width to compute the radius
     * in which it can be grabbed when accessibility is enabled (more generous).
     */
    static constexpr Float GRAB_HANDLE_RADIUS_SCALE_ACCESSIBILITY_ENABLED = 1.0f;

    static const AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

private:
    AutoPtr<IRunnable> mLockTimerActions;
    AutoPtr<IRunnable> mAddWaveAction;
    AutoPtr<IVibrator> mVibrator;
    AutoPtr<IWaveViewOnTriggerListener> mOnTriggerListener;
    List<AutoPtr<IDrawableHolder> > mDrawables;// = new ArrayList<DrawableHolder>(3);
    List<AutoPtr<IDrawableHolder> >  mLightWaves;// = new ArrayList<DrawableHolder>(WAVE_COUNT);
    Boolean mFingerDown;// = false;
    Float mRingRadius;// = 182.0f; // Radius of bitmap ring. Used to snap halo to it
    Int32 mSnapRadius;// = 136; // minimum threshold for drag unlock
    Int32 mWaveCount;// = WAVE_COUNT;  // number of waves
    Int64 mWaveTimerDelay;// = WAVE_DELAY;
    Int32 mCurrentWave;// = 0;
    Float mLockCenterX; // center of widget as dictated by widget size
    Float mLockCenterY;
    Float mMouseX; // current mouse position as of last touch event
    Float mMouseY;
    AutoPtr<IDrawableHolder> mUnlockRing;
    AutoPtr<IDrawableHolder> mUnlockDefault;
    AutoPtr<IDrawableHolder> mUnlockHalo;
    Int32 mLockState;
    Int32 mGrabbedState;
    Boolean mWavesRunning;
    Boolean mFinishWaves;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_WAVEVIEW_H__
