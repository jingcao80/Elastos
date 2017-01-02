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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WINDOWORIENTATIONLISTENER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WINDOWORIENTATIONLISTENER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IPrintWriter;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * A special helper class used by the WindowManager
  * for receiving notifications from the SensorManager when
  * the orientation of the device has changed.
  *
  * NOTE: If changing anything here, please run the API demo
  * "App/Activity/Screen Orientation" to ensure that all orientation
  * modes still work correctly.
  *
  * You can also visualize the behavior of the WindowOrientationListener.
  * Refer to frameworks/base/tools/orientationplot/README.txt for details.
  *
  * @hide
  */
class WindowOrientationListener
    : public Object
    , public IWindowOrientationListener
{
public:
    /**
      * This class filters the raw accelerometer data and tries to detect actual changes in
      * orientation. This is a very ill-defined problem so there are a lot of tweakable parameters,
      * but here's the outline:
      *
      *  - Low-pass filter the accelerometer vector in cartesian coordinates.  We do it in
      *    cartesian space because the orientation calculations are sensitive to the
      *    absolute magnitude of the acceleration.  In particular, there are singularities
      *    in the calculation as the magnitude approaches 0.  By performing the low-pass
      *    filtering early, we can eliminate most spurious high-frequency impulses due to noise.
      *
      *  - Convert the acceleromter vector from cartesian to spherical coordinates.
      *    Since we're dealing with rotation of the device, this is the sensible coordinate
      *    system to work in.  The zenith direction is the Z-axis, the direction the screen
      *    is facing.  The radial distance is referred to as the magnitude below.
      *    The elevation angle is referred to as the "tilt" below.
      *    The azimuth angle is referred to as the "orientation" below (and the azimuth axis is
      *    the Y-axis).
      *    See http://en.wikipedia.org/wiki/Spherical_coordinate_system for reference.
      *
      *  - If the tilt angle is too close to horizontal (near 90 or -90 degrees), do nothing.
      *    The orientation angle is not meaningful when the device is nearly horizontal.
      *    The tilt angle thresholds are set differently for each orientation and different
      *    limits are applied when the device is facing down as opposed to when it is facing
      *    forward or facing up.
      *
      *  - When the orientation angle reaches a certain threshold, consider transitioning
      *    to the corresponding orientation.  These thresholds have some hysteresis built-in
      *    to avoid oscillations between adjacent orientations.
      *
      *  - Wait for the device to settle for a little bit.  Once that happens, issue the
      *    new orientation proposal.
      *
      * Details are explained inline.
      *
      * See http://en.wikipedia.org/wiki/Low-pass_filter#Discrete-time_realization for
      * signal processing background.
      */
    class SensorEventListenerImpl
        : public Object
        , public ISensorEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        SensorEventListenerImpl(
            /* [in] */ WindowOrientationListener* owner);

        virtual CARAPI GetProposedRotationLocked(
            /* [out] */ Int32* result);

        virtual CARAPI DumpLocked(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& prefix);

        // @Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

        // @Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        CARAPI_(void) ResetLocked();

    private:
        //CARAPI_(AutoPtr< ArrayOf< AutoPtr< ArrayOf<Int32> > > >) MiddleInitTiltTolerance();

        //CARAPI_(AutoPtr< ArrayOf<Float> >) MiddleInitMtilthistory();

        //CARAPI_(AutoPtr< ArrayOf<Int64> >) MiddleInitMtilthistorytimestampnanos();

        /**
          * Returns true if the tilt angle is acceptable for a given predicted rotation.
          */
        CARAPI_(Boolean) IsTiltAngleAcceptableLocked(
            /* [in] */ Int32 rotation,
            /* [in] */ Int32 tiltAngle);

        /**
          * Returns true if the orientation angle is acceptable for a given predicted rotation.
          *
          * This function takes into account the gap between adjacent orientations
          * for hysteresis.
          */
        CARAPI_(Boolean) IsOrientationAngleAcceptableLocked(
            /* [in] */ Int32 rotation,
            /* [in] */ Int32 orientationAngle);

        /**
          * Returns true if the predicted rotation is ready to be advertised as a
          * proposed rotation.
          */
        CARAPI_(Boolean) IsPredictedRotationAcceptableLocked(
            /* [in] */ Int64 now);

        CARAPI_(void) ClearPredictedRotationLocked();

        CARAPI_(void) UpdatePredictedRotationLocked(
            /* [in] */ Int64 now,
            /* [in] */ Int32 rotation);

        CARAPI_(Boolean) IsAcceleratingLocked(
            /* [in] */ Float magnitude);

        CARAPI_(void) ClearTiltHistoryLocked();

        CARAPI_(void) AddTiltHistoryEntryLocked(
            /* [in] */ Int64 now,
            /* [in] */ Float tilt);

        CARAPI_(Boolean) IsFlatLocked(
            /* [in] */ Int64 now);

        CARAPI_(Boolean) IsSwingingLocked(
            /* [in] */ Int64 now,
            /* [in] */ Float tilt);

        CARAPI_(Int32) NextTiltHistoryIndexLocked(
            /* [in] */ Int32 index);

        CARAPI_(Float) GetLastTiltLocked();

        CARAPI_(Float) RemainingMS(
            /* [in] */ Int64 now,
            /* [in] */ Int64 until);

    private:
        // We work with all angles in degrees in this class.
        static const Float RADIANS_TO_DEGREES;
        // Number of nanoseconds per millisecond.
        static const Int64 NANOS_PER_MS = 1000000;
        // Indices into SensorEvent.values for the accelerometer sensor.
        static const Int32 ACCELEROMETER_DATA_X = 0;
        static const Int32 ACCELEROMETER_DATA_Y = 1;
        static const Int32 ACCELEROMETER_DATA_Z = 2;
        // The minimum amount of time that a predicted rotation must be stable before it
        // is accepted as a valid rotation proposal.  This value can be quite small because
        // the low-pass filter already suppresses most of the noise so we're really just
        // looking for quick confirmation that the last few samples are in agreement as to
        // the desired orientation.
        static const Int64 PROPOSAL_SETTLE_TIME_NANOS = 40 * NANOS_PER_MS;
        // The minimum amount of time that must have elapsed since the device last exited
        // the flat state (time since it was picked up) before the proposed rotation
        // can change.
        static const Int64 PROPOSAL_MIN_TIME_SINCE_FLAT_ENDED_NANOS = 500 * NANOS_PER_MS;
        // The minimum amount of time that must have elapsed since the device stopped
        // swinging (time since device appeared to be in the process of being put down
        // or put away into a pocket) before the proposed rotation can change.
        static const Int64 PROPOSAL_MIN_TIME_SINCE_SWING_ENDED_NANOS = 300 * NANOS_PER_MS;
        // The minimum amount of time that must have elapsed since the device stopped
        // undergoing external acceleration before the proposed rotation can change.
        static const Int64 PROPOSAL_MIN_TIME_SINCE_ACCELERATION_ENDED_NANOS = 500 * NANOS_PER_MS;
        // If the tilt angle remains greater than the specified angle for a minimum of
        // the specified time, then the device is deemed to be lying flat
        // (just chillin' on a table).
        static const Float FLAT_ANGLE;
        static const Int64 FLAT_TIME_NANOS = 1000 * NANOS_PER_MS;
        // If the tilt angle has increased by at least delta degrees within the specified amount
        // of time, then the device is deemed to be swinging away from the user
        // down towards flat (tilt = 90).
        static const Float SWING_AWAY_ANGLE_DELTA;
        static const Int64 SWING_TIME_NANOS = 300 * NANOS_PER_MS;
        // The maximum sample inter-arrival time in milliseconds.
        // If the acceleration samples are further apart than this amount in time, we reset the
        // state of the low-pass filter and orientation properties.  This helps to handle
        // boundary conditions when the device is turned on, wakes from suspend or there is
        // a significant gap in samples.
        static const Int64 MAX_FILTER_DELTA_TIME_NANOS = 1000 * NANOS_PER_MS;
        // The acceleration filter time constant.
        //
        // This time constant is used to tune the acceleration filter such that
        // impulses and vibrational noise (think car dock) is suppressed before we
        // try to calculate the tilt and orientation angles.
        //
        // The filter time constant is related to the filter cutoff frequency, which is the
        // frequency at which signals are attenuated by 3dB (half the passband power).
        // Each successive octave beyond this frequency is attenuated by an additional 6dB.
        //
        // Given a time constant t in seconds, the filter cutoff frequency Fc in Hertz
        // is given by Fc = 1 / (2pi * t).
        //
        // The higher the time constant, the lower the cutoff frequency, so more noise
        // will be suppressed.
        //
        // Filtering adds latency proportional the time constant (inversely proportional
        // to the cutoff frequency) so we don't want to make the time constant too
        // large or we can lose responsiveness.  Likewise we don't want to make it too
        // small or we do a poor job suppressing acceleration spikes.
        // Empirically, 100ms seems to be too small and 500ms is too large.
        static const Float FILTER_TIME_CONSTANT_MS;
        /* State for orientation detection. */
        // Thresholds for minimum and maximum allowable deviation from gravity.
        //
        // If the device is undergoing external acceleration (being bumped, in a car
        // that is turning around a corner or a plane taking off) then the magnitude
        // may be substantially more or less than gravity.  This can skew our orientation
        // detection by making us think that up is pointed in a different direction.
        //
        // Conversely, if the device is in freefall, then there will be no gravity to
        // measure at all.  This is problematic because we cannot detect the orientation
        // without gravity to tell us which way is up.  A magnitude near 0 produces
        // singularities in the tilt and orientation calculations.
        //
        // In both cases, we postpone choosing an orientation.
        //
        // However, we need to tolerate some acceleration because the angular momentum
        // of turning the device can skew the observed acceleration for a short period of time.
        static const Float NEAR_ZERO_MAGNITUDE;
        // m/s^2
        static const Float ACCELERATION_TOLERANCE;
        // m/s^2
        static const Float MIN_ACCELERATION_MAGNITUDE;
        static const Float MAX_ACCELERATION_MAGNITUDE;
        // Maximum absolute tilt angle at which to consider orientation data.  Beyond this (i.e.
        // when screen is facing the sky or ground), we completely ignore orientation data.
        static const Int32 MAX_TILT = 75;
        // The tilt angle range in degrees for each orientation.
        // Beyond these tilt angles, we don't even consider transitioning into the
        // specified orientation.  We place more stringent requirements on unnatural
        // orientations than natural ones to make it less likely to accidentally transition
        // into those states.
        // The first value of each pair is negative so it applies a limit when the device is
        // facing down (overhead reading in bed).
        // The second value of each pair is positive so it applies a limit when the device is
        // facing up (resting on a table).
        // The ideal tilt angle is 0 (when the device is vertical) so the limits establish
        // how close to vertical the device must be in order to change orientation.
        AutoPtr< ArrayOf< AutoPtr< ArrayOf<Int32> > > > TILT_TOLERANCE;
        // The tilt angle below which we conclude that the user is holding the device
        // overhead reading in bed and lock into that state.
        const Int32 TILT_OVERHEAD_ENTER;
        // The tilt angle above which we conclude that the user would like a rotation
        // change to occur and unlock from the overhead state.
        const Int32 TILT_OVERHEAD_EXIT;
        // The gap angle in degrees between adjacent orientation angles for hysteresis.
        // This creates a "dead zone" between the current orientation and a proposed
        // adjacent orientation.  No orientation proposal is made when the orientation
        // angle is within the gap between the current orientation and the adjacent
        // orientation.
        static const Int32 ADJACENT_ORIENTATION_ANGLE_GAP = 45;
        // Timestamp and value of the last accelerometer sample.
        Int64 mLastFilteredTimestampNanos;
        Float mLastFilteredX;
        Float mLastFilteredY;
        Float mLastFilteredZ;
        // The last proposed rotation, -1 if unknown.
        Int32 mProposedRotation;
        // Value of the current predicted rotation, -1 if unknown.
        Int32 mPredictedRotation;
        // Timestamp of when the predicted rotation most recently changed.
        Int64 mPredictedRotationTimestampNanos;
        // Timestamp when the device last appeared to be flat for sure (the flat delay elapsed).
        Int64 mFlatTimestampNanos;
        Boolean mFlat;
        // Timestamp when the device last appeared to be swinging.
        Int64 mSwingTimestampNanos;
        Boolean mSwinging;
        // Timestamp when the device last appeared to be undergoing external acceleration.
        Int64 mAccelerationTimestampNanos;
        Boolean mAccelerating;
        // Whether we are locked into an overhead usage mode.
        Boolean mOverhead;
        // History of observed tilt angles.
        static const Int32 TILT_HISTORY_SIZE = 40;
        AutoPtr< ArrayOf<Float> > mTiltHistory;
        AutoPtr< ArrayOf<Int64> > mTiltHistoryTimestampNanos;
        Int32 mTiltHistoryIndex;
        WindowOrientationListener* mOwner;
    };

