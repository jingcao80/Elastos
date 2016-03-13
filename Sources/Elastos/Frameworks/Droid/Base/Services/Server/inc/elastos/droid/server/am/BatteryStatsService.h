
#ifndef __ELASTOS_DROID_SERVER_AM_BATTERYSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_BATTERYSTATSSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Bluetooth.h"
#include "elastos/core/Thread.h"

using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Os::IBatteryStatsImpl;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class BatteryStatsService
    : public Object
    , public IIBatteryStats
    , public ILowPowerModeListener
    , public IBinder
{
private:
    class BluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        CAR_INTERFACE_DECL()

        BluetoothProfileServiceListener(
            /* [in] */ BatteryStatsService* host)
            : mHost(host)
        {}

        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        BatteryStatsService* mHost;
    };

    class WakeupReasonThread
        : public Thread
    {
    public:
        WakeupReasonThread(
            /* [in] */ BatteryStatsService* owner);

        CARAPI Run();

    private:
        AutoPtr<ArrayOf<Int32> > mIrqs;
        AutoPtr<ArrayOf<String> > mReasons;
        BatteryStatsService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BatteryStatsService();

    CARAPI constructor(
        /* [in] */ IFile* systemDir,
        /* [in] */ IHandler* handler);

    CARAPI Publish(
        /* [in] */ IContext* context);

    /**
      * At the time when the constructor runs, the power manager has not yet been
      * initialized.  So we initialize the low power observer later.
    */
    CARAPI_(void) InitPowerManagement();

    CARAPI_(void) Shutdown();

    static CARAPI_(AutoPtr<IIBatteryStats>) GetService();

    CARAPI OnLowPowerModeChanged(
        /* [in] */ Boolean enabled);

   /**
    * @return the current statistics object, which may be modified
    * to reflect events that affect battery usage.  You must lock the
    * stats object before doing anything with it.
    */
    CARAPI_(AutoPtr<IBatteryStatsImpl>) GetActiveStatistics();

    // These are for direct use by the activity manager...
    CARAPI_(void) AddIsolatedUid(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI_(void) RemoveIsolatedUid(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI_(void) NoteProcessStart(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteProcessState(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 state);

    CARAPI_(void) NoteProcessFinish(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI GetStatistics(
        /* [out, callee] */ ArrayOf<Byte>** statistics);

    CARAPI GetStatisticsStream(
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI ComputeBatteryTimeRemaining(
        /* [out] */ Int64* timeRemaining);

    CARAPI ComputeChargeTimeRemaining(
        /* [out] */ Int64* timeRemaining);

    CARAPI NoteEvent(
        /* [in] */ Int32 code,
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteSyncStart(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteSyncFinish(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteJobStart(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteJobFinish(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteStartWakelock(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Boolean unimportantForLogging);

    CARAPI NoteStopWakelock(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type);

   CARAPI NoteStartWakelockFromSource(
       /* [in] */ IWorkSource* ws,
       /* [in] */ Int32 pid,
       /* [in] */ const String& name,
       /* [in] */ const String& historyName,
       /* [in] */ Int32 type,
       /* [in] */ Boolean unimportantForLogging);

   CARAPI NoteChangeWakelockFromSource(
       /* [in] */ IWorkSource* ws,
       /* [in] */ Int32 pid,
       /* [in] */ const String& name,
       /* [in] */ const String& historyName,
       /* [in] */ Int32 type,
       /* [in] */ IWorkSource* newWs,
       /* [in] */ Int32 newPid,
       /* [in] */ const String& newName,
       /* [in] */ const String& newHistoryName,
       /* [in] */ Int32 newType,
       /* [in] */ Boolean newUnimportantForLogging);

   CARAPI NoteStopWakelockFromSource(
       /* [in] */ IWorkSource* ws,
       /* [in] */ Int32 pid,
       /* [in] */ const String& name,
       /* [in] */ const String& newHistoryName,
       /* [in] */ Int32 type);

    CARAPI NoteStartSensor(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteStopSensor(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteVibratorOn(
        /* [in] */ Int32 uid,
        /* [in] */ Int64 durationMillis);

    CARAPI NoteVibratorOff(
        /* [in] */ Int32 uid);

    CARAPI NoteStartGps(
        /* [in] */ Int32 uid);

    CARAPI NoteStopGps(
        /* [in] */ Int32 uid);

    CARAPI NoteScreenState(
        /* [in] */ Int32 state);

    CARAPI NoteScreenBrightness(
        /* [in] */ Int32 brightness);

    CARAPI NoteUserActivity(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 event);

    CARAPI NoteInteractive(
        /* [in] */ Boolean interactive);

    CARAPI NoteMobileRadioPowerState(
        /* [in] */ Int32 powerState,
        /* [in] */ Int64 timestampNs);

    CARAPI NotePhoneOn();

    CARAPI NotePhoneOff();

    CARAPI NotePhoneSignalStrength(
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI NotePhoneDataConnectionState(
        /* [in] */ Int32 dataType,
        /* [in] */ Boolean hasData);

    CARAPI NotePhoneState(
        /* [in] */ Int32 state);

    CARAPI NoteWifiOn();

    CARAPI NoteWifiOff();

    CARAPI NoteStartAudio(
        /* [in] */ Int32 uid);

    CARAPI NoteStopAudio(
        /* [in] */ Int32 uid);

    CARAPI NoteStartVideo(
        /* [in] */ Int32 uid);

    CARAPI NoteStopVideo(
        /* [in] */ Int32 uid);

    CARAPI NoteResetAudio();

    CARAPI NoteResetVideo();

    CARAPI NoteFlashlightOn();

    CARAPI NoteFlashlightOff();

    CARAPI NoteWifiRunning(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiRunningChanged(
        /* [in] */ IWorkSource* oldWs,
        /* [in] */ IWorkSource* newWs);

    CARAPI NoteWifiStopped(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiState(
        /* [in] */ Int32 wifiState,
        /* [in] */ const String& accessPoint);

    CARAPI NoteWifiSupplicantStateChanged(
        /* [in] */ Int32 supplState,
        /* [in] */ Boolean failedAuth);

    CARAPI NoteWifiRssiChanged(
        /* [in] */ Int32 newRssi);

    CARAPI NoteBluetoothOn();

    CARAPI NoteBluetoothOff();

    CARAPI NoteBluetoothState(
        /* [in] */ Int32 bluetoothState);

    CARAPI NoteFullWifiLockAcquired(
        /* [in] */ Int32 uid);

    CARAPI NoteFullWifiLockReleased(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiScanStarted(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiScanStopped(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiMulticastEnabled(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiMulticastDisabled(
        /* [in] */ Int32 uid);

    CARAPI NoteFullWifiLockAcquiredFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteFullWifiLockReleasedFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiScanStartedFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiScanStoppedFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiBatchedScanStartedFromSource(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 csph);

    CARAPI NoteWifiBatchedScanStoppedFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiMulticastEnabledFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiMulticastDisabledFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteNetworkInterfaceType(
        /* [in] */ const String& iface,
        /* [in] */ Int32 type);

    CARAPI NoteNetworkStatsEnabled();

    CARAPI_(Boolean) IsOnBattery();

    CARAPI SetBatteryState(
        /* [in] */ Int32 status,
        /* [in] */ Int32 health,
        /* [in] */ Int32 plugType,
        /* [in] */ Int32 level,
        /* [in] */ Int32 temp,
        /* [in] */ Int32 volt);

    CARAPI GetAwakeTimeBattery(
        /* [out] */ Int64* time);

    CARAPI GetAwakeTimePlugged(
        /* [out] */ Int64* time);

    CARAPI_(void) EnforceCallingPermission();

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    static CARAPI_(Int32) NativeWaitWakeup(
        /* [in] */ ArrayOf<Int32>* outIrqs,
        /* [in] */ ArrayOf<String>* outReasons);

    CARAPI_(void) DumpHelp(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(Int32) DoEnableOrDisable(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 i,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Boolean enable);

public:
    static const String TAG;
    static AutoPtr<IIBatteryStats> sService;

    AutoPtr<IBatteryStatsImpl> mStats;
    Object mStatsLock;
    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerInternal> mPowerManagerInternal;

private:
    Boolean mBluetoothPendingStats;
    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;
    AutoPtr<IBluetoothProfileServiceListener> mBluetoothProfileServiceListener;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_BATTERYSTATSSERVICE_H__
