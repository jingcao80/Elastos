#ifndef __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYPOWERCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYPOWERCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Animation.h>
#include <Elastos.Droid.View.h>
#include "elastos/droid/server/display/DisplayPowerState.h"
#include "elastos/droid/server/display/CDisplayPowerState.h"
#include "elastos/droid/server/display/AutomaticBrightnessController.h"
#include "elastos/droid/server/display/RampAnimator.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Server::Lights::ILightsManager;

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::Display::IDisplayPowerCallbacks;
using Elastos::Droid::Hardware::Display::IDisplayPowerRequest;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::ISpline;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IScreenOnListener;
using Elastos::Droid::View::IWindowManagerPolicy;

using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Controls the power state of the display.
 *
 * Handles the proximity sensor, light sensor, and animations between states
 * including the screen off animation.
 *
 * This component acts independently of the rest of the power manager service.
 * In particular, it does not share any state and it only communicates
 * via asynchronous callbacks to inform the power manager that something has
 * changed.
 *
 * Everything this class does internally is serialized on its handler although
 * it may be accessed by other threads from the outside.
 *
 * Note that the power manager service guarantees that it will hold a suspend
 * blocker as Int64 as the display is not ready.  So most of the work done here
 * does not need to worry about holding a suspend blocker unless it happens
 * independently of the display ready signal.
   *
 * For debugging, you can make the color fade and brightness animations run
 * slower by changing the "animator duration scale" option in Development Settings.
 */
