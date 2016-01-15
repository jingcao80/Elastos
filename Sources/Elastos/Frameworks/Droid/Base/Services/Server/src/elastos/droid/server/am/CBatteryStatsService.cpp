
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::App::EIID_IIBatteryStats;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(CBatteryStatsService::BluetoothProfileServiceListener, IBluetoothProfileServiceListener)

ECode CBatteryStatsService::BluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
    AutoLock lock(mHost->mStatsLock);
    if (mHost->mBluetoothPendingStats) {
        mHost->mStats->NoteBluetoothOnLocked();
        mHost->mStats->SetBtHeadset(mHost->mBluetoothHeadset);
        mHost->mBluetoothPendingStats = FALSE;
    }
    return NOERROR;
}

ECode CBatteryStatsService::BluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothHeadset = NULL;
    return NOERROR;
}


AutoPtr<IIBatteryStats> CBatteryStatsService::sService;

CBatteryStatsService::CBatteryStatsService()
    : mBluetoothPendingStats(FALSE)
{
    mBluetoothProfileServiceListener = new BluetoothProfileServiceListener(this);
}

ECode CBatteryStatsService::constructor(
    /* [in] */ const String& filename)
{
    mStats = new BatteryStatsImpl(filename);
    return NOERROR;
}

void CBatteryStatsService::Publish(
    /* [in] */ IContext* context)
{
    mContext = context;
    ServiceManager::AddService(String("batteryinfo"), THIS_PROBE(IIBatteryStats));
//TODO:
//    mStats->SetNumSpeedSteps(new PowerProfile(mContext).getNumSpeedSteps());
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 value;
    resources->GetInteger(
            R::integer::config_radioScanningTimeout, &value);
    mStats->SetRadioScanningTimeout(value * 1000L);
}

void CBatteryStatsService::Shutdown()
{
    Slogger::W("BatteryStats", "Writing battery stats before shutdown...");
    AutoLock lock(mStatsLock);
    mStats->ShutdownLocked();

}

AutoPtr<IIBatteryStats> CBatteryStatsService::GetService()
{
    if (sService != NULL) {
        return sService;
    }
    AutoPtr<IInterface> batteryService = ServiceManager::GetService(String("batteryinfo"));
    sService = IIBatteryStats::Probe(batteryService);
    return sService;
}

AutoPtr<BatteryStatsImpl> CBatteryStatsService::GetActiveStatistics()
{
    return mStats;
}

