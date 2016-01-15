#ifndef __ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__
#define __ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__

#include "_Elastos_Droid_Server_CBatteryService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/os/Runnable.h"
#include "LightsService.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Server::LightsService;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::App::IIBatteryStats;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBatteryService)
{
private:
    class Led : public ElRefBase
    {
    public:
        Led(
            /* [in] */ IContext* context,
            /* [in] */ LightsService* lights,
            /* [in] */ CBatteryService* host);

        /**
         * Synchronize on BatteryService.
         */
        CARAPI UpdateLightsLocked();

    private:
        AutoPtr<LightsService::Light> mBatteryLight;

        Int32 mBatteryLowARGB;
        Int32 mBatteryMediumARGB;
        Int32 mBatteryFullARGB;
        Int32 mBatteryLedOn;
        Int32 mBatteryLedOff;

        CBatteryService* mHost;
    };

    class PowerSupplyObserver
        : public UEventObserver
    {
    public:
        PowerSupplyObserver(
            /* [in] */ CBatteryService* host) : mHost(host)
        {}

        CARAPI_(void) OnUEvent(
            /* [in] */ UEventObserver::UEvent* event);

    private:
        CBatteryService* mHost;
    };

    class InvalidChargerObserver
        : public UEventObserver
    {
    public:
        InvalidChargerObserver(
            /* [in] */ CBatteryService* host) : mHost(host)
        {}

        CARAPI_(void) OnUEvent(
            /* [in] */ UEventObserver::UEvent* event);

    private:
        CBatteryService* mHost;
    };

    class ShutdownIfNoPowerLockedRunnable
        : public Runnable
    {
    public:
        ShutdownIfNoPowerLockedRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ShutdownIfInBootFastModeRunnable
        : public Runnable
    {
    public:
        ShutdownIfInBootFastModeRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ShutdownIfOverTempRunnable
        : public Runnable
    {
    public:
        ShutdownIfOverTempRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ActionPowerConnectedRunnable
        : public Runnable
    {
    public:
        ActionPowerConnectedRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ActionPowerDisconnectedRunnable
        : public Runnable
    {
    public:
        ActionPowerDisconnectedRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ActionBatteryLowRunnable
        : public Runnable
    {
    public:
        ActionBatteryLowRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class ActionBatteryOkayRunnable
        : public Runnable
    {
    public:
        ActionBatteryOkayRunnable(
           /* [in] */  CBatteryService* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
    };

    class SendIntentRunnable
        : public Runnable
    {
    public:
        SendIntentRunnable(
            /* [in] */  CBatteryService* host,
            /* [in] */  IIntent* intent)
            : mHost(host)
            , mIntent(intent)
        {}

        CARAPI Run();

    private:
        CBatteryService* mHost;
        AutoPtr<IIntent> mIntent;
    };

public:
    CBatteryService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Handle32 lights);

    CARAPI_(void) SystemReady();

    /**
     * Returns true if the device is plugged into any of the specified plug types.
     */
    CARAPI_(Boolean) IsPowered(
        /* [in] */ Int32 plugTypeSet);

    /**
     * Returns the current plug type.
     */
    CARAPI_(Int32) GetPlugType();

    /**
     * Returns battery level as a percentage.
     */
    CARAPI_(Int32) GetBatteryLevel();

    /**
     * Returns true if battery level is below the first warning threshold.
     */
    CARAPI_(Boolean) IsBatteryLow();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) NativeUpdate();

    CARAPI_(void) NativeShutdown();

    CARAPI_(Boolean) IsPoweredLocked(
        /* [in] */ Int32 plugTypeSet);

    CARAPI_(void) ShutdownIfNoPowerLocked();

    CARAPI_(void) ShutdownIfInBootFastModeLocked();

    CARAPI_(void) ShutdownIfOverTempLocked();

    CARAPI_(void) UpdateLocked();

    CARAPI_(void) ProcessValuesLocked();

    CARAPI_(void) SendIntentLocked();

    CARAPI_(void) LogBatteryStatsLocked();

    CARAPI_(void) LogOutlierLocked(
        /* [in] */ Int64 duration);

    CARAPI_(Int32) GetIconLocked(
        /* [in] */ Int32 level);

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const Int32 BATTERY_SCALE = 100;    // battery capacity is a percentage

    // Used locally for determining when to make a last ditch effort to log
    // discharge stats before the device dies.
    Int32 mCriticalBatteryLevel;

    static const Int32 DUMP_MAX_LENGTH = 24 * 1024;
    static const AutoPtr< ArrayOf<String> > DUMPSYS_ARGS;
    static const String BATTERY_STATS_SERVICE_NAME;

    static const String DUMPSYS_DATA_PATH;

    // This should probably be exposed in the API, though it's not critical
    static const Int32 BATTERY_PLUGGED_NONE = 0;
    static const Int32 BOOT_FAST_REAL_SHUT_DOWN_LEVEL = 5;

    AutoPtr<IContext> mContext;
    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IHandler> mHandler;

    Object mLock;

    /* Begin native fields: All of these fields are set by native code. */
    Boolean mAcOnline;
    Boolean mUsbOnline;
    Boolean mWirelessOnline;
    Int32 mBatteryStatus;
    Int32 mBatteryHealth;
    Boolean mBatteryPresent;
    Int32 mBatteryLevel;
    Int32 mBatteryVoltage;
    Int32 mBatteryTemperature;
    String mBatteryTechnology;
    Boolean mBatteryLevelCritical;
    /* End native fields. */

    Int32 mLastBatteryStatus;
    Int32 mLastBatteryHealth;
    Boolean mLastBatteryPresent;
    Int32 mLastBatteryLevel;
    Int32 mLastBatteryVoltage;
    Int32 mLastBatteryTemperature;
    Boolean mLastBatteryLevelCritical;

    Int32 mInvalidCharger;
    Int32 mLastInvalidCharger;

    Int32 mLowBatteryWarningLevel;
    Int32 mLowBatteryCloseWarningLevel;
    Int32 mShutdownBatteryTemperature;

    Int32 mPlugType;
    Int32 mLastPlugType; // Extra state so we can detect first run

    Int64 mDischargeStartTime;
    Int32 mDischargeStartLevel;

    Boolean mUpdatesStopped;

    AutoPtr<Led> mLed;

    Boolean mSentLowBatteryBroadcast;

    AutoPtr<PowerSupplyObserver> mPowerSupplyObserver;
    AutoPtr<InvalidChargerObserver> mInvalidChargerObserver;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__
