
#ifndef __ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__

#include "_Elastos_Droid_Server_Am_CBatteryStatsService.h"
#include "elastos/droid/os/BatteryStatsImpl.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::BatteryStatsImpl;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CBatteryStatsService)
{
private:
    class BluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        BluetoothProfileServiceListener(
            /* [in] */ CBatteryStatsService* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        CBatteryStatsService* mHost;
    };

public:
    CBatteryStatsService();

    CARAPI constructor(
        /* [in] */ const String& filename);

    CARAPI_(void) Publish(
        /* [in] */ IContext* context);

    CARAPI_(void) Shutdown();

    static CARAPI_(AutoPtr<IIBatteryStats>) GetService();

   /**
    * @return the current statistics object, which may be modified
    * to reflect events that affect battery usage.  You must lock the
    * stats object before doing anything with it.
    */
    CARAPI_(AutoPtr<BatteryStatsImpl>) GetActiveStatistics();

    CARAPI GetStatistics(
        /* [out, callee] */ ArrayOf<Byte>** statistics);

    CARAPI NoteStartWakelock(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ Int32 type);

    CARAPI NoteStopWakelock(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ Int32 type);

   CARAPI NoteStartWakelockFromSource(
       /* [in] */ IWorkSource* ws,
       /* [in] */ Int32 pid,
       /* [in] */ const String& name,
       /* [in] */ Int32 type);

   CARAPI NoteStopWakelockFromSource(
       /* [in] */ IWorkSource* ws,
       /* [in] */ Int32 pid,
       /* [in] */ const String& name,
       /* [in] */ Int32 type);

    CARAPI NoteStartSensor(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteStopSensor(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteStartGps(
        /* [in] */ Int32 uid);

    CARAPI NoteStopGps(
        /* [in] */ Int32 uid);

    CARAPI NoteScreenOn();

    CARAPI NoteScreenBrightness(
        /* [in] */ Int32 brightness);

    CARAPI NoteScreenOff();

    CARAPI NoteInputEvent();

    CARAPI NoteUserActivity(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 event);

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

    CARAPI NoteWifiRunning(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiRunningChanged(
        /* [in] */ IWorkSource* oldWs,
        /* [in] */ IWorkSource* newWs);

    CARAPI NoteWifiStopped(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteBluetoothOn();

    CARAPI NoteBluetoothOff();

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

    CARAPI NoteWifiMulticastEnabledFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiMulticastDisabledFromSource(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteNetworkInterfaceType(
        /* [in] */ const String& iface,
        /* [in] */ Int32 type);

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

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) DumpHelp(
        /* [in] */ IPrintWriter* pw);

public:
    static AutoPtr<IIBatteryStats> sService;

    AutoPtr<BatteryStatsImpl> mStats;
    Object mStatsLock;
    AutoPtr<IContext> mContext;

private:
    Boolean mBluetoothPendingStats;
    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;

    AutoPtr<IBluetoothProfileServiceListener> mBluetoothProfileServiceListener;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__