ECode CBatteryStatsService::GetStatistics(
    /* [out, callee] */ ArrayOf<Byte>** statistics)
{
    VALIDATE_NOT_NULL(statistics)
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)));
    //Slog.i("foo", "SENDING BATTERY INFO:");
    //mStats.dumpLocked(new LogPrinter(Log.INFO, "foo", Log.LOG_ID_SYSTEM));
    AutoPtr<IParcel> out;
    CParcel::New((IParcel**)&out);
    mStats->WriteToParcel(out);
    AutoPtr< ArrayOf<Byte> > data;
    out->Marshall((ArrayOf<Byte>**)&data);
    *statistics = data;
    REFCOUNT_ADD(*statistics);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartWakelock(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStartWakeLocked(uid, pid, name, type);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopWakelock(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStopWakeLocked(uid, pid, name, type);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartWakelockFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStartWakeFromSourceLocked(ws, pid, name, type);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopWakelockFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStopWakeFromSourceLocked(ws, pid, name, type);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartSensor(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStartSensorLocked(uid, sensor);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopSensor(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStopSensorLocked(uid, sensor);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartGps(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStartGpsLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopGps(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteStopGpsLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteScreenOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteScreenOnLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NoteScreenBrightness(
    /* [in] */ Int32 brightness)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteScreenBrightnessLocked(brightness);
    return NOERROR;
}

ECode CBatteryStatsService::NoteScreenOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteScreenOffLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NoteInputEvent()
{
    EnforceCallingPermission();
    mStats->NoteInputEventAtomic();
    return NOERROR;
}

ECode CBatteryStatsService::NoteUserActivity(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 event)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteUserActivityLocked(uid, event);
    return NOERROR;
}

ECode CBatteryStatsService::NotePhoneOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NotePhoneOnLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NotePhoneOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NotePhoneOffLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NotePhoneSignalStrength(
   /* [in] */ ISignalStrength* signalStrength)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NotePhoneSignalStrengthLocked(signalStrength);
    return NOERROR;
}

ECode CBatteryStatsService::NotePhoneDataConnectionState(
    /* [in] */ Int32 dataType,
    /* [in] */ Boolean hasData)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NotePhoneDataConnectionStateLocked(dataType, hasData);
    return NOERROR;
}

ECode CBatteryStatsService::NotePhoneState(
    /* [in] */ Int32 state)
{
    EnforceCallingPermission();
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> tm;
    helper->GetDefault((ITelephonyManager**)&tm);
    Int32 simState;
    tm->GetSimState(&simState);
    AutoLock lock(mStatsLock);
    mStats->NotePhoneStateLocked(state, simState);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiOnLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiOffLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartAudio(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteAudioOnLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopAudio(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteAudioOffLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStartVideo(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteVideoOnLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteStopVideo(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteVideoOffLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiRunning(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiRunningLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiRunningChanged(
    /* [in] */ IWorkSource* oldWs,
    /* [in] */ IWorkSource* newWs)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiRunningChangedLocked(oldWs, newWs);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiStopped(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiStoppedLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteBluetoothOn()
{
    EnforceCallingPermission();
    AutoPtr<IBluetoothAdapterHelper> baHelper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&baHelper);
    AutoPtr<IBluetoothAdapter> adapter;
    baHelper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    if (adapter != NULL) {
        Boolean result;
        adapter->GetProfileProxy(mContext, mBluetoothProfileServiceListener,
                                IBluetoothProfile::HEADSET, &result);
    }
    AutoLock lock(mStatsLock);
    if (mBluetoothHeadset != NULL) {
        mStats->NoteBluetoothOnLocked();
        mStats->SetBtHeadset(mBluetoothHeadset);
    }
    else {
        mBluetoothPendingStats = TRUE;
    }
    return NOERROR;
}

ECode CBatteryStatsService::NoteBluetoothOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mBluetoothPendingStats = FALSE;
    mStats->NoteBluetoothOffLocked();
    return NOERROR;
}

ECode CBatteryStatsService::NoteFullWifiLockAcquired(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteFullWifiLockAcquiredLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteFullWifiLockReleased(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteFullWifiLockReleasedLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiScanStarted(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiScanStartedLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiScanStopped(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiScanStoppedLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiMulticastEnabled(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiMulticastEnabledLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiMulticastDisabled(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiMulticastDisabledLocked(uid);
    return NOERROR;
}

ECode CBatteryStatsService::NoteFullWifiLockAcquiredFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteFullWifiLockAcquiredFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteFullWifiLockReleasedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteFullWifiLockReleasedFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiScanStartedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiScanStartedFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiScanStoppedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiScanStoppedFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiMulticastEnabledFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiMulticastEnabledFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteWifiMulticastDisabledFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteWifiMulticastDisabledFromSourceLocked(ws);
    return NOERROR;
}

ECode CBatteryStatsService::NoteNetworkInterfaceType(
    /* [in] */ const String& iface,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStatsLock);
    mStats->NoteNetworkInterfaceTypeLocked(iface, type);
    return NOERROR;
}

Boolean CBatteryStatsService::IsOnBattery()
{
    Boolean result = mStats->IsOnBattery();
    return result;
}

ECode CBatteryStatsService::SetBatteryState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 health,
    /* [in] */ Int32 plugType,
    /* [in] */ Int32 level,
    /* [in] */ Int32 temp,
    /* [in] */ Int32 volt)
{
    EnforceCallingPermission();
    mStats->SetBatteryState(status, health, plugType, level, temp, volt);
    return NOERROR;
}

ECode CBatteryStatsService::GetAwakeTimeBattery(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    FAIL_RETURN( mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)) );
    *time = mStats->GetAwakeTimeBattery();
    return NOERROR;
}

ECode CBatteryStatsService::GetAwakeTimePlugged(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    FAIL_RETURN( mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)) );
    *time = mStats->GetAwakeTimePlugged();
    return NOERROR;
}

void CBatteryStatsService::EnforceCallingPermission()
{
    if (Binder::GetCallingPid() == Process::MyPid()) return;
    mContext->EnforcePermission(Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS,
            Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL));
}

void CBatteryStatsService::DumpHelp(
    /* [in] */ IPrintWriter* pw)
{
    pw->PrintString(String("Battery stats (batteryinfo) dump options:"));
    pw->PrintString(String("  [--checkin] [--reset] [--write] [-h]"));
    pw->PrintString(String("  --checkin: format output for a checkin report."));
    pw->PrintString(String("  --reset: reset the stats, clearing all current data."));
    pw->PrintString(String("  --write: force write current collected stats to disk."));
    pw->PrintString(String("  -h: print this help text."));
}

ECode CBatteryStatsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 permission;
    if (mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &permission),
            permission != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("Permission Denial: can't dump BatteryStats from from pid=");
        sb += Binder::GetCallingPid();
        sb += ", uid=";
        sb += Binder::GetCallingUid();
        sb += " without permission ";
        sb += Elastos::Droid::Manifest::permission::DUMP;
        pw->PrintString(sb.ToString());
        return NOERROR;
    }

    Boolean isCheckin = FALSE;
    Boolean noOutput = FALSE;
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            String arg = (*args)[i];
            if (arg.Equals("--checkin")) {
                isCheckin = TRUE;
            }
            else if (arg.Equals("--reset")) {
                AutoLock lock(mStatsLock);
                mStats->ResetAllStatsLocked();
                pw->PrintString(String("Battery stats reset."));
                noOutput = TRUE;
            }
            else if (arg.Equals("--write")) {
                AutoLock lock(mStatsLock);
                mStats->WriteSyncLocked();
                pw->PrintString(String("Battery stats written."));
                noOutput = TRUE;
            }
            else if (arg.Equals("-h")) {
                DumpHelp(pw);
                return NOERROR;
            }
            else if (arg.Equals("-a")) {
                // fall through
            }
            else {
                pw->PrintString(String("Unknown option: ") + arg);
                DumpHelp(pw);
            }
        }
    }
    if (noOutput) {
        return NOERROR;
    }
    if (isCheckin) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IObjectContainer> apps;
        pm->GetInstalledApplications(0 , (IObjectContainer**)&apps);
        Int32 count;
        apps->GetObjectCount(&count);
        List< AutoPtr<IApplicationInfo> >* temp = new List< AutoPtr<IApplicationInfo> >(count);
        AutoPtr<IObjectEnumerator> enumerator;
        apps->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> a;
            enumerator->Current((IInterface**)&a);
            AutoPtr<IApplicationInfo> app = IApplicationInfo::Probe(a);
            temp->PushBack(app);
        }
        AutoLock lock(mStatsLock);
        mStats->DumpCheckinLocked(pw, args, *temp);
    }
    else {
        AutoLock lock(mStatsLock);
        mStats->DumpLocked(pw);
    }
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