public:
    CAR_INTERFACE_DECL()
    /**
      * Creates a new WindowOrientationListener.
      *
      * @param context for the WindowOrientationListener.
      * @param handler Provides the Looper for receiving sensor updates.
      */
    WindowOrientationListener();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    /**
      * Enables the WindowOrientationListener so it will monitor the sensor and call
      * {@link #onProposedRotationChanged(int)} when the device orientation changes.
      */
    virtual CARAPI Enable();

    /**
      * Disables the WindowOrientationListener.
      */
    virtual CARAPI Disable();

    /**
      * Sets the current rotation.
      *
      * @param rotation The current rotation.
      */
    virtual CARAPI SetCurrentRotation(
        /* [in] */ Int32 rotation);

    /**
      * Gets the proposed rotation.
      *
      * This method only returns a rotation if the orientation listener is certain
      * of its proposal.  If the rotation is indeterminate, returns -1.
      *
      * @return The proposed rotation, or -1 if unknown.
      */
    virtual CARAPI GetProposedRotation(
        /* [out] */ Int32* result);

    /**
      * Returns true if sensor is enabled and false otherwise
      */
    virtual CARAPI CanDetectOrientation(
        /* [out] */ Boolean* result);

    /**
      * Called when the rotation view of the device has changed.
      *
      * This method is called whenever the orientation becomes certain of an orientation.
      * It is called each time the orientation determination transitions from being
      * uncertain to being certain again, even if it is the same orientation as before.
      *
      * @param rotation The new orientation of the device, one of the Surface.ROTATION_* constants.
      * @see android.view.Surface
      */
    virtual CARAPI OnProposedRotationChanged(
        /* [in] */ Int32 rotation) = 0;

    virtual CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

private:
    /**
      * Creates a new WindowOrientationListener.
      *
      * @param context for the WindowOrientationListener.
      * @param handler Provides the Looper for receiving sensor updates.
      * @param rate at which sensor events are processed (see also
      * {@link android.hardware.SensorManager SensorManager}). Use the default
      * value of {@link android.hardware.SensorManager#SENSOR_DELAY_NORMAL
      * SENSOR_DELAY_NORMAL} for simple screen orientation change detection.
      *
      * This constructor is private since no one uses it.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 rate);

private:
    static const String TAG;
    static const Boolean LOG;
    static const Boolean USE_GRAVITY_SENSOR;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ISensorManager> mSensorManager;
    Boolean mEnabled;
    Int32 mRate;
    AutoPtr<ISensor> mSensor;
    AutoPtr<SensorEventListenerImpl> mSensorEventListener;
    Int32 mCurrentRotation;
    Object mLock;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WINDOWORIENTATIONLISTENER_H__

