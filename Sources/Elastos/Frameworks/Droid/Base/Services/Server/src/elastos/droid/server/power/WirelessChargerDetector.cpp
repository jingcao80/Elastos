
#include "elastos/droid/server/power/WirelessChargerDetector.h"
#include <elastos/droid/utility/TimeUtils.h>
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

const String WirelessChargerDetector::TAG("WirelessChargerDetector");
const Boolean WirelessChargerDetector::DEBUG;
const Int64 WirelessChargerDetector::SETTLE_TIME_MILLIS;
const Int32 WirelessChargerDetector::SAMPLING_INTERVAL_MILLIS;
const Int32 WirelessChargerDetector::MIN_SAMPLES;
const Int32 WirelessChargerDetector::WIRELESS_CHARGER_TURN_ON_BATTERY_LEVEL_LIMIT;
const Double WirelessChargerDetector::MOVEMENT_ANGLE_COS_THRESHOLD = Elastos::Core::Math::Cos(5 * 3.141592653589793/*Math::PI*/ / 180);
const Double WirelessChargerDetector::MIN_GRAVITY = ISensorManager::GRAVITY_EARTH - 1.0f;
const Double WirelessChargerDetector::MAX_GRAVITY = ISensorManager::GRAVITY_EARTH + 1.0f;

//==============================================================================
//          WirelessChargerDetector::MySensorEventListener
//==============================================================================

CAR_INTERFACE_IMPL(WirelessChargerDetector::MySensorEventListener, Object, ISensorEventListener);

WirelessChargerDetector::MySensorEventListener::MySensorEventListener(
    /* [in] */ WirelessChargerDetector* host)
    : mHost(host)
{}

ECode WirelessChargerDetector::MySensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        AutoPtr< ArrayOf<Float> > values;
        event->GetValues((ArrayOf<Float>**)&values);
        mHost->ProcessSampleLocked((*values)[0], (*values)[1], (*values)[2]);
    }
    return NOERROR;
}

ECode WirelessChargerDetector::MySensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}

//==============================================================================
//          WirelessChargerDetector::MyRunnable
//==============================================================================

WirelessChargerDetector::MyRunnable::MyRunnable(
    /* [in] */ WirelessChargerDetector* host)
    : mHost(host)
{}

ECode WirelessChargerDetector::MyRunnable::Run()
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        mHost->FinishDetectionLocked();
    }
    return NOERROR;
}

//==============================================================================
//          WirelessChargerDetector
//==============================================================================

WirelessChargerDetector::WirelessChargerDetector(
    /* [in] */ ISensorManager* sensorManager,
    /* [in] */ ISuspendBlocker* suspendBlocker,
    /* [in] */ IHandler* handler)
    : mSensorManager(sensorManager)
    , mSuspendBlocker(suspendBlocker)
    , mHandler(handler)
    , mPoweredWirelessly(FALSE)
    , mAtRest(FALSE)
    , mRestX(0)
    , mRestY(0)
    , mRestZ(0)
    , mDetectionInProgress(FALSE)
    , mDetectionStartTime(0)
    , mMustUpdateRestPosition(FALSE)
    , mTotalSamples(0)
    , mMovingSamples(0)
    , mFirstSampleX(0)
    , mFirstSampleY(0)
    , mFirstSampleZ(0)
    , mLastSampleX(0)
    , mLastSampleY(0)
    , mLastSampleZ(0)
{
    sensorManager->GetDefaultSensor(ISensor::TYPE_GRAVITY, (ISensor**)&mGravitySensor);
    mListener = (ISensorEventListener*)new MySensorEventListener(this);
    mSensorTimeout = (IRunnable*)new MyRunnable(this);
}

