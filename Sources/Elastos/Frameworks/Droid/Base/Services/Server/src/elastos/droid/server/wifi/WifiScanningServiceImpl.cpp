#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/server/wifi/WifiScanningServiceImpl.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/StringBuffer.h"
#include <elastos/utility/logging/Logger.h>


using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Manifest;
using Elastos::Droid::Wifi::EIID_IIWifiScanner;
using Elastos::Droid::Wifi::IWifiScanner;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::CScanResult;
using Elastos::Droid::Wifi::CWifiScannerScanSettings;
using Elastos::Droid::Wifi::CWifiScannerChannelSpec;
using Elastos::Droid::Wifi::IWifiScannerBssidInfo;
using Elastos::Droid::Wifi::CWifiScannerBssidInfo;
using Elastos::Droid::Wifi::CWifiScannerWifiChangeSettings;
using Elastos::Droid::Wifi::IWifiScannerParcelableScanResults;
using Elastos::Droid::Wifi::CWifiScannerParcelableScanResults;
using Elastos::Droid::Wifi::IWifiSsidHelper;
using Elastos::Droid::Wifi::CWifiSsidHelper;
using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Droid::Wifi::IScanResultInformationElement;
using Elastos::Droid::Wifi::IWifiScannerOperationResult;
using Elastos::Droid::Wifi::CWifiScannerOperationResult;
using Elastos::Droid::Wifi::CWifiScannerHotlistSettings;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//   WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState::Enter()
{
    if (WifiScanningServiceImpl::DBG) Logger::D(TAG, "DefaultState");
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (WifiScanningServiceImpl::DBG) Logger::D(WifiScanningServiceImpl::TAG, "DefaultState got");// + msg);

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    AutoPtr<IInterface> objValue;
    mOwner->mOwner->mClients->Get(TO_IINTERFACE(replyTo), (IInterface**)&objValue);
    AutoPtr<ClientInfo> ci = (ClientInfo*)(IObject::Probe(objValue));

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WifiScanningServiceImpl::CMD_DRIVER_LOADED:
            if (WifiNative::StartHal() && WifiNative::GetInterfaces() != 0) {
                AutoPtr<WifiNative::ScanCapabilities> capabilities = new WifiNative::ScanCapabilities();
                if (WifiNative::GetScanCapabilities(capabilities)) {
                    mOwner->TransitionTo(mOwner->mStartedState);
                } else {
                    Logger::E(WifiScanningServiceImpl::TAG, "could not get scan capabilities");
                }
            } else {
                Logger::E(WifiScanningServiceImpl::TAG, "could not start HAL");
            }
            break;
        case IWifiScanner::CMD_SCAN:
        case IWifiScanner::CMD_START_BACKGROUND_SCAN:
        case IWifiScanner::CMD_STOP_BACKGROUND_SCAN:
        case IWifiScanner::CMD_SET_HOTLIST:
        case IWifiScanner::CMD_RESET_HOTLIST:
        case IWifiScanner::CMD_CONFIGURE_WIFI_CHANGE:
        case IWifiScanner::CMD_START_TRACKING_CHANGE:
        case IWifiScanner::CMD_STOP_TRACKING_CHANGE:
            mOwner->mOwner->ReplyFailed(msg, IWifiScanner::REASON_UNSPECIFIED, String("not available"));
            break;

        case WifiScanningServiceImpl::CMD_SCAN_RESULTS_AVAILABLE:
            if (WifiScanningServiceImpl::DBG) Logger::D(WifiScanningServiceImpl::TAG, "ignored scan results available event");
            break;

        case WifiScanningServiceImpl::CMD_FULL_SCAN_RESULTS:
            if (WifiScanningServiceImpl::DBG) Logger::D(WifiScanningServiceImpl::TAG, "ignored full scan result event");
            break;

        default:
            break;
    }

    return TRUE;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiScanningStateMachine::StartedState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::StartedState::Enter()
{
    if (DBG) Logger::D(TAG, "StartedState");
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::StartedState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "StartedState got" );//+ msg);

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    AutoPtr<IInterface> objValue;
    mOwner->mOwner->mClients->Get(TO_IINTERFACE(replyTo), (IInterface**)&objValue);
    AutoPtr<ClientInfo> ci = (ClientInfo*)(IObject::Probe(objValue));

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_DRIVER_UNLOADED:
            mOwner->TransitionTo(mOwner->mDefaultState);
            break;
        case IWifiScanner::CMD_SCAN:
            mOwner->mOwner->ReplyFailed(msg, IWifiScanner::REASON_UNSPECIFIED, String("not implemented"));
            break;
        case IWifiScanner::CMD_START_BACKGROUND_SCAN: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Boolean addRes;
            if (mOwner->mOwner->AddScanRequest(ci, arg2, IWifiScannerScanSettings::Probe(obj), &addRes), addRes) {
                mOwner->mOwner->ReplySucceeded(msg, NULL);
            } else {
                mOwner->mOwner->ReplyFailed(msg, IWifiScanner::REASON_INVALID_REQUEST, String("bad request"));
            }
            break;
        }
        case IWifiScanner::CMD_STOP_BACKGROUND_SCAN: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            mOwner->mOwner->RemoveScanRequest(ci, arg2);
            break;
        }
        case IWifiScanner::CMD_GET_SCAN_RESULTS: {

            AutoPtr<ArrayOf<IScanResult*> > scanResults;;
            mOwner->mOwner->GetScanResults(ci, (ArrayOf<IScanResult*>**)&scanResults);
            AutoPtr<IArrayOf> result = CoreUtils::Convert(scanResults.Get());
            mOwner->mOwner->ReplySucceeded(msg, result);
            break;
        }
        case IWifiScanner::CMD_SET_HOTLIST: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWifiScannerHotlistSettings* wshls = IWifiScannerHotlistSettings::Probe(obj);
            mOwner->mOwner->SetHotlist(ci, arg2, wshls);
            mOwner->mOwner->ReplySucceeded(msg, NULL);
            break;
        }
        case IWifiScanner::CMD_RESET_HOTLIST: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            mOwner->mOwner->ResetHotlist(ci, arg2);
            break;
        }
        case IWifiScanner::CMD_START_TRACKING_CHANGE: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            mOwner->mOwner->TrackWifiChanges(ci, arg2);
            mOwner->mOwner->ReplySucceeded(msg, NULL);
            break;
        }
        case IWifiScanner::CMD_STOP_TRACKING_CHANGE: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            mOwner->mOwner->UntrackWifiChanges(ci, arg2);
            break;
        }
        case IWifiScanner::CMD_CONFIGURE_WIFI_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWifiScannerWifiChangeSettings* wswcs = IWifiScannerWifiChangeSettings::Probe(obj);
            mOwner->mOwner->ConfigureWifiChange(wswcs);
            break;
        }
        case CMD_SCAN_RESULTS_AVAILABLE: {
                AutoPtr<ArrayOf<IScanResult*> > results = WifiNative::GetScanResults();
                AutoPtr<ICollection> clients;
                mOwner->mOwner->mClients->GetValues((ICollection**)&clients);
                AutoPtr<ArrayOf<IInterface*> > array;
                clients->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); ++i) {
                    AutoPtr<IInterface> obj = (*array)[i];
                    ClientInfo* ci2 = (ClientInfo*)(IObject::Probe(obj));
                    ci2->ReportScanResults(results);
                }
            }
            break;
        case CMD_FULL_SCAN_RESULTS: {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);

                AutoPtr<IScanResult> result = IScanResult::Probe(obj);
                String ssid;
                result->GetSSID(&ssid);
                if (DBG) Logger::D(TAG, "reporting fullscan result for %s", ssid.string());
                AutoPtr<ICollection> clients;
                mOwner->mOwner->mClients->GetValues((ICollection**)&clients);
                AutoPtr<ArrayOf<IInterface*> > array;
                clients->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); ++i) {
                    AutoPtr<IInterface> obj = (*array)[i];
                    ClientInfo* ci2 = (ClientInfo*)(IObject::Probe(obj));
                    ci2->ReportFullScanResult(result);
                }
            }
            break;

        case CMD_HOTLIST_AP_FOUND: {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

                Int32 length;
                iarray->GetLength(&length);
                AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
                for (Int32 i = 0; i < length; ++i) {
                    AutoPtr<IInterface> object;
                    iarray->Get(i, (IInterface**)&object);
                    results->Set(i, IScanResult::Probe(object));
                }
                if (DBG) Logger::D(TAG, "Found %d results", length);
                AutoPtr<ICollection> clients;
                mOwner->mOwner->mClients->GetValues((ICollection**)&clients);
                AutoPtr<ArrayOf<IInterface*> > array;
                clients->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); ++i) {
                    AutoPtr<IInterface> obj = (*array)[i];
                    ClientInfo* ci2 = (ClientInfo*)(IObject::Probe(obj));
                    ci2->ReportHotlistResults(results);
                }
            }
            break;
        case CMD_WIFI_CHANGE_DETECTED: {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

                Int32 length;
                iarray->GetLength(&length);
                AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
                for (Int32 i = 0; i < length; ++i) {
                    AutoPtr<IInterface> object;
                    iarray->Get(i, (IInterface**)&object);
                    results->Set(i, IScanResult::Probe(object));
                }
                mOwner->mOwner->ReportWifiChanged(results);
            }
            break;
        case CMD_WIFI_CHANGES_STABILIZED: {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

                Int32 length;
                iarray->GetLength(&length);
                AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
                for (Int32 i = 0; i < length; ++i) {
                    AutoPtr<IInterface> object;
                    iarray->Get(i, (IInterface**)&object);
                    results->Set(i, IScanResult::Probe(object));
                }
                mOwner->mOwner->ReportWifiStabilized(results);
            }
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

//=====================================================================
//    WifiScanningServiceImpl::WifiScanningStateMachine::PausedState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::PausedState::Enter()
{
    if (DBG) Logger::D(TAG, "PausedState");
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::PausedState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "PausedState got");// + msg);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_SCAN_RESTARTED:
            mOwner->TransitionTo(mOwner->mStartedState);
            break;
        default:
            mOwner->DeferMessage(msg);
            break;
    }
    return TRUE;
}

