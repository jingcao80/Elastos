#ifndef __ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__
#define __ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__

#include "_Elastos_Droid_Server_CBatteryService.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/lights/Light.h"
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/database/ContentObserver.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <batteryservice/IBatteryPropertiesListener.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Os::IBatteryProperties;
using Elastos::Droid::Os::IBatteryManagerInternal;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Server::Lights::Light;
using Elastos::Droid::Server::Lights::ILightsManager;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * <p>BatteryService monitors the charging status, and charge level of the device
 * battery.  When these values change this service broadcasts the new values
 * to all {@link android.content.BroadcastReceiver IntentReceivers} that are
 * watching the {@link android.content.Intent#ACTION_BATTERY_CHANGED
 * BATTERY_CHANGED} action.</p>
 * <p>The new values are stored in the Intent data and can be retrieved by
 * calling {@link android.content.Intent#getExtra Intent.getExtra} with the
 * following keys:</p>
 * <p>&quot;scale&quot; - int, the maximum value for the charge level</p>
 * <p>&quot;level&quot; - int, charge level, from 0 through &quot;scale&quot; inclusive</p>
 * <p>&quot;status&quot; - String, the current charging status.<br />
 * <p>&quot;health&quot; - String, the current battery health.<br />
 * <p>&quot;present&quot; - boolean, true if the battery is present<br />
 * <p>&quot;icon-small&quot; - int, suggested small icon to use for this state</p>
 * <p>&quot;plugged&quot; - int, 0 if the device is not plugged in; 1 if plugged
 * into an AC power adapter; 2 if plugged in via USB.</p>
 * <p>&quot;voltage&quot; - int, current battery voltage in millivolts</p>
 * <p>&quot;temperature&quot; - int, current battery temperature in tenths of
 * a degree Centigrade</p>
 * <p>&quot;technology&quot; - String, the type of battery installed, e.g. "Li-ion"</p>
 *
 * <p>
 * The battery service may be called by the power manager while holding its locks so
 * we take care to post all outcalls into the activity manager to a handler.
 *
 * FIXME: Ideally the power manager would perform all of its calls into the battery
 * service asynchronously itself.
 * </p>
 */
CarClass(CBatteryService)
    , public SystemService
{
public:
    class BinderService
        : public Binder
    {
    public:
        BinderService();

        CARAPI constructor(
            /* [in] */ ISystemService* batteryService);

        // //@Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        CBatteryService* mHost;
    };

    class BatteryListener
        : public android::BnInterface<android::IBatteryPropertiesListener>
    {
    public:
        BatteryListener(
            /* [in] */ CBatteryService* host)
            : mHost(host)
        {}

        void batteryPropertiesChanged(struct android::BatteryProperties props);

        android::status_t onTransact(uint32_t code, const android::Parcel& data,
                                android::Parcel* reply, uint32_t flags = 0);

    private:
        CBatteryService* mHost;
    };

private:
    class BootPhaseContentObserver
        : public ContentObserver
    {
    public:
        BootPhaseContentObserver(
            /* [in] */ CBatteryService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CBatteryService* mHost;

    };

    class Led : public Object
    {
    public:
        Led(
            /* [in] */ IContext* context,
            /* [in] */ ILightsManager* lights,
            /* [in] */ CBatteryService* host);

        /**
         * Synchronize on BatteryService.
         */
        CARAPI UpdateLightsLocked();

    private:
        AutoPtr<Light> mBatteryLight;

        Int32 mBatteryLedOn;
        Int32 mBatteryLedOff;

        CBatteryService* mHost;
    };

    class InvalidChargerObserver
        : public UEventObserver
    {
    public:
        InvalidChargerObserver(
            /* [in] */ CBatteryService* host) : mHost(host)
        {}

        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);

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

    class LocalService
        : public Object
        , public IBatteryManagerInternal
    {
    public:
        CAR_INTERFACE_DECL();

        LocalService(
            /* [in] */ CBatteryService* host);

        //@Override
        CARAPI IsPowered(
            /* [in] */ Int32 plugTypeSet,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetPlugType(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetBatteryLevel(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetBatteryLevelLow(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetInvalidCharger(
            /* [out] */ Int32* result);

    private:
        CBatteryService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ CBatteryService* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        CARAPI Observe();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI Update();

    public:
        CBatteryService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CBatteryService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnStart();

    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI UpdateBatteryWarningLevelLocked();

    CARAPI_(Boolean) IsPoweredLocked(
        /* [in] */ Int32 plugTypeSet);

    Boolean ShouldSendBatteryLowLocked();

    CARAPI_(void) ShutdownIfNoPowerLocked();

    CARAPI_(void) ShutdownIfOverTempLocked();

    CARAPI_(void) Update(
        /* [in] */ IBatteryProperties* props);

    CARAPI_(void) ProcessValuesLocked(
        /* [in] */ Boolean force);

    CARAPI_(void) SendIntentLocked();

    CARAPI_(void) LogBatteryStatsLocked();

    CARAPI_(void) LogOutlierLocked(
        /* [in] */ Int64 duration);

    CARAPI_(Int32) GetIconLocked(
        /* [in] */ Int32 level);

    CARAPI DumpInternal(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(void) UpdateLedPulse();

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const Int32 BATTERY_SCALE = 100;    // battery capacity is a percentage

    // Used locally for determining when to make a last ditch effort to log
    // discharge stats before the device dies.
    Int32 mCriticalBatteryLevel;

    static const Int32 DUMP_MAX_LENGTH = 24 * 1024;
    static const AutoPtr< ArrayOf<String> > DUMPSYS_ARGS;

    static const String DUMPSYS_DATA_PATH;

    // This should probably be exposed in the API, though it's not critical
    static const Int32 BATTERY_PLUGGED_NONE = 0;

    AutoPtr<IContext> mContext;
    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IHandler> mHandler;

    Object mLock;

    AutoPtr<IBatteryProperties> mBatteryProps;
    AutoPtr<IBatteryProperties> mLastBatteryProps;
    Boolean mBatteryLevelCritical;

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

    Boolean mBatteryLevelLow;

    Int64 mDischargeStartTime;
    Int32 mDischargeStartLevel;

    Boolean mUpdatesStopped;

    AutoPtr<Led> mLed;
    // Disable LED until SettingsObserver can be started
    Boolean mLightEnabled;
    Boolean mLedPulseEnabled;
    Int32 mBatteryLowARGB;
    Int32 mBatteryMediumARGB;
    Int32 mBatteryFullARGB;
    Boolean mMultiColorLed;

    Boolean mSentLowBatteryBroadcast;

    AutoPtr<InvalidChargerObserver> mInvalidChargerObserver;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBATTERYSERVICE_H__