void WirelessChargerDetector::Dump(
    /* [in] */ IPrintWriter* pw)
{
    {    AutoLock syncLock(mLock);
        pw->Println();
        pw->Println(String("Wireless Charger Detector State:"));
        StringBuilder buider;
        buider += "  mGravitySensor=";
        buider += mGravitySensor;
        pw->Println(buider.ToString());
        pw->Println(String("  mPoweredWirelessly=") + StringUtils::BooleanToString(mPoweredWirelessly));
        pw->Println(String("  mAtRest=") + StringUtils::BooleanToString(mAtRest));
        buider.Reset();
        buider += "  mRestX=";
        buider += mRestX;
        buider += ", mRestY=";
        buider += mRestY;
        buider += ", mRestZ=";
        buider += mRestZ;
        pw->Println(buider.ToString());
        pw->Println(String("  mDetectionInProgress=") + StringUtils::BooleanToString(mDetectionInProgress));
        pw->Println(String("  mDetectionStartTime=") + (mDetectionStartTime == 0 ? String("0 (never)")
                : TimeUtils::FormatUptime(mDetectionStartTime)));
        pw->Println(String("  mMustUpdateRestPosition=") + StringUtils::BooleanToString(mMustUpdateRestPosition));
        pw->Println(String("  mTotalSamples=") + StringUtils::ToString(mTotalSamples));
        pw->Println(String("  mMovingSamples=") + StringUtils::ToString(mMovingSamples));
        buider.Reset();
        buider += "  mFirstSampleX=";
        buider += mFirstSampleX;
        buider += ", mFirstSampleY=";
        buider += mFirstSampleY;
        buider += ", mFirstSampleZ=";
        buider += mFirstSampleZ;
        pw->Println(buider.ToString());
        buider.Reset();
        buider += "  mLastSampleX=";
        buider += mLastSampleX;
        buider += ", mLastSampleY=";
        buider += mLastSampleY;
        buider += ", mLastSampleZ=";
        buider += mLastSampleZ;
        pw->Println(buider.ToString());
    }
}

Boolean WirelessChargerDetector::Update(
    /* [in] */ Boolean isPowered,
    /* [in] */ Int32 plugType,
    /* [in] */ Int32 batteryLevel)
{
    {    AutoLock syncLock(mLock);

        Boolean wasPoweredWirelessly = mPoweredWirelessly;

        if (isPowered && plugType == IBatteryManager::BATTERY_PLUGGED_WIRELESS) {
            // The device is receiving power from the wireless charger.
            // Update the rest position asynchronously.
            mPoweredWirelessly = TRUE;
            mMustUpdateRestPosition = TRUE;
            StartDetectionLocked();
        }
        else {
            // The device may or may not be on the wireless charger depending on whether
            // the unplug signal that we received was spurious.
            mPoweredWirelessly = FALSE;
            if (mAtRest) {
                if (plugType != 0 && plugType != IBatteryManager::BATTERY_PLUGGED_WIRELESS) {
                    // The device was plugged into a new non-wireless power source.
                    // It's safe to assume that it is no longer on the wireless charger.
                    mMustUpdateRestPosition = FALSE;
                    ClearAtRestLocked();
                }
                else {
                    // The device may still be on the wireless charger but we don't know.
                    // Check whether the device has remained at rest on the charger
                    // so that we will know to ignore the next wireless plug event
                    // if needed.
                    StartDetectionLocked();
                }
            }
        }

        // Report that the device has been docked only if the device just started
        // receiving power wirelessly, has a high enough battery level that we
        // can be assured that charging was not delayed due to the battery previously
        // having been full, and the device is not known to already be at rest
        // on the wireless charger from earlier.
        return mPoweredWirelessly && !wasPoweredWirelessly
                && batteryLevel < WIRELESS_CHARGER_TURN_ON_BATTERY_LEVEL_LIMIT
                && !mAtRest;
    }
    return FALSE;
}

