
#ifndef __ELASTOS_DROID_SERVER_POWER_POWERMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_POWER_POWERMANAGERSERVICE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Service.h>
#include <_Elastos.Droid.Server.h>
#include "elastos/droid/server/lights/Light.h"
#include "elastos/droid/server/lights/LightsManager.h"
#include "elastos/droid/server/power/Notifier.h"
#include "elastos/droid/server/power/WirelessChargerDetector.h"
#include "elastos/droid/server/power/PerformanceManager.h"
#include "elastos/droid/server/ServiceThread.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>
#include <utils/Timers.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::Display::IDisplayManagerInternal;
using Elastos::Droid::Hardware::Display::IDisplayPowerCallbacks;
using Elastos::Droid::Hardware::Display::IDisplayPowerRequest;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Os::IBatteryManagerInternal;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Server::Lights::Light;
using Elastos::Droid::Server::Lights::LightsManager;
using Elastos::Droid::Service::Dreams::IDreamManagerInternal;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Core::IRunnable;
using Elastos::Core::Thread;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

/**
 * The power manager service is responsible for coordinating power management
 * functions on the device.
 */
class PowerManagerService
    : public SystemService
    , public IWatchdogMonitor
{
public:
    class BinderService
        : public Object
        , public IIPowerManager
        , public IBinder
    {
    private:
        class WakeUpRunnable : public Runnable
        {
        public:
            WakeUpRunnable(
                /* [in] */ PowerManagerService* host,
                /* [in] */ Int64 eventTime,
                /* [in] */ Int32 uid)
                : mHost(host)
                , mEventTime(eventTime)
                , mUid(uid)
            {}

            CARAPI Run();

        private:
            PowerManagerService* mHost;
            Int64 mEventTime;
            Int32 mUid;
        };

        class SensorEventListener
            : public Object
            , public ISensorEventListener
        {
        public:
            SensorEventListener(
                /* [in] */ PowerManagerService* host,
                /* [in] */ IRunnable* r)
                : mHost(host)
                , mR(r)
            {}

            CAR_INTERFACE_DECL()

            CARAPI OnSensorChanged(
                /* [in] */ ISensorEvent* event);

            CARAPI OnAccuracyChanged(
                /* [in] */ ISensor* sensor,
                /* [in] */ Int32 accuracy);

        private:
            PowerManagerService* mHost;
            AutoPtr<IRunnable> mR;
        };

    public:
        CAR_INTERFACE_DECL()

        BinderService();

        ~BinderService();

        CARAPI constructor(
            /* [in] */ ISystemService* service);

        // @Override // Binder call
        CARAPI AcquireWakeLockWithUid(
            /* [in] */ IBinder* lock,
            /* [in] */ Int32 flags,
            /* [in] */ const String& tag,
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        // @Override // Binder call
        CARAPI PowerHint(
            /* [in] */ Int32 hintId,
            /* [in] */ Int32 data);

        // @Override // Binder call
        CARAPI AcquireWakeLock(
            /* [in] */ IBinder* lock,
            /* [in] */ Int32 flags,
            /* [in] */ const String& tag,
            /* [in] */ const String& packageName,
            /* [in] */ IWorkSource* ws,
            /* [in] */ const String& historyTag);

        // @Override // Binder call
        CARAPI ReleaseWakeLock(
            /* [in] */ IBinder* lock,
            /* [in] */ Int32 flags);

        // @Override // Binder call
        CARAPI UpdateWakeLockUids(
            /* [in] */ IBinder* lock,
            /* [in] */ ArrayOf<Int32>* uids);

        // @Override // Binder call
        CARAPI UpdateWakeLockWorkSource(
            /* [in] */ IBinder* lock,
            /* [in] */ IWorkSource* ws,
            /* [in] */ const String& historyTag);

        // @Override // Binder call
        CARAPI IsWakeLockLevelSupported(
            /* [in] */ Int32 level,
            /* [out] */ Boolean* res);

        // @Override // Binder call
        CARAPI UserActivity(
            /* [in] */ Int64 eventTime,
            /* [in] */ Int32 event,
            /* [in] */ Int32 flags);

        // @Override // Binder call
        CARAPI WakeUp(
            /* [in] */ Int64 eventTime);

        // @Override // Binder call
        CARAPI GoToSleep(
            /* [in] */ Int64 eventTime,
            /* [in] */ Int32 reason,
            /* [in] */ Int32 flags);

        // @Override // Binder call
        CARAPI Nap(
            /* [in] */ Int64 eventTime);

        // @Override // Binder call
        CARAPI IsInteractive(
            /* [out] */ Boolean* result);

        // @Override // Binder call
        CARAPI IsPowerSaveMode(
            /* [out] */ Boolean* result);

        // @Override // Binder call
        CARAPI SetPowerSaveMode(
            /* [in] */ Boolean mode,
            /* [out] */ Boolean* result);

        /**
         * Reboots the device.
         *
         * @param confirm If true, shows a reboot confirmation dialog.
         * @param reason The reason for the reboot, or null if none.
         * @param wait If true, this call waits for the reboot to complete and does not return.
         */
        // @Override // Binder call
        CARAPI Reboot(
            /* [in] */ Boolean confirm,
            /* [in] */ const String& reason,
            /* [in] */ Boolean wait);

        /**
         * Shuts down the device.
         *
         * @param confirm If true, shows a shutdown confirmation dialog.
         * @param wait If true, this call waits for the shutdown to complete and does not return.
         */
        // @Override // Binder call
        CARAPI Shutdown(
            /* [in] */ Boolean confirm,
            /* [in] */ Boolean wait);

        /**
         * Crash the runtime (causing a complete restart of the Android framework).
         * Requires REBOOT permission.  Mostly for testing.  Should not return.
         */
        // @Override // Binder call
        CARAPI Crash(
            /* [in] */ const String& message);

        /**
         * Set the setting that determines whether the device stays on when plugged in.
         * The argument is a bit string, with each bit specifying a power source that,
         * when the device is connected to that source, causes the device to stay on.
         * See {@link android.os.BatteryManager} for the list of power sources that
         * can be specified. Current values include
         * {@link android.os.BatteryManager#BATTERY_PLUGGED_AC}
         * and {@link android.os.BatteryManager#BATTERY_PLUGGED_USB}
         *
         * Used by "adb shell svc power stayon ..."
         *
         * @param val an {@code int} containing the bits that specify which power sources
         * should cause the device to stay on.
         */
        // @Override // Binder call
        CARAPI SetStayOnSetting(
            /* [in] */ Int32 val);

        /**
         * Used by device administration to set the maximum screen off timeout.
         *
         * This method must only be called by the device administration policy manager.
         */
        // @Override // Binder call
        CARAPI SetMaximumScreenOffTimeoutFromDeviceAdmin(
            /* [in] */ Int32 timeMs);

        /**
         * Used by the settings application and brightness control widgets to
         * temporarily override the current screen brightness setting so that the
         * user can observe the effect of an intended settings change without applying
         * it immediately.
         *
         * The override will be canceled when the setting value is next updated.
         *
         * @param brightness The overridden brightness.
         *
         * @see android.provider.Settings.System#SCREEN_BRIGHTNESS
         */
        // @Override // Binder call
        CARAPI SetTemporaryScreenBrightnessSettingOverride(
            /* [in] */ Int32 brightness);

        /**
         * Used by the settings application and brightness control widgets to
         * temporarily override the current screen auto-brightness adjustment setting so that the
         * user can observe the effect of an intended settings change without applying
         * it immediately.
         *
         * The override will be canceled when the setting value is next updated.
         *
         * @param adj The overridden brightness, or Float.NaN to disable the override.
         *
         * @see android.provider.Settings.System#SCREEN_AUTO_BRIGHTNESS_ADJ
         */
        // @Override // Binder call
        CARAPI SetTemporaryScreenAutoBrightnessAdjustmentSettingOverride(
            /* [in] */ Float adj);

        /**
         * Used by the phone application to make the attention LED flash when ringing.
         */
        // @Override // Binder call
        CARAPI SetAttentionLight(
            /* [in] */ Boolean on,
            /* [in] */ Int32 color);

        CARAPI ToString(
            /* [out] */ String* str);

        // update the uids being synchronized by network socket request manager
        CARAPI UpdateBlockedUids(
            /* [in] */ Int32 uid,
            /* [in] */ Boolean isBlocked);

        CARAPI CpuBoost(
            /* [in] */ Int32 duration);

        CARAPI SetKeyboardVisibility(
            /* [in] */ Boolean visible);

        CARAPI SetKeyboardLight(
            /* [in] */ Boolean on,
            /* [in] */ Int32 key);

        CARAPI WakeUpWithProximityCheck(
            /* [in] */ Int64 time);

        CARAPI SetPowerProfile(
            /* [in] */ const String& profile,
            /* [out] */ Boolean* result);

        CARAPI GetPowerProfile(
            /* [out] */ String* result);

        CARAPI ActivityResumed(
            /* [in] */ const String& componentName);

    protected:
        // @Override // Binder call
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        CARAPI_(void) RunWithProximityCheck(
            /* [in] */ IRunnable* r);

        CARAPI_(void) RunPostProximityCheck(
            /* [in] */ IRunnable* r);

    private:
        PowerManagerService* mHost;
    };

private:
    class BatteryReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PowerManagerService::BatteryReceiver: ")

        BatteryReceiver(
            /* [in] */ PowerManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PowerManagerService* mHost;
    };

    class DreamReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PowerManagerService::DreamReceiver: ")

        DreamReceiver(
            /* [in] */ PowerManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PowerManagerService* mHost;
    };

    class UserSwitchedReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PowerManagerService::UserSwitchedReceiver: ")

        UserSwitchedReceiver(
            /* [in] */ PowerManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PowerManagerService* mHost;
    };

    class DockReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PowerManagerService::DockReceiver: ")

        DockReceiver(
            /* [in] */ PowerManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PowerManagerService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("PowerManagerService::SettingsObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ PowerManagerService* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        PowerManagerService* mHost;
    };

    /**
     * Handler for asynchronous operations performed by the power manager.
     */
    class PowerManagerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PowerManagerService::PowerManagerHandler")

        PowerManagerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ PowerManagerService* service)
            : Handler(looper, NULL, TRUE)
            , mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PowerManagerService* mHost;
    };

    /**
     * Represents a wake lock that has been acquired by an application.
     */
    class WakeLock
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        WakeLock(
            /* [in] */ IBinder* lock,
            /* [in] */ Int32 flags,
            /* [in] */ const String& tag,
            /* [in] */ const String& packageName,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ const String& historyTag,
            /* [in] */ Int32 ownerUid,
            /* [in] */ Int32 ownerPid,
            /* [in] */ PowerManagerService* host);

        CARAPI ProxyDied();

        CARAPI_(Boolean) HasSameProperties(
            /* [in] */ Int32 flags,
            /* [in] */ const String& tag,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ Int32 ownerUid,
            /* [in] */ Int32 ownerPid);

        CARAPI UpdateProperties(
            /* [in] */ Int32 flags,
            /* [in] */ const String& tag,
            /* [in] */ const String& packageName,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ const String& historyTag,
            /* [in] */ Int32 ownerUid,
            /* [in] */ Int32 ownerPid);

        CARAPI_(Boolean) HasSameWorkSource(
            /* [in] */ IWorkSource* workSource);

        CARAPI_(void) UpdateWorkSource(
            /* [in] */ IWorkSource* workSource);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        // @SuppressWarnings("deprecation")
        CARAPI_(String) GetLockLevelString();

        CARAPI_(String) GetLockFlagsString();

    public:
        AutoPtr<IBinder> mLock;
        Int32 mFlags;
        String mTag;
        String mPackageName;
        AutoPtr<IWorkSource> mWorkSource;
        String mHistoryTag;
        Int32 mOwnerUid;
        Int32 mOwnerPid;
        Boolean mNotifiedAcquired;

    private:
        PowerManagerService* mHost;
    };

    class SuspendBlockerImpl
        : public Object
        , public ISuspendBlocker
    {
    public:
        CAR_INTERFACE_DECL()

        SuspendBlockerImpl(
            /* [in] */ const String& name);

        ~SuspendBlockerImpl();

        // @Override
        CARAPI AcquireBlocker();

        // @Override
        CARAPI ReleaseBlocker();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        String mName;
        String mTraceName;
        Int32 mReferenceCount;
    };

    class LocalService
        // : public PowerManagerInternal
        : public Object
        , public IPowerManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ PowerManagerService* host);

        ~LocalService();

        // @Override
        CARAPI SetScreenBrightnessOverrideFromWindowManager(
            /* [in] */ Int32 screenBrightness);

        // @Override
        CARAPI SetButtonBrightnessOverrideFromWindowManager(
            /* [in] */ Int32 screenBrightness);

        // @Override
        CARAPI SetDozeOverrideFromDreamManager(
            /* [in] */ Int32 screenState,
            /* [in] */ Int32 screenBrightness);

        // @Override
        CARAPI SetUserActivityTimeoutOverrideFromWindowManager(
            /* [in] */ Int64 timeoutMillis);

        // @Override
        CARAPI GetLowPowerModeEnabled(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI RegisterLowPowerModeObserver(
            /* [in] */ ILowPowerModeListener* listener);

    private:
        PowerManagerService* mHost;
    };

    // used in function UpdateLowPowerModeLocked();
    class UpdateLowPowerModeLockedRunnable
        : public Runnable
    {
    public:
        UpdateLowPowerModeLockedRunnable(
            /* [in] */ PowerManagerService* host,
            /* [in] */ Boolean arg);

        // @Override
        CARAPI Run();

    private:
        PowerManagerService* mHost;
        Boolean mArg;
    };

    class MyDisplayPowerCallbacks
        : public Object
        , public IDisplayPowerCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        MyDisplayPowerCallbacks(
            /* [in] */ PowerManagerService* host);

        // @Override
        CARAPI OnStateChanged();

        // @Override
        CARAPI OnProximityPositive();

        // @Override
        CARAPI OnProximityNegative();

        // @Override
        CARAPI OnDisplayStateChange(
            /* [in] */ Int32 state);

        // @Override
        CARAPI AcquireSuspendBlocker();

        // @Override
        CARAPI ReleaseSuspendBlocker();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        PowerManagerService* mHost;
        Int32 mDisplayState;
    };

    // used in function ShutdownOrRebootInternal();
    class ShutdownOrRebootRunnable
        : public Runnable
    {
    public:
        ShutdownOrRebootRunnable(
            /* [in] */ Boolean shutdown,
            /* [in] */ Boolean confirm,
            /* [in] */ const String& reason,
            /* [in] */ IContext* ctx);

        CARAPI Run();

    private:
        Boolean mShutdown;
        Boolean mConfirm;
        String mReason;
        AutoPtr<IContext> mContext;
    };

    // used in function CrashInternal();
    class CrashThread
        : public Thread
    {
    public:
        CrashThread(
            /* [in] */ const String& s);

        // @Override
        CARAPI Run();
    };

    class NativePowerManagerService : public android::BBinder
    {
    private:
        class ElLock
            : public Object
            , public Elastos::Droid::Os::IBinder
        {
        public:
            ElLock(
                /* [in] */ android::sp<android::IBinder> lock)
                : mLock(lock)
            {}

            CAR_INTERFACE_DECL()

            TO_STRING_IMPL("NativePowerManagerService::ElLock")

        private:
            android::sp<android::IBinder> mLock;
        };

    public:
        NativePowerManagerService(
            /* [in] */ BinderService* host)
            : mHost(host)
        {}

        android::status_t onTransact(
            /* [in] */ uint32_t code,
            /* [in] */ const android::Parcel& data,
            /* [in] */ android::Parcel* reply,
            /* [in] */ uint32_t flags = 0);

    private:
        BinderService* mHost;
        HashMap<Int64, AutoPtr<ElLock> > mLockMap;
    };