class DisplayPowerController
    : public Object
    , public IAutomaticBrightnessControllerCallbacks
{
private:
    class DisplayControllerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("DisplayControllerHandler")

        DisplayControllerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        DisplayPowerController* mHost;
    };

    class SensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        SensorEventListener(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        //@Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);
    private:
        DisplayPowerController* mHost;
    };

    class ScreenOnUnblocker
        : public Object
        , public IScreenOnListener
    {
    public:
        CAR_INTERFACE_DECL()

        ScreenOnUnblocker(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI OnScreenOn();
    private:
        DisplayPowerController* mHost;
    };

    class AnimatorListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorListener(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);
    private:
        DisplayPowerController* mHost;
    };

    class RampAnimatorListener
        : public Object
        , public IRampAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        RampAnimatorListener(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI OnAnimationEnd();
    private:
        DisplayPowerController* mHost;
    };

    class CleanListener
        : public Runnable
    {
    public:
        CleanListener(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI Run();
    private:
        DisplayPowerController* mHost;
    };

    class OnStateChangedRunnable
        : public Runnable
    {
    public:
        OnStateChangedRunnable(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI Run();
    private:
        DisplayPowerController* mHost;
    };

    class OnProximityPositiveRunnable
        : public Runnable
    {
    public:
        OnProximityPositiveRunnable(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI Run();
    private:
        DisplayPowerController* mHost;
    };

    class OnProximityNegativeRunnable
        : public Runnable
    {
    public:
        OnProximityNegativeRunnable(
            /* [in] */ DisplayPowerController* host);

        //@Override
        CARAPI Run();
    private:
        DisplayPowerController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DisplayPowerController();

    /**
     * Creates the display power controller.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDisplayPowerCallbacks* callbacks,
        /* [in] */ IHandler* handler,
        /* [in] */ ISensorManager* sensorManager,
        /* [in] */ IDisplayBlanker* blanker);

    /**
     * Returns TRUE if the proximity sensor screen-off function is available.
     */
    Boolean IsProximitySensorAvailable();

    /**
     * Requests a new power state.
     * The controller makes a copy of the provided object and then
     * begins adjusting the power state to match what was requested.
     *
     * @param request The requested power state.
     * @param waitForNegativeProximity If TRUE, issues a request to wait for
     * negative proximity before turning the screen back on, assuming the screen
     * was turned off by the proximity sensor.
     * @return True if display is ready, FALSE if there are important changes that must
     * be made asynchronously (such as turning the screen on), in which case the caller
     * should grab a wake lock, watch for {@link DisplayPowerCallbacks#onStateChanged()}
     * then try the request again later until the state converges.
     */
    Boolean RequestPowerState(
        /* [in] */ IDisplayPowerRequest* request,
        /* [in] */ Boolean waitForNegativeProximity);

    //@Override
    CARAPI UpdateBrightness();

    void Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI SendUpdatePowerState();

    CARAPI SendUpdatePowerStateLocked();

    void Initialize();

    void UpdatePowerState();

    void BlockScreenOn();

    void UnBlockScreenOn();

    Boolean SetScreenState(
        /* [in] */ Int32 state);

    Int32 ClampScreenBrightness(
        /* [in] */ Int32 value);

    void AnimateScreenBrightness(
        /* [in] */ Int32 target,
        /* [in] */ Int32 rate);

    void AnimateScreenStateChange(
        /* [in] */ Int32 target,
        /* [in] */ Boolean performScreenOffTransition);

    void SetProximitySensorEnabled(
        /* [in] */ Boolean enable);

    void HandleProximitySensorEvent(
        /* [in] */ Int64 time,
        /* [in] */ Boolean positive);

    void DebounceProximitySensor();

    void ClearPendingProximityDebounceTime();

    void SetPendingProximityDebounceTime(
        /* [in] */ Int64 debounceTime);

    void SendOnStateChangedWithWakelock();

    void SendOnProximityPositiveWithWakelock();

    void SendOnProximityNegativeWithWakelock();

    void DumpLocal(
        /* [in] */ IPrintWriter* pw);

    static String ProximityToString(
        /* [in] */ Int32 state);

    static AutoPtr<ISpline> CreateAutoBrightnessSpline(
        /* [in] */ ArrayOf<Int32>* lux,
        /* [in] */ ArrayOf<Int32>* brightness);

    static Float NormalizeAbsoluteBrightness(
        /* [in] */ Int32 value);

    static Int32 ClampAbsoluteBrightness(
        /* [in] */ Int32 value);

private:
    static const String TAG;

    static Boolean DEBUG;
    static const Boolean DEBUG_PRETEND_PROXIMITY_SENSOR_ABSENT;

    static const String SCREEN_ON_BLOCKED_TRACE_NAME;

    // If TRUE, uses the color fade on animation.
    // We might want to turn this off if we cannot get a guarantee that the screen
    // actually turns on and starts showing new content after the call to set the
    // screen state returns.  Playing the animation can also be somewhat slow.
    static const Boolean USE_COLOR_FADE_ON_ANIMATION;

    // The minimum reduction in brightness when dimmed.
    static const Int32 SCREEN_DIM_MINIMUM_REDUCTION;

    static const Int32 COLOR_FADE_ON_ANIMATION_DURATION_MILLIS;
    static const Int32 COLOR_FADE_OFF_ANIMATION_DURATION_MILLIS;

    static const Int32 MSG_UPDATE_POWER_STATE;
    static const Int32 MSG_PROXIMITY_SENSOR_DEBOUNCED;
    static const Int32 MSG_SCREEN_ON_UNBLOCKED;

    static const Int32 PROXIMITY_UNKNOWN;
    static const Int32 PROXIMITY_NEGATIVE;
    static const Int32 PROXIMITY_POSITIVE;

    // Proximity sensor debounce delay in milliseconds for positive or negative transitions.
    static const Int32 PROXIMITY_SENSOR_POSITIVE_DEBOUNCE_DELAY;
    static const Int32 PROXIMITY_SENSOR_NEGATIVE_DEBOUNCE_DELAY;

    // Trigger proximity if distance is less than 5 cm.
    static const Float TYPICAL_PROXIMITY_THRESHOLD;

    // Brightness animation ramp rate in brightness units per second.
    static const Int32 BRIGHTNESS_RAMP_RATE_FAST;
    static const Int32 BRIGHTNESS_RAMP_RATE_SLOW;

    Object mLock;

    AutoPtr<IContext> mContext;

    // Our handler.
    AutoPtr<DisplayControllerHandler> mHandler;

    // Asynchronous callbacks into the power manager service.
    // Only invoked from the handler thread while no locks are held.
    AutoPtr<IDisplayPowerCallbacks> mCallbacks;

    // Battery stats.
    AutoPtr<IIBatteryStats> mBatteryStats;

    // The lights service.
    AutoPtr<ILightsManager> mLights;

    // The sensor manager.
    AutoPtr<ISensorManager> mSensorManager;

    // The window manager policy.
    AutoPtr<IWindowManagerPolicy> mWindowManagerPolicy;

    // The display blanker.
    AutoPtr<IDisplayBlanker> mBlanker;

    // The proximity sensor, or NULL if not available or needed.
    AutoPtr<ISensor> mProximitySensor;

    // The doze screen brightness.
    Int32 mScreenBrightnessDozeConfig;

    // The dim screen brightness.
    Int32 mScreenBrightnessDimConfig;

    // The minimum screen brightness to use in a very dark room.
    Int32 mScreenBrightnessDarkConfig;

    // The minimum allowed brightness.
    Int32 mScreenBrightnessRangeMinimum;

    // The maximum allowed brightness.
    Int32 mScreenBrightnessRangeMaximum;

    // True if auto-brightness should be used.
    Boolean mUseSoftwareAutoBrightnessConfig;

    // True if we should fade the screen while turning it off, FALSE if we should play
    // a stylish color fade animation instead.
    Boolean mColorFadeFadesConfig;

    // The pending power request.
    // Initially NULL until the first call to RequestPowerState.
    // Guarded by mLock.
    AutoPtr<IDisplayPowerRequest> mPendingRequestLocked;

    // True if a request has been made to wait for the proximity sensor to go negative.
    // Guarded by mLock.
    Boolean mPendingWaitForNegativeProximityLocked;

    // True if the pending power request or wait for negative proximity flag
    // has been changed since the last update occurred.
    // Guarded by mLock.
    Boolean mPendingRequestChangedLocked;

    // Set to TRUE when the important parts of the pending power request have been applied.
    // The important parts are mainly the screen state.  Brightness changes may occur
    // concurrently.
    // Guarded by mLock.
    Boolean mDisplayReadyLocked;

    // Set to TRUE if a power state update is required.
    // Guarded by mLock.
    Boolean mPendingUpdatePowerStateLocked;

    /* The following state must only be accessed by the handler thread. */

    // The currently requested power state.
    // The power controller will progressively update its internal state to match
    // the requested power state.  Initially NULL until the first update.
    AutoPtr<IDisplayPowerRequest> mPowerRequest;

    // The current power state.
    // Must only be accessed on the handler thread.
    AutoPtr<DisplayPowerState> mPowerState;

    // True if the device should wait for negative proximity sensor before
    // waking up the screen.  This is set to FALSE as soon as a negative
    // proximity sensor measurement is observed or when the device is forced to
    // go to sleep by the user.  While TRUE, the screen remains off.
    Boolean mWaitingForNegativeProximity;

    // The actual proximity sensor threshold value.
    Float mProximityThreshold;

    // Set to TRUE if the proximity sensor listener has been registered
    // with the sensor manager.
    Boolean mProximitySensorEnabled;

    // The debounced proximity sensor state.
    Int32 mProximity;

    // The raw non-debounced proximity sensor state.
    Int32 mPendingProximity;
    Int64 mPendingProximityDebounceTime; // -1 if fully debounced

    // True if the screen was turned off because of the proximity sensor.
    // When the screen turns on again, we report user activity to the power manager.
    Boolean mScreenOffBecauseOfProximity;

    // The currently active screen on unblocker.  This field is non-NULL whenever
    // we are waiting for a callback to release it and unblock the screen.
    AutoPtr<ScreenOnUnblocker> mPendingScreenOnUnblocker;

    // True if we were in the process of turning off the screen.
    // This allows us to recover more gracefully from situations where we abort
    // turning off the screen.
    Boolean mPendingScreenOff;

    // True if we have unfinished business and are holding a suspend blocker.
    Boolean mUnfinishedBusiness;

    // The elapsed real time when the screen on was blocked.
    Int64 mScreenOnBlockStartRealTime;

    // Remembers whether certain kinds of brightness adjustments
    // were recently applied so that we can decide how to transition.
    Boolean mAppliedAutoBrightness;
    Boolean mAppliedDimming;
    Boolean mAppliedLowPower;

    // The controller for the automatic brightness level.
    AutoPtr<AutomaticBrightnessController> mAutomaticBrightnessController;

    // The controller for LiveDisplay
    AutoPtr<LiveDisplayController> mLiveDisplayController;

    // Animators.
    AutoPtr<IObjectAnimator> mColorFadeOnAnimator;
    AutoPtr<IObjectAnimator> mColorFadeOffAnimator;
    AutoPtr<RampAnimator> mScreenBrightnessRampAnimator;

    AutoPtr<ISensorEventListener> mProximitySensorListener;
    AutoPtr<IAnimatorListener> mAnimatorListener;
    AutoPtr<IRampAnimatorListener> mRampAnimatorListener;
    AutoPtr<IRunnable> mCleanListener;
    AutoPtr<IRunnable> mOnStateChangedRunnable;
    AutoPtr<IRunnable> mOnProximityPositiveRunnable;
    AutoPtr<IRunnable> mOnProximityNegativeRunnable;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYPOWERCONTROLLER_H__
