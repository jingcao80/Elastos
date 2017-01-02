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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/policy/impl/WindowOrientationListener.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//          WindowOrientationListener::SensorEventListenerImpl
//=====================================================================
CAR_INTERFACE_IMPL(WindowOrientationListener::SensorEventListenerImpl, Object, ISensorEventListener)

const Float WindowOrientationListener::SensorEventListenerImpl::RADIANS_TO_DEGREES = (Float) (180 / Elastos::Core::Math::PI);
const Int64 WindowOrientationListener::SensorEventListenerImpl::NANOS_PER_MS;
const Int32 WindowOrientationListener::SensorEventListenerImpl::ACCELEROMETER_DATA_X;
const Int32 WindowOrientationListener::SensorEventListenerImpl::ACCELEROMETER_DATA_Y;
const Int32 WindowOrientationListener::SensorEventListenerImpl::ACCELEROMETER_DATA_Z;
const Int64 WindowOrientationListener::SensorEventListenerImpl::PROPOSAL_SETTLE_TIME_NANOS;
const Int64 WindowOrientationListener::SensorEventListenerImpl::PROPOSAL_MIN_TIME_SINCE_FLAT_ENDED_NANOS;
const Int64 WindowOrientationListener::SensorEventListenerImpl::PROPOSAL_MIN_TIME_SINCE_SWING_ENDED_NANOS;
const Int64 WindowOrientationListener::SensorEventListenerImpl::PROPOSAL_MIN_TIME_SINCE_ACCELERATION_ENDED_NANOS;
const Float WindowOrientationListener::SensorEventListenerImpl::FLAT_ANGLE = 75;
const Int64 WindowOrientationListener::SensorEventListenerImpl::FLAT_TIME_NANOS;
const Float WindowOrientationListener::SensorEventListenerImpl::SWING_AWAY_ANGLE_DELTA = 20;
const Int64 WindowOrientationListener::SensorEventListenerImpl::SWING_TIME_NANOS;
const Int64 WindowOrientationListener::SensorEventListenerImpl::MAX_FILTER_DELTA_TIME_NANOS;
const Float WindowOrientationListener::SensorEventListenerImpl::FILTER_TIME_CONSTANT_MS = 200.0f;
const Float WindowOrientationListener::SensorEventListenerImpl::NEAR_ZERO_MAGNITUDE = 1;
const Float WindowOrientationListener::SensorEventListenerImpl::ACCELERATION_TOLERANCE = 4;
const Float WindowOrientationListener::SensorEventListenerImpl::MIN_ACCELERATION_MAGNITUDE = ISensorManager::STANDARD_GRAVITY - ACCELERATION_TOLERANCE;
const Float WindowOrientationListener::SensorEventListenerImpl::MAX_ACCELERATION_MAGNITUDE = ISensorManager::STANDARD_GRAVITY + ACCELERATION_TOLERANCE;
const Int32 WindowOrientationListener::SensorEventListenerImpl::MAX_TILT;
const Int32 WindowOrientationListener::SensorEventListenerImpl::ADJACENT_ORIENTATION_ANGLE_GAP;
const Int32 WindowOrientationListener::SensorEventListenerImpl::TILT_HISTORY_SIZE;

