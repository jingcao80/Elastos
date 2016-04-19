#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/server/wifi/WifiScanningServiceImpl.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Wifi::EIID_IIWifiScanner;
using Elastos::Droid::Wifi::IWifiScanner;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//   WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "DefaultState");
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    //
    // if (DBG) Log.d(TAG, "DefaultState got" + msg);
    //
    // ClientInfo ci = mClients.get(msg.replyTo);
    //
    // switch (msg.what) {
    //     case CMD_DRIVER_LOADED:
    //         if (WifiNative.startHal() && WifiNative.getInterfaces() != 0) {
    //             WifiNative.ScanCapabilities capabilities =
    //                     new WifiNative.ScanCapabilities();
    //             if (WifiNative.getScanCapabilities(capabilities)) {
    //                 transitionTo(mStartedState);
    //             } else {
    //                 loge("could not get scan capabilities");
    //             }
    //         } else {
    //             loge("could not start HAL");
    //         }
    //         break;
    //     case WifiScanner.CMD_SCAN:
    //     case WifiScanner.CMD_START_BACKGROUND_SCAN:
    //     case WifiScanner.CMD_STOP_BACKGROUND_SCAN:
    //     case WifiScanner.CMD_SET_HOTLIST:
    //     case WifiScanner.CMD_RESET_HOTLIST:
    //     case WifiScanner.CMD_CONFIGURE_WIFI_CHANGE:
    //     case WifiScanner.CMD_START_TRACKING_CHANGE:
    //     case WifiScanner.CMD_STOP_TRACKING_CHANGE:
    //         replyFailed(msg, WifiScanner.REASON_UNSPECIFIED, "not available");
    //         break;
    //
    //     case CMD_SCAN_RESULTS_AVAILABLE:
    //         if (DBG) log("ignored scan results available event");
    //         break;
    //
    //     case CMD_FULL_SCAN_RESULTS:
    //         if (DBG) log("ignored full scan result event");
    //         break;
    //
    //     default:
    //         break;
    // }
    //
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiScanningStateMachine::StartedState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::StartedState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "StartedState");
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::StartedState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    //
    // if (DBG) Log.d(TAG, "StartedState got" + msg);
    //
    // ClientInfo ci = mClients.get(msg.replyTo);
    //
    // switch (msg.what) {
    //     case CMD_DRIVER_UNLOADED:
    //         transitionTo(mDefaultState);
    //         break;
    //     case WifiScanner.CMD_SCAN:
    //         replyFailed(msg, WifiScanner.REASON_UNSPECIFIED, "not implemented");
    //         break;
    //     case WifiScanner.CMD_START_BACKGROUND_SCAN:
    //         if (addScanRequest(ci, msg.arg2, (ScanSettings) msg.obj)) {
    //             replySucceeded(msg, null);
    //         } else {
    //             replyFailed(msg, WifiScanner.REASON_INVALID_REQUEST, "bad request");
    //         }
    //         break;
    //     case WifiScanner.CMD_STOP_BACKGROUND_SCAN:
    //         removeScanRequest(ci, msg.arg2);
    //         break;
    //     case WifiScanner.CMD_GET_SCAN_RESULTS:
    //         replySucceeded(msg, getScanResults(ci));
    //         break;
    //     case WifiScanner.CMD_SET_HOTLIST:
    //         setHotlist(ci, msg.arg2, (WifiScanner.HotlistSettings) msg.obj);
    //         replySucceeded(msg, null);
    //         break;
    //     case WifiScanner.CMD_RESET_HOTLIST:
    //         resetHotlist(ci, msg.arg2);
    //         break;
    //     case WifiScanner.CMD_START_TRACKING_CHANGE:
    //         trackWifiChanges(ci, msg.arg2);
    //         replySucceeded(msg, null);
    //         break;
    //     case WifiScanner.CMD_STOP_TRACKING_CHANGE:
    //         untrackWifiChanges(ci, msg.arg2);
    //         break;
    //     case WifiScanner.CMD_CONFIGURE_WIFI_CHANGE:
    //         configureWifiChange((WifiScanner.WifiChangeSettings) msg.obj);
    //         break;
    //     case CMD_SCAN_RESULTS_AVAILABLE: {
    //             ScanResult[] results = WifiNative.getScanResults();
    //             Collection<ClientInfo> clients = mClients.values();
    //             for (ClientInfo ci2 : clients) {
    //                 ci2.reportScanResults(results);
    //             }
    //         }
    //         break;
    //     case CMD_FULL_SCAN_RESULTS: {
    //             ScanResult result = (ScanResult) msg.obj;
    //             if (DBG) Log.d(TAG, "reporting fullscan result for " + result.SSID);
    //             Collection<ClientInfo> clients = mClients.values();
    //             for (ClientInfo ci2 : clients) {
    //                 ci2.reportFullScanResult(result);
    //             }
    //         }
    //         break;
    //
    //     case CMD_HOTLIST_AP_FOUND: {
    //             ScanResult[] results = (ScanResult[])msg.obj;
    //             if (DBG) Log.d(TAG, "Found " + results.length + " results");
    //             Collection<ClientInfo> clients = mClients.values();
    //             for (ClientInfo ci2 : clients) {
    //                 ci2.reportHotlistResults(results);
    //             }
    //         }
    //         break;
    //     case CMD_WIFI_CHANGE_DETECTED: {
    //             ScanResult[] results = (ScanResult[])msg.obj;
    //             reportWifiChanged(results);
    //         }
    //         break;
    //     case CMD_WIFI_CHANGES_STABILIZED: {
    //             ScanResult[] results = (ScanResult[])msg.obj;
    //             reportWifiStabilized(results);
    //         }
    //         break;
    //
    //     default:
    //         return NOT_HANDLED;
    // }
    //
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//    WifiScanningServiceImpl::WifiScanningStateMachine::PausedState
//=====================================================================
ECode WifiScanningServiceImpl::WifiScanningStateMachine::PausedState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "PausedState");
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiScanningStateMachine::PausedState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    //
    // if (DBG) Log.d(TAG, "PausedState got" + msg);
    //
    // switch (msg.what) {
    //     case CMD_SCAN_RESTARTED:
    //         transitionTo(mStartedState);
    //         break;
    //     default:
    //         deferMessage(msg);
    //         break;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
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
    /* [in] */ ILooper* looper)
{
    mDefaultState = new DefaultState();
    mStartedState = new StartedState();
    mPausedState = new PausedState();
    // ==================before translated======================
    // super(TAG, looper);
    //
    // setLogRecSize(512);
    // setLogOnlyTransitions(false);
    // // setDbg(DBG);
    //
    // addState(mDefaultState);
    //     addState(mStartedState, mDefaultState);
    //     addState(mPausedState, mDefaultState);
    //
    // setInitialState(mDefaultState);
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanResultsAvailable()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onScanResultAvailable event received");
    // sendMessage(CMD_SCAN_RESULTS_AVAILABLE);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnSingleScanComplete()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onSingleScanComplete event received");
    // sendMessage(CMD_SCAN_RESULTS_AVAILABLE);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnFullScanResult(
    /* [in] */ IScanResult* fullScanResult)
{
    VALIDATE_NOT_NULL(fullScanResult);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Full scanresult received");
    // sendMessage(CMD_FULL_SCAN_RESULTS, 0, 0, fullScanResult);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanPaused()
{
    // ==================before translated======================
    // sendMessage(CMD_SCAN_PAUSED);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnScanRestarted()
{
    // ==================before translated======================
    // sendMessage(CMD_SCAN_RESTARTED);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnHotlistApFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "HotlistApFound event received");
    // sendMessage(CMD_HOTLIST_AP_FOUND, 0, 0, results);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiScanningStateMachine::OnChangesFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onWifiChangesFound event received");
    // sendMessage(CMD_WIFI_CHANGE_DETECTED, 0, 0, results);
    assert(0);
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
    // ==================before translated======================
    // super.dump(fd, pw, args);
    // pw.println("number of clients : " + mClients.size());
    // pw.println();
    assert(0);
    return NOERROR;
}

//=====================================================================
//    WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Entering IdleState");
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "DefaultState state got " + msg);
    // switch (msg.what) {
    //     case WIFI_CHANGE_CMD_ENABLE :
    //         transitionTo(mMovingState);
    //         break;
    //     case WIFI_CHANGE_CMD_DISABLE:
    //         // nothing to do
    //         break;
    //     case WIFI_CHANGE_CMD_NEW_SCAN_RESULTS:
    //         // nothing to do
    //         break;
    //     case WIFI_CHANGE_CMD_CONFIGURE:
    //         /* save configuration till we transition to moving state */
    //         deferMessage(msg);
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Entering StationaryState");
    // reportWifiStabilized(mCurrentBssids);
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::StationaryState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Stationary state got " + msg);
    // switch (msg.what) {
    //     case WIFI_CHANGE_CMD_ENABLE :
    //         // do nothing
    //         break;
    //     case WIFI_CHANGE_CMD_CHANGE_DETECTED:
    //         if (DBG) Log.d(TAG, "Got wifi change detected");
    //         reportWifiChanged((ScanResult[])msg.obj);
    //         transitionTo(mMovingState);
    //         break;
    //     case WIFI_CHANGE_CMD_DISABLE:
    //         if (DBG) Log.d(TAG, "Got Disable Wifi Change");
    //         mCurrentBssids = null;
    //         removeScanRequest();
    //         untrackSignificantWifiChange();
    //         transitionTo(mDefaultState);
    //         break;
    //     case WIFI_CHANGE_CMD_CONFIGURE:
    //         /* save configuration till we transition to moving state */
    //         deferMessage(msg);
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//     WifiScanningServiceImpl::WifiChangeStateMachine::MovingState
//=====================================================================
ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::Enter()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Entering MovingState");
    // issueFullScan();
    assert(0);
    return NOERROR;
}

Boolean WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "MovingState state got " + msg);
    // switch (msg.what) {
    //     case WIFI_CHANGE_CMD_ENABLE :
    //         // do nothing
    //         break;
    //     case WIFI_CHANGE_CMD_DISABLE:
    //         if (DBG) Log.d(TAG, "Got Disable Wifi Change");
    //         mCurrentBssids = null;
    //         removeScanRequest();
    //         untrackSignificantWifiChange();
    //         transitionTo(mDefaultState);
    //         break;
    //     case WIFI_CHANGE_CMD_NEW_SCAN_RESULTS:
    //         if (DBG) Log.d(TAG, "Got scan results");
    //         if (mScanResultsPending) {
    //             if (DBG) Log.d(TAG, "reconfiguring scan");
    //             reconfigureScan((ScanResult[])msg.obj, STATIONARY_SCAN_PERIOD_MS);
    //             mWifiChangeDetected = false;
    //             mAlarmManager.setExact(AlarmManager.RTC_WAKEUP,
    //                     System.currentTimeMillis() + MOVING_STATE_TIMEOUT_MS,
    //                     mTimeoutIntent);
    //             mScanResultsPending = false;
    //         }
    //         break;
    //     case WIFI_CHANGE_CMD_CONFIGURE:
    //         if (DBG) Log.d(TAG, "Got configuration from app");
    //         WifiScanner.WifiChangeSettings settings =
    //                 (WifiScanner.WifiChangeSettings) msg.obj;
    //         reconfigureScan(settings);
    //         mWifiChangeDetected = false;
    //         long unchangedDelay = settings.unchangedSampleSize * settings.periodInMs;
    //         mAlarmManager.cancel(mTimeoutIntent);
    //         mAlarmManager.setExact(AlarmManager.RTC_WAKEUP,
    //                 System.currentTimeMillis() + unchangedDelay,
    //                 mTimeoutIntent);
    //         break;
    //     case WIFI_CHANGE_CMD_CHANGE_DETECTED:
    //         if (DBG) Log.d(TAG, "Change detected");
    //         mAlarmManager.cancel(mTimeoutIntent);
    //         reportWifiChanged((ScanResult[])msg.obj);
    //         mWifiChangeDetected = true;
    //         issueFullScan();
    //         break;
    //     case WIFI_CHANGE_CMD_CHANGE_TIMEOUT:
    //         if (DBG) Log.d(TAG, "Got timeout event");
    //         if (mWifiChangeDetected == false) {
    //             transitionTo(mStationaryState);
    //         }
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::Exit()
{
    // ==================before translated======================
    // mAlarmManager.cancel(mTimeoutIntent);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::MovingState::IssueFullScan()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Issuing full scan");
    // WifiScanner.ScanSettings settings = new WifiScanner.ScanSettings();
    // settings.band = WifiScanner.WIFI_BAND_BOTH;
    // settings.periodInMs = MOVING_SCAN_PERIOD_MS;
    // settings.reportEvents = WifiScanner.REPORT_EVENT_AFTER_EACH_SCAN;
    // addScanRequest(settings);
    // mScanResultsPending = true;
    assert(0);
    return NOERROR;
}

//=====================================================================
//   WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal
//=====================================================================
WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::ClientInfoLocal()
    : ClientInfo(NULL, NULL)
{
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::DeliverScanResults(
    /* [in] */ Int32 handler,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Delivering messages directly");
    // sendMessage(WIFI_CHANGE_CMD_NEW_SCAN_RESULTS, 0, 0, results);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ClientInfoLocal::ReportPeriodChanged(
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 newPeriodInMs)
{
    VALIDATE_NOT_NULL(settings);
    // ==================before translated======================
    // // nothing to do; no one is listening for this
    assert(0);
    return NOERROR;
}

//=====================================================================
//WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3
//=====================================================================
WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3::InnerBroadcastReceiver3(
    /* [in] */ WifiChangeStateMachine* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::InnerBroadcastReceiver3::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // sendMessage(WIFI_CHANGE_CMD_CHANGE_TIMEOUT);
    assert(0);
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
    /* [in] */ ILooper* looper)
{
    mDefaultState = new DefaultState();
    mStationaryState = new StationaryState();
    mMovingState = new MovingState();
    mClientInfo = new ClientInfoLocal();
    // ==================before translated======================
    // super("SignificantChangeStateMachine", looper);
    //
    // mClients.put(null, mClientInfo);
    //
    // addState(mDefaultState);
    // addState(mStationaryState, mDefaultState);
    // addState(mMovingState, mDefaultState);
    //
    // setInitialState(mDefaultState);
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Enable()
{
    // ==================before translated======================
    // if (mAlarmManager == null) {
    //     mAlarmManager = (AlarmManager) mContext.getSystemService(Context.ALARM_SERVICE);
    // }
    //
    // if (mTimeoutIntent == null) {
    //     Intent intent = new Intent(ACTION_TIMEOUT, null);
    //     mTimeoutIntent = PendingIntent.getBroadcast(mContext, 0, intent, 0);
    //
    //     mContext.registerReceiver(
    //             new BroadcastReceiver() {
    //                 @Override
    //                 public void onReceive(Context context, Intent intent) {
    //                     sendMessage(WIFI_CHANGE_CMD_CHANGE_TIMEOUT);
    //                 }
    //             }, new IntentFilter(ACTION_TIMEOUT));
    // }
    //
    // sendMessage(WIFI_CHANGE_CMD_ENABLE);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Disable()
{
    // ==================before translated======================
    // sendMessage(WIFI_CHANGE_CMD_DISABLE);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::Configure(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    // ==================before translated======================
    // sendMessage(WIFI_CHANGE_CMD_CONFIGURE, settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ReconfigureScan(
    /* [in] */ ArrayOf<IScanResult*>* results,
    /* [in] */ Int32 period)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // // find brightest APs and set them as sentinels
    // if (results.length < MAX_APS_TO_TRACK) {
    //     Log.d(TAG, "too few APs (" + results.length + ") available to track wifi change");
    //     return;
    // }
    //
    // removeScanRequest();
    //
    // // remove duplicate BSSIDs
    // HashMap<String, ScanResult> bssidToScanResult = new HashMap<String, ScanResult>();
    // for (ScanResult result : results) {
    //     ScanResult saved = bssidToScanResult.get(result.BSSID);
    //     if (saved == null) {
    //         bssidToScanResult.put(result.BSSID, result);
    //     } else if (saved.level > result.level) {
    //         bssidToScanResult.put(result.BSSID, result);
    //     }
    // }
    //
    // // find brightest BSSIDs
    // ScanResult brightest[] = new ScanResult[MAX_APS_TO_TRACK];
    // Collection<ScanResult> results2 = bssidToScanResult.values();
    // for (ScanResult result : results2) {
    //     for (int j = 0; j < brightest.length; j++) {
    //         if (brightest[j] == null
    //                 || (brightest[j].level < result.level)) {
    //             for (int k = brightest.length; k > (j + 1); k--) {
    //                 brightest[k - 1] = brightest[k - 2];
    //             }
    //             brightest[j] = result;
    //             break;
    //         }
    //     }
    // }
    //
    // // Get channels to scan for
    // ArrayList<Integer> channels = new ArrayList<Integer>();
    // for (int i = 0; i < brightest.length; i++) {
    //     boolean found = false;
    //     for (int j = i + 1; j < brightest.length; j++) {
    //         if (brightest[j].frequency == brightest[i].frequency) {
    //             found = true;
    //         }
    //     }
    //     if (!found) {
    //         channels.add(brightest[i].frequency);
    //     }
    // }
    //
    // if (DBG) Log.d(TAG, "Found " + channels.size() + " channels");
    //
    // // set scanning schedule
    // WifiScanner.ScanSettings settings = new WifiScanner.ScanSettings();
    // settings.band = WifiScanner.WIFI_BAND_UNSPECIFIED;
    // settings.channels = new WifiScanner.ChannelSpec[channels.size()];
    // for (int i = 0; i < channels.size(); i++) {
    //     settings.channels[i] = new WifiScanner.ChannelSpec(channels.get(i));
    // }
    //
    // settings.periodInMs = period;
    // addScanRequest(settings);
    //
    // WifiScanner.WifiChangeSettings settings2 = new WifiScanner.WifiChangeSettings();
    // settings2.rssiSampleSize = 3;
    // settings2.lostApSampleSize = 3;
    // settings2.unchangedSampleSize = 3;
    // settings2.minApsBreachingThreshold = 2;
    // settings2.bssidInfos = new WifiScanner.BssidInfo[brightest.length];
    //
    // for (int i = 0; i < brightest.length; i++) {
    //     WifiScanner.BssidInfo BssidInfo = new WifiScanner.BssidInfo();
    //     BssidInfo.bssid = brightest[i].BSSID;
    //     int threshold = (100 + brightest[i].level) / 32 + 2;
    //     BssidInfo.low = brightest[i].level - threshold;
    //     BssidInfo.high = brightest[i].level + threshold;
    //     settings2.bssidInfos[i] = BssidInfo;
    //
    //     if (DBG) Log.d(TAG, "Setting bssid=" + BssidInfo.bssid + ", " +
    //             "low=" + BssidInfo.low + ", high=" + BssidInfo.high);
    // }
    //
    // trackSignificantWifiChange(settings2);
    // mCurrentBssids = brightest;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::ReconfigureScan(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    // ==================before translated======================
    //
    // if (settings.bssidInfos.length < MAX_APS_TO_TRACK) {
    //     Log.d(TAG, "too few APs (" + settings.bssidInfos.length
    //             + ") available to track wifi change");
    //     return;
    // }
    //
    // if (DBG) Log.d(TAG, "Setting configuration specified by app");
    //
    // mCurrentBssids = new ScanResult[settings.bssidInfos.length];
    // HashSet<Integer> channels = new HashSet<Integer>();
    //
    // for (int i = 0; i < settings.bssidInfos.length; i++) {
    //     ScanResult result = new ScanResult();
    //     result.BSSID = settings.bssidInfos[i].bssid;
    //     mCurrentBssids[i] = result;
    //     channels.add(settings.bssidInfos[i].frequencyHint);
    // }
    //
    // // cancel previous scan
    // removeScanRequest();
    //
    // // set new scanning schedule
    // WifiScanner.ScanSettings settings2 = new WifiScanner.ScanSettings();
    // settings2.band = WifiScanner.WIFI_BAND_UNSPECIFIED;
    // settings2.channels = new WifiScanner.ChannelSpec[channels.size()];
    // int i = 0;
    // for (Integer channel : channels) {
    //     settings2.channels[i++] = new WifiScanner.ChannelSpec(channel);
    // }
    //
    // settings2.periodInMs = settings.periodInMs;
    // addScanRequest(settings2);
    //
    // // start tracking new APs
    // trackSignificantWifiChange(settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::OnChangesFound(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // sendMessage(WIFI_CHANGE_CMD_CHANGE_DETECTED, 0, 0, results);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::AddScanRequest(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Starting scans");
    // Message msg = Message.obtain();
    // msg.what = WifiScanner.CMD_START_BACKGROUND_SCAN;
    // msg.arg2 = SCAN_COMMAND_ID;
    // msg.obj = settings;
    // mClientHandler.sendMessage(msg);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::RemoveScanRequest()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Stopping scans");
    // Message msg = Message.obtain();
    // msg.what = WifiScanner.CMD_STOP_BACKGROUND_SCAN;
    // msg.arg2 = SCAN_COMMAND_ID;
    // mClientHandler.sendMessage(msg);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::TrackSignificantWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    // ==================before translated======================
    // WifiNative.untrackSignificantWifiChange();
    // WifiNative.trackSignificantWifiChange(settings, this);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::WifiChangeStateMachine::UntrackSignificantWifiChange()
{
    // ==================before translated======================
    // WifiNative.untrackSignificantWifiChange();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                WifiScanningServiceImpl::ClientHandler
//=====================================================================
WifiScanningServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ ILooper* looper)
{
    // ==================before translated======================
    // super(looper);
}

ECode WifiScanningServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    //
    // if (DBG) Log.d(TAG, "ClientHandler got" + msg);
    //
    // switch (msg.what) {
    //
    //     case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED:
    //         if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
    //             AsyncChannel c = (AsyncChannel) msg.obj;
    //             if (DBG) Slog.d(TAG, "New client listening to asynchronous messages: " +
    //                     msg.replyTo);
    //             ClientInfo cInfo = new ClientInfo(c, msg.replyTo);
    //             mClients.put(msg.replyTo, cInfo);
    //         } else {
    //             Slog.e(TAG, "Client connection failure, error=" + msg.arg1);
    //         }
    //         return;
    //     case AsyncChannel.CMD_CHANNEL_DISCONNECTED:
    //         if (msg.arg1 == AsyncChannel.STATUS_SEND_UNSUCCESSFUL) {
    //             Slog.e(TAG, "Send failed, client connection lost");
    //         } else {
    //             if (DBG) Slog.d(TAG, "Client connection lost with reason: " + msg.arg1);
    //         }
    //         if (DBG) Slog.d(TAG, "closing client " + msg.replyTo);
    //         ClientInfo ci = mClients.remove(msg.replyTo);
    //         if (ci != null) {                       /* can be null if send failed above */
    //             ci.cleanup();
    //         }
    //         return;
    //     case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION:
    //         AsyncChannel ac = new AsyncChannel();
    //         ac.connect(mContext, this, msg.replyTo);
    //         return;
    // }
    //
    // ClientInfo ci = mClients.get(msg.replyTo);
    // if (ci == null) {
    //     Slog.e(TAG, "Could not find client info for message " + msg.replyTo);
    //     replyFailed(msg, WifiScanner.REASON_INVALID_LISTENER, "Could not find listener");
    //     return;
    // }
    //
    // try {
    //     enforceConnectivityInternalPermission();
    // } catch (SecurityException e) {
    //     replyFailed(msg, WifiScanner.REASON_NOT_AUTHORIZED, "Not authorized");
    //     return;
    // }
    //
    // int validCommands[] = {
    //         WifiScanner.CMD_SCAN,
    //         WifiScanner.CMD_START_BACKGROUND_SCAN,
    //         WifiScanner.CMD_STOP_BACKGROUND_SCAN,
    //         WifiScanner.CMD_SET_HOTLIST,
    //         WifiScanner.CMD_RESET_HOTLIST,
    //         WifiScanner.CMD_CONFIGURE_WIFI_CHANGE,
    //         WifiScanner.CMD_START_TRACKING_CHANGE,
    //         WifiScanner.CMD_STOP_TRACKING_CHANGE };
    //
    // for (int cmd : validCommands) {
    //     if (cmd == msg.what) {
    //         mStateMachine.sendMessage(Message.obtain(msg));
    //         return;
    //     }
    // }
    //
    // replyFailed(msg, WifiScanner.REASON_INVALID_REQUEST, "Invalid request");
    assert(0);
    return NOERROR;
}

//=====================================================================
//           WifiScanningServiceImpl::InnerBroadcastReceiver1
//=====================================================================
WifiScanningServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiScanningServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiScanningServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // int state = intent.getIntExtra(
    //         WifiManager.EXTRA_SCAN_AVAILABLE, WifiManager.WIFI_STATE_DISABLED);
    // if (DBG) Log.d(TAG, "SCAN_AVAILABLE : " + state);
    // if (state == WifiManager.WIFI_STATE_ENABLED) {
    //     mStateMachine.sendMessage(CMD_DRIVER_LOADED);
    // } else if (state == WifiManager.WIFI_STATE_DISABLED) {
    //     mStateMachine.sendMessage(CMD_DRIVER_UNLOADED);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                 WifiScanningServiceImpl::ClientInfo
//=====================================================================
const Int32 WifiScanningServiceImpl::ClientInfo::MAX_LIMIT;

WifiScanningServiceImpl::ClientInfo::ClientInfo(
    /* [in] */ IAsyncChannel* c,
    /* [in] */ IMessenger* m)
{
    CHashMap::New((IHashMap**)&mScanSettings);
    CHashMap::New((IHashMap**)&mScanPeriods);
    CHashMap::New((IHashMap**)&mHotlistSettings);
    CHashSet::New((IHashSet**)&mSignificantWifiHandlers);
    // ==================before translated======================
    // mChannel = c;
    // mMessenger = m;
    // if (DBG) Slog.d(TAG, "New client, channel: " + c + " messenger: " + m);
}

ECode WifiScanningServiceImpl::ClientInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // StringBuffer sb = new StringBuffer();
    // sb.append("mChannel ").append(mChannel).append("\n");
    // sb.append("mMessenger ").append(mMessenger).append("\n");
    //
    // Iterator<Map.Entry<Integer, ScanSettings>> it = mScanSettings.entrySet().iterator();
    // for (; it.hasNext(); ) {
    //     Map.Entry<Integer, ScanSettings> entry = it.next();
    //     sb.append("[ScanId ").append(entry.getKey()).append("\n");
    //     sb.append("ScanSettings ").append(entry.getValue()).append("\n");
    //     sb.append("]");
    // }
    //
    // return sb.toString();
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::AddScanRequest(
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 id)
{
    VALIDATE_NOT_NULL(settings);
    // ==================before translated======================
    // mScanSettings.put(id, settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveScanRequest(
    /* [in] */ Int32 id)
{
    // ==================before translated======================
    // mScanSettings.remove(id);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetScans(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mScanSettings.entrySet().iterator();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetScanSettings(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mScanSettings.values();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportScanResults(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // Iterator<Integer> it = mScanSettings.keySet().iterator();
    // while (it.hasNext()) {
    //     int handler = it.next();
    //     reportScanResults(results, handler);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportScanResults(
    /* [in] */ ArrayOf<IScanResult*>* results,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // ScanSettings settings = mScanSettings.get(handler);
    // WifiScanner.ChannelSpec desiredChannels[] = settings.channels;
    // if (settings.band != WifiScanner.WIFI_BAND_UNSPECIFIED
    //         || desiredChannels == null || desiredChannels.length == 0)  {
    //     desiredChannels = getChannelsForBand(settings.band);
    // }
    //
    // // check the channels this client asked for ..
    // int num_results = 0;
    //
    // if (results != null) {
    //     for (ScanResult result : results) {
    //         for (WifiScanner.ChannelSpec channelSpec : desiredChannels) {
    //             if (channelSpec.frequency == result.frequency) {
    //                 num_results++;
    //                 break;
    //             }
    //         }
    //     }
    // }
    //
    // if (num_results == 0) {
    //     // nothing to report
    //     return;
    // }
    //
    // ScanResult results2[] = new ScanResult[num_results];
    // int index = 0;
    // for (ScanResult result : results) {
    //     for (WifiScanner.ChannelSpec channelSpec : desiredChannels) {
    //         if (channelSpec.frequency == result.frequency) {
    //             WifiSsid wifiSsid = WifiSsid.createFromAsciiEncoded(result.SSID);
    //             ScanResult newResult = new ScanResult(wifiSsid, result.BSSID, "",
    //                     result.level, result.frequency, result.timestamp);
    //             results2[index] = newResult;
    //             index++;
    //             break;
    //         }
    //     }
    // }
    //
    // deliverScanResults(handler, results2);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::DeliverScanResults(
    /* [in] */ Int32 handler,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // WifiScanner.ParcelableScanResults parcelableScanResults =
    //         new WifiScanner.ParcelableScanResults(results);
    // mChannel.sendMessage(WifiScanner.CMD_SCAN_RESULT, 0, handler, parcelableScanResults);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportFullScanResult(
    /* [in] */ IScanResult* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Iterator<Integer> it = mScanSettings.keySet().iterator();
    // while (it.hasNext()) {
    //     int handler = it.next();
    //     ScanSettings settings = mScanSettings.get(handler);
    //     WifiScanner.ChannelSpec desiredChannels[] = settings.channels;
    //     if (settings.band != WifiScanner.WIFI_BAND_UNSPECIFIED
    //             || desiredChannels == null || desiredChannels.length == 0)  {
    //         desiredChannels = getChannelsForBand(settings.band);
    //     }
    //     for (WifiScanner.ChannelSpec channelSpec : desiredChannels) {
    //         if (channelSpec.frequency == result.frequency) {
    //             WifiSsid wifiSsid = WifiSsid.createFromAsciiEncoded(result.SSID);
    //             ScanResult newResult = new ScanResult(wifiSsid, result.BSSID, "",
    //                     result.level, result.frequency, result.timestamp);
    //             if (DBG) Log.d(TAG, "sending it to " + handler);
    //             newResult.informationElements = result.informationElements.clone();
    //             mChannel.sendMessage(
    //                     WifiScanner.CMD_FULL_SCAN_RESULT, 0, handler, newResult);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportPeriodChanged(
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [in] */ Int32 newPeriodInMs)
{
    VALIDATE_NOT_NULL(settings);
    // ==================before translated======================
    // Integer prevPeriodObject = mScanPeriods.get(handler);
    // int prevPeriodInMs = settings.periodInMs;
    // if (prevPeriodObject != null) {
    //     prevPeriodInMs = prevPeriodObject;
    // }
    //
    // if (prevPeriodInMs != newPeriodInMs) {
    //     mChannel.sendMessage(WifiScanner.CMD_PERIOD_CHANGED, newPeriodInMs, handler);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::AddHostlistSettings(
    /* [in] */ IWifiScannerHotlistSettings* settings,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(settings);
    // ==================before translated======================
    // mHotlistSettings.put(handler, settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveHostlistSettings(
    /* [in] */ Int32 handler)
{
    // ==================before translated======================
    // mHotlistSettings.remove(handler);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetHotlistSettings(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHotlistSettings.values();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportHotlistResults(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // Iterator<Map.Entry<Integer, WifiScanner.HotlistSettings>> it =
    //         mHotlistSettings.entrySet().iterator();
    // while (it.hasNext()) {
    //     Map.Entry<Integer, WifiScanner.HotlistSettings> entry = it.next();
    //     int handler = entry.getKey();
    //     WifiScanner.HotlistSettings settings = entry.getValue();
    //     int num_results = 0;
    //
    //     if (results != null) {
    //         for (ScanResult result : results) {
    //             for (WifiScanner.BssidInfo BssidInfo : settings.bssidInfos) {
    //                 if (result.BSSID.equalsIgnoreCase(BssidInfo.bssid)) {
    //                     num_results++;
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //
    //     if (num_results == 0) {
    //         // nothing to report
    //         return;
    //     }
    //
    //     ScanResult results2[] = new ScanResult[num_results];
    //     int index = 0;
    //     for (ScanResult result : results) {
    //         for (WifiScanner.BssidInfo BssidInfo : settings.bssidInfos) {
    //             if (result.BSSID.equalsIgnoreCase(BssidInfo.bssid)) {
    //                 results2[index] = result;
    //                 index++;
    //             }
    //         }
    //     }
    //
    //     WifiScanner.ParcelableScanResults parcelableScanResults =
    //             new WifiScanner.ParcelableScanResults(results2);
    //
    //     mChannel.sendMessage(WifiScanner.CMD_AP_FOUND, 0, handler, parcelableScanResults);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::AddSignificantWifiChange(
    /* [in] */ Int32 handler)
{
    // ==================before translated======================
    // mSignificantWifiHandlers.add(handler);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::RemoveSignificantWifiChange(
    /* [in] */ Int32 handler)
{
    // ==================before translated======================
    // mSignificantWifiHandlers.remove(handler);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::GetWifiChangeHandlers(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSignificantWifiHandlers;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportWifiChanged(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // WifiScanner.ParcelableScanResults parcelableScanResults =
    //         new WifiScanner.ParcelableScanResults(results);
    // Iterator<Integer> it = mSignificantWifiHandlers.iterator();
    // while (it.hasNext()) {
    //     int handler = it.next();
    //     mChannel.sendMessage(WifiScanner.CMD_WIFI_CHANGE_DETECTED,
    //             0, handler, parcelableScanResults);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::ReportWifiStabilized(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // WifiScanner.ParcelableScanResults parcelableScanResults =
    //         new WifiScanner.ParcelableScanResults(results);
    // Iterator<Integer> it = mSignificantWifiHandlers.iterator();
    // while (it.hasNext()) {
    //     int handler = it.next();
    //     mChannel.sendMessage(WifiScanner.CMD_WIFI_CHANGES_STABILIZED,
    //             0, handler, parcelableScanResults);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ClientInfo::Cleanup()
{
    // ==================before translated======================
    // mScanSettings.clear();
    // resetBuckets();
    //
    // mHotlistSettings.clear();
    // resetHotlist();
    //
    // for (Integer handler :  mSignificantWifiHandlers) {
    //     untrackWifiChanges(this, handler);
    // }
    //
    // mSignificantWifiHandlers.clear();
    // Log.d(TAG, "Successfully stopped all requests for client " + this);
    assert(0);
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
    // ==================before translated======================
    // periodInSecond = p;
    // periodMinInSecond = min;
    // periodMaxInSecond = max;
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
    //TODO
    //{
    //    mSettings = new WifiNative.ScanSettings();
    //    mSettings.max_ap_per_scan = DEFAULT_MAX_AP_PER_SCAN;
    //    mSettings.base_period_ms = DEFAULT_BASE_PERIOD_MS;
    //    mSettings.report_threshold = DEFAULT_REPORT_THRESHOLD;

    //    mSettings.buckets = new WifiNative.BucketSettings[MAX_BUCKETS];
    //    for (int i = 0; i < mSettings.buckets.length; i++) {
    //        WifiNative.BucketSettings bucketSettings = new WifiNative.BucketSettings();
    //        bucketSettings.bucket = i;
    //        bucketSettings.report_events = 0;
    //        bucketSettings.channels = new WifiNative.ChannelSettings[MAX_CHANNELS];
    //        bucketSettings.num_channels = 0;
    //        for (int j = 0; j < bucketSettings.channels.length; j++) {
    //            WifiNative.ChannelSettings channelSettings = new WifiNative.ChannelSettings();
    //            bucketSettings.channels[j] = channelSettings;
    //        }
    //        mSettings.buckets[i] = bucketSettings;
    //    }
    //}
}

ECode WifiScanningServiceImpl::SettingsComputer::PrepChannelMap(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    // ==================before translated======================
    // getBestBucket(settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::AddScanRequestToBucket(
    /* [in] */  IWifiScannerScanSettings* settings,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // int bucketIndex = getBestBucket(settings);
    // if (bucketIndex == -1) {
    //     Log.e(TAG, "Ignoring invalid settings");
    //     return -1;
    // }
    //
    // WifiScanner.ChannelSpec desiredChannels[] = settings.channels;
    // if (settings.band != WifiScanner.WIFI_BAND_UNSPECIFIED
    //         || desiredChannels == null
    //         || desiredChannels.length == 0) {
    //     // set channels based on band
    //     desiredChannels = getChannelsForBand(settings.band);
    //     if (desiredChannels == null) {
    //         // still no channels; then there's nothing to scan
    //         Log.e(TAG, "No channels to scan!!");
    //         return -1;
    //     }
    // }
    //
    // // merge the channel lists for these buckets
    // Log.d(TAG, "merging " + desiredChannels.length + " channels "
    //         + " for period " + settings.periodInMs);
    //
    // WifiNative.BucketSettings bucket = mSettings.buckets[bucketIndex];
    // boolean added = (bucket.num_channels == 0)
    //         && (bucket.band == WifiScanner.WIFI_BAND_UNSPECIFIED);
    // Log.d(TAG, "existing " + bucket.num_channels + " channels ");
    //
    // HashSet<WifiScanner.ChannelSpec> newChannels = new HashSet<WifiScanner.ChannelSpec>();
    // for (WifiScanner.ChannelSpec desiredChannelSpec : desiredChannels) {
    //
    //     Log.d(TAG, "desired channel " + desiredChannelSpec.frequency);
    //
    //     boolean found = false;
    //     for (WifiNative.ChannelSettings existingChannelSpec : bucket.channels) {
    //         if (desiredChannelSpec.frequency == existingChannelSpec.frequency) {
    //             found = true;
    //             break;
    //         }
    //     }
    //
    //     if (!found) {
    //         newChannels.add(desiredChannelSpec);
    //     } else {
    //         if (DBG) Log.d(TAG, "Already scanning channel " + desiredChannelSpec.frequency);
    //     }
    // }
    //
    // if (settings.band != WifiScanner.WIFI_BAND_UNSPECIFIED
    //         || (bucket.num_channels + newChannels.size()) > bucket.channels.length) {
    //     // can't accommodate all channels; switch to specifying band
    //     bucket.num_channels = 0;
    //     bucket.band = getBandFromChannels(bucket.channels)
    //             | getBandFromChannels(desiredChannels);
    //     bucket.channels = new WifiNative.ChannelSettings[0];
    //     Log.d(TAG, "switching to using band " + bucket.band);
    // } else {
    //     for (WifiScanner.ChannelSpec desiredChannelSpec : newChannels) {
    //
    //         Log.d(TAG, "adding new channel spec " + desiredChannelSpec.frequency);
    //
    //         WifiNative.ChannelSettings channelSettings = bucket.channels[bucket.num_channels];
    //         channelSettings.frequency = desiredChannelSpec.frequency;
    //         bucket.num_channels++;
    //         mChannelToBucketMap.put(bucketIndex, channelSettings.frequency);
    //     }
    // }
    //
    // if (bucket.report_events < settings.reportEvents) {
    //     if (DBG) Log.d(TAG, "setting report_events to " + settings.reportEvents);
    //     bucket.report_events = settings.reportEvents;
    // } else {
    //     if (DBG) Log.d(TAG, "report_events is " + settings.reportEvents);
    // }
    //
    // if (added) {
    //     bucket.period_ms = mTimeBuckets[bucketIndex].periodInSecond * 1000;
    //     mSettings.num_buckets++;
    // }
    //
    // if (mSettings.max_ap_per_scan < settings.numBssidsPerScan) {
    //     mSettings.max_ap_per_scan = settings.numBssidsPerScan;
    // }
    //
    // return bucket.period_ms;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::GetComputedSettings(
    /* [out] */ WifiNative::ScanSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSettings;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SettingsComputer::CompressBuckets()
{
    // ==================before translated======================
    // int num_buckets = 0;
    // for (int i = 0; i < mSettings.buckets.length; i++) {
    //     if (mSettings.buckets[i].num_channels != 0
    //             || mSettings.buckets[i].band != WifiScanner.WIFI_BAND_UNSPECIFIED) {
    //         mSettings.buckets[num_buckets] = mSettings.buckets[i];
    //         num_buckets++;
    //     }
    // }
    // // remove unused buckets
    // for (int i = num_buckets; i < mSettings.buckets.length; i++) {
    //     mSettings.buckets[i] = null;
    // }
    //
    // mSettings.num_buckets = num_buckets;
    // if (num_buckets != 0) {
    //     mSettings.base_period_ms = mSettings.buckets[0].period_ms;
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<ArrayOf<WifiScanningServiceImpl::SettingsComputer::TimeBucket*> > WifiScanningServiceImpl::SettingsComputer::MiddleInitMtimebuckets()
{
    // ==================before translated======================
    // TimeBucket[] result = new TimeBucket[] {
    //                  new TimeBucket( 1, 0, 5 ),
    //                  new TimeBucket( 5, 5, 10 ),
    //                  new TimeBucket( 10, 10, 25 ),
    //                  new TimeBucket( 30, 25, 55 ),
    //                  new TimeBucket( 60, 55, 100),
    //                  new TimeBucket( 300, 240, 500),
    //                  new TimeBucket( 600, 500, 1500),
    //                  new TimeBucket( 1800, 1500, WifiScanner.MAX_SCAN_PERIOD_MS) };
    // assert(0 && "TODO");
    AutoPtr<ArrayOf<TimeBucket*> > empty;
    return empty;
}

Int32 WifiScanningServiceImpl::SettingsComputer::GetBestBucket(
    /* [in] */ IWifiScannerScanSettings* settings)
{
    // ==================before translated======================
    //
    // // check to see if any of the channels are being scanned already
    // // and find the smallest bucket index (it represents the quickest
    // // period of scan)
    //
    // WifiScanner.ChannelSpec channels[] = settings.channels;
    // if (channels == null) {
    //     // set channels based on band
    //     channels = getChannelsForBand(settings.band);
    // }
    //
    // if (channels == null) {
    //     // still no channels; then there's nothing to scan
    //     Log.e(TAG, "No channels to scan!!");
    //     return -1;
    // }
    //
    // int mostFrequentBucketIndex = mTimeBuckets.length;
    //
    // for (WifiScanner.ChannelSpec desiredChannelSpec : channels) {
    //     if (mChannelToBucketMap.containsKey(desiredChannelSpec.frequency)) {
    //         int bucket = mChannelToBucketMap.get(desiredChannelSpec.frequency);
    //         if (bucket < mostFrequentBucketIndex) {
    //             mostFrequentBucketIndex = bucket;
    //         }
    //     }
    // }
    //
    // int bestBucketIndex = -1;                                   // best by period
    // for (int i = 0; i < mTimeBuckets.length; i++) {
    //     TimeBucket bucket = mTimeBuckets[i];
    //     if (bucket.periodMinInSecond * 1000 <= settings.periodInMs
    //             && settings.periodInMs < bucket.periodMaxInSecond * 1000) {
    //         // we set the time period to this
    //         bestBucketIndex = i;
    //         break;
    //     }
    // }
    //
    // if (mostFrequentBucketIndex < bestBucketIndex) {
    //     for (WifiScanner.ChannelSpec desiredChannelSpec : channels) {
    //         mChannelToBucketMap.put(desiredChannelSpec.frequency, mostFrequentBucketIndex);
    //     }
    //     Log.d(TAG, "returning mf bucket number " + mostFrequentBucketIndex);
    //     return mostFrequentBucketIndex;
    // } else if (bestBucketIndex != -1) {
    //     for (WifiScanner.ChannelSpec desiredChannelSpec : channels) {
    //         mChannelToBucketMap.put(desiredChannelSpec.frequency, bestBucketIndex);
    //     }
    //     Log.d(TAG, "returning best bucket number " + bestBucketIndex);
    //     return bestBucketIndex;
    // }
    //
    // Log.e(TAG, "Could not find suitable bucket for period " + settings.periodInMs);
    // return -1;
    assert(0);
    return 0;
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
    // ==================before translated======================
    // mContext = context;
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetMessenger(
    /* [out] */ IMessenger** messenger)
{
    // ==================before translated======================
    // return new Messenger(mClientHandler);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetAvailableChannels(
    /* [in] */ Int32 band,
    /* [out] */ IBundle** bundle)
{
    // ==================before translated======================
    // WifiScanner.ChannelSpec channelSpecs[] = getChannelsForBand(band);
    // ArrayList<Integer> list = new ArrayList<Integer>(channelSpecs.length);
    // for (WifiScanner.ChannelSpec channelSpec : channelSpecs) {
    //     list.add(channelSpec.frequency);
    // }
    // Bundle b = new Bundle();
    // b.putIntegerArrayList(WifiScanner.GET_AVAILABLE_CHANNELS_EXTRA, list);
    // return b;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::StartService(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    // ==================before translated======================
    // mContext = context;
    //
    // HandlerThread thread = new HandlerThread("WifiScanningService");
    // thread.start();
    //
    // mClientHandler = new ClientHandler(thread.getLooper());
    // mStateMachine = new WifiScanningStateMachine(thread.getLooper());
    // mWifiChangeStateMachine = new WifiChangeStateMachine(thread.getLooper());
    //
    // mContext.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 int state = intent.getIntExtra(
    //                         WifiManager.EXTRA_SCAN_AVAILABLE, WifiManager.WIFI_STATE_DISABLED);
    //                 if (DBG) Log.d(TAG, "SCAN_AVAILABLE : " + state);
    //                 if (state == WifiManager.WIFI_STATE_ENABLED) {
    //                     mStateMachine.sendMessage(CMD_DRIVER_LOADED);
    //                 } else if (state == WifiManager.WIFI_STATE_DISABLED) {
    //                     mStateMachine.sendMessage(CMD_DRIVER_UNLOADED);
    //                 }
    //             }
    //         }, new IntentFilter(WifiManager.WIFI_SCAN_AVAILABLE));
    //
    // mStateMachine.start();
    // mWifiChangeStateMachine.start();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReplySucceeded(
    /* [in] */ IMessage* msg,
    /* [in] */ IObject* obj)
{
    VALIDATE_NOT_NULL(msg);
    VALIDATE_NOT_NULL(obj);
    // ==================before translated======================
    // if (msg.replyTo != null) {
    //     Message reply = Message.obtain();
    //     reply.what = WifiScanner.CMD_OP_SUCCEEDED;
    //     reply.arg2 = msg.arg2;
    //     reply.obj = obj;
    //     try {
    //         msg.replyTo.send(reply);
    //     } catch (RemoteException e) {
    //         // There's not much we can do if reply can't be sent!
    //     }
    // } else {
    //     // locally generated message; doesn't need a reply!
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // if (msg.replyTo != null) {
    //     Message reply = Message.obtain();
    //     reply.what = WifiScanner.CMD_OP_FAILED;
    //     reply.arg2 = msg.arg2;
    //     reply.obj = new WifiScanner.OperationResult(reason, description);
    //     try {
    //         msg.replyTo.send(reply);
    //     } catch (RemoteException e) {
    //         // There's not much we can do if reply can't be sent!
    //     }
    // } else {
    //     // locally generated message; doesn't need a reply!
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetBuckets(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SettingsComputer c = new SettingsComputer();
    // Collection<ClientInfo> clients = mClients.values();
    // for (ClientInfo ci : clients) {
    //     Collection<ScanSettings> settings = ci.getScanSettings();
    //     for (ScanSettings s : settings) {
    //         c.prepChannelMap(s);
    //     }
    // }
    //
    // for (ClientInfo ci : clients) {
    //     Iterator it = ci.getScans();
    //     while (it.hasNext()) {
    //         Map.Entry<Integer, WifiScanner.ScanSettings> entry =
    //                 (Map.Entry<Integer,WifiScanner.ScanSettings>)it.next();
    //         int id = entry.getKey();
    //         ScanSettings s = entry.getValue();
    //         int newPeriodInMs = c.addScanRequestToBucket(s);
    //         if (newPeriodInMs  == -1) {
    //             if (DBG) Log.d(TAG, "could not find a good bucket");
    //             return false;
    //         }
    //         if (newPeriodInMs != s.periodInMs) {
    //             ci.reportPeriodChanged(id, s, newPeriodInMs);
    //         }
    //     }
    // }
    //
    // c.compressBuckets();
    //
    // WifiNative.ScanSettings s = c.getComputedSettings();
    // if (s.num_buckets == 0) {
    //     if (DBG) Log.d(TAG, "Stopping scan because there are no buckets");
    //     WifiNative.stopScan();
    //     return true;
    // } else {
    //     if (WifiNative.startScan(s, mStateMachine)) {
    //         if (DBG) Log.d(TAG, "Successfully started scan of " + s.num_buckets + " buckets at"
    //                 + "time = " + SystemClock.elapsedRealtimeNanos()/1000);
    //         return true;
    //     } else {
    //         if (DBG) Log.d(TAG, "Failed to start scan of " + s.num_buckets + " buckets");
    //         return false;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::AddScanRequest(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerScanSettings* settings,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ci);
    VALIDATE_NOT_NULL(settings);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // sanity check the input
    // if (settings.periodInMs < WifiScanner.MIN_SCAN_PERIOD_MS) {
    //     Log.d(TAG, "Failing scan request because periodInMs is " + settings.periodInMs);
    //     return false;
    // }
    //
    // int minSupportedPeriodMs = 0;
    // if (settings.channels != null) {
    //     minSupportedPeriodMs = settings.channels.length * MIN_PERIOD_PER_CHANNEL_MS;
    // } else {
    //     if ((settings.band & WifiScanner.WIFI_BAND_24_GHZ) == 0) {
    //         /* 2.4 GHz band has 11 to 13 channels */
    //         minSupportedPeriodMs += 1000;
    //     }
    //     if ((settings.band & WifiScanner.WIFI_BAND_5_GHZ) == 0) {
    //         /* 5 GHz band has another 10 channels */
    //         minSupportedPeriodMs += 1000;
    //     }
    //     if ((settings.band & WifiScanner.WIFI_BAND_5_GHZ_DFS_ONLY) == 0) {
    //         /* DFS requires passive scan which takes longer time */
    //         minSupportedPeriodMs += 2000;
    //     }
    // }
    //
    // if (settings.periodInMs < minSupportedPeriodMs) {
    //     Log.d(TAG, "Failing scan request because minSupportedPeriodMs is "
    //             + minSupportedPeriodMs + " but the request wants " + settings.periodInMs);
    //     return false;
    // }
    //
    // ci.addScanRequest(settings, handler);
    // if (resetBuckets()) {
    //     return true;
    // } else {
    //     ci.removeScanRequest(handler);
    //     Log.d(TAG, "Failing scan request because failed to reset scan");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::RemoveScanRequest(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(ci);
    // ==================before translated======================
    // ci.removeScanRequest(handler);
    // resetBuckets();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::GetScanResults(
    /* [in] */ ClientInfo* ci,
    /* [out] */ ArrayOf<IScanResult*>** result)
{
    VALIDATE_NOT_NULL(ci);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ScanResult results[] = WifiNative.getScanResults();
    // ci.reportScanResults(results);
    // return results;
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetHotlist()
{
    // ==================before translated======================
    // Collection<ClientInfo> clients = mClients.values();
    // int num_hotlist_ap = 0;
    //
    // for (ClientInfo ci : clients) {
    //     Collection<WifiScanner.HotlistSettings> c = ci.getHotlistSettings();
    //     for (WifiScanner.HotlistSettings s : c) {
    //         num_hotlist_ap +=  s.bssidInfos.length;
    //     }
    // }
    //
    // if (num_hotlist_ap == 0) {
    //     WifiNative.resetHotlist();
    // } else {
    //     WifiScanner.BssidInfo bssidInfos[] = new WifiScanner.BssidInfo[num_hotlist_ap];
    //     int index = 0;
    //     for (ClientInfo ci : clients) {
    //         Collection<WifiScanner.HotlistSettings> settings = ci.getHotlistSettings();
    //         for (WifiScanner.HotlistSettings s : settings) {
    //             for (int i = 0; i < s.bssidInfos.length; i++, index++) {
    //                 bssidInfos[index] = s.bssidInfos[i];
    //             }
    //         }
    //     }
    //
    //     WifiScanner.HotlistSettings settings = new WifiScanner.HotlistSettings();
    //     settings.bssidInfos = bssidInfos;
    //     settings.apLostThreshold = 3;
    //     WifiNative.setHotlist(settings, mStateMachine);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::SetHotlist(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler,
    /* [in] */ IWifiScannerHotlistSettings* settings)
{
    VALIDATE_NOT_NULL(ci);
    // ==================before translated======================
    // ci.addHostlistSettings(settings, handler);
    // resetHotlist();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ResetHotlist(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(ci);
    // ==================before translated======================
    // ci.removeHostlistSettings(handler);
    // resetHotlist();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::TrackWifiChanges(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(ci);
    // ==================before translated======================
    // mWifiChangeStateMachine.enable();
    // ci.addSignificantWifiChange(handler);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::UntrackWifiChanges(
    /* [in] */ ClientInfo* ci,
    /* [in] */ Int32 handler)
{
    VALIDATE_NOT_NULL(ci);
    // ==================before translated======================
    // ci.removeSignificantWifiChange(handler);
    // Collection<ClientInfo> clients = mClients.values();
    // for (ClientInfo ci2 : clients) {
    //     if (ci2.getWifiChangeHandlers().size() != 0) {
    //         // there is at least one client watching for
    //         // significant changes; so nothing more to do
    //         return;
    //     }
    // }
    //
    // // no more clients looking for significant wifi changes
    // // no need to keep the state machine running; disable it
    // mWifiChangeStateMachine.disable();
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ConfigureWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    // ==================before translated======================
    // mWifiChangeStateMachine.configure(settings);
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReportWifiChanged(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // Collection<ClientInfo> clients = mClients.values();
    // for (ClientInfo ci : clients) {
    //     ci.reportWifiChanged(results);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiScanningServiceImpl::ReportWifiStabilized(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    VALIDATE_NOT_NULL(results);
    // ==================before translated======================
    // Collection<ClientInfo> clients = mClients.values();
    // for (ClientInfo ci : clients) {
    //     ci.reportWifiStabilized(results);
    // }
    assert(0);
    return NOERROR;
}

void WifiScanningServiceImpl::EnforceConnectivityInternalPermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(
    //         android.Manifest.permission.CONNECTIVITY_INTERNAL,
    //         "WifiScanningServiceImpl");
    assert(0);
}

AutoPtr<ArrayOf<IWifiScannerChannelSpec*> > WifiScanningServiceImpl::GetChannelsForBand(
    /* [in] */ Int32 band)
{
    // ==================before translated======================
    // int channels[] = WifiNative.getChannelsForBand(band);
    // if (channels != null) {
    //     WifiScanner.ChannelSpec channelSpecs[] = new WifiScanner.ChannelSpec[channels.length];
    //     for (int i = 0; i < channels.length; i++) {
    //         channelSpecs[i] = new WifiScanner.ChannelSpec(channels[i]);
    //     }
    //     return channelSpecs;
    // } else {
    //     return new WifiScanner.ChannelSpec[0];
    // }
    assert(0);
    return NULL;
}

Int32 WifiScanningServiceImpl::GetBandFromChannels(
    /* [in] */ ArrayOf<IWifiScannerChannelSpec*>* channels)
{
    // ==================before translated======================
    // int band = WifiScanner.WIFI_BAND_UNSPECIFIED;
    // for (WifiScanner.ChannelSpec channel : channels) {
    //     if (2400 <= channel.frequency && channel.frequency < 2500) {
    //         band |= WifiScanner.WIFI_BAND_24_GHZ;
    //     } else if (5100 <= channel.frequency && channel.frequency < 6000) {
    //         band |= WifiScanner.WIFI_BAND_5_GHZ;
    //     } else {
    //         /* TODO: Add DFS Range */
    //     }
    // }
    // return band;
    assert(0);
    return 0;
}

Int32 WifiScanningServiceImpl::GetBandFromChannels(
    /* [in] */ ArrayOf<WifiNative::ChannelSettings*>* channels)
{
    // ==================before translated======================
    // int band = WifiScanner.WIFI_BAND_UNSPECIFIED;
    // for (WifiNative.ChannelSettings channel : channels) {
    //     if (2400 <= channel.frequency && channel.frequency < 2500) {
    //         band |= WifiScanner.WIFI_BAND_24_GHZ;
    //     } else if (5100 <= channel.frequency && channel.frequency < 6000) {
    //         band |= WifiScanner.WIFI_BAND_5_GHZ;
    //     } else {
    //         /* TODO: Add DFS Range */
    //     }
    // }
    // return band;
    assert(0);
    return 0;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


