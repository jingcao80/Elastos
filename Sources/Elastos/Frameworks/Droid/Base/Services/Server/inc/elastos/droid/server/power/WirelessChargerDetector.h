
#ifndef __ELASTOS_DROID_SERVER_POWER_WIRELESSCHARGERDETECTOR_H__
#define __ELASTOS_DROID_SERVER_POWER_WIRELESSCHARGERDETECTOR_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Os.h>
#include <_Elastos.Droid.Server.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Server::Power::ISuspendBlocker;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

/**
 * Implements heuristics to detect docking or undocking from a wireless charger.
 * <p>
 * Some devices have wireless charging circuits that are unable to detect when the
 * device is resting on a wireless charger except when the device is actually
 * receiving power from the charger.  The device may stop receiving power
 * if the battery is already nearly full or if it is too hot.  As a result, we cannot
 * always rely on the battery service wireless plug signal to accurately indicate
 * whether the device has been docked or undocked from a wireless charger.
 * </p><p>
 * This is a problem because the power manager typically wakes up the screen and
 * plays a tone when the device is docked in a wireless charger.  It is important
 * for the system to suppress spurious docking and undocking signals because they
 * can be intrusive for the user (especially if they cause a tone to be played
 * late at night for no apparent reason).
 * </p><p>
 * To avoid spurious signals, we apply some special policies to wireless chargers.
 * </p><p>
 * 1. Don't wake the device when undocked from the wireless charger because
 * it might be that the device is still resting on the wireless charger
 * but is not receiving power anymore because the battery is full.
 * Ideally we would wake the device if we could be certain that the user had
 * picked it up from the wireless charger but due to hardware limitations we
 * must be more conservative.
 * </p><p>
 * 2. Don't wake the device when docked on a wireless charger if the
 * battery already appears to be mostly full.  This situation may indicate
 * that the device was resting on the charger the whole time and simply
 * wasn't receiving power because the battery was already full.  We can't tell
 * whether the device was just placed on the charger or whether it has
 * been there for half of the night slowly discharging until it reached
 * the point where it needed to start charging again.  So we suppress docking
 * signals that occur when the battery level is above a given threshold.
 * </p><p>
 * 3. Don't wake the device when docked on a wireless charger if it does
 * not appear to have moved since it was last undocked because it may
 * be that the prior undocking signal was spurious.  We use the gravity
 * sensor to detect this case.
 * </p>
 */
class WirelessChargerDetector
    : public Object
{
private:
    class MySensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        MySensorEventListener(
            /* [in] */ WirelessChargerDetector* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        // @Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

    private:
        WirelessChargerDetector* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ WirelessChargerDetector* host);

        // @Override
        CARAPI Run();

    private:
        WirelessChargerDetector* mHost;
    };

public:
    WirelessChargerDetector(
        /* [in] */ ISensorManager* sensorManager,
        /* [in] */ ISuspendBlocker* suspendBlocker,
        /* [in] */ IHandler* handler);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

    /**
     * Updates the charging state and returns true if docking was detected.
     *
     * @param isPowered True if the device is powered.
     * @param plugType The current plug type.
     * @param batteryLevel The current battery level.
     * @return True if the device is determined to have just been docked on a wireless
     * charger, after suppressing spurious docking or undocking signals.
     */
    CARAPI_(Boolean) Update(
        /* [in] */ Boolean isPowered,
        /* [in] */ Int32 plugType,
        /* [in] */ Int32 batteryLevel);

private:
    CARAPI_(void) StartDetectionLocked();

    CARAPI_(void) FinishDetectionLocked();

    CARAPI_(void) ProcessSampleLocked(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI_(void) ClearAtRestLocked();

    static CARAPI_(Boolean) HasMoved(
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Float z1,
        /* [in] */ Float x2,
        /* [in] */ Float y2,
        /* [in] */ Float z2);

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    // The minimum amount of time to spend watching the sensor before making
    // a determination of whether movement occurred.
    static const Int64 SETTLE_TIME_MILLIS = 800;

    // The sensor sampling interval.
    static const Int32 SAMPLING_INTERVAL_MILLIS = 50;

    // The minimum number of samples that must be collected.
    static const Int32 MIN_SAMPLES = 3;

    // Upper bound on the battery charge percentage in order to consider turning
    // the screen on when the device starts charging wirelessly.
    static const Int32 WIRELESS_CHARGER_TURN_ON_BATTERY_LEVEL_LIMIT = 95;

    // To detect movement, we compute the angle between the gravity vector
    // at rest and the current gravity vector.  This field specifies the
    // cosine of the maximum angle variance that we tolerate while at rest.
    static const Double MOVEMENT_ANGLE_COS_THRESHOLD;

    // Sanity thresholds for the gravity vector.
    static const Double MIN_GRAVITY;
    static const Double MAX_GRAVITY;

    Object mLock;

    AutoPtr<ISensorManager> mSensorManager;
    AutoPtr<ISuspendBlocker> mSuspendBlocker;
    AutoPtr<IHandler> mHandler;

    // The gravity sensor, or null if none.
    AutoPtr<ISensor> mGravitySensor;

    // Previously observed wireless power state.
    Boolean mPoweredWirelessly;

    // True if the device is thought to be at rest on a wireless charger.
    Boolean mAtRest;

    // The gravity vector most recently observed while at rest.
    Float mRestX;
    Float mRestY;
    Float mRestZ;

    /* These properties are only meaningful while detection is in progress. */

    // True if detection is in progress.
    // The suspend blocker is held while this is the case.
    Boolean mDetectionInProgress;

    // The time when detection was last performed.
    Int64 mDetectionStartTime;

    // True if the rest position should be updated if at rest.
    // Otherwise, the current rest position is simply checked and cleared if movement
    // is detected but no new rest position is stored.
    Boolean mMustUpdateRestPosition;

    // The total number of samples collected.
    Int32 mTotalSamples;

    // The number of samples collected that showed evidence of not being at rest.
    Int32 mMovingSamples;

    // The value of the first sample that was collected.
    Float mFirstSampleX;
    Float mFirstSampleY;
    Float mFirstSampleZ;

    // The value of the last sample that was collected.
    Float mLastSampleX;
    Float mLastSampleY;
    Float mLastSampleZ;

    AutoPtr<ISensorEventListener> mListener;
    AutoPtr<IRunnable> mSensorTimeout;

    friend class MySensorEventListener;
};

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_WIRELESSCHARGERDETECTOR_H__