//=====================================================================
//          WifiScanningServiceImpl::WifiScanningStateMachine
//=====================================================================

CAR_INTERFACE_IMPL_3(WifiScanningServiceImpl::WifiScanningStateMachine,
        StateMachine,
        IWifiNativeScanEventHandler,
        IWifiNativeHotlistEventHandler,
        IWifiNativeSignificantWifiChangeEventHandler);

WifiScanningServiceImpl::WifiScanningStateMachine::WifiScanningStateMachine(
    /* [in] */ WifiScanningServiceImpl* owner,
    /* [in] */ ILooper* looper)
    : StateMachine(TAG, looper)
    , mOwner(owner)
{
    mDefaultState = new DefaultState(this);
    mStartedState = new StartedState(this);
    mPausedState = new PausedState(this);

    SetLogRecSize(512);
    SetLogOnlyTransitions(false);
    // setDbg(DBG);

    AddState(mDefaultState);
        AddState(mStartedState, mDefaultState);
        AddState(mPausedState, mDefaultState);

    SetInitialState(mDefaultState);
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanResultsAvailable()
{
    if (DBG) Logger::D(TAG, "onScanResultAvailable event received");
    SendMessage(CMD_SCAN_RESULTS_AVAILABLE);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnSingleScanComplete()
{
    if (DBG) Logger::D(TAG, "onSingleScanComplete event received");
    SendMessage(CMD_SCAN_RESULTS_AVAILABLE);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnFullScanResult(
    /* [in] */ IScanResult* fullScanResult)
{
    VALIDATE_NOT_NULL(fullScanResult);
    if (DBG) Logger::D(TAG, "Full scanresult received");
    SendMessage(CMD_FULL_SCAN_RESULTS, 0, 0, TO_IINTERFACE(fullScanResult));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanPaused()
{
    SendMessage(CMD_SCAN_PAUSED);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanRestarted()
{
    SendMessage(CMD_SCAN_RESTARTED);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnHotlistApFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    if (DBG) Logger::D(TAG, "HotlistApFound event received");
    SendMessage(CMD_HOTLIST_AP_FOUND, 0, 0, CoreUtils::Convert(results));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnChangesFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    if (DBG) Logger::D(TAG, "onWifiChangesFound event received");
    SendMessage(CMD_WIFI_CHANGE_DETECTED, 0, 0, CoreUtils::Convert(results));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    StateMachine::Dump(fd, pw, args);
    Int32 size;
    mOwner->mClients->GetSize(&size);
    pw->Println(String("number of clients : ") + StringUtils::ToString(size));
    pw->Println();
    return NOERROR;
}

//=====================================================================
//    WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState::Enter()
{
    if (DBG) Logger::D(TAG, "Entering IdleState");
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "DefaultState state got ");// + msg);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WIFI_CHANGE_CMD_ENABLE :
            mOwner->TransitionTo(mOwner->mMovingState);
            break;
        case WIFI_CHANGE_CMD_DISABLE:
            // nothing to do
            break;
        case WIFI_CHANGE_CMD_NEW_SCAN_RESULTS:
            // nothing to do
            break;
        case WIFI_CHANGE_CMD_CONFIGURE:
            /* save configuration till we transition to moving state */
            mOwner->DeferMessage(msg);
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState::Enter()
{
    if (DBG) Logger::D(TAG, "Entering StationaryState");
    mOwner->mOwner->ReportWifiStabilized(mOwner->mCurrentBssids);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "Stationary state got ");// + msg);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WIFI_CHANGE_CMD_ENABLE :
            // do nothing
            break;
        case WIFI_CHANGE_CMD_CHANGE_DETECTED: {
            if (DBG) Logger::D(TAG, "Got wifi change detected");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

            Int32 length;
            iarray->GetLength(&length);
            AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> object;
                iarray->Get(i, (IInterface**)&object);
                results->Set(i, IScanResult::Probe(object));
            }
            mOwner->mOwner->ReportWifiChanged(results);
            mOwner->TransitionTo(mOwner->mMovingState);
            break;
        }
        case WIFI_CHANGE_CMD_DISABLE:
            if (DBG) Logger::D(TAG, "Got Disable Wifi Change");
            mOwner->mCurrentBssids = NULL;
            mOwner->RemoveScanRequest();
            mOwner->UntrackSignificantWifiChange();
            mOwner->TransitionTo(mOwner->mDefaultState);
            break;
        case WIFI_CHANGE_CMD_CONFIGURE:
            /* save configuration till we transition to moving state */
            mOwner->DeferMessage(msg);
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

//=====================================================================
//     WifiScanningServiceImpl::WifiChangeStateMachine::MovingState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::Enter()
{
    if (DBG) Logger::D(TAG, "Entering MovingState");
    IssueFullScan();
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "MovingState state got ");// + msg);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WIFI_CHANGE_CMD_ENABLE :
            // do nothing
            break;
        case WIFI_CHANGE_CMD_DISABLE:
            if (DBG) Logger::D(TAG, "Got Disable Wifi Change");
            mOwner->mCurrentBssids = NULL;
            mOwner->RemoveScanRequest();
            mOwner->UntrackSignificantWifiChange();
            mOwner->TransitionTo(mOwner->mDefaultState);
            break;
        case WIFI_CHANGE_CMD_NEW_SCAN_RESULTS: {
            if (DBG) Logger::D(TAG, "Got scan results");
            if (mScanResultsPending) {
                if (DBG) Logger::D(TAG, "reconfiguring scan");
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

                Int32 length;
                iarray->GetLength(&length);
                AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
                for (Int32 i = 0; i < length; ++i) {
                    AutoPtr<IInterface> object;
                    iarray->Get(i, (IInterface**)&object);
                    results->Set(i, IScanResult::Probe(object));
                }
                mOwner->ReconfigureScan(results, STATIONARY_SCAN_PERIOD_MS);
                mWifiChangeDetected = FALSE;
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 currentTimeMillis;
                system->GetCurrentTimeMillis(&currentTimeMillis);
                mOwner->mAlarmManager->SetExact(IAlarmManager::RTC_WAKEUP,
                        currentTimeMillis + MOVING_STATE_TIMEOUT_MS,
                        mOwner->mTimeoutIntent);
                mScanResultsPending = FALSE;
            }
            break;
        }
        case WIFI_CHANGE_CMD_CONFIGURE: {
            if (DBG) Logger::D(TAG, "Got configuration from app");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IWifiScannerWifiChangeSettings> settings =
                    IWifiScannerWifiChangeSettings::Probe(obj);
            mOwner->ReconfigureScan(settings);
            mWifiChangeDetected = FALSE;
            Int32 unchangedSampleSize;
            settings->GetUnchangedSampleSize(&unchangedSampleSize);
            Int32 periodInMs;
            settings->GetPeriodInMs(&periodInMs);
            Int64 unchangedDelay = unchangedSampleSize * periodInMs;
            mOwner->mAlarmManager->Cancel(mOwner->mTimeoutIntent);
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 currentTimeMillis;
            system->GetCurrentTimeMillis(&currentTimeMillis);
            mOwner->mAlarmManager->SetExact(IAlarmManager::RTC_WAKEUP,
                    currentTimeMillis + unchangedDelay,
                    mOwner->mTimeoutIntent);
            break;
        }
        case WIFI_CHANGE_CMD_CHANGE_DETECTED: {
            if (DBG) Logger::D(TAG, "Change detected");
            mOwner->mAlarmManager->Cancel(mOwner->mTimeoutIntent);

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);

            Int32 length;
            iarray->GetLength(&length);
            AutoPtr<ArrayOf<IScanResult*> > results = ArrayOf<IScanResult*>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> object;
                iarray->Get(i, (IInterface**)&object);
                results->Set(i, IScanResult::Probe(object));
            }

            mOwner->mOwner->ReportWifiChanged(results);
            mWifiChangeDetected = TRUE;
            IssueFullScan();
            break;
        }
        case WIFI_CHANGE_CMD_CHANGE_TIMEOUT:
            if (DBG) Logger::D(TAG, "Got timeout event");
            if (mWifiChangeDetected == FALSE) {
                mOwner->TransitionTo(mOwner->mStationaryState);
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::Exit()
{
    mOwner->mAlarmManager->Cancel(mOwner->mTimeoutIntent);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::IssueFullScan()
{
    if (DBG) Logger::D(TAG, "Issuing full scan");
    AutoPtr<IWifiScannerScanSettings> settings;
    CWifiScannerScanSettings::New((IWifiScannerScanSettings**)&settings);
    settings->SetBand(IWifiScanner::WIFI_BAND_BOTH);
    settings->SetPeriodInMs(MOVING_SCAN_PERIOD_MS);
    settings->SetReportEvents(IWifiScanner::REPORT_EVENT_AFTER_EACH_SCAN);
    mOwner->AddScanRequest(settings);
    mScanResultsPending = TRUE;
    return NOERROR;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal
//=====================================================================
WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::ClientInfoLocal(
    /* [in] */ WifiChangeStateMachine* owner)
    : ClientInfo(owner->mOwner, NULL, NULL)
    , mOwner(owner)
{
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::DeliverScanResults(
    /* [in] */ Int32 handler,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    if (DBG) Logger::D(TAG, "Delivering messages directly");
    mOwner->SendMessage(WIFI_CHANGE_CMD_NEW_SCAN_RESULTS, 0, 0, CoreUtils::Convert(results));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::ReportPeriodChanged(
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 newPeriodInMs)
{
    // // nothing to do; no one is listening for this
    return NOERROR;
}

//=====================================================================
//WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3
//=====================================================================
WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3::InnerBroadcastReceiver3(
    /* [in] */ WifiChangeStateMachine* owner)
    : mOwner(owner)
{
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mOwner->SendMessage(WIFI_CHANGE_CMD_CHANGE_TIMEOUT);
    return NOERROR;
}

//=====================================================================
//           WifiScanningServiceImpl::WifiChangeStateMachine
//=====================================================================
const String WifiScanningServiceImpl::WifiChangeStateMachine::TAG("WifiChangeStateMachine");
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_NEW_SCAN_RESULTS;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_CHANGE_DETECTED;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_CHANGE_TIMEOUT;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_ENABLE;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_DISABLE;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::WIFI_CHANGE_CMD_CONFIGURE;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::MAX_APS_TO_TRACK;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::MOVING_SCAN_PERIOD_MS;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::STATIONARY_SCAN_PERIOD_MS;
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::MOVING_STATE_TIMEOUT_MS;
const String WifiScanningServiceImpl::WifiChangeStateMachine::ACTION_TIMEOUT("com.android.server.WifiScanningServiceImpl.action.TIMEOUT");
const Int32 WifiScanningServiceImpl::WifiChangeStateMachine::SCAN_COMMAND_ID;

CAR_INTERFACE_IMPL(WifiScanningServiceImpl::WifiChangeStateMachine,
        StateMachine,
        IWifiNativeSignificantWifiChangeEventHandler);

WifiScanningServiceImpl::WifiChangeStateMachine::WifiChangeStateMachine(
    /* [in] */ WifiScanningServiceImpl* owner,
    /* [in] */ ILooper* looper)
    : StateMachine(String("SignificantChangeStateMachine"), looper)
    , mOwner(owner)
{
    mDefaultState = new DefaultState(this);
    mStationaryState = new StationaryState(this);
    mMovingState = new MovingState(this);
    mClientInfo = new ClientInfoLocal(this);

    mOwner->mClients->Put(NULL, TO_IINTERFACE(mClientInfo));

    AddState(mDefaultState.Get());
    AddState(mStationaryState.Get(), mDefaultState.Get());
    AddState(mMovingState.Get(), mDefaultState.Get());

    SetInitialState(mDefaultState.Get());
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Enable()
{
    if (mAlarmManager == NULL) {
        AutoPtr<IInterface> alarmObj;
        mOwner->mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&alarmObj);
        mAlarmManager = IAlarmManager::Probe(alarmObj);
    }

    if (mTimeoutIntent == NULL) {
        AutoPtr<IIntent> intent;
        CIntent::New(ACTION_TIMEOUT, NULL, (IIntent**)&intent);
        AutoPtr<IPendingIntentHelper> piHelper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
        piHelper->GetBroadcast(mOwner->mContext, 0, intent, 0, (IPendingIntent**)&mTimeoutIntent);

        AutoPtr<IBroadcastReceiver> br= new InnerBroadcastReceiver3(this);
        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(ACTION_TIMEOUT, (IIntentFilter**)&intentFilter);
        AutoPtr<IIntent> intentTemp;
        mOwner->mContext->RegisterReceiver(br, intentFilter, (IIntent**)&intentTemp);
    }

    SendMessage(WIFI_CHANGE_CMD_ENABLE);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Disable()
{
    SendMessage(WIFI_CHANGE_CMD_DISABLE);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Configure(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    SendMessage(WIFI_CHANGE_CMD_CONFIGURE, TO_IINTERFACE(settings));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ReconfigureScan(
    /* [in] */ ArrayOf<IScanResult*>* results,
    /* [in] */ Int32 period)
{
    // find brightest APs and set them as sentinels
    if (results->GetLength() < MAX_APS_TO_TRACK) {
        Logger::D(TAG, "too few APs (%d) available to track wifi change", results->GetLength());
        return NOERROR;
    }

    RemoveScanRequest();

    // remove duplicate BSSIDs
    AutoPtr<IHashMap> bssidToScanResult;
    CHashMap::New((IHashMap**)&bssidToScanResult);
    for (Int32 i = 0; i < results->GetLength(); ++i) {
        AutoPtr<IScanResult> result = (*results)[i];
        String bssid;
        result->GetBSSID(&bssid);
        AutoPtr<IInterface> valueObj;
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(bssid);
        bssidToScanResult->Get(cs, (IInterface**)&valueObj);
        if (valueObj == NULL) {
            bssidToScanResult->Put(cs, TO_IINTERFACE(result));
        } else {
            AutoPtr<IScanResult> saved = IScanResult::Probe(valueObj);
            Int32 level1;
            saved->GetLevel(&level1);
            Int32 level2;
            result->GetLevel(&level2);
            if (level1 > level2)
                bssidToScanResult->Put(cs, TO_IINTERFACE(result));
        }
    }

    // find brightest BSSIDs
    AutoPtr<ArrayOf<IScanResult*> > brightest = ArrayOf<IScanResult*>::Alloc(MAX_APS_TO_TRACK);
    AutoPtr<ICollection> results2;
    bssidToScanResult->GetValues((ICollection**)&results2);

    AutoPtr<ArrayOf<IInterface*> > array;
    results2->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*array)[i];
        AutoPtr<IScanResult> result = IScanResult::Probe(obj);

        for (Int32 j = 0; j < brightest->GetLength(); j++) {
            Boolean update = (*brightest)[j] == NULL;
            if (!update) {
                Int32 level1;
                (*brightest)[j]->GetLevel(&level1);
                Int32 level2;
                result->GetLevel(&level2);
                update = level1 < level2;
            }
            if (update) {
                for (Int32 k = brightest->GetLength(); k > (j + 1); k--) {
                    brightest->Set(k-1, (*brightest)[k - 2]);
                }
                brightest->Set(j, result);
                break;
            }
        }
    }

    // Get channels to scan for
    AutoPtr<IArrayList> channels;//Integer
    CArrayList::New((IArrayList**)&channels);
    for (Int32 i = 0; i < brightest->GetLength(); ++i) {
        Boolean found = FALSE;
        Int32 frequency;
        (*brightest)[i]->GetFrequency(&frequency);
        for (Int32 j = i + 1; j < brightest->GetLength(); j++) {
            Int32 frequency2;
            (*brightest)[j]->GetFrequency(&frequency2);
            if (frequency2 == frequency) {
                found = TRUE;
            }
        }
        if (!found) {
            channels->Add(CoreUtils::Convert(frequency));
        }
    }

    Int32 size;
    channels->GetSize(&size);

    if (DBG) Logger::D(TAG, "Found %d channels", size);

    // set scanning schedule
    AutoPtr<IWifiScannerScanSettings> settings;
    CWifiScannerScanSettings::New((IWifiScannerScanSettings**)&settings);
    settings->SetBand(IWifiScanner::WIFI_BAND_UNSPECIFIED);
    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > wscs = ArrayOf<IWifiScannerChannelSpec*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        channels->Get(i, (IInterface**)&obj);
        IInteger32* i32 = IInteger32::Probe(obj);
        Int32 fre;
        i32->GetValue(&fre);
        AutoPtr<IWifiScannerChannelSpec> channelSpec;
        CWifiScannerChannelSpec::New(fre, (IWifiScannerChannelSpec**)&channelSpec);
        wscs->Set(i, channelSpec);
    }
    settings->SetChannels(wscs);

    settings->SetPeriodInMs(period);
    AddScanRequest(settings);

    AutoPtr<IWifiScannerWifiChangeSettings> settings2;
    CWifiScannerWifiChangeSettings::New((IWifiScannerWifiChangeSettings**)&settings2);
    settings2->SetRssiSampleSize(3);
    settings2->SetLostApSampleSize(3);
    settings2->SetUnchangedSampleSize(3);
    settings2->SetMinApsBreachingThreshold(2);

    AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > wsbi = ArrayOf<IWifiScannerBssidInfo*>::Alloc(brightest->GetLength());
    for (Int32 i = 0; i < brightest->GetLength(); ++i) {
        AutoPtr<IWifiScannerBssidInfo> BssidInfo;
        CWifiScannerBssidInfo::New((IWifiScannerBssidInfo**)&BssidInfo);
        String bssid;
        (*brightest)[i]->GetBSSID(&bssid);
        BssidInfo->SetBssid(bssid);
        Int32 level;
        (*brightest)[i]->GetLevel(&level);
        Int32 threshold = (100 + level) / 32 + 2;
        BssidInfo->SetLow(level - threshold);
        BssidInfo->SetHigh(level + threshold);
        wsbi->Set(i, BssidInfo);

        if (DBG) Logger::D(TAG, "Setting bssid=%s, low=%d, high=%d",  bssid.string(), level - threshold, level + threshold);
    }
    settings2->SetBssidInfos(wsbi);

    TrackSignificantWifiChange(settings2);
    mCurrentBssids = brightest;
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ReconfigureScan(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssidInfos;
    settings->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&bssidInfos);
    if (bssidInfos->GetLength() < MAX_APS_TO_TRACK) {
        Logger::D(TAG, "too few APs (%d) available to track wifi change", bssidInfos->GetLength());
        return NOERROR;
    }

    if (DBG) Logger::D(TAG, "Setting configuration specified by app");

    //AutoPtr<ArrayOf<IScanResult*> > scanResults =
    mCurrentBssids = ArrayOf<IScanResult*>::Alloc(bssidInfos->GetLength());
    AutoPtr<IHashSet> channels;//Integer
    CHashSet::New((IHashSet**)&channels);

    for (Int32 i = 0; i < bssidInfos->GetLength(); ++i) {
        AutoPtr<IScanResult> result;
        CScanResult::New((IScanResult**)&result);
        String bssid;
        (*bssidInfos)[i]->GetBssid(&bssid);
        result->SetBSSID(bssid);
        mCurrentBssids->Set(i, result);
        Int32 frequencyHint;
        (*bssidInfos)[i]->GetFrequencyHint(&frequencyHint);
        channels->Add(CoreUtils::Convert(frequencyHint));
    }

    // cancel previous scan
    RemoveScanRequest();

    // set new scanning schedule
    AutoPtr<IWifiScannerScanSettings> settings2;
    CWifiScannerScanSettings::New((IWifiScannerScanSettings**)&settings2);
    settings2->SetBand(IWifiScanner::WIFI_BAND_UNSPECIFIED);
    Int32 size;
    channels->GetSize(&size);
    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > wscs = ArrayOf<IWifiScannerChannelSpec*>::Alloc(size);
    Int32 i = 0;
    AutoPtr<ArrayOf<IInterface*> > setArray;
    channels->ToArray((ArrayOf<IInterface*>**)&setArray);
    for (Int32 j = 0; i < setArray->GetLength(); ++j) {
        AutoPtr<IInterface> obj = (*setArray)[j];
        IInteger32* i32 = IInteger32::Probe(obj);
        Int32 fre;
        i32->GetValue(&fre);
        AutoPtr<IWifiScannerChannelSpec> channelSpec;
        CWifiScannerChannelSpec::New(fre, (IWifiScannerChannelSpec**)&channelSpec);
        wscs->Set(i++, channelSpec);
    }
    settings2->SetChannels(wscs);

    Int32 periodInMs;
    settings->GetPeriodInMs(&periodInMs);
    settings2->SetPeriodInMs(periodInMs);
    AddScanRequest(settings2);

    // start tracking new APs
    TrackSignificantWifiChange(settings);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::OnChangesFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    SendMessage(WIFI_CHANGE_CMD_CHANGE_DETECTED, 0, 0, CoreUtils::Convert(results));
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::AddScanRequest(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    if (DBG) Logger::D(TAG, "Starting scans");
    AutoPtr<IMessage> msg;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain((IMessage**)&msg);

    msg->SetWhat(IWifiScanner::CMD_START_BACKGROUND_SCAN);
    msg->SetArg2(SCAN_COMMAND_ID);
    msg->SetObj(settings);
    Boolean bTemp;
    mOwner->mClientHandler->SendMessage(msg, &bTemp);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::RemoveScanRequest()
{
    if (DBG) Logger::D(TAG, "Stopping scans");
    AutoPtr<IMessage> msg;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain((IMessage**)&msg);

    msg->SetWhat(IWifiScanner::CMD_STOP_BACKGROUND_SCAN);
    msg->SetArg2(SCAN_COMMAND_ID);
    Boolean bTemp;
    mOwner->mClientHandler->SendMessage(msg, &bTemp);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::TrackSignificantWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    WifiNative::UntrackSignificantWifiChange();
    WifiNative::TrackSignificantWifiChange(settings, this);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::UntrackSignificantWifiChange()
{
    WifiNative::UntrackSignificantWifiChange();
    return NOERROR;
}

//=====================================================================
//                WifiScanningServiceImpl::ClientHandler
//=====================================================================
WifiScanningServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ WifiScanningServiceImpl* owner,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mOwner(owner)
{
}

ECode WifiScanningServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(TAG, "ClientHandler got");// + msg);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {

        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IAsyncChannel> c = IAsyncChannel::Probe(obj);
                if (DBG) Logger::D(TAG, "New client listening to asynchronous messages: ");// + msg.replyTo);
                AutoPtr<IMessenger> replyTo;
                msg->GetReplyTo((IMessenger**)&replyTo);
                AutoPtr<ClientInfo> cInfo = new ClientInfo(mOwner, c, replyTo);
                mOwner->mClients->Put(replyTo, TO_IINTERFACE(cInfo));
            } else {
                Logger::E(TAG, "Client connection failure, error=", arg1);
            }
            return NOERROR;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                Logger::E(TAG, "Send failed, client connection lost");
            } else {
                if (DBG) Logger::D(TAG, "Client connection lost with reason: %d", arg1);
            }
            if (DBG) Logger::D(TAG, "closing client ");// + msg.replyTo);
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            AutoPtr<IInterface> obj;
            mOwner->mClients->Remove(replyTo, (IInterface**)&obj);
            ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
            if (ci != NULL) {                       /* can be null if send failed above */
                ci->Cleanup();
            }
            return NOERROR;
        }
        case IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            AutoPtr<IAsyncChannel> ac;
            CAsyncChannel::New((IAsyncChannel**)&ac);
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            ac->Connect(mOwner->mContext, this, replyTo);
            return NOERROR;
        }
    }

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    AutoPtr<IInterface> obj;
    mOwner->mClients->Get(replyTo, (IInterface**)&obj);
    ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
    if (ci == NULL) {
        Logger::E(TAG, "Could not find client info for message ");// + msg.replyTo);
        mOwner->ReplyFailed(msg, IWifiScanner::REASON_INVALID_LISTENER, String("Could not find listener"));
        return NOERROR;
    }

    //try {
    mOwner->EnforceConnectivityInternalPermission();
    //} catch (SecurityException e) {
    //TODO    replyFailed(msg, WifiScanner.REASON_NOT_AUTHORIZED, "Not authorized");
    //    return;
    //}

    Int32 validCommands[] = {
            IWifiScanner::CMD_SCAN,
            IWifiScanner::CMD_START_BACKGROUND_SCAN,
            IWifiScanner::CMD_STOP_BACKGROUND_SCAN,
            IWifiScanner::CMD_SET_HOTLIST,
            IWifiScanner::CMD_RESET_HOTLIST,
            IWifiScanner::CMD_CONFIGURE_WIFI_CHANGE,
            IWifiScanner::CMD_START_TRACKING_CHANGE,
            IWifiScanner::CMD_STOP_TRACKING_CHANGE };

    for (UInt32 i = 0; i < sizeof(validCommands)/sizeof(Int32); ++i) {
        Int32 cmd = validCommands[i];
        Int32 what;
        msg->GetWhat(&what);
        if (cmd == what) {
            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            AutoPtr<IMessage> message;
            helper->Obtain(msg, (IMessage**)&message);
            mOwner->mStateMachine->SendMessage(message);
            return NOERROR;
        }
    }

    mOwner->ReplyFailed(msg, IWifiScanner::REASON_INVALID_REQUEST, String("Invalid request"));
    return NOERROR;
}

//=====================================================================
//           WifiScanningServiceImpl::InnerBroadcastReceiver1
//=====================================================================
WifiScanningServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiScanningServiceImpl* owner)
    : mOwner(owner)
{
}

ECode WifiScanningServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 state;
    intent->GetInt32Extra(IWifiManager::EXTRA_SCAN_AVAILABLE, IWifiManager::WIFI_STATE_DISABLED, &state);
    if (DBG) Logger::D(TAG, "SCAN_AVAILABLE : %d", state);
    if (state == IWifiManager::WIFI_STATE_ENABLED) {
        mOwner->mStateMachine->SendMessage(CMD_DRIVER_LOADED);
    } else if (state == IWifiManager::WIFI_STATE_DISABLED) {
        mOwner->mStateMachine->SendMessage(CMD_DRIVER_UNLOADED);
    }
    return NOERROR;
}

//=====================================================================
//                 WifiScanningServiceImpl::ClientInfo
//=====================================================================
const Int32 WifiScanningServiceImpl::ClientInfo::MAX_LIMIT;

WifiScanningServiceImpl::ClientInfo::ClientInfo(
    /* [in] */ WifiScanningServiceImpl* owner,
    /* [in] */ IAsyncChannel* c,
    /* [in] */ IMessenger* m)
    : mOwner(owner)
{
    CHashMap::New((IHashMap**)&mScanSettings);
    CHashMap::New((IHashMap**)&mScanPeriods);
    CHashMap::New((IHashMap**)&mHotlistSettings);
    CHashSet::New((IHashSet**)&mSignificantWifiHandlers);
    mChannel = c;
    mMessenger = m;
    if (DBG) Logger::D(TAG, "New client, channel: TODO  messenger: TODO");
}

ECode WifiScanningServiceImpl::ClientInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<StringBuffer> sb = new StringBuffer();
    sb->Append("mChannel ");
    sb->Append("TODO"/*mChannel*/);
    sb->Append("\n");
    sb->Append("mMessenger ");
    sb->Append("TODO"/*mMessenger*/);
    sb->Append("\n");

    //TODO
    //Iterator<Map.Entry<Integer, ScanSettings>> it = mScanSettings.entrySet().iterator();
    //for (; it.hasNext(); ) {
    //    Map.Entry<Integer, ScanSettings> entry = it.next();
    //    sb.append("[ScanId ").append(entry.getKey()).append("\n");
    //    sb.append("ScanSettings ").append(entry.getValue()).append("\n");
    //    sb.append("]");
    //}

    return sb->ToString(result);
}

ECode WifiScanningServiceImpl::ClientInfo::AddScanRequest(
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 id)
{
    mScanSettings->Put(CoreUtils::Convert(id), settings);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveScanRequest(
    /* [in] */ Int32 id)
{
    mScanSettings->Remove(CoreUtils::Convert(id));
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetScans(
    /* [out] */ IIterator** result)
{
    AutoPtr<ISet> entrySet;
    mScanSettings->GetEntrySet((ISet**)&entrySet);
    return entrySet->GetIterator(result);
}

ECode WifiScanningServiceImpl::ClientInfo::GetScanSettings(
    /* [out] */ ICollection** result)
{
    return mScanSettings->GetValues(result);
}

ECode WifiScanningServiceImpl::ClientInfo::ReportScanResults(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<ISet> keySet;
    mScanSettings->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);

    Boolean next;
    while (it->HasNext(&next), next) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInteger32* i32 = IInteger32::Probe(obj);
        Int32 handler;
        i32->GetValue(&handler);
        ReportScanResults(results, handler);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportScanResults(
    /* [in] */ ArrayOf<IScanResult*>* results,
    /* [in] */ Int32 handler)
{
    AutoPtr<IInterface> obj;
    mScanSettings->Get(CoreUtils::Convert(handler), (IInterface**)&obj);
    AutoPtr<IWifiScannerScanSettings> settings = IWifiScannerScanSettings::Probe(obj);
    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > desiredChannels;
    settings->GetChannels((ArrayOf<IWifiScannerChannelSpec*>**)&desiredChannels);
    Int32 band;
    settings->GetBand(&band);
    if (band != IWifiScanner::WIFI_BAND_UNSPECIFIED
            || desiredChannels == NULL || desiredChannels->GetLength() == 0)  {
        desiredChannels = mOwner->GetChannelsForBand(band);
    }

    // check the channels this client asked for ..
    Int32 num_results = 0;

    if (results != NULL) {
        for (Int32 i = 0; i < results->GetLength(); ++i) {
            AutoPtr<IScanResult> result = (*results)[i];
            for (Int32 j = 0; j < desiredChannels->GetLength(); ++j) {
                AutoPtr<IWifiScannerChannelSpec> channelSpec = (*desiredChannels)[j];
                Int32 freq1, freq2;
                channelSpec->GetFrequency(&freq1);
                result->GetFrequency(&freq2);
                if (freq1 == freq2) {
                    num_results++;
                    break;
                }
            }
        }
    }

    if (num_results == 0) {
        // nothing to report
        return NOERROR;
    }

    AutoPtr<ArrayOf<IScanResult*> >  results2 = ArrayOf<IScanResult*>::Alloc(num_results);
    Int32 index = 0;
    for (Int32 i = 0; i < results->GetLength(); ++i) {
        AutoPtr<IScanResult> result = (*results)[i];
        for (Int32 j = 0; j < desiredChannels->GetLength(); ++j) {
            AutoPtr<IWifiScannerChannelSpec> channelSpec = (*desiredChannels)[j];
            Int32 freq1, freq2;
            channelSpec->GetFrequency(&freq1);
            result->GetFrequency(&freq2);
            if (freq1 == freq2) {
                AutoPtr<IWifiSsidHelper> wsHelper;
                CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&wsHelper);
                String ssid;
                result->GetSSID(&ssid);
                AutoPtr<IWifiSsid> wifiSsid;
                wsHelper->CreateFromAsciiEncoded(ssid, (IWifiSsid**)&wifiSsid);

                Int32 level;
                result->GetLevel(&level);
                Int64 timestamp;
                result->GetTimestamp(&timestamp);
                String bssid;
                result->GetBSSID(&bssid);
                AutoPtr<IScanResult> newResult;
                CScanResult::New(wifiSsid, bssid, String(""),
                        level, freq2, timestamp, (IScanResult**)&newResult);
                results2->Set(index,  newResult);
                index++;
                break;
            }
        }
    }

    DeliverScanResults(handler, results2);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::DeliverScanResults(
    /* [in] */ Int32 handler,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<IWifiScannerParcelableScanResults> parcelableScanResults;
    CWifiScannerParcelableScanResults::New(results, (IWifiScannerParcelableScanResults**)&parcelableScanResults);
    mChannel->SendMessage(IWifiScanner::CMD_SCAN_RESULT, 0, handler, parcelableScanResults);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportFullScanResult(
    /* [in] */ IScanResult* result)
{
    AutoPtr<ISet> keySet;
    mScanSettings->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> keyObj;
        it->GetNext((IInterface**)&keyObj);
        Int32 handler;
        IInteger32::Probe(keyObj)->GetValue(&handler);

        AutoPtr<IInterface> valueObj;
        mScanSettings->Get(keyObj, (IInterface**)&valueObj);
        AutoPtr<IWifiScannerScanSettings> settings = IWifiScannerScanSettings::Probe(valueObj);
        AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > desiredChannels;
        settings->GetChannels((ArrayOf<IWifiScannerChannelSpec*>**)&desiredChannels);

        Int32 band;
        settings->GetBand(&band);
        if (band != IWifiScanner::WIFI_BAND_UNSPECIFIED
                || desiredChannels == NULL || desiredChannels->GetLength() == 0)  {
            desiredChannels = GetChannelsForBand(band);
        }

        for (Int32 i = 0; i < desiredChannels->GetLength(); ++i) {
            AutoPtr<IWifiScannerChannelSpec> channelSpec = (*desiredChannels)[i];
            Int32 freq1, freq2;
            channelSpec->GetFrequency(&freq1);
            result->GetFrequency(&freq2);
            if (freq1 == freq2) {
                AutoPtr<IWifiSsidHelper> wsHelper;
                CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&wsHelper);
                String ssid;
                result->GetSSID(&ssid);
                AutoPtr<IWifiSsid> wifiSsid;
                wsHelper->CreateFromAsciiEncoded(ssid, (IWifiSsid**)&wifiSsid);

                String bssid;
                result->GetBSSID(&bssid);
                Int32 level;
                result->GetLevel(&level);
                Int64 timestamp;
                result->GetTimestamp(&timestamp);

                AutoPtr<IScanResult> newResult;
                CScanResult::New(wifiSsid, bssid, String(""),
                        level, freq2, timestamp, (IScanResult**)&newResult);
                if (DBG) Logger::D(TAG, "sending it to %d", handler);
                AutoPtr<ArrayOf<IScanResultInformationElement*> > ie;
                result->GetInformationElements((ArrayOf<IScanResultInformationElement*>**)&ie);
                AutoPtr<ArrayOf<IScanResultInformationElement*> > ie2 = ie->Clone();
                newResult->SetInformationElements(ie2);
                mChannel->SendMessage(IWifiScanner::CMD_FULL_SCAN_RESULT, 0, handler, newResult);
            }
        }
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportPeriodChanged(
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 newPeriodInMs)
{
    AutoPtr<IInterface> obj;
    mScanPeriods->Get(CoreUtils::Convert(handler), (IInterface**)&obj);
    IInteger32* prevPeriodObject = IInteger32::Probe(obj);
    Int32 prevPeriodInMs;
    settings->GetPeriodInMs(&prevPeriodInMs);
    if (prevPeriodObject != NULL) {
        prevPeriodObject->GetValue(&prevPeriodInMs);
    }

    if (prevPeriodInMs != newPeriodInMs) {
        mChannel->SendMessage(IWifiScanner::CMD_PERIOD_CHANGED, newPeriodInMs, handler);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::AddHostlistSettings(
    /* [in] */ IWifiScannerHotlistSettings* settings,
    /* [in] */ Int32 handler)
{
    mHotlistSettings->Put(CoreUtils::Convert(handler), settings);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveHostlistSettings(
    /* [in] */ Int32 handler)
{
    mHotlistSettings->Remove(CoreUtils::Convert(handler));
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetHotlistSettings(
    /* [out] */ ICollection** result)
{
    return mHotlistSettings->GetValues(result);
}

ECode WifiScanningServiceImpl::ClientInfo::ReportHotlistResults(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<ISet> entrySet;
    mHotlistSettings->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);

    Boolean hasNext;
    while (it->HasNext(&hasNext), &hasNext) {
        AutoPtr<IInterface> mapEntryObj;
        it->GetNext((IInterface**)&mapEntryObj);
        IMapEntry* mapEntry = IMapEntry::Probe(mapEntryObj);
        AutoPtr<IInterface> ko, vo;
        mapEntry->GetKey((IInterface**)&ko);
        mapEntry->GetValue((IInterface**)&vo);
        IInteger32* i32 = IInteger32::Probe(ko);
        Int32 handler;
        i32->GetValue(&handler);
        IWifiScannerHotlistSettings* settings = IWifiScannerHotlistSettings::Probe(vo);
        Int32 num_results = 0;

        if (results != NULL) {
            for (Int32 i = 0; i < results->GetLength(); ++i) {
                AutoPtr<IScanResult> result = (*results)[i];
                AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssidInfos;
                settings->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&bssidInfos);
                String resultBssid;
                result->GetBSSID(&resultBssid);
                for (Int32 j = 0; j < bssidInfos->GetLength(); ++j) {
                    AutoPtr<IWifiScannerBssidInfo> bssidInfo = (*bssidInfos)[j];
                    String bssidInfoBssid;
                    bssidInfo->GetBssid(&bssidInfoBssid);
                    if (resultBssid.EqualsIgnoreCase(bssidInfoBssid)) {
                        num_results++;
                        break;
                    }
                }
            }
        }

        if (num_results == 0) {
            // nothing to report
            return NOERROR;
        }

        AutoPtr<ArrayOf<IScanResult*> > results2 = ArrayOf<IScanResult*>::Alloc(num_results);
        Int32 index = 0;
        for (Int32 i = 0; i < results->GetLength(); ++i) {
            AutoPtr<IScanResult> result = (*results)[i];
            AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssidInfos;
            settings->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&bssidInfos);
            String resultBssid;
            result->GetBSSID(&resultBssid);
            for (Int32 j = 0; j < bssidInfos->GetLength(); ++j) {
                AutoPtr<IWifiScannerBssidInfo> bssidInfo = (*bssidInfos)[j];
                String bssidInfoBssid;
                bssidInfo->GetBssid(&bssidInfoBssid);
                if (resultBssid.EqualsIgnoreCase(bssidInfoBssid)) {
                    results2->Set(index, result);
                    index++;
                }
            }
        }

        AutoPtr<IWifiScannerParcelableScanResults> parcelableScanResults;
        CWifiScannerParcelableScanResults::New(results2, (IWifiScannerParcelableScanResults**)&parcelableScanResults);

        mChannel->SendMessage(IWifiScanner::CMD_AP_FOUND, 0, handler, parcelableScanResults);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::AddSignificantWifiChange(
    /* [in] */ Int32 handler)
{
    return mSignificantWifiHandlers->Add(CoreUtils::Convert(handler));
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveSignificantWifiChange(
    /* [in] */ Int32 handler)
{
    mSignificantWifiHandlers->Remove(CoreUtils::Convert(handler));
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetWifiChangeHandlers(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollection> temp = ICollection::Probe(mSignificantWifiHandlers);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportWifiChanged(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<IWifiScannerParcelableScanResults> parcelableScanResults;
    CWifiScannerParcelableScanResults::New(results, (IWifiScannerParcelableScanResults**)&parcelableScanResults);

    AutoPtr<IIterator> it;
    mSignificantWifiHandlers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 handler;
        IInteger32::Probe(obj)->GetValue(&handler);
        mChannel->SendMessage(IWifiScanner::CMD_WIFI_CHANGE_DETECTED,
                0, handler, parcelableScanResults);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportWifiStabilized(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<IWifiScannerParcelableScanResults> parcelableScanResults;
    CWifiScannerParcelableScanResults::New(results, (IWifiScannerParcelableScanResults**)&parcelableScanResults);

    AutoPtr<IIterator> it;
    mSignificantWifiHandlers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 handler;
        IInteger32::Probe(obj)->GetValue(&handler);
        mChannel->SendMessage(IWifiScanner::CMD_WIFI_CHANGES_STABILIZED,
                0, handler, parcelableScanResults);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::Cleanup()
{
    mScanSettings->Clear();
    Boolean bTemp;
    mOwner->ResetBuckets(&bTemp);

    mHotlistSettings->Clear();
    mOwner->ResetHotlist();

    AutoPtr<ArrayOf<IInterface*> > array;
    ICollection::Probe(mSignificantWifiHandlers)->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*array)[i];
        Int32 handler;
        IInteger32::Probe(obj)->GetValue(&handler);
        mOwner->UntrackWifiChanges(this, handler);
    }

    mSignificantWifiHandlers->Clear();
    Logger::D(TAG, "Successfully stopped all requests for client %p", this);
    return NOERROR;
}

//=====================================================================
//        WifiScanningServiceImpl::SettingsComputer::TimeBucket
//=====================================================================
WifiScanningServiceImpl::SettingsComputer::TimeBucket::TimeBucket(
    /* [in] */ Int32 p,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    periodInSecond = p;
    periodMinInSecond = min;
    periodMaxInSecond = max;
}

//=====================================================================
//              WifiScanningServiceImpl::SettingsComputer
//=====================================================================
AutoPtr< ArrayOf<WifiScanningServiceImpl::SettingsComputer::TimeBucket*> > WifiScanningServiceImpl::SettingsComputer::mTimeBuckets = WifiScanningServiceImpl::SettingsComputer::MiddleInitMtimebuckets();
const Int32 WifiScanningServiceImpl::SettingsComputer::MAX_BUCKETS;
const Int32 WifiScanningServiceImpl::SettingsComputer::MAX_CHANNELS;
const Int32 WifiScanningServiceImpl::SettingsComputer::DEFAULT_MAX_AP_PER_SCAN;
const Int32 WifiScanningServiceImpl::SettingsComputer::DEFAULT_REPORT_THRESHOLD;
const Int32 WifiScanningServiceImpl::SettingsComputer::DEFAULT_BASE_PERIOD_MS;

WifiScanningServiceImpl::SettingsComputer::SettingsComputer()
{
    CHashMap::New((IHashMap**)&mChannelToBucketMap);
    {
        mSettings = new WifiNative::ScanSettings();
        mSettings->max_ap_per_scan = DEFAULT_MAX_AP_PER_SCAN;
        mSettings->base_period_ms = DEFAULT_BASE_PERIOD_MS;
        mSettings->report_threshold = DEFAULT_REPORT_THRESHOLD;

        mSettings->buckets = ArrayOf<WifiNative::BucketSettings*>::Alloc(MAX_BUCKETS);
        for (Int32 i = 0; i < mSettings->buckets->GetLength(); ++i) {
            AutoPtr<WifiNative::BucketSettings> bucketSettings = new WifiNative::BucketSettings();
            bucketSettings->bucket = i;
            bucketSettings->report_events = 0;
            bucketSettings->channels = ArrayOf<WifiNative::ChannelSettings*>::Alloc(MAX_CHANNELS);
            bucketSettings->num_channels = 0;
            for (Int32 j = 0; j < bucketSettings->channels->GetLength(); ++j) {
                AutoPtr<WifiNative::ChannelSettings> channelSettings = new WifiNative::ChannelSettings();
                bucketSettings->channels->Set(j, channelSettings);
            }
            mSettings->buckets->Set(i, bucketSettings);
        }
    }
}

ECode WifiScanningServiceImpl::SettingsComputer::PrepChannelMap(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    GetBestBucket(settings);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::AddScanRequestToBucket(
    /* [in] */  IWifiScannerScanSettings* settings,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 bucketIndex = GetBestBucket(settings);
    if (bucketIndex == -1) {
        Logger::E(TAG, "Ignoring invalid settings");
        *result = -1;
        return NOERROR;
    }

    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > desiredChannels;
    settings->GetChannels((ArrayOf<IWifiScannerChannelSpec*>**)&desiredChannels);

    Int32 band;
    settings->GetBand(&band);
    if (band != IWifiScanner::WIFI_BAND_UNSPECIFIED
            || desiredChannels == NULL
            || desiredChannels->GetLength() == 0) {
        // set channels based on band
        desiredChannels = GetChannelsForBand(band);
        if (desiredChannels == NULL) {
            // still no channels; then there's nothing to scan
            Logger::E(TAG, "No channels to scan!!");
            *result = -1;
            return NOERROR;
        }
    }

    Int32 periodInMs;
    settings->GetPeriodInMs(&periodInMs);
    // merge the channel lists for these buckets
    Logger::D(TAG, "merging %d channels for period %d", desiredChannels->GetLength() , periodInMs);


    AutoPtr<WifiNative::BucketSettings> bucket = (*mSettings->buckets)[bucketIndex];
    Boolean added = (bucket->num_channels == 0)
            && (bucket->band == IWifiScanner::WIFI_BAND_UNSPECIFIED);
    Logger::D(TAG, "existing %d channels ", bucket->num_channels);

    AutoPtr<IHashSet> newChannels;//IWifiScannerChannelSpec
    CHashSet::New((IHashSet**)&newChannels);

    for (Int32 i = 0; i < desiredChannels->GetLength(); ++i) {
        AutoPtr<IWifiScannerChannelSpec> desiredChannelSpec = (*desiredChannels)[i];

        Int32 frequency;
        desiredChannelSpec->GetFrequency(&frequency);
        Logger::D(TAG, "desired channel %d", frequency);

        Boolean found = FALSE;
        AutoPtr<ArrayOf<WifiNative::ChannelSettings*> > bucketChannels = bucket->channels;
        for (Int32 j = 0; j < bucketChannels->GetLength(); ++j) {
            AutoPtr<WifiNative::ChannelSettings> existingChannelSpec = (*bucketChannels)[j];
            if (frequency == existingChannelSpec->frequency) {
                found = TRUE;
                break;
            }
        }

        if (!found) {
            newChannels->Add(desiredChannelSpec);
        } else {
            if (DBG) Logger::D(TAG, "Already scanning channel %d", frequency);
        }
    }

    Int32 newChannelsSize;
    newChannels->GetSize(&newChannelsSize);
    if (band != IWifiScanner::WIFI_BAND_UNSPECIFIED
            || (bucket->num_channels + newChannelsSize) > bucket->channels->GetLength()) {
        // can't accommodate all channels; switch to specifying band
        bucket->num_channels = 0;
        bucket->band = GetBandFromChannels(bucket->channels)
                | GetBandFromChannels(desiredChannels);
        bucket->channels = ArrayOf<WifiNative::ChannelSettings*>::Alloc(0);
        Logger::D(TAG, "switching to using band %d", band);
    } else {
        AutoPtr<ArrayOf<IInterface*> > array;
        newChannels->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IInterface> obj = (*array)[i];
            IWifiScannerChannelSpec* desiredChannelSpec = IWifiScannerChannelSpec::Probe(obj);

            Int32 frequency;
            desiredChannelSpec->GetFrequency(&frequency);
            Logger::D(TAG, "adding new channel spec %d", frequency);

            AutoPtr<WifiNative::ChannelSettings> channelSettings = (*bucket->channels)[bucket->num_channels];
            channelSettings->frequency = frequency;
            bucket->num_channels++;
            mChannelToBucketMap->Put(CoreUtils::Convert(bucketIndex),
                    CoreUtils::Convert(channelSettings->frequency));
        }
    }

    Int32 reportEvents;
    settings->GetReportEvents(&reportEvents);
    if (bucket->report_events < reportEvents) {
        if (DBG) Logger::D(TAG, "setting report_events to %d", reportEvents);
        bucket->report_events = reportEvents;
    } else {
        if (DBG) Logger::D(TAG, "report_events is %d", reportEvents);
    }

    if (added) {
        bucket->period_ms = (*mTimeBuckets)[bucketIndex]->periodInSecond * 1000;
        mSettings->num_buckets++;
    }

    Int32 numBssidsPerScan;
    settings->GetNumBssidsPerScan(&numBssidsPerScan);
    if (mSettings->max_ap_per_scan < numBssidsPerScan) {
        mSettings->max_ap_per_scan = numBssidsPerScan;
    }

    *result = bucket->period_ms;
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::GetComputedSettings(
    /* [out] */ WifiNative::ScanSettings** result)
{
    VALIDATE_NOT_NULL(*result);
    *result = mSettings;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::CompressBuckets()
{
    Int32 num_buckets = 0;
    for (Int32 i = 0; i < mSettings->buckets->GetLength(); ++i) {
        if ((*mSettings->buckets)[i]->num_channels != 0
                || (*mSettings->buckets)[i]->band != IWifiScanner::WIFI_BAND_UNSPECIFIED) {
            mSettings->buckets->Set(num_buckets, (*mSettings->buckets)[i]);
            num_buckets++;
        }
    }
    // remove unused buckets
    for (Int32 i = num_buckets; i < mSettings->buckets->GetLength(); ++i) {
        mSettings->buckets->Set(i,  NULL);
    }

    mSettings->num_buckets = num_buckets;
    if (num_buckets != 0) {
        mSettings->base_period_ms = (*mSettings->buckets)[0]->period_ms;
    }
    return NOERROR;
}

AutoPtr<ArrayOf<WifiScanningServiceImpl::SettingsComputer::TimeBucket*> > WifiScanningServiceImpl::SettingsComputer::MiddleInitMtimebuckets()
{
    AutoPtr<ArrayOf<TimeBucket*> > timeBuckets = ArrayOf<TimeBucket*>::Alloc(8);
    timeBuckets->Set(0, new TimeBucket( 1, 0, 5 ));
    timeBuckets->Set(1, new TimeBucket( 5, 5, 10 ));
    timeBuckets->Set(2, new TimeBucket( 10, 10, 25 ));
    timeBuckets->Set(3, new TimeBucket( 30, 25, 55 ));
    timeBuckets->Set(4, new TimeBucket( 60, 55, 100));
    timeBuckets->Set(5, new TimeBucket( 300, 240, 500));
    timeBuckets->Set(6, new TimeBucket( 600, 500, 1500));
    timeBuckets->Set(7, new TimeBucket( 1800, 1500, IWifiScanner::MAX_SCAN_PERIOD_MS));
    return timeBuckets;
}

Int32 WifiScanningServiceImpl::SettingsComputer::GetBestBucket(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    // check to see if any of the channels are being scanned already
    // and find the smallest bucket index (it represents the quickest
    // period of scan)

    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > channels;
    settings->GetChannels((ArrayOf<IWifiScannerChannelSpec*>**)&channels);
    if (channels == NULL) {
        // set channels based on band
        Int32 band;
        settings->GetBand(&band);
        channels = GetChannelsForBand(band);
    }

    if (channels == NULL) {
        // still no channels; then there's nothing to scan
        Logger::E(TAG, "No channels to scan!!");
        return -1;
    }

    Int32 mostFrequentBucketIndex = mTimeBuckets->GetLength();

    for (Int32 i = 0; i < channels->GetLength(); ++i) {
        AutoPtr<IWifiScannerChannelSpec> desiredChannelSpec = (*channels)[i];
        Int32 frequency;
        desiredChannelSpec->GetFrequency(&frequency);
        Boolean containskey;
        if (mChannelToBucketMap->ContainsKey(CoreUtils::Convert(frequency), &containskey), containskey) {
            AutoPtr<IInterface> valObj;
            mChannelToBucketMap->Get(CoreUtils::Convert(frequency), (IInterface**)&valObj);
            Int32 bucket;
            IInteger32::Probe(valObj)->GetValue(&bucket);
            if (bucket < mostFrequentBucketIndex) {
                mostFrequentBucketIndex = bucket;
            }
        }
    }

    Int32 bestBucketIndex = -1;                                   // best by period
    Int32 periodInMs;
    settings->GetPeriodInMs(&periodInMs);
    for (Int32 i = 0; i < mTimeBuckets->GetLength(); i++) {
        AutoPtr<TimeBucket> bucket = (*mTimeBuckets)[i];
        if (bucket->periodMinInSecond * 1000 <= periodInMs
                && periodInMs < bucket->periodMaxInSecond * 1000) {
            // we set the time period to this
            bestBucketIndex = i;
            break;
        }
    }

    if (mostFrequentBucketIndex < bestBucketIndex) {
        for (Int32 i = 0; i < channels->GetLength(); ++i) {
            AutoPtr<IWifiScannerChannelSpec> desiredChannelSpec = (*channels)[i];
            Int32 frequency;
            desiredChannelSpec->GetFrequency(&frequency);
            mChannelToBucketMap->Put(CoreUtils::Convert(frequency), CoreUtils::Convert(mostFrequentBucketIndex));
        }
        Logger::D(TAG, "returning mf bucket number %d", mostFrequentBucketIndex);
        return mostFrequentBucketIndex;
    } else if (bestBucketIndex != -1) {
        for (Int32 i = 0; i < channels->GetLength(); ++i) {
            AutoPtr<IWifiScannerChannelSpec> desiredChannelSpec = (*channels)[i];
            Int32 frequency;
            desiredChannelSpec->GetFrequency(&frequency);
            mChannelToBucketMap->Put(CoreUtils::Convert(frequency), CoreUtils::Convert(bestBucketIndex));
        }
        Logger::D(TAG, "returning best bucket number %d", bestBucketIndex);
        return bestBucketIndex;
    }

    Logger::E(TAG, "Could not find suitable bucket for period %d", periodInMs);
    return -1;
}

//=====================================================================
//                       WifiScanningServiceImpl
//=====================================================================
const String WifiScanningServiceImpl::TAG("WifiScanningService");
const Boolean WifiScanningServiceImpl::DBG = true;
const Int32 WifiScanningServiceImpl::INVALID_KEY;
const Int32 WifiScanningServiceImpl::MIN_PERIOD_PER_CHANNEL_MS;
const Int32 WifiScanningServiceImpl::BASE;
const Int32 WifiScanningServiceImpl::CMD_SCAN_RESULTS_AVAILABLE;
const Int32 WifiScanningServiceImpl::CMD_FULL_SCAN_RESULTS;
const Int32 WifiScanningServiceImpl::CMD_HOTLIST_AP_FOUND;
const Int32 WifiScanningServiceImpl::CMD_HOTLIST_AP_LOST;
const Int32 WifiScanningServiceImpl::CMD_WIFI_CHANGE_DETECTED;
const Int32 WifiScanningServiceImpl::CMD_WIFI_CHANGES_STABILIZED;
const Int32 WifiScanningServiceImpl::CMD_DRIVER_LOADED;
const Int32 WifiScanningServiceImpl::CMD_DRIVER_UNLOADED;
const Int32 WifiScanningServiceImpl::CMD_SCAN_PAUSED;
const Int32 WifiScanningServiceImpl::CMD_SCAN_RESTARTED;

CAR_INTERFACE_IMPL_2(WifiScanningServiceImpl, Object, IIWifiScanner, IBinder);

WifiScanningServiceImpl::WifiScanningServiceImpl()
{
    CHashMap::New((IHashMap**)&mClients);
}

ECode WifiScanningServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetMessenger(
    /* [out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger);
    AutoPtr<IMessenger> mes;
    CMessenger::New(mClientHandler, (IMessenger**)&mes);
    *messenger = mes;
    REFCOUNT_ADD(*messenger);
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetAvailableChannels(
    /* [in] */ Int32 band,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > channelSpecs = GetChannelsForBand(band);
    AutoPtr<IArrayList> list;
    CArrayList::New(channelSpecs->GetLength(), (IArrayList**)&list);

    for (Int32 i = 0; i < channelSpecs->GetLength(); ++i) {
        AutoPtr<IWifiScannerChannelSpec> channelSpec = (*channelSpecs)[i];
        Int32 frequency;
        channelSpec->GetFrequency(&frequency);
        list->Add(CoreUtils::Convert(frequency));
    }
    AutoPtr<IBundle> b;
    CBundle::New((IBundle**)&b);
    b->PutIntegerArrayList(IWifiScanner::GET_AVAILABLE_CHANNELS_EXTRA, list);
    *bundle = b;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

ECode WifiScanningServiceImpl::StartService(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<HandlerThread> thread = new HandlerThread(String("WifiScanningService"));
    thread->Start();

    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    mClientHandler = new ClientHandler(this, looper);
    mStateMachine = new WifiScanningStateMachine(this, looper);
    mWifiChangeStateMachine = new WifiChangeStateMachine(this, looper);

    AutoPtr<IBroadcastReceiver> receiver = new InnerBroadcastReceiver1(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IWifiManager::WIFI_SCAN_AVAILABLE, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intentTemp;
    mContext->RegisterReceiver(receiver, intentFilter, (IIntent**)&intentTemp);

    mStateMachine->Start();
    mWifiChangeStateMachine->Start();
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReplySucceeded(
    /* [in] */ IMessage* msg,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo != NULL) {
        AutoPtr<IMessage> reply;
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        helper->Obtain((IMessage**)&reply);
        reply->SetWhat(IWifiScanner::CMD_OP_SUCCEEDED);
        Int32 arg2;
        msg->GetArg2(&arg2);
        reply->SetArg2(arg2);
        reply->SetObj(obj);
        //try {
            replyTo->Send(reply);
        //} catch (RemoteException e) {
        //    // There's not much we can do if reply can't be sent!
        //}
    } else {
        // locally generated message; doesn't need a reply!
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo != NULL) {
        AutoPtr<IMessage> reply;
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        helper->Obtain((IMessage**)&reply);

        reply->SetWhat(IWifiScanner::CMD_OP_FAILED);
        Int32 arg2;
        msg->GetArg2(&arg2);
        reply->SetArg2(arg2);
        AutoPtr<IWifiScannerOperationResult> wfsor;
        CWifiScannerOperationResult::New(reason, description, (IWifiScannerOperationResult**)&wfsor);
        reply->SetObj(wfsor);
        //try {
        replyTo->Send(reply);
        //} catch (RemoteException e) {
            // There's not much we can do if reply can't be sent!
        //}
    } else {
        // locally generated message; doesn't need a reply!
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetBuckets(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<SettingsComputer> c = new SettingsComputer();
    AutoPtr<ICollection> clients;
    mClients->GetValues((ICollection**)&clients);
    AutoPtr<ArrayOf<IInterface*> > clientsArray;
    clients->ToArray((ArrayOf<IInterface*>**)&clientsArray);
    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));

        AutoPtr<ICollection> settings;
        ci->GetScanSettings((ICollection**)&settings);
        AutoPtr<ArrayOf<IInterface*> > settingsArray;
        settings->ToArray((ArrayOf<IInterface*>**)&settingsArray);
        for (Int32 j = 0; j < settingsArray->GetLength(); ++j) {
            AutoPtr<IInterface> obj = (*settingsArray)[j];
            IWifiScannerScanSettings* s = IWifiScannerScanSettings::Probe(obj);
            c->PrepChannelMap(s);
        }
    }

    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
        AutoPtr<IIterator> it;
        ci->GetScans((IIterator**)&it);

        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            //Map.Entry<Integer, WifiScanner.ScanSettings> entry =
            //        (Map.Entry<Integer,WifiScanner.ScanSettings>)it.next();
            AutoPtr<IInterface> mapEntryObj;
            it->GetNext((IInterface**)&mapEntryObj);
            IMapEntry* mapEntry = IMapEntry::Probe(mapEntryObj);
            AutoPtr<IInterface> ko, vo;
            mapEntry->GetKey((IInterface**)&ko);
            mapEntry->GetValue((IInterface**)&vo);
            Int32 id;
            IInteger32::Probe(ko)->GetValue(&id);
            IWifiScannerScanSettings* s = IWifiScannerScanSettings::Probe(vo);
            Int32 newPeriodInMs;
            c->AddScanRequestToBucket(s, &newPeriodInMs);
            if (newPeriodInMs  == -1) {
                if (DBG) Logger::D(TAG, "could not find a good bucket");
                *result = FALSE;
                return NOERROR;
            }
            Int32 periodInMs;
            s->GetPeriodInMs(&periodInMs);
            if (newPeriodInMs != periodInMs) {
                ci->ReportPeriodChanged(id, s, newPeriodInMs);
            }
        }
    }

    c->CompressBuckets();

    AutoPtr<WifiNative::ScanSettings> s;
    c->GetComputedSettings((WifiNative::ScanSettings**)&s);
    if (s->num_buckets == 0) {
        if (DBG) Logger::D(TAG, "Stopping scan because there are no buckets");
        WifiNative::StopScan();
        *result = TRUE;
        return NOERROR;
    } else {
        if (WifiNative::StartScan(s, mStateMachine)) {
            if (DBG) Logger::D(TAG, "Successfully started scan of %d buckets", s->num_buckets);
                    //+ "at time = " + SystemClock.elapsedRealtimeNanos()/1000);
            *result = TRUE;
            return NOERROR;
        } else {
            if (DBG) Logger::D(TAG, "Failed to start scan of %d buckets", s->num_buckets);
            *result= FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::AddScanRequest(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // sanity check the input
    Int32 periodInMs;
    settings->GetPeriodInMs(&periodInMs);
    if (periodInMs < IWifiScanner::MIN_SCAN_PERIOD_MS) {
        Logger::D(TAG, "Failing scan request because periodInMs is %d", periodInMs);
        *result = FALSE;
        return NOERROR;
    }

    Int32 minSupportedPeriodMs = 0;
    AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > channels;
    settings->GetChannels((ArrayOf<IWifiScannerChannelSpec*>**)&channels);
    Int32 band;
    settings->GetBand(&band);
    if (channels != NULL) {
        minSupportedPeriodMs = channels->GetLength() * MIN_PERIOD_PER_CHANNEL_MS;
    } else {
        if ((band & IWifiScanner::WIFI_BAND_24_GHZ) == 0) {
            /* 2.4 GHz band has 11 to 13 channels */
            minSupportedPeriodMs += 1000;
        }
        if ((band & IWifiScanner::WIFI_BAND_5_GHZ) == 0) {
            /* 5 GHz band has another 10 channels */
            minSupportedPeriodMs += 1000;
        }
        if ((band & IWifiScanner::WIFI_BAND_5_GHZ_DFS_ONLY) == 0) {
            /* DFS requires passive scan which takes longer time */
            minSupportedPeriodMs += 2000;
        }
    }

    if (periodInMs < minSupportedPeriodMs) {
        Logger::D(TAG, "Failing scan request because minSupportedPeriodMs is %d but the request wants %d",
                minSupportedPeriodMs, periodInMs);
        *result = FALSE;
        return NOERROR;
    }

    ci->AddScanRequest(settings, handler);
    Boolean bTemp;
    if (ResetBuckets(&bTemp), bTemp) {
        *result = TRUE;
        return NOERROR;
    } else {
        ci->RemoveScanRequest(handler);
        Logger::D(TAG, "Failing scan request because failed to reset scan");
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::RemoveScanRequest(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    ci->RemoveScanRequest(handler);
    Boolean bTemp;
    ResetBuckets(&bTemp);
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetScanResults(
    /* [in] */ ClientInfo* ci,
    /* [out] */ ArrayOf<IScanResult*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IScanResult*> > results = WifiNative::GetScanResults();
    ci->ReportScanResults(results);
    *result = results;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetHotlist()
{
    AutoPtr<ICollection> clients;
    mClients->GetValues((ICollection**)&clients);
    AutoPtr<ArrayOf<IInterface*> > clientsArray;
    clients->ToArray((ArrayOf<IInterface*>**)&clientsArray);
    Int32 num_hotlist_ap = 0;

    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
        AutoPtr<ICollection> c;
        ci->GetHotlistSettings((ICollection**)&c);
        AutoPtr<ArrayOf<IInterface*> > cArray;
        c->ToArray((ArrayOf<IInterface*>**)&cArray);
        for (Int32 j = 0; j < cArray->GetLength(); ++j) {
            AutoPtr<IInterface> sObj = (*cArray)[j];
            IWifiScannerHotlistSettings* s = IWifiScannerHotlistSettings::Probe(sObj);

            AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssidInfos;
            s->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&bssidInfos);
            num_hotlist_ap +=  bssidInfos->GetLength();
        }
    }

    if (num_hotlist_ap == 0) {
        WifiNative::ResetHotlist();
    } else {
        AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssidInfos = ArrayOf<IWifiScannerBssidInfo*>::Alloc(num_hotlist_ap);
        Int32 index = 0;
        for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
            AutoPtr<IInterface> obj = (*clientsArray)[i];
            ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
            AutoPtr<ICollection> c;
            ci->GetHotlistSettings((ICollection**)&c);
            AutoPtr<ArrayOf<IInterface*> > cArray;
            c->ToArray((ArrayOf<IInterface*>**)&cArray);
            for (Int32 j = 0; j < cArray->GetLength(); ++j) {
                AutoPtr<IInterface> sObj = (*cArray)[j];
                IWifiScannerHotlistSettings* s = IWifiScannerHotlistSettings::Probe(sObj);

                AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > sbssidInfos;
                s->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&sbssidInfos);
                for (Int32 k = 0; k < sbssidInfos->GetLength(); k++, index++) {
                    bssidInfos->Set(index, (*sbssidInfos)[k]);
                }
            }
        }

        AutoPtr<IWifiScannerHotlistSettings> settings;
        CWifiScannerHotlistSettings::New((IWifiScannerHotlistSettings**)&settings);
        settings->SetBssidInfos(bssidInfos);
        settings->SetApLostThreshold(3);
        WifiNative::SetHotlist(settings, mStateMachine);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::SetHotlist(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerHotlistSettings* settings)
{
    ci->AddHostlistSettings(settings, handler);
    ResetHotlist();
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetHotlist(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    ci->RemoveHostlistSettings(handler);
    ResetHotlist();
    return NOERROR;
}

ECode WifiScanningServiceImpl::TrackWifiChanges(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    mWifiChangeStateMachine->Enable();
    ci->AddSignificantWifiChange(handler);
    return NOERROR;
}

ECode WifiScanningServiceImpl::UntrackWifiChanges(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    ci->RemoveSignificantWifiChange(handler);
    AutoPtr<ICollection> clients;
    mClients->GetValues((ICollection**)&clients);
    AutoPtr<ArrayOf<IInterface*> > clientsArray;
    clients->ToArray((ArrayOf<IInterface*>**)&clientsArray);

    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci2 = (ClientInfo*)(IObject::Probe(obj));
        AutoPtr<ICollection> c;
        ci2->GetWifiChangeHandlers((ICollection**)&c);
        Int32 size;
        c->GetSize(&size);
        if (size != 0) {
            // there is at least one client watching for
            // significant changes; so nothing more to do
            return NOERROR;
        }
    }

    // no more clients looking for significant wifi changes
    // no need to keep the state machine running; disable it
    mWifiChangeStateMachine->Disable();
    return NOERROR;
}

ECode WifiScanningServiceImpl::ConfigureWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    mWifiChangeStateMachine->Configure(settings);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReportWifiChanged(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<ICollection> clients;
    mClients->GetValues((ICollection**)&clients);
    AutoPtr<ArrayOf<IInterface*> > clientsArray;
    clients->ToArray((ArrayOf<IInterface*>**)&clientsArray);

    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
        ci->ReportWifiChanged(results);
    }
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReportWifiStabilized(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoPtr<ICollection> clients;
    mClients->GetValues((ICollection**)&clients);
    AutoPtr<ArrayOf<IInterface*> > clientsArray;
    clients->ToArray((ArrayOf<IInterface*>**)&clientsArray);

    for (Int32 i = 0; i < clientsArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*clientsArray)[i];
        ClientInfo* ci = (ClientInfo*)(IObject::Probe(obj));
        ci->ReportWifiStabilized(results);
    }
    return NOERROR;
}

void WifiScanningServiceImpl::EnforceConnectivityInternalPermission()
{
    mContext->EnforceCallingOrSelfPermission(
             Manifest::permission::CONNECTIVITY_INTERNAL,
             String("WifiScanningServiceImpl"));
}

AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > WifiScanningServiceImpl::GetChannelsForBand(
    /* [in] */ Int32 band)
{
    AutoPtr<ArrayOf<Int32> > channels = WifiNative::GetChannelsForBand(band);
    if (channels != NULL) {
        AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > channelSpecs =
            ArrayOf<IWifiScannerChannelSpec*>::Alloc(channels->GetLength());
        for (Int32 i = 0; i < channels->GetLength(); i++) {
            AutoPtr<IWifiScannerChannelSpec> wfscs;
            CWifiScannerChannelSpec::New((*channels)[i], (IWifiScannerChannelSpec**)&wfscs);
            channelSpecs->Set(i, wfscs);
        }
        return channelSpecs;
    } else {
        AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > channelSpecs =
            ArrayOf<IWifiScannerChannelSpec*>::Alloc(0);
        return channelSpecs;
    }
    return NULL;
}

Int32 WifiScanningServiceImpl::GetBandFromChannels(
    /* [in] */ ArrayOf<IWifiScannerChannelSpec*>* channels)
{
    Int32 band = IWifiScanner::WIFI_BAND_UNSPECIFIED;
    for (Int32 i = 0; i < channels->GetLength(); ++i) {
        AutoPtr<IWifiScannerChannelSpec> channel = (*channels)[i];
        Int32 frequency;
        channel->GetFrequency(&frequency);
        if (2400 <= frequency && frequency < 2500) {
            band |= IWifiScanner::WIFI_BAND_24_GHZ;
        } else if (5100 <= frequency && frequency < 6000) {
            band |= IWifiScanner::WIFI_BAND_5_GHZ;
        } else {
            /* TODO: Add DFS Range */
        }
    }
    return band;
}

Int32 WifiScanningServiceImpl::GetBandFromChannels(
    /* [in] */ ArrayOf<WifiNative::ChannelSettings*>* channels)
{
    Int32 band = IWifiScanner::WIFI_BAND_UNSPECIFIED;
    for (Int32 i = 0; i < channels->GetLength(); ++i) {
        AutoPtr<WifiNative::ChannelSettings> channel = (*channels)[i];
        if (2400 <= channel->frequency && channel->frequency < 2500) {
            band |= IWifiScanner::WIFI_BAND_24_GHZ;
        } else if (5100 <= channel->frequency && channel->frequency < 6000) {
            band |= IWifiScanner::WIFI_BAND_5_GHZ;
        } else {
            /* TODO: Add DFS Range */
        }
    }
    return band;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