void WirelessChargerDetector::StartDetectionLocked()
{
    if (!mDetectionInProgress && mGravitySensor != NULL) {
        Boolean result;
        mSensorManager->RegisterListener(mListener, mGravitySensor,
                SAMPLING_INTERVAL_MILLIS * 1000, &result);
        if (result) {
            mSuspendBlocker->AcquireBlocker();
            mDetectionInProgress = TRUE;
            mDetectionStartTime = SystemClock::GetUptimeMillis();
            mTotalSamples = 0;
            mMovingSamples = 0;

            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            AutoPtr<IMessage> msg;
            helper->Obtain(mHandler, mSensorTimeout, (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            mHandler->SendMessageDelayed(msg, SETTLE_TIME_MILLIS, &result);
        }
    }
}

void WirelessChargerDetector::FinishDetectionLocked()
{
    if (mDetectionInProgress) {
        mSensorManager->UnregisterListener(mListener);
        mHandler->RemoveCallbacks(mSensorTimeout);

        if (mMustUpdateRestPosition) {
            ClearAtRestLocked();
            if (mTotalSamples < MIN_SAMPLES) {
                Slogger::W(TAG, "Wireless charger detector is broken.  Only received %d samples from the gravity sensor but we need at least %d and we expect to see about %d on average.",
                        mTotalSamples, MIN_SAMPLES, SETTLE_TIME_MILLIS / SAMPLING_INTERVAL_MILLIS);
            }
            else if (mMovingSamples == 0){
                mAtRest = TRUE;
                mRestX = mLastSampleX;
                mRestY = mLastSampleY;
                mRestZ = mLastSampleZ;
            }
            mMustUpdateRestPosition = FALSE;
        }

        if (DEBUG) {
            Slogger::D(TAG, "New state: mAtRest=%f, mRestX=%f, mRestY=%f, mRestZ=%f, mTotalSamples=%d, mMovingSamples=%d"
                    , mAtRest, mRestX, mRestY, mRestZ, mTotalSamples, mMovingSamples);
        }

        mDetectionInProgress = FALSE;
        mSuspendBlocker->ReleaseBlocker();
    }
}

void WirelessChargerDetector::ProcessSampleLocked(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    if (mDetectionInProgress) {
        mLastSampleX = x;
        mLastSampleY = y;
        mLastSampleZ = z;

        mTotalSamples += 1;
        if (mTotalSamples == 1) {
            // Save information about the first sample collected.
            mFirstSampleX = x;
            mFirstSampleY = y;
            mFirstSampleZ = z;
        }
        else {
            // Determine whether movement has occurred relative to the first sample.
            if (HasMoved(mFirstSampleX, mFirstSampleY, mFirstSampleZ, x, y, z)) {
                mMovingSamples += 1;
            }
        }

        // Clear the at rest flag if movement has occurred relative to the rest sample.
        if (mAtRest && HasMoved(mRestX, mRestY, mRestZ, x, y, z)) {
            if (DEBUG) {
                Slogger::D(TAG, "No longer at rest: mRestX=%f, mRestY=%f, mRestZ=%f, x=%f, y=%f, z=%f"
                        , mRestX, mRestY, mRestZ, x, y, z);
            }
            ClearAtRestLocked();
        }
    }
}

void WirelessChargerDetector::ClearAtRestLocked()
{
    mAtRest = FALSE;
    mRestX = 0;
    mRestY = 0;
    mRestZ = 0;
}

Boolean WirelessChargerDetector::HasMoved(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float z1,
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float z2)
{
    using Elastos::Core::Math;

    Double dotProduct = (x1 * x2) + (y1 * y2) + (z1 * z2);
    Double mag1 = Math::Sqrt((x1 * x1) + (y1 * y1) + (z1 * z1));
    Double mag2 = Math::Sqrt((x2 * x2) + (y2 * y2) + (z2 * z2));
    if (mag1 < MIN_GRAVITY || mag1 > MAX_GRAVITY
            || mag2 < MIN_GRAVITY || mag2 > MAX_GRAVITY) {
        if (DEBUG) {
            Slogger::D(TAG, "Weird gravity vector: mag1=%d, mag2=%d", mag1, mag2);
        }
        return TRUE;
    }
    Boolean moved = (dotProduct < mag1 * mag2 * MOVEMENT_ANGLE_COS_THRESHOLD);
    if (DEBUG) {
        Slogger::D(TAG, "Check: moved=%d, x1=%f, y1=%f, z1=%f, x2=%f, y2=%f, z2=%f, angle=%d, dotProduct=%d, mag1=%d, mag2=%d"
                , moved, x1, y1, z1, x2, y2, z2, (Math::Acos(dotProduct / mag1 / mag2) * 180 / 3.1415926/*Math.PI*/)
                , dotProduct, mag1, mag2);
    }
    return moved;
}

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos
