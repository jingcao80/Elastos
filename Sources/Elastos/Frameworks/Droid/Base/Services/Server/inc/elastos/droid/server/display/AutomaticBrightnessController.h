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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_AUTOMATIC_BRIGHTNESS_CONTROLLER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_AUTOMATIC_BRIGHTNESS_CONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/server/display/LiveDisplayController.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Utility::ISpline;
using Elastos::Droid::Server::Twilight::ITwilightListener;
using Elastos::Droid::Server::Twilight::ITwilightManager;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

class AutomaticBrightnessController
    : public Object
{
private:
    class AutomaticBrightnessHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AutomaticBrightnessController::AutomaticBrightnessHandler")

        AutomaticBrightnessHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AutomaticBrightnessController* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutomaticBrightnessController* mHost;
    };

    class MySensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySensorEventListener(
            /* [in] */ AutomaticBrightnessController* host);

        //@Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        //@Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);
    private:
        AutomaticBrightnessController* mHost;
    };

    class MyTwilightListener
        : public Object
        , public ITwilightListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyTwilightListener(
            /* [in] */ AutomaticBrightnessController* host);

        //@Override
        CARAPI OnTwilightStateChanged();
    private:
        AutomaticBrightnessController* mHost;
    };

    class AmbientLightRingBuffer
        : public Object
    {
    public:
        AmbientLightRingBuffer();

        AmbientLightRingBuffer(
            /* [in] */ Int32 initialCapacity);

        Float GetLux(
            /* [in] */ Int32 index);

        Int64 GetTime(
            /* [in] */ Int32 index);

        void Push(
            /* [in] */ Int64 time,
            /* [in] */ Float lux);

        void Prune(
            /* [in] */ Int64 horizon);

        Int32 Size();

        Boolean IsEmpty();

        void Clear();

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CARAPI OffsetOf(
            /* [in] */ Int32 index,
            /* [out] */ Int32* offset);

    private:
        // Proportional extra capacity of the buffer beyond the expected number of light samples
        // in the horizon
        static const Float BUFFER_SLACK;
        static const Int32 DEFAULT_CAPACITY;
        AutoPtr<ArrayOf<Float> > mRingLux;
        AutoPtr<ArrayOf<Int64> > mRingTime;
        Int32 mCapacity;

        // The first valid element and the next open slot.
        // Note that if mCount is zero then there are no valid elements.
        Int32 mStart;
        Int32 mEnd;
        Int32 mCount;
    };

public:
    AutomaticBrightnessController(
        /* [in] */ IContext* ctx,
        /* [in] */ IAutomaticBrightnessControllerCallbacks* callbacks,
        /* [in] */ ILooper* looper,
        /* [in] */ ISensorManager* sensorManager,
        /* [in] */ ISpline* autoBrightnessSpline,
        /* [in] */ Int32 lightSensorWarmUpTime,
        /* [in] */ Int32 brightnessMin,
        /* [in] */ Int32 brightnessMax,
        /* [in] */ LiveDisplayController* ldc);

    Int32 GetAutomaticScreenBrightness();

    void Configure(
        /* [in] */ Boolean enable,
        /* [in] */ Float adjustment);

    void Dump(
        /* [in] */ IPrintWriter* pw);

private:

    Boolean SetLightSensorEnabled(
        /* [in] */ Boolean enable);

    void HandleLightSensorEvent(
        /* [in] */ Int64 time,
        /* [in] */ Float lux);

    void ApplyLightSensorMeasurement(
        /* [in] */ Int64 time,
        /* [in] */ Float lux);

    Boolean SetScreenAutoBrightnessAdjustment(
        /* [in] */ Float adjustment);

    void SetAmbientLux(
        /* [in] */ Float lux);

    Float CalculateAmbientLux(
        /* [in] */ Int64 now);

    static Float CalculateWeight(
        /* [in] */ Int64 startDelta,
        /* [in] */ Int64 endDelta);

    // Evaluates the integral of y = x + WEIGHTING_INTERCEPT. This is always positive for the
    // horizon we're looking at and provides a non-linear weighting for light samples.
    static Float WeightIntegral(
        /* [in] */ Int64 x);

    Int64 NextAmbientLightBrighteningTransition(
        /* [in] */ Int64 time);

    Int64 NextAmbientLightDarkeningTransition(
        /* [in] */ Int64 time);

    void UpdateAmbientLux();

    void UpdateAmbientLux(
        /* [in] */ Int64 time);

    void UpdateAutoBrightness(
        /* [in] */ Boolean sendUpdate);

    Int32 ClampScreenBrightness(
        /* [in] */ Int32 value);

    static Float GetTwilightGamma(
        /* [in] */ Int64 now,
        /* [in] */ Int64 lastSunset,
        /* [in] */ Int64 nextSunrise);

private:

    static const String TAG;

    static const Boolean DEBUG;
    static const Boolean DEBUG_PRETEND_LIGHT_SENSOR_ABSENT;

    // If true, enables the use of the screen auto-brightness adjustment setting.
    static const Boolean USE_SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT;

    // The maximum range of gamma adjustment possible using the screen
    // auto-brightness adjustment setting.
    static const Float SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT_MAX_GAMMA;

    // Light sensor event rate in milliseconds.
    static const Int32 LIGHT_SENSOR_RATE_MILLIS;

    // Period of time in which to consider light samples in milliseconds.
    static const Int32 AMBIENT_LIGHT_HORIZON;

    // Stability requirements in milliseconds for accepting a new brightness level.  This is used
    // for debouncing the light sensor.  Different constants are used to debounce the light sensor
    // when adapting to brighter or darker environments.  This parameter controls how quickly
    // brightness changes occur in response to an observed change in light level that exceeds the
    // hysteresis threshold.
    static const Int64 BRIGHTENING_LIGHT_DEBOUNCE;
    static const Int64 DARKENING_LIGHT_DEBOUNCE;

    // Hysteresis constraints for brightening or darkening.
    // The recent lux must have changed by at least this fraction relative to the
    // current ambient lux before a change will be considered.
    static const Float BRIGHTENING_LIGHT_HYSTERESIS;
    static const Float DARKENING_LIGHT_HYSTERESIS;

    // The intercept used for the weighting calculation. This is used in order to keep all possible
    // weighting values positive.
    static const Int32 WEIGHTING_INTERCEPT;

    // How Int64 the current sensor reading is assumed to be valid beyond the current time.
    // This provides a bit of prediction, as well as ensures that the weight for the last sample is
    // non-zero, which in turn ensures that the total weight is non-zero.
    static const Int64 AMBIENT_LIGHT_PREDICTION_TIME_MILLIS;

    // If true, enables the use of the current time as an auto-brightness adjustment.
    // The basic idea here is to expand the dynamic range of auto-brightness
    // when it is especially dark outside.  The light sensor tends to perform
    // poorly at low light levels so we compensate for it by making an
    // assumption about the environment.
    static const Boolean USE_TWILIGHT_ADJUSTMENT;

    // Specifies the maximum magnitude of the time of day adjustment.
    static const Float TWILIGHT_ADJUSTMENT_MAX_GAMMA;

    // The amount of time after or before sunrise over which to start adjusting
    // the gamma.  We want the change to happen gradually so that it is below the
    // threshold of perceptibility and so that the adjustment has maximum effect
    // well after dusk.
    static const Int64 TWILIGHT_ADJUSTMENT_TIME;

    static const Int32 MSG_UPDATE_AMBIENT_LUX;

    // Stability requirements in milliseconds for accepting a new brightness level.  This is used
    // for debouncing the light sensor.  Different constants are used to debounce the light sensor
    // when adapting to brighter or darker environments.  This parameter controls how quickly
    // brightness changes occur in response to an observed change in light level that exceeds the
    // hysteresis threshold.
    Int64 mBrighteningLightDebounceConfig;
    Int64 mDarkeningLightDebounceConfig;

    // Callbacks for requesting updates to the the display's power state
    AutoPtr<IAutomaticBrightnessControllerCallbacks> mCallbacks;

    // The sensor manager.
    AutoPtr<ISensorManager> mSensorManager;

    // The light sensor, or null if not available or needed.
    AutoPtr<ISensor> mLightSensor;

    // The twilight service.
    AutoPtr<ITwilightManager> mTwilight;

    // The auto-brightness spline adjustment.
    // The brightness values have been scaled to a range of 0..1.
    AutoPtr<ISpline> mScreenAutoBrightnessSpline;

    // The minimum and maximum screen brightnesses.
    Int32 mScreenBrightnessRangeMinimum;
    Int32 mScreenBrightnessRangeMaximum;

    // Amount of time to delay auto-brightness after screen on while waiting for
    // the light sensor to warm-up in milliseconds.
    // May be 0 if no warm-up is required.
    Int32 mLightSensorWarmUpTimeConfig;

    // Set to true if the light sensor is enabled.
    Boolean mLightSensorEnabled;

    // The time when the light sensor was enabled.
    Int64 mLightSensorEnableTime;

    // The currently accepted nominal ambient light level.
    Float mAmbientLux;

    // True if mAmbientLux holds a valid value.
    Boolean mAmbientLuxValid;

    // The ambient light level threshold at which to brighten or darken the screen.
    Float mBrighteningLuxThreshold;
    Float mDarkeningLuxThreshold;

    // The most recent light sample.
    Float mLastObservedLux;

    // The time of the most light recent sample.
    Int64 mLastObservedLuxTime;

    // The number of light samples collected since the light sensor was enabled.
    Int32 mRecentLightSamples;

    // A ring buffer containing all of the recent ambient light sensor readings.
    AutoPtr<AmbientLightRingBuffer> mAmbientLightRingBuffer;

    // The handler
    AutoPtr<AutomaticBrightnessHandler> mHandler;

    // The screen brightness level that has been chosen by the auto-brightness
    // algorithm.  The actual brightness should ramp towards this value.
    // We preserve this value even when we stop using the light sensor so
    // that we can quickly revert to the previous auto-brightness level
    // while the light sensor warms up.
    // Use -1 if there is no current auto-brightness value available.
    Int32 mScreenAutoBrightness;

    // The screen auto-brightness adjustment factor in the range -1 (dimmer) to 1 (brighter)
    Float mScreenAutoBrightnessAdjustment;

    // The last screen auto-brightness gamma.  (For printing in dump() only.)
    Float mLastScreenAutoBrightnessGamma;

    // Night mode color temperature adjustments
    AutoPtr<LiveDisplayController> mLiveDisplay;

    AutoPtr<IContext> mContext;

    AutoPtr<ISensorEventListener> mLightSensorListener;
    AutoPtr<ITwilightListener> mTwilightListener;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_AUTOMATIC_BRIGHTNESS_CONTROLLER_H__