public:
    CAR_INTERFACE_DECL()

    PowerManagerService();

    virtual ~PowerManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

    CARAPI SystemReady(
        /* [in] */ IIAppOpsService* appOps);

    /**
     * Low-level function turn the device off immediately, without trying
     * to be clean.  Most people should use {@link ShutdownThread} for a clean shutdown.
     */
    static CARAPI_(void) LowLevelShutdown();

    /**
     * Low-level function to reboot the device. On success, this
     * function doesn't return. If more than 20 seconds passes from
     * the time a reboot is requested (120 seconds for reboot to
     * recovery), this method returns.
     *
     * @param reason code to pass to the kernel (e.g. "recovery"), or null.
     */
    static CARAPI LowLevelReboot(
        /* [in] */ const String& reason);

    // @Override // Watchdog.Monitor implementation
    CARAPI Monitor();

    // method for NativeInputManager
    static CARAPI_(void) UserActivity(
        /* [in] */ nsecs_t eventTime,
        /* [in] */ int32_t eventType);

    static CARAPI_(void) NativeSetPowerProfile(
        /* [in] */ Int32 profile);

protected:
    CARAPI_(void) UpdateLowPowerModeLocked();

private:
    CARAPI_(void) ReadConfigurationLocked();

    CARAPI_(void) UpdateSettingsLocked();

    CARAPI_(void) HandleSettingsChangedLocked();

    CARAPI AcquireWakeLockInternal(
        /* [in] */ IBinder* lock,
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ IWorkSource* ws,
        /* [in] */ const String& historyTag,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    // @SuppressWarnings("deprecation")
    static CARAPI_(Boolean) IsScreenLock(
        /* [in] */ WakeLock* wakeLock);

    CARAPI_(void) ApplyWakeLockFlagsOnAcquireLocked(
        /* [in] */ WakeLock* wakeLock,
        /* [in] */ Int32 uid);

    CARAPI_(void) ReleaseWakeLockInternal(
        /* [in] */ IBinder* lock,
        /* [in] */ Int32 flags);

    CARAPI_(void) HandleWakeLockDeath(
        /* [in] */ WakeLock* wakeLock);

    CARAPI_(void) RemoveWakeLockLocked(
        /* [in] */ WakeLock* wakeLock,
        /* [in] */ Int32 index);

    CARAPI_(void) ApplyWakeLockFlagsOnReleaseLocked(
        /* [in] */ WakeLock* wakeLock);

    CARAPI UpdateWakeLockWorkSourceInternal(
        /* [in] */ IBinder* lock,
        /* [in] */ IWorkSource* ws,
        /* [in] */ const String& historyTag,
        /* [in] */ Int32 callingUid);

    CARAPI_(Boolean) CheckWorkSourceObjectId(
        /* [in] */ Int32 uid,
        /* [in] */ WakeLock* wl);

    CARAPI_(Int32) FindWakeLockIndexLocked(
        /* [in] */ IBinder* lock);

    CARAPI_(void) NotifyWakeLockAcquiredLocked(
        /* [in] */ WakeLock* wakeLock);

    CARAPI_(void) NotifyWakeLockChangingLocked(
        /* [in] */ WakeLock* wakeLock,
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ IWorkSource* ws,
        /* [in] */ const String& historyTag);

    CARAPI_(void) NotifyWakeLockReleasedLocked(
        /* [in] */ WakeLock* wakeLock);

    // @SuppressWarnings("deprecation")
    CARAPI_(Boolean) IsWakeLockLevelSupportedInternal(
        /* [in] */ Int32 level);

    CARAPI_(Boolean) IsQuickBootCall();

    CARAPI_(void) UserActivityFromNative(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 event,
        /* [in] */ Int32 flags);

    CARAPI_(void) UserActivityInternal(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 event,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) UserActivityNoUpdateLocked(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 event,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 uid);

    CARAPI_(void) WakeUpInternal(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) WakeUpNoUpdateLocked(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 uid);

    CARAPI_(void) EnableQbCharger(
        /* [in] */ Boolean enable);

    CARAPI_(void) GoToSleepInternal(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 uid);

    // This method is called goToSleep for historical reasons but we actually start
    // dozing before really going to sleep.
    // @SuppressWarnings("deprecation")
    CARAPI_(Boolean) GoToSleepNoUpdateLocked(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 uid);

    CARAPI_(void) NapInternal(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) NapNoUpdateLocked(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 uid);

    // Done dozing, drop everything and go to sleep.
    CARAPI_(Boolean) ReallyGoToSleepNoUpdateLocked(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 uid);

    CARAPI_(void) SetInteractiveStateLocked(
        /* [in] */ Boolean interactive,
        /* [in] */ Int32 reason);

    CARAPI_(void) FinishInteractiveStateChangeLocked();

    /**
     * Updates the global power state based on dirty bits recorded in mDirty.
     *
     * This is the main function that performs power state transitions.
     * We centralize them here so that we can recompute the power state completely
     * each time something important changes, and ensure that we do it the same
     * way each time.  The point is to gather all of the transition logic here.
     */
    CARAPI_(void) UpdatePowerStateLocked();

    /**
      * Updates the value of mIsPowered.
      * Sets DIRTY_IS_POWERED if a change occurred.
      */
    CARAPI_(void) UpdateIsPoweredLocked(
        /* [in] */ Int32 dirty);

    CARAPI_(Boolean) ShouldWakeUpWhenPluggedOrUnpluggedLocked(
        /* [in] */ Boolean wasPowered,
        /* [in] */ Int32 oldPlugType,
        /* [in] */ Boolean dockedOnWirelessCharger);

    /**
     * Updates the value of mStayOn.
     * Sets DIRTY_STAY_ON if a change occurred.
     */
    CARAPI_(void) UpdateStayOnLocked(
        /* [in] */ Int32 dirty);

    /**
     * Updates the value of mWakeLockSummary to summarize the state of all active wake locks.
     * Note that most wake-locks are ignored when the system is asleep.
     *
     * This function must have no other side-effects.
     */
    // @SuppressWarnings("deprecation")
    CARAPI_(void) UpdateWakeLockSummaryLocked(
        /* [in] */ Int32 dirty);

    /**
     * Updates the value of mUserActivitySummary to summarize the user requested
     * state of the system such as whether the screen should be bright or dim.
     * Note that user activity is ignored when the system is asleep.
     *
     * This function must have no other side-effects.
     */
    CARAPI_(void) UpdateUserActivitySummaryLocked(
        /* [in] */ Int64 now,
        /* [in] */ Int32 dirty);

    /**
     * Called when a user activity timeout has occurred.
     * Simply indicates that something about user activity has changed so that the new
     * state can be recomputed when the power state is updated.
     *
     * This function must have no other side-effects besides setting the dirty
     * bit and calling update power state.  Wakefulness transitions are handled elsewhere.
     */
    CARAPI_(void) HandleUserActivityTimeout();

    CARAPI_(Int32) GetSleepTimeoutLocked();

    CARAPI_(Int32) GetScreenOffTimeoutLocked(
        /* [in] */ Int32 sleepTimeout);

    CARAPI_(Int32) GetScreenDimDurationLocked(
        /* [in] */ Int32 screenOffTimeout);

    /**
     * Updates the wakefulness of the device.
     *
     * This is the function that decides whether the device should start dreaming
     * based on the current wake locks and user activity state.  It may modify mDirty
     * if the wakefulness changes.
     *
     * Returns true if the wakefulness changed and we need to restart power state calculation.
     */
    CARAPI_(Boolean) UpdateWakefulnessLocked(
        /* [in] */ Int32 dirty);

    /**
     * Returns true if the device should automatically nap and start dreaming when the user
     * activity timeout has expired and it's bedtime.
     */
    CARAPI_(Boolean) ShouldNapAtBedTimeLocked();

    /**
     * Returns true if the device should go to sleep now.
     * Also used when exiting a dream to determine whether we should go back
     * to being fully awake or else go to sleep for good.
     */
    CARAPI_(Boolean) IsItBedTimeYetLocked();

    /**
     * Returns true if the device is being kept awake by a wake lock, user activity
     * or the stay on while powered setting.  We also keep the phone awake when
     * the proximity sensor returns a positive result so that the device does not
     * lock while in a phone call.  This function only controls whether the device
     * will go to sleep or dream which is independent of whether it will be allowed
     * to suspend.
     */
    CARAPI_(Boolean) IsBeingKeptAwakeLocked();

    /**
     * Determines whether to post a message to the sandman to update the dream state.
     */
    CARAPI_(void) UpdateDreamLocked(
        /* [in] */ Int32 dirty,
        /* [in] */ Boolean displayBecameReady);

    CARAPI_(void) ScheduleSandmanLocked();

    /**
     * Called when the device enters or exits a dreaming or dozing state.
     *
     * We do this asynchronously because we must call out of the power manager to start
     * the dream and we don't want to hold our lock while doing so.  There is a risk that
     * the device will wake or go to sleep in the meantime so we have to handle that case.
     */
    CARAPI_(void) HandleSandman(); // runs on handler thread

    /**
     * Returns true if the device is allowed to dream in its current state
     */
    CARAPI_(Boolean) CanDreamLocked();

    /**
     * Returns true if the device is allowed to doze in its current state.
     */
    CARAPI_(Boolean) CanDozeLocked();

    /**
     * Updates the display power state asynchronously.
     * When the update is finished, mDisplayReady will be set to true.  The display
     * controller posts a message to tell us when the actual display power state
     * has been updated so we come back here to double-check and finish up.
     *
     * This function recalculates the display power state each time.
     *
     * @return True if the display became ready.
     */
    CARAPI_(Boolean) UpdateDisplayPowerStateLocked(
        /* [in] */ Int32 dirty);

    static CARAPI_(Boolean) IsValidBrightness(
        /* [in] */ Int32 value);

    static CARAPI_(Boolean) IsValidAutoBrightnessAdjustment(
        /* [in] */ Float value);

    CARAPI_(Int32) GetDesiredScreenPolicyLocked();

    CARAPI_(Boolean) ShouldUseProximitySensorLocked();

    /**
     * Updates the suspend blocker that keeps the CPU alive.
     *
     * This function must have no other side-effects.
     */
    CARAPI_(void) UpdateSuspendBlockerLocked();

    /**
     * Return true if we must keep a suspend blocker active on behalf of the display.
     * We do so if the screen is on or is in transition between states.
     */
    CARAPI_(Boolean) NeedDisplaySuspendBlockerLocked();

    CARAPI_(void) SetHalAutoSuspendModeLocked(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetHalInteractiveModeLocked(
        /* [in] */ Boolean enable);

    CARAPI_(Boolean) IsInteractiveInternal();

    CARAPI_(Boolean) IsLowPowerModeInternal();

    CARAPI_(Boolean) SetLowPowerModeInternal(
        /* [in] */ Boolean mode);

    CARAPI_(void) HandleBatteryStateChangedLocked();

    CARAPI ShutdownOrRebootInternal(
        /* [in] */ Boolean shutdown,
        /* [in] */ Boolean confirm,
        /* [in] */ const String& reason,
        /* [in] */ Boolean wait);

    CARAPI CrashInternal(
        /* [in] */ const String& message);

    CARAPI_(void) SetStayOnSettingInternal(
        /* [in] */ Int32 val);

    CARAPI_(void) SetMaximumScreenOffTimeoutFromDeviceAdminInternal(
        /* [in] */ Int32 timeMs);

    CARAPI_(Boolean) IsMaximumScreenOffTimeoutFromDeviceAdminEnforcedLocked();

    CARAPI_(void) SetAttentionLightInternal(
        /* [in] */ Boolean on,
        /* [in] */ Int32 color);

    CARAPI_(void) SetScreenBrightnessOverrideFromWindowManagerInternal(
        /* [in] */ Int32 brightness);

    CARAPI_(void) SetUserActivityTimeoutOverrideFromWindowManagerInternal(
        /* [in] */ Int64 timeoutMillis);

    CARAPI_(void) SetTemporaryScreenBrightnessSettingOverrideInternal(
        /* [in] */ Int32 brightness);

    CARAPI_(void) SetTemporaryScreenAutoBrightnessAdjustmentSettingOverrideInternal(
        /* [in] */ Float adj);

    CARAPI_(void) SetDozeOverrideFromDreamManagerInternal(
        /* [in] */ Int32 screenState,
        /* [in] */ Int32 screenBrightness);

    CARAPI_(void) PowerHintInternal(
        /* [in] */ Int32 hintId,
        /* [in] */ Int32 data);

    CARAPI_(void) DumpInternal(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(AutoPtr<ISuspendBlocker>) CreateSuspendBlockerLocked(
        /* [in] */ const String& name);

    static CARAPI_(String) WakefulnessToString(
        /* [in] */ Int32 wakefulness);

    static CARAPI_(AutoPtr<IWorkSource>) CopyWorkSource(
        /* [in] */ IWorkSource* workSource);

    // native
    CARAPI_(void) NativeInit();

    static CARAPI_(void) NativeAcquireSuspendBlocker(
        /* [in] */ const String& name);

    static CARAPI_(void) NativeReleaseSuspendBlocker(
        /* [in] */ const String& name);

    static CARAPI_(void) NativeSetInteractive(
        /* [in] */ Boolean enable);

    static CARAPI_(void) NativeSetAutoSuspend(
        /* [in] */ Boolean enable);

    static CARAPI_(void) NativeSendPowerHint(
        /* [in] */ Int32 hintId,
        /* [in] */ Int32 data);

    static CARAPI_(void) NativeCpuBoost(
        /* [in] */ Int32 duration);

    CARAPI_(void) CleanupProximity();

    CARAPI_(void) SetButtonBrightnessOverrideFromWindowManagerInternal(
        /* [in] */ Int32 brightness);

private:
    static const String TAG;

    static const Boolean DEBUG;
    static const Boolean DEBUG_SPEW;

    // Message: Sent when a user activity timeout occurs to update the power state.
    static const Int32 MSG_USER_ACTIVITY_TIMEOUT;// = 1;
    // Message: Sent when the device enters or exits a dreaming or dozing state.
    static const Int32 MSG_SANDMAN;// = 2;
    static const Int32 MSG_WAKE_UP;

    // Dirty bit: mWakeLocks changed
    static const Int32 DIRTY_WAKE_LOCKS;
    // Dirty bit: mWakefulness changed
    static const Int32 DIRTY_WAKEFULNESS;
    // Dirty bit: user activity was poked or may have timed out
    static const Int32 DIRTY_USER_ACTIVITY;
    // Dirty bit: actual display power state was updated asynchronously
    static const Int32 DIRTY_ACTUAL_DISPLAY_POWER_STATE_UPDATED;
    // Dirty bit: mBootCompleted changed
    static const Int32 DIRTY_BOOT_COMPLETED;
    // Dirty bit: settings changed
    static const Int32 DIRTY_SETTINGS;
    // Dirty bit: mIsPowered changed
    static const Int32 DIRTY_IS_POWERED;
    // Dirty bit: mStayOn changed
    static const Int32 DIRTY_STAY_ON;
    // Dirty bit: battery state changed
    static const Int32 DIRTY_BATTERY_STATE;
    // Dirty bit: proximity state changed
    static const Int32 DIRTY_PROXIMITY_POSITIVE;
    // Dirty bit: dock state changed
    static const Int32 DIRTY_DOCK_STATE;

    // Wakefulness: The device is asleep and can only be awoken by a call to wakeUp().
    // The screen should be off or in the process of being turned off by the display controller.
    // The device typically passes through the dozing state first.
    static const Int32 WAKEFULNESS_ASLEEP;
    // Wakefulness: The device is fully awake.  It can be put to sleep by a call to goToSleep().
    // When the user activity timeout expires, the device may start dreaming or go to sleep.
    static const Int32 WAKEFULNESS_AWAKE;
    // Wakefulness: The device is dreaming.  It can be awoken by a call to wakeUp(),
    // which ends the dream.  The device goes to sleep when goToSleep() is called, when
    // the dream ends or when unplugged.
    // User activity may brighten the screen but does not end the dream.
    static const Int32 WAKEFULNESS_DREAMING;

    // Wakefulness: The device is dozing.  It is almost asleep but is allowing a special
    // low-power "doze" dream to run which keeps the display on but lets the application
    // processor be suspended.  It can be awoken by a call to wakeUp() which ends the dream.
    // The device fully goes to sleep if the dream cannot be started or ends on its own.
    static const Int32 WAKEFULNESS_DOZING;

    // Summarizes the state of all active wakelocks.
    static const Int32 WAKE_LOCK_CPU;
    static const Int32 WAKE_LOCK_SCREEN_BRIGHT;
    static const Int32 WAKE_LOCK_SCREEN_DIM;
    static const Int32 WAKE_LOCK_BUTTON_BRIGHT;
    static const Int32 WAKE_LOCK_PROXIMITY_SCREEN_OFF;
    static const Int32 WAKE_LOCK_STAY_AWAKE; // only set if already awake
    static const Int32 WAKE_LOCK_DOZE;

    // Summarizes the user activity state.
    static const Int32 USER_ACTIVITY_SCREEN_BRIGHT;
    static const Int32 USER_ACTIVITY_SCREEN_DIM;
    static const Int32 USER_ACTIVITY_SCREEN_DREAM;

    // Default timeout in milliseconds.  This is only used until the settings
    // provider populates the actual default value (R.integer.def_screen_off_timeout).
    static const Int32 DEFAULT_SCREEN_OFF_TIMEOUT;
    static const Int32 DEFAULT_SLEEP_TIMEOUT;

    // Power hints defined in hardware/libhardware/include/hardware/power.h.
    static const Int32 POWER_HINT_INTERACTION;
    static const Int32 POWER_HINT_LOW_POWER;

    static const Int32 DEFAULT_BUTTON_ON_DURATION;

    // Config value for NSRM
    static const Int32 DPM_CONFIG_FEATURE_MASK_NSRM;

    // Max time (microseconds) to allow a CPU boost for
    static const Int32 MAX_CPU_BOOST_TIME;

    static const Float PROXIMITY_NEAR_THRESHOLD;

    AutoPtr<IContext> mContext;
    AutoPtr<ServiceThread> mHandlerThread;
    AutoPtr<PowerManagerHandler> mHandler;

    AutoPtr<LightsManager> mLightsManager;
    AutoPtr<IBatteryManagerInternal> mBatteryManagerInternal;
    AutoPtr<IDisplayManagerInternal> mDisplayManagerInternal;
    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IIAppOpsService> mAppOps;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<Notifier> mNotifier;
    AutoPtr<WirelessChargerDetector> mWirelessChargerDetector;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<IDreamManagerInternal> mDreamManager;
    AutoPtr<Light> mAttentionLight;
    AutoPtr<Light> mButtonsLight;
    AutoPtr<Light> mKeyboardLight;
    AutoPtr<Light> mCapsLight;
    AutoPtr<Light> mFnLight;

    Int32 mButtonTimeout;
    Int32 mButtonBrightness;
    Int32 mButtonBrightnessSettingDefault;
    Int32 mKeyboardBrightness;
    Int32 mKeyboardBrightnessSettingDefault;

    Object mLock;

    // A bitfield that indicates what parts of the power state have
    // changed and need to be recalculated.
    Int32 mDirty;

    // Indicates whether the device is awake or asleep or somewhere in between.
    // This is distinct from the screen power state, which is managed separately.
    Int32 mWakefulness;

    // True if the sandman has just been summoned for the first time since entering the
    // dreaming or dozing state.  Indicates whether a new dream should begin.
    Boolean mSandmanSummoned;

    // True if MSG_SANDMAN has been scheduled.
    Boolean mSandmanScheduled;

    // Table of all suspend blockers.
    // There should only be a few of these.
    // private final ArrayList<SuspendBlocker> mSuspendBlockers = new ArrayList<SuspendBlocker>();
    AutoPtr<IArrayList> mSuspendBlockers;

    // Table of all wake locks acquired by applications.
    // private final ArrayList<WakeLock> mWakeLocks = new ArrayList<WakeLock>();
    AutoPtr<IArrayList> mWakeLocks;

    // A bitfield that summarizes the state of all active wakelocks.
    Int32 mWakeLockSummary;

    // True if the device is in an interactive state.
    Boolean mInteractive;
    Boolean mInteractiveChanging;

    // If true, instructs the display controller to wait for the proximity sensor to
    // go negative before turning the screen on.
    Boolean mRequestWaitForNegativeProximity;

    // Timestamp of the last time the device was awoken or put to sleep.
    Int64 mLastWakeTime;
    Int64 mLastSleepTime;

    // Timestamp of the last call to user activity.
    Int64 mLastUserActivityTime;
    Int64 mLastUserActivityTimeNoChangeLights;

    // Timestamp of last interactive power hint.
    Int64 mLastInteractivePowerHintTime;

    // A bitfield that summarizes the effect of the user activity timer.
    Int32 mUserActivitySummary;

    // The desired display power state.  The actual state may lag behind the
    // requested because it is updated asynchronously by the display power controller.
    AutoPtr<IDisplayPowerRequest> mDisplayPowerRequest;

    // True if the display power state has been fully applied, which means the display
    // is actually on or actually off or whatever was requested.
    Boolean mDisplayReady;

    // The suspend blocker used to keep the CPU alive when an application has acquired
    // a wake lock.
    AutoPtr<ISuspendBlocker> mWakeLockSuspendBlocker;

    // True if the wake lock suspend blocker has been acquired.
    Boolean mHoldingWakeLockSuspendBlocker;

    // The suspend blocker used to keep the CPU alive when the display is on, the
    // display is getting ready or there is user activity (in which case the display
    // must be on).
    AutoPtr<ISuspendBlocker> mDisplaySuspendBlocker;

    // True if the display suspend blocker has been acquired.
    Boolean mHoldingDisplaySuspendBlocker;

    // True if systemReady() has been called.
    Boolean mSystemReady;

    // True if boot completed occurred.  We keep the screen on until this happens.
    Boolean mBootCompleted;

    // True if auto-suspend mode is enabled.
    // Refer to autosuspend.h.
    Boolean mHalAutoSuspendModeEnabled;

    // True if interactive mode is enabled.
    // Refer to power.h.
    Boolean mHalInteractiveModeEnabled;

    // True if the device is plugged into a power source.
    Boolean mIsPowered;

    // The current plug type, such as BatteryManager.BATTERY_PLUGGED_WIRELESS.
    Int32 mPlugType;

    // The current battery level percentage.
    Int32 mBatteryLevel;

    // The battery level percentage at the time the dream started.
    // This is used to terminate a dream and go to sleep if the battery is
    // draining faster than it is charging and the user activity timeout has expired.
    Int32 mBatteryLevelWhenDreamStarted;

    // The current dock state.
    Int32 mDockState;

    // True to decouple auto-suspend mode from the display state.
    Boolean mDecoupleHalAutoSuspendModeFromDisplayConfig;

    // True to decouple interactive mode from the display state.
    Boolean mDecoupleHalInteractiveModeFromDisplayConfig;

    // True if the device should wake up when plugged or unplugged.
    Boolean mWakeUpWhenPluggedOrUnpluggedConfig;

    // True if the device should suspend when the screen is off due to proximity.
    Boolean mSuspendWhenScreenOffDueToProximityConfig;

    // True if dreams are supported on this device.
    Boolean mDreamsSupportedConfig;

    // Default value for dreams enabled
    Boolean mDreamsEnabledByDefaultConfig;

    // Default value for dreams activate-on-sleep
    Boolean mDreamsActivatedOnSleepByDefaultConfig;

    // Default value for dreams activate-on-dock
    Boolean mDreamsActivatedOnDockByDefaultConfig;

    // True if dreams can run while not plugged in.
    Boolean mDreamsEnabledOnBatteryConfig;

    // Minimum battery level to allow dreaming when powered.
    // Use -1 to disable this safety feature.
    Int32 mDreamsBatteryLevelMinimumWhenPoweredConfig;

    // Minimum battery level to allow dreaming when not powered.
    // Use -1 to disable this safety feature.
    Int32 mDreamsBatteryLevelMinimumWhenNotPoweredConfig;

    // If the battery level drops by this percentage and the user activity timeout
    // has expired, then assume the device is receiving insufficient current to charge
    // effectively and terminate the dream.  Use -1 to disable this safety feature.
    Int32 mDreamsBatteryLevelDrainCutoffConfig;

    // True if dreams are enabled by the user.
    Boolean mDreamsEnabledSetting;

    // True if dreams should be activated on sleep.
    Boolean mDreamsActivateOnSleepSetting;

    // True if dreams should be activated on dock.
    Boolean mDreamsActivateOnDockSetting;

    // True if doze should not be started until after the screen off transition.
    Boolean mDozeAfterScreenOffConfig;

    // The minimum screen off timeout, in milliseconds.
    Int32 mMinimumScreenOffTimeoutConfig;

    // The screen dim duration, in milliseconds.
    // This is subtracted from the end of the screen off timeout so the
    // minimum screen off timeout should be longer than this.
    Int32 mMaximumScreenDimDurationConfig;

    // The maximum screen dim time expressed as a ratio relative to the screen
    // off timeout.  If the screen off timeout is very short then we want the
    // dim timeout to also be quite short so that most of the time is spent on.
    // Otherwise the user won't get much screen on time before dimming occurs.
    Float mMaximumScreenDimRatioConfig;

    // The screen off timeout setting value in milliseconds.
    Int32 mScreenOffTimeoutSetting;

    // The sleep timeout setting value in milliseconds.
    Int32 mSleepTimeoutSetting;

    // The maximum allowable screen off timeout according to the device
    // administration policy.  Overrides other settings.
    Int32 mMaximumScreenOffTimeoutFromDeviceAdmin;//Integer.MAX_VALUE;

    // The stay on while plugged in setting.
    // A bitfield of battery conditions under which to make the screen stay on.
    Int32 mStayOnWhilePluggedInSetting;

    // True if the device should wake up when plugged or unplugged
    Int32 mWakeUpWhenPluggedOrUnpluggedSetting;

    // True if the device should stay on.
    Boolean mStayOn;

    // True if the proximity sensor reads a positive result.
    Boolean mProximityPositive;

    // Screen brightness setting limits.
    Int32 mScreenBrightnessSettingMinimum;
    Int32 mScreenBrightnessSettingMaximum;
    Int32 mScreenBrightnessSettingDefault;

    // The screen brightness setting, from 0 to 255.
    // Use -1 if no value has been set.
    Int32 mScreenBrightnessSetting;

    // The screen auto-brightness adjustment setting, from -1 to 1.
    // Use 0 if there is no adjustment.
    Float mScreenAutoBrightnessAdjustmentSetting;

    // The screen brightness mode.
    // One of the Settings.System.SCREEN_BRIGHTNESS_MODE_* constants.
    Int32 mScreenBrightnessModeSetting;

    // The screen brightness setting override from the window manager
    // to allow the current foreground activity to override the brightness.
    // Use -1 to disable.
    Int32 mScreenBrightnessOverrideFromWindowManager; // = -1

    // The button brightness setting override from the window manager
    // to allow the current foreground activity to override the button brightness.
    // Use -1 to disable.
    Int32 mButtonBrightnessOverrideFromWindowManager;

    // The user activity timeout override from the window manager
    // to allow the current foreground activity to override the user activity timeout.
    // Use -1 to disable.
    Int64 mUserActivityTimeoutOverrideFromWindowManager; // = -1

    // The screen brightness setting override from the settings application
    // to temporarily adjust the brightness until next updated,
    // Use -1 to disable.
    Int32 mTemporaryScreenBrightnessSettingOverride; // = -1

    // The screen brightness adjustment setting override from the settings
    // application to temporarily adjust the auto-brightness adjustment factor
    // until next updated, in the range -1..1.
    // Use NaN to disable.
    Float mTemporaryScreenAutoBrightnessAdjustmentSettingOverride; // = Float.NaN

    // The screen state to use while dozing.
    Int32 mDozeScreenStateOverrideFromDreamManager; // = Display.STATE_UNKNOWN;

    // The screen brightness to use while dozing.
    Int32 mDozeScreenBrightnessOverrideFromDreamManager; // = PowerManager.BRIGHTNESS_DEFAULT;

    // Time when we last logged a warning about calling userActivity() without permission.
    Int64 mLastWarningAboutUserActivityPermission; // = Long.MIN_VALUE

    // If true, the device is in low power mode.
    Boolean mLowPowerModeEnabled;

    // Current state of the low power mode setting.
    Boolean mLowPowerModeSetting;

    // Current state of whether the settings are allowing auto low power mode.
    Boolean mAutoLowPowerModeConfigured;

    // The user turned off low power mode below the trigger level
    Boolean mAutoLowPowerModeSnoozing;

    // True if the battery level is currently considered low.
    Boolean mBatteryLevelLow;

    AutoPtr<IArrayList> mLowPowerModeListeners;
    // private final ArrayList<PowerManagerInternal.LowPowerModeListener> mLowPowerModeListeners
    //         = new ArrayList<PowerManagerInternal.LowPowerModeListener>();

    //track the blocked uids.
    AutoPtr<IArrayList> mBlockedUids;

    AutoPtr<IDisplayPowerCallbacks> mDisplayPowerCallbacks;

    Boolean mKeyboardVisible;

    AutoPtr<ISensorManager> mSensorManager;
    AutoPtr<ISensor> mProximitySensor;
    Boolean mProximityWakeEnabled;
    Int32 mProximityTimeOut;
    Boolean mProximityWakeSupported;
    AutoPtr<IPowerManagerWakeLock> mProximityWakeLock;
    AutoPtr<ISensorEventListener> mProximityListener;

    AutoPtr<PerformanceManager> mPerformanceManager;

    android::sp<NativePowerManagerService> mNative;

    friend class MyDisplayPowerCallbacks;
    friend class WakeLock;
    friend class SuspendBlockerImpl;
    friend class BinderService;
};

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_POWERMANAGERSERVICE_H__