WindowOrientationListener::SensorEventListenerImpl::SensorEventListenerImpl(
    /* [in] */ WindowOrientationListener* owner)
    : TILT_OVERHEAD_ENTER(-40)
    , TILT_OVERHEAD_EXIT(-15)
    , mLastFilteredTimestampNanos(0)
    , mLastFilteredX(0.f)
    , mLastFilteredY(0.f)
    , mLastFilteredZ(0.f)
    , mProposedRotation(0)
    , mPredictedRotation(0)
    , mPredictedRotationTimestampNanos(0)
    , mFlatTimestampNanos(0)
    , mFlat(FALSE)
    , mSwingTimestampNanos(0)
    , mSwinging(FALSE)
    , mAccelerationTimestampNanos(0)
    , mAccelerating(FALSE)
    , mOverhead(FALSE)
    , mTiltHistoryIndex(0)
{
    TILT_TOLERANCE = ArrayOf<AutoPtr<ArrayOf<Int32> > >::Alloc(4);
    AutoPtr<ArrayOf<Int32> > t0 = ArrayOf<Int32>::Alloc(2);
    (*t0)[0] = -25;
    (*t0)[1] = 70;
    TILT_TOLERANCE->Set(0, t0);

    AutoPtr<ArrayOf<Int32> > t1 = ArrayOf<Int32>::Alloc(2);
    (*t1)[0] = -25;
    (*t1)[1] = 65;
    TILT_TOLERANCE->Set(1, t1);

    AutoPtr<ArrayOf<Int32> > t2 = ArrayOf<Int32>::Alloc(2);
    (*t2)[0] = -25;
    (*t2)[1] = 60;
    TILT_TOLERANCE->Set(2, t2);

    AutoPtr<ArrayOf<Int32> > t3 = ArrayOf<Int32>::Alloc(2);
    (*t3)[0] = -25;
    (*t3)[1] = 65;
    TILT_TOLERANCE->Set(3, t3);

    mTiltHistory = ArrayOf<Float>::Alloc(TILT_HISTORY_SIZE);
    mTiltHistoryTimestampNanos = ArrayOf<Int64>::Alloc(TILT_HISTORY_SIZE);

    mOwner = owner;
}

ECode WindowOrientationListener::SensorEventListenerImpl::GetProposedRotationLocked(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mProposedRotation;
    return NOERROR;
}

ECode WindowOrientationListener::SensorEventListenerImpl::DumpLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);
    pw->Print(prefix + String("mProposedRotation="));
    pw->Println(mProposedRotation);
    pw->Print(prefix + String("mPredictedRotation="));
    pw->Println(mPredictedRotation);
    pw->Print(prefix + String("mLastFilteredX="));
    pw->Println(mLastFilteredX);
    pw->Print(prefix + String("mLastFilteredY="));
    pw->Println(mLastFilteredY);
    pw->Print(prefix + String("mLastFilteredZ="));
    pw->Println(mLastFilteredZ);
    pw->Print(prefix + String("mTiltHistory={last: ")); pw->Print(GetLastTiltLocked());
    pw->Print(String("}"));
    pw->Print(prefix + String("mFlat="));
    pw->Println(mFlat);
    pw->Print(prefix + String("mSwinging="));
    pw->Println(mSwinging);
    pw->Print(prefix + String("mAccelerating="));
    pw->Println(mAccelerating);
    pw->Print(prefix + String("mOverhead="));
    pw->Println(mOverhead);
    return NOERROR;
}

ECode WindowOrientationListener::SensorEventListenerImpl::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}

ECode WindowOrientationListener::SensorEventListenerImpl::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    VALIDATE_NOT_NULL(event);
    Int32 proposedRotation;
    Int32 oldProposedRotation;

    {
        AutoLock lock(mOwner->mLock);
        // The vector given in the SensorEvent points straight up (towards the sky) under
        // ideal conditions (the phone is not accelerating).  I'll call this up vector
        // elsewhere.
        AutoPtr<ArrayOf<Float> > values;
        event->GetValues((ArrayOf<Float>**)&values);
        Float x = (*values)[ACCELEROMETER_DATA_X];
        Float y = (*values)[ACCELEROMETER_DATA_Y];
        Float z = (*values)[ACCELEROMETER_DATA_Z];

        if (LOG)
        {
            Slogger::V(TAG, "Raw acceleration vector: x=%f, y=%f, z=%f, magnitude=%d", x, y, z, Elastos::Core::Math::Sqrt(x * x + y * y + z * z));
        }

        // Apply a low-pass filter to the acceleration up vector in cartesian space.
        // Reset the orientation listener state if the samples are too far apart in time
        // or when we see values of (0, 0, 0) which indicates that we polled the
        // accelerometer too soon after turning it on and we don't have any data yet.
        Int64 now;
        event->GetTimestamp(&now);
        Int64 then = mLastFilteredTimestampNanos;
        Float timeDeltaMS = (now - then) * 0.000001f;
        Boolean skipSample;
        if (now < then
                || now > then + MAX_FILTER_DELTA_TIME_NANOS
                || (x == 0 && y == 0 && z == 0))
        {
            if (LOG)
            {
                Slogger::V(TAG, "Resetting orientation listener.");
            }
            ResetLocked();
            skipSample = TRUE;
        }
        else
        {
            Float alpha = timeDeltaMS / (FILTER_TIME_CONSTANT_MS + timeDeltaMS);
            x = alpha * (x - mLastFilteredX) + mLastFilteredX;
            y = alpha * (y - mLastFilteredY) + mLastFilteredY;
            z = alpha * (z - mLastFilteredZ) + mLastFilteredZ;
            if (LOG)
            {
                Slogger::V(TAG, "Filtered acceleration vector: x=%f, y=%f, z=%f, magnitude=%d", x, y, z, Elastos::Core::Math::Sqrt(x * x + y * y + z * z));
            }
            skipSample = FALSE;
        }
        mLastFilteredTimestampNanos = now;
        mLastFilteredX = x;
        mLastFilteredY = y;
        mLastFilteredZ = z;

        Boolean isAccelerating = FALSE;
        Boolean isFlat = FALSE;
        Boolean isSwinging = FALSE;
        if (!skipSample)
        {
            // Calculate the magnitude of the acceleration vector.
            Float magnitude = Elastos::Core::Math::Sqrt(x * x + y * y + z * z);
            if (magnitude < NEAR_ZERO_MAGNITUDE)
            {
                if (LOG)
                {
                    Slogger::V(TAG, "Ignoring sensor data, magnitude too close to zero.");
                }
                ClearPredictedRotationLocked();
            }
            else
            {
                // Determine whether the device appears to be undergoing external
                // acceleration.
                if (IsAcceleratingLocked(magnitude))
                {
                    isAccelerating = TRUE;
                    mAccelerationTimestampNanos = now;
                }

                // Calculate the tilt angle.
                // This is the angle between the up vector and the x-y plane (the plane of
                // the screen) in a range of [-90, 90] degrees.
                //   -90 degrees: screen horizontal and facing the ground (overhead)
                //     0 degrees: screen vertical
                //    90 degrees: screen horizontal and facing the sky (on table)
                Int32 tiltAngle =
                    (Int32) Elastos::Core::Math::Round(Elastos::Core::Math::Asin(z / magnitude) * RADIANS_TO_DEGREES);
                AddTiltHistoryEntryLocked(now, tiltAngle);

                // Determine whether the device appears to be flat or swinging.
                if (IsFlatLocked(now))
                {
                    isFlat = TRUE;
                    mFlatTimestampNanos = now;
                }
                if (IsSwingingLocked(now, tiltAngle))
                {
                    isSwinging = TRUE;
                    mSwingTimestampNanos = now;
                }

                // If the tilt angle is too close to horizontal then we cannot determine
                // the orientation angle of the screen.
                if (tiltAngle <= TILT_OVERHEAD_ENTER)
                {
                    mOverhead = TRUE;
                }
                else if (tiltAngle >= TILT_OVERHEAD_EXIT)
                {
                    mOverhead = FALSE;
                }
                if (mOverhead)
                {
                    if (LOG)
                    {
                        Slogger::V(TAG, "Ignoring sensor data, device is overhead: tiltAngle=%d", tiltAngle);
                    }
                    ClearPredictedRotationLocked();
                }
                else if (Elastos::Core::Math::Abs(tiltAngle) > MAX_TILT)
                {
                    if (LOG)
                    {
                        Slogger::V(TAG, "Ignoring sensor data, tilt angle too high: tiltAngle=%d", tiltAngle);
                    }
                    ClearPredictedRotationLocked();
                }
                else
                {
                    // Calculate the orientation angle.
                    // This is the angle between the x-y projection of the up vector onto
                    // the +y-axis, increasing clockwise in a range of [0, 360] degrees.
                    Int32 orientationAngle = (Int32) Elastos::Core::Math::Round(
                            -Elastos::Core::Math::Atan2(-x, y) * RADIANS_TO_DEGREES);
                    if (orientationAngle < 0)
                    {
                        // atan2 returns [-180, 180]; normalize to [0, 360]
                        orientationAngle += 360;
                    }

                    // Find the nearest rotation.
                    Int32 nearestRotation = (orientationAngle + 45) / 90;
                    if (nearestRotation == 4)
                    {
                        nearestRotation = 0;
                    }

                    // Determine the predicted orientation.
                    if (IsTiltAngleAcceptableLocked(nearestRotation, tiltAngle)
                            && IsOrientationAngleAcceptableLocked(nearestRotation, orientationAngle))
                    {
                        UpdatePredictedRotationLocked(now, nearestRotation);
                        if (LOG)
                        {
                            Slogger::V(TAG, "Predicted: tiltAngle=%d, orientationAngle=%d, predictedRotation=%d, predictedRotationAgeMS=%f",
                                    tiltAngle, orientationAngle, mPredictedRotation, ((now - mPredictedRotationTimestampNanos) * 0.000001));
                        }
                    }
                    else
                    {
                        if (LOG)
                        {
                            Slogger::V(TAG, "Ignoring sensor data, no predicted rotation: tiltAngle=%d, orientationAngle=%d", tiltAngle, orientationAngle);
                        }
                        ClearPredictedRotationLocked();
                    }
                }
            }
        }
        mFlat = isFlat;
        mSwinging = isSwinging;
        mAccelerating = isAccelerating;

        // Determine new proposed rotation.
        oldProposedRotation = mProposedRotation;
        if (mPredictedRotation < 0 || IsPredictedRotationAcceptableLocked(now))
        {
            mProposedRotation = mPredictedRotation;
        }
        proposedRotation = mProposedRotation;

        // Write final statistics about where we are in the orientation detection process.
        if (LOG)
        {
            Slogger::V(TAG, "Result: currentRotation=%d, proposedRotation=%d, predictedRotation=%d, timeDeltaMS=%f, isAccelerating=%d, isFlat=%d, isSwinging=%d\
                    , isOverhead=%d, timeUntilSettledMS=%f, timeUntilAccelerationDelayExpiredMS=%f, timeUntilFlatDelayExpiredMS=%f, timeUntilSwingDelayExpiredMS=%f",
                    mOwner->mCurrentRotation, proposedRotation, mPredictedRotation, timeDeltaMS, isAccelerating, isFlat, isSwinging, mOverhead,
                    RemainingMS(now, mPredictedRotationTimestampNanos + PROPOSAL_SETTLE_TIME_NANOS),
                    RemainingMS(now, mAccelerationTimestampNanos + PROPOSAL_MIN_TIME_SINCE_ACCELERATION_ENDED_NANOS),
                    RemainingMS(now, mFlatTimestampNanos + PROPOSAL_MIN_TIME_SINCE_FLAT_ENDED_NANOS),
                    RemainingMS(now, mSwingTimestampNanos + PROPOSAL_MIN_TIME_SINCE_SWING_ENDED_NANOS));
        }
    }

    // Tell the listener.
    if (proposedRotation != oldProposedRotation && proposedRotation >= 0)
    {
        if (LOG)
        {
            Slogger::V(TAG, "Proposed rotation changed!  proposedRotation=%d, oldProposedRotation=%d", proposedRotation, oldProposedRotation);
        }
        mOwner->OnProposedRotationChanged(proposedRotation);
    }
    return NOERROR;
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsTiltAngleAcceptableLocked(
    /* [in] */ Int32 rotation,
    /* [in] */ Int32 tiltAngle)
{
    AutoPtr<ArrayOf<Int32> > array = (*TILT_TOLERANCE)[rotation];
    Int32 a0 = (*array)[0];
    Int32 a1 = (*array)[1];
    return tiltAngle >= a0 && tiltAngle <= a1;
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsOrientationAngleAcceptableLocked(
    /* [in] */ Int32 rotation,
    /* [in] */ Int32 orientationAngle)
{
    // If there is no current rotation, then there is no gap.
    // The gap is used only to introduce hysteresis among advertised orientation
    // changes to avoid flapping.
    Int32 currentRotation = mOwner->mCurrentRotation;
    if (currentRotation >= 0)
    {
        // If the specified rotation is the same or is counter-clockwise adjacent
        // to the current rotation, then we set a lower bound on the orientation angle.
        // For example, if currentRotation is ROTATION_0 and proposed is ROTATION_90,
        // then we want to check orientationAngle > 45 + GAP / 2.
        if (rotation == currentRotation
                || rotation == (currentRotation + 1) % 4)
        {
            Int32 lowerBound = rotation * 90 - 45 + ADJACENT_ORIENTATION_ANGLE_GAP / 2;
            if (rotation == 0)
            {
                if (orientationAngle >= 315 && orientationAngle < lowerBound + 360)
                {
                    return FALSE;
                }
            }
            else
            {
                if (orientationAngle < lowerBound)
                {
                    return FALSE;
                }
            }
        }

        // If the specified rotation is the same or is clockwise adjacent,
        // then we set an upper bound on the orientation angle.
        // For example, if currentRotation is ROTATION_0 and rotation is ROTATION_270,
        // then we want to check orientationAngle < 315 - GAP / 2.
        if (rotation == currentRotation
                || rotation == (currentRotation + 3) % 4)
        {
            Int32 upperBound = rotation * 90 + 45 - ADJACENT_ORIENTATION_ANGLE_GAP / 2;
            if (rotation == 0)
            {
                if (orientationAngle <= 45 && orientationAngle > upperBound)
                {
                    return FALSE;
                }
            }
            else
            {
                if (orientationAngle > upperBound)
                {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsPredictedRotationAcceptableLocked(
    /* [in] */ Int64 now)
{
    // The predicted rotation must have settled long enough.
    if (now < mPredictedRotationTimestampNanos + PROPOSAL_SETTLE_TIME_NANOS) {
        return FALSE;
    }

    // The last flat state (time since picked up) must have been sufficiently long ago.
    if (now < mFlatTimestampNanos + PROPOSAL_MIN_TIME_SINCE_FLAT_ENDED_NANOS) {
        return FALSE;
    }

    // The last swing state (time since last movement to put down) must have been
    // sufficiently long ago.
    if (now < mSwingTimestampNanos + PROPOSAL_MIN_TIME_SINCE_SWING_ENDED_NANOS) {
        return FALSE;
    }

    // The last acceleration state must have been sufficiently long ago.
    if (now < mAccelerationTimestampNanos + PROPOSAL_MIN_TIME_SINCE_ACCELERATION_ENDED_NANOS) {
        return FALSE;
    }

    // Looks good!
    return TRUE;
}

void WindowOrientationListener::SensorEventListenerImpl::ResetLocked()
{
    mLastFilteredTimestampNanos = Elastos::Core::Math::INT64_MIN_VALUE;
    mProposedRotation = -1;
    mFlatTimestampNanos = Elastos::Core::Math::INT64_MIN_VALUE;
    mFlat = FALSE;
    mSwingTimestampNanos = Elastos::Core::Math::INT64_MIN_VALUE;
    mSwinging = FALSE;
    mAccelerationTimestampNanos = Elastos::Core::Math::INT64_MIN_VALUE;
    mAccelerating = FALSE;
    mOverhead = FALSE;
    ClearPredictedRotationLocked();
    ClearTiltHistoryLocked();
}

void WindowOrientationListener::SensorEventListenerImpl::ClearPredictedRotationLocked()
{
    mPredictedRotation = -1;
    mPredictedRotationTimestampNanos = Elastos::Core::Math::INT64_MIN_VALUE;
}

void WindowOrientationListener::SensorEventListenerImpl::UpdatePredictedRotationLocked(
    /* [in] */ Int64 now,
    /* [in] */ Int32 rotation)
{
    if (mPredictedRotation != rotation)
    {
        mPredictedRotation = rotation;
        mPredictedRotationTimestampNanos = now;
    }
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsAcceleratingLocked(
    /* [in] */ Float magnitude)
{
    return magnitude < MIN_ACCELERATION_MAGNITUDE || magnitude > MAX_ACCELERATION_MAGNITUDE;
}

void WindowOrientationListener::SensorEventListenerImpl::ClearTiltHistoryLocked()
{
    (*mTiltHistoryTimestampNanos)[0] = Elastos::Core::Math::INT64_MIN_VALUE;
    mTiltHistoryIndex = 1;
}

void WindowOrientationListener::SensorEventListenerImpl::AddTiltHistoryEntryLocked(
    /* [in] */ Int64 now,
    /* [in] */ Float tilt)
{
    (*mTiltHistory)[mTiltHistoryIndex] = tilt;
    (*mTiltHistoryTimestampNanos)[mTiltHistoryIndex] = now;
    mTiltHistoryIndex = (mTiltHistoryIndex + 1) % TILT_HISTORY_SIZE;
    (*mTiltHistoryTimestampNanos)[mTiltHistoryIndex] = Elastos::Core::Math::INT64_MIN_VALUE;
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsFlatLocked(
    /* [in] */ Int64 now)
{
    for (Int32 i = mTiltHistoryIndex; (i = NextTiltHistoryIndexLocked(i)) >= 0; )
    {
        if ((*mTiltHistory)[i] < FLAT_ANGLE)
        {
            break;
        }
        if ((*mTiltHistoryTimestampNanos)[i] + FLAT_TIME_NANOS <= now)
        {
            // Tilt has remained greater than FLAT_TILT_ANGLE for FLAT_TIME_NANOS.
            return TRUE;
        }
    }
    return FALSE;
}

Boolean WindowOrientationListener::SensorEventListenerImpl::IsSwingingLocked(
    /* [in] */ Int64 now,
    /* [in] */ Float tilt)
{
    for (Int32 i = mTiltHistoryIndex; (i = NextTiltHistoryIndexLocked(i)) >= 0; )
    {
        if ((*mTiltHistoryTimestampNanos)[i] + SWING_TIME_NANOS < now)
        {
            break;
        }
        if ((*mTiltHistory)[i] + SWING_AWAY_ANGLE_DELTA <= tilt)
        {
            // Tilted away by SWING_AWAY_ANGLE_DELTA within SWING_TIME_NANOS.
            return TRUE;
        }
    }
    return FALSE;
}

Int32 WindowOrientationListener::SensorEventListenerImpl::NextTiltHistoryIndexLocked(
    /* [in] */ Int32 index)
{
    index = (index == 0 ? TILT_HISTORY_SIZE : index) - 1;
    return (*mTiltHistoryTimestampNanos)[index] != Elastos::Core::Math::INT64_MIN_VALUE ? index : -1;
}

Float WindowOrientationListener::SensorEventListenerImpl::GetLastTiltLocked()
{
    Int32 index = NextTiltHistoryIndexLocked(mTiltHistoryIndex);
    return index >= 0 ? (*mTiltHistory)[index] : Elastos::Core::Math::FLOAT_NAN;
}

Float WindowOrientationListener::SensorEventListenerImpl::RemainingMS(
    /* [in] */ Int64 now,
    /* [in] */ Int64 until)
{
    return now >= until ? 0 : (until - now) * 0.000001;
}

//=====================================================================
//                      WindowOrientationListener
//=====================================================================
static Boolean InitDefaultLOG()
{
    Boolean result = FALSE;
    SystemProperties::GetBoolean(String("debug.orientation.log"), FALSE, &result);
    return result;
}

CAR_INTERFACE_IMPL(WindowOrientationListener, Object, IWindowOrientationListener)

const String WindowOrientationListener::TAG("WindowOrientationListener");
const Boolean WindowOrientationListener::LOG = InitDefaultLOG();
//SystemProperties.getBoolean("debug.orientation.log", false);
const Boolean WindowOrientationListener::USE_GRAVITY_SENSOR = FALSE;

WindowOrientationListener::WindowOrientationListener()
{
}

ECode WindowOrientationListener::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    return constructor(context, handler, ISensorManager::SENSOR_DELAY_UI);
}

ECode WindowOrientationListener::Enable()
{
    AutoLock lock(mLock);
    if (mSensor == NULL) {
        Logger::W(TAG, "Cannot detect sensors. Not enabled");
        return NOERROR;
    }
    if (mEnabled == FALSE) {
        if (LOG) {
            Logger::D(TAG, "WindowOrientationListener enabled");
        }
        if (mSensorEventListener != NULL) {
            mSensorEventListener->ResetLocked();
            Boolean res;
            mSensorManager->RegisterListener(mSensorEventListener, mSensor, mRate, mHandler, &res);
            mEnabled = TRUE;
        }
    }
    return NOERROR;
}

ECode WindowOrientationListener::Disable()
{
    AutoLock lock(mLock);
    if (mSensor == NULL) {
        Logger::W(TAG, "Cannot detect sensors. Invalid disable");
        return NOERROR;
    }
    if (mEnabled == TRUE) {
        if (LOG) {
            Logger::D(TAG, "WindowOrientationListener disabled");
        }
        if (mSensorEventListener != NULL) {
            mSensorManager->UnregisterListener(mSensorEventListener);
            mEnabled = FALSE;
        }
    }
    return NOERROR;
}

ECode WindowOrientationListener::SetCurrentRotation(
    /* [in] */ Int32 rotation)
{
    AutoLock lock(mLock);
    mCurrentRotation = rotation;
    return NOERROR;
}

ECode WindowOrientationListener::GetProposedRotation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    AutoLock lock(mLock);
    if (mEnabled) {
        if (mSensorEventListener != NULL) {
            mSensorEventListener->GetProposedRotationLocked(result);
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode WindowOrientationListener::CanDetectOrientation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    *result = mSensor != NULL;
    return NOERROR;
}

ECode WindowOrientationListener::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);
    AutoLock lock(mLock);
    pw->Println(prefix + TAG);
    String p = prefix + String("  ");
    pw->Print(p + String("mEnabled="));
    pw->Println(mEnabled);
    pw->Print(p + String("mCurrentRotation="));
    pw->Println(mCurrentRotation);
    pw->Print(p + String("mSensor="));
    pw->Println(mSensor);
    pw->Print(p + String("mRate="));
    pw->Println(mRate);

    if (mSensorEventListener != NULL) {
        mSensorEventListener->DumpLocked(pw, p);
    }
    return NOERROR;
}

ECode WindowOrientationListener::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 rate)
{
    mHandler = handler;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&obj);
    mSensorManager = ISensorManager::Probe(obj);
    mRate = rate;
    mSensorManager->GetDefaultSensor(
        USE_GRAVITY_SENSOR? ISensor::TYPE_GRAVITY : ISensor::TYPE_ACCELEROMETER,
        (ISensor**)&mSensor);
    if (mSensor != NULL) {
        // Create listener only if sensors do exist
        mSensorEventListener = new SensorEventListenerImpl(this);
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

