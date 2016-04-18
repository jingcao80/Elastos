
#include "elastos/droid/server/wifi/WifiServiceImpl.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Wifi::EIID_IIWifiManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                      WifiServiceImpl::TdlsTask
//=====================================================================
//AutoPtr<Integer> WifiServiceImpl::TdlsTask::DoInBackground(
//    /* [in] */  TdlsTaskParams)
ECode WifiServiceImpl::TdlsTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    // ==================before translated======================
    //
    // // Retrieve parameters for the call
    // TdlsTaskParams param = params[0];
    // String remoteIpAddress = param.remoteIpAddress.trim();
    // boolean enable = param.enable;
    //
    // // Get MAC address of Remote IP
    // String macAddress = null;
    //
    // BufferedReader reader = null;
    //
    // try {
    //     reader = new BufferedReader(new FileReader("/proc/net/arp"));
    //
    //     // Skip over the line bearing colum titles
    //     String line = reader.readLine();
    //
    //     while ((line = reader.readLine()) != null) {
    //         String[] tokens = line.split("[ ]+");
    //         if (tokens.length < 6) {
    //             continue;
    //         }
    //
    //         // ARP column format is
    //         // Address HWType HWAddress Flags Mask IFace
    //         String ip = tokens[0];
    //         String mac = tokens[3];
    //
    //         if (remoteIpAddress.equals(ip)) {
    //             macAddress = mac;
    //             break;
    //         }
    //     }
    //
    //     if (macAddress == null) {
    //         Slog.w(TAG, "Did not find remoteAddress {" + remoteIpAddress + "} in " +
    //                 "/proc/net/arp");
    //     } else {
    //         enableTdlsWithMacAddress(macAddress, enable);
    //     }
    //
    // } catch (FileNotFoundException e) {
    //     Slog.e(TAG, "Could not open /proc/net/arp to lookup mac address");
    // } catch (IOException e) {
    //     Slog.e(TAG, "Could not read /proc/net/arp to lookup mac address");
    // } finally {
    //     try {
    //         if (reader != null) {
    //             reader.close();
    //         }
    //     }
    //     catch (IOException e) {
    //         // Do nothing
    //     }
    // }
    //
    // return 0;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                      WifiServiceImpl::LockList
//=====================================================================
// synchronized
ECode WifiServiceImpl::LockList::HasLocks(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return !mList.isEmpty();
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::LockList::GetStrongestLockMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mList.isEmpty()) {
    //     return WifiManager.WIFI_MODE_FULL;
    // }
    //
    // if (mFullHighPerfLocksAcquired > mFullHighPerfLocksReleased) {
    //     return WifiManager.WIFI_MODE_FULL_HIGH_PERF;
    // }
    //
    // if (mFullLocksAcquired > mFullLocksReleased) {
    //     return WifiManager.WIFI_MODE_FULL;
    // }
    //
    // return WifiManager.WIFI_MODE_SCAN_ONLY;
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::LockList::UpdateWorkSource(
    /* [in] */ IWorkSource* ws)
{
    VALIDATE_NOT_NULL(ws);
    // ==================before translated======================
    // for (int i = 0; i < mLocks.mList.size(); i++) {
    //     ws.add(mLocks.mList.get(i).mWorkSource);
    // }
    assert(0);
    return NOERROR;
}

WifiServiceImpl::LockList::LockList()
{
    // ==================before translated======================
    // mList = new ArrayList<WifiLock>();
}

void WifiServiceImpl::LockList::AddLock(
    /* [in] */ WifiLock* lock)
{
    // ==================before translated======================
    // if (findLockByBinder(lock.mBinder) < 0) {
    //     mList.add(lock);
    // }
    assert(0);
}

AutoPtr<WifiServiceImpl::WifiLock> WifiServiceImpl::LockList::RemoveLock(
    /* [in] */ IBinder* binder)
{
    // ==================before translated======================
    // int index = findLockByBinder(binder);
    // if (index >= 0) {
    //     WifiLock ret = mList.remove(index);
    //     ret.unlinkDeathRecipient();
    //     return ret;
    // } else {
    //     return null;
    // }
    assert(0);
    AutoPtr<WifiLock> empty;
    return empty;
}

Int32 WifiServiceImpl::LockList::FindLockByBinder(
    /* [in] */ IBinder* binder)
{
    // ==================before translated======================
    // int size = mList.size();
    // for (int i = size - 1; i >= 0; i--) {
    //     if (mList.get(i).mBinder == binder)
    //         return i;
    // }
    // return -1;
    assert(0);
    return 0;
}

void WifiServiceImpl::LockList::Dump(
    /* [in] */ IPrintWriter* pw)
{
    // ==================before translated======================
    // for (WifiLock l : mList) {
    //     pw.print("    ");
    //     pw.println(l);
    // }
    assert(0);
}

//=====================================================================
//                    WifiServiceImpl::ClientHandler
//=====================================================================
WifiServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ ILooper* looper)
{
    // ==================before translated======================
    // super(looper);
}

ECode WifiServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch (msg.what) {
    //     case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
    //         if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
    //             if (DBG) Slog.d(TAG, "New client listening to asynchronous messages");
    //             // We track the clients by the Messenger
    //             // since it is expected to be always available
    //             mTrafficPoller.addClient(msg.replyTo);
    //         } else {
    //             Slog.e(TAG, "Client connection failure, error=" + msg.arg1);
    //         }
    //         break;
    //     }
    //     case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
    //         if (msg.arg1 == AsyncChannel.STATUS_SEND_UNSUCCESSFUL) {
    //             if (DBG) Slog.d(TAG, "Send failed, client connection lost");
    //         } else {
    //             if (DBG) Slog.d(TAG, "Client connection lost with reason: " + msg.arg1);
    //         }
    //         mTrafficPoller.removeClient(msg.replyTo);
    //         break;
    //     }
    //     case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION: {
    //         AsyncChannel ac = new AsyncChannel();
    //         ac.connect(mContext, this, msg.replyTo);
    //         break;
    //     }
    //     /* Client commands are forwarded to state machine */
    //     case WifiManager.CONNECT_NETWORK:
    //     case WifiManager.SAVE_NETWORK: {
    //         WifiConfiguration config = (WifiConfiguration) msg.obj;
    //         int networkId = msg.arg1;
    //         if (msg.what == WifiManager.SAVE_NETWORK) {
    //             if (config != null) {
    //                 if (config.networkId == WifiConfiguration.INVALID_NETWORK_ID) {
    //                     config.creatorUid = Binder.getCallingUid();
    //                 } else {
    //                     config.lastUpdateUid = Binder.getCallingUid();
    //                 }
    //             }
    //             Slog.e("WiFiServiceImpl ", "SAVE"
    //                     + " nid=" + Integer.toString(networkId)
    //                     + " uid=" + Integer.toString(config.creatorUid)
    //                     + "/" + Integer.toString(config.lastUpdateUid));
    //         }
    //         if (msg.what == WifiManager.CONNECT_NETWORK) {
    //             if (config != null) {
    //                 if (config.networkId == WifiConfiguration.INVALID_NETWORK_ID) {
    //                     config.creatorUid = Binder.getCallingUid();
    //                 } else {
    //                     config.lastUpdateUid = Binder.getCallingUid();
    //                 }
    //             }
    //             Slog.e("WiFiServiceImpl ", "CONNECT "
    //                     + " nid=" + Integer.toString(networkId)
    //                     + " uid=" + Binder.getCallingUid());
    //         }
    //         if (config != null && config.isValid()) {
    //             if (DBG) Slog.d(TAG, "Connect with config" + config);
    //             mWifiStateMachine.sendMessage(Message.obtain(msg));
    //         } else if (config == null
    //                 && networkId != WifiConfiguration.INVALID_NETWORK_ID) {
    //             if (DBG) Slog.d(TAG, "Connect with networkId" + networkId);
    //             mWifiStateMachine.sendMessage(Message.obtain(msg));
    //         } else {
    //             Slog.e(TAG, "ClientHandler.handleMessage ignoring invalid msg=" + msg);
    //             if (msg.what == WifiManager.CONNECT_NETWORK) {
    //                 replyFailed(msg, WifiManager.CONNECT_NETWORK_FAILED,
    //                         WifiManager.INVALID_ARGS);
    //             } else {
    //                 replyFailed(msg, WifiManager.SAVE_NETWORK_FAILED,
    //                         WifiManager.INVALID_ARGS);
    //             }
    //         }
    //         break;
    //     }
    //     case WifiManager.FORGET_NETWORK:
    //         if (isOwner(msg.sendingUid)) {
    //             mWifiStateMachine.sendMessage(Message.obtain(msg));
    //         } else {
    //             Slog.e(TAG, "Forget is not authorized for user");
    //             replyFailed(msg, WifiManager.FORGET_NETWORK_FAILED,
    //                     WifiManager.NOT_AUTHORIZED);
    //         }
    //         break;
    //     case WifiManager.START_WPS:
    //     case WifiManager.CANCEL_WPS:
    //     case WifiManager.DISABLE_NETWORK:
    //     case WifiManager.RSSI_PKTCNT_FETCH: {
    //         mWifiStateMachine.sendMessage(Message.obtain(msg));
    //         break;
    //     }
    //     default: {
    //         Slog.d(TAG, "ClientHandler.handleMessage ignoring msg=" + msg);
    //         break;
    //     }
    // }
    assert(0);
    return NOERROR;
}

void WifiServiceImpl::ClientHandler::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 why)
{
    // ==================before translated======================
    // Message reply = msg.obtain();
    // reply.what = what;
    // reply.arg1 = why;
    // try {
    //     msg.replyTo.send(reply);
    // } catch (RemoteException e) {
    //     // There's not much we can do if reply can't be sent!
    // }
    assert(0);
}

//=====================================================================
//               WifiServiceImpl::WifiStateMachineHandler
//=====================================================================
WifiServiceImpl::WifiStateMachineHandler::WifiStateMachineHandler(
    /* [in] */ ILooper* looper)
{
    // ==================before translated======================
    // super(looper);
    // mWsmChannel = new AsyncChannel();
    // mWsmChannel.connect(mContext, this, mWifiStateMachine.getHandler());
}

ECode WifiServiceImpl::WifiStateMachineHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch (msg.what) {
    //     case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
    //         if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
    //             mWifiStateMachineChannel = mWsmChannel;
    //         } else {
    //             Slog.e(TAG, "WifiStateMachine connection failure, error=" + msg.arg1);
    //             mWifiStateMachineChannel = null;
    //         }
    //         break;
    //     }
    //     case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
    //         Slog.e(TAG, "WifiStateMachine channel lost, msg.arg1 =" + msg.arg1);
    //         mWifiStateMachineChannel = null;
    //         //Re-establish connection to state machine
    //         mWsmChannel.connect(mContext, this, mWifiStateMachine.getHandler());
    //         break;
    //     }
    //     default: {
    //         Slog.d(TAG, "WifiStateMachineHandler.handleMessage ignoring msg=" + msg);
    //         break;
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//               WifiServiceImpl::InnerBroadcastReceiver1
//=====================================================================
WifiServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // if (mSettingsStore.handleAirplaneModeToggled()) {
    //     mWifiController.sendMessage(CMD_AIRPLANE_TOGGLED);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                 WifiServiceImpl::BatchedScanRequest
//=====================================================================
WifiServiceImpl::BatchedScanRequest::BatchedScanRequest(
    /* [in] */ IBatchedScanSettings* settings,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
    : DeathRecipient(0, String(NULL), binder, NULL)
{
    // ==================before translated======================
    // this.settings = settings;
    // this.uid = getCallingUid();
    // this.pid = getCallingPid();
    // workSource = ws;
}

ECode WifiServiceImpl::BatchedScanRequest::BinderDied()
{
    // ==================before translated======================
    // stopBatchedScan(settings, uid, pid);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::BatchedScanRequest::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "BatchedScanRequest{settings=" + settings + ", binder=" + mBinder + "}";
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::BatchedScanRequest::IsSameApp(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (this.uid == uid && this.pid == pid);
    assert(0);
    return NOERROR;
}

//=====================================================================
//               WifiServiceImpl::InnerBroadcastReceiver2
//=====================================================================
WifiServiceImpl::InnerBroadcastReceiver2::InnerBroadcastReceiver2(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiServiceImpl::InnerBroadcastReceiver2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // String action = intent.getAction();
    // if (action.equals(Intent.ACTION_SCREEN_ON)) {
    //     mWifiController.sendMessage(CMD_SCREEN_ON);
    // } else if (action.equals(Intent.ACTION_USER_PRESENT)) {
    //     mWifiController.sendMessage(CMD_USER_PRESENT);
    // } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
    //     mWifiController.sendMessage(CMD_SCREEN_OFF);
    // } else if (action.equals(Intent.ACTION_BATTERY_CHANGED)) {
    //     int pluggedType = intent.getIntExtra("plugged", 0);
    //     mWifiController.sendMessage(CMD_BATTERY_CHANGED, pluggedType, 0, null);
    // } else if (action.equals(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED)) {
    //     int state = intent.getIntExtra(BluetoothAdapter.EXTRA_CONNECTION_STATE,
    //             BluetoothAdapter.STATE_DISCONNECTED);
    //     mWifiStateMachine.sendBluetoothAdapterStateChange(state);
    // } else if (action.equals(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
    //     boolean emergencyMode = intent.getBooleanExtra("phoneinECMState", false);
    //     mWifiController.sendMessage(CMD_EMERGENCY_MODE_CHANGED, emergencyMode ? 1 : 0, 0);
    // } else if (action.equals(WifiManager.WIFI_AP_STATE_CHANGED_ACTION)) {
    //     int wifiApState = intent.getIntExtra(WifiManager.EXTRA_WIFI_AP_STATE,
    //             WifiManager.WIFI_AP_STATE_FAILED);
    //     Slog.d(TAG, "wifiApState=" + wifiApState);
    //     /*
    //      * If start SoftAp fails, WifiStateMachine would transition to InitialState,
    //      * but WifiController is left stuck in ApEnabledState, which in turn
    //      * fails to turn on WLAN again.
    //      *
    //      * Register WifiService to receive WIFI_AP_STATE_CHANGED_ACTION intent
    //      * from WifiStateMachine, and if wifiApState is failed, inform WifiController
    //      * to transtion to ApStaDisabledState.
    //      */
    //     if (wifiApState == WifiManager.WIFI_AP_STATE_FAILED) {
    //         setWifiApEnabled(null, false);
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                WifiServiceImpl::InnerContentObserver1
//=====================================================================
WifiServiceImpl::InnerContentObserver1::InnerContentObserver1(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiServiceImpl::InnerContentObserver1::OnChange(
    /* [in] */ Boolean selfChange)
{
    // ==================before translated======================
    // mSettingsStore.handleWifiScanAlwaysAvailableToggled();
    // mWifiController.sendMessage(CMD_SCAN_ALWAYS_MODE_CHANGED);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                      WifiServiceImpl::WifiLock
//=====================================================================
WifiServiceImpl::WifiLock::WifiLock(
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
    : DeathRecipient(lockMode, tag, binder, ws)
{
}

ECode WifiServiceImpl::WifiLock::BinderDied()
{
    // ==================before translated======================
    // synchronized (mLocks) {
    //     releaseWifiLockLocked(mBinder);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::WifiLock::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "WifiLock{" + mTag + " type=" + mMode + " binder=" + mBinder + "}";
    assert(0);
    return NOERROR;
}

//=====================================================================
//                   WifiServiceImpl::DeathRecipient
//=====================================================================
WifiServiceImpl::DeathRecipient::DeathRecipient(
    /* [in] */ Int32 mode,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
{
    // ==================before translated======================
    // super();
    // mTag = tag;
    // mMode = mode;
    // mBinder = binder;
    // mWorkSource = ws;
    // try {
    //     mBinder.linkToDeath(this, 0);
    // } catch (RemoteException e) {
    //     binderDied();
    // }
}

ECode WifiServiceImpl::DeathRecipient::UnlinkDeathRecipient()
{
    // ==================before translated======================
    // mBinder.unlinkToDeath(this, 0);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                     WifiServiceImpl::Multicaster
//=====================================================================
WifiServiceImpl::Multicaster::Multicaster(
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder)
    : DeathRecipient(0, tag, binder, NULL)
{
    // ==================before translated======================
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    mMode = uid;
}

ECode WifiServiceImpl::Multicaster::BinderDied()
{
    // ==================before translated======================
    // Slog.e(TAG, "Multicaster binderDied");
    // synchronized (mMulticasters) {
    //     int i = mMulticasters.indexOf(this);
    //     if (i != -1) {
    //         removeMulticasterLocked(i, mMode);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::Multicaster::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "Multicaster{" + mTag + " binder=" + mBinder + "}";
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::Multicaster::GetUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMode;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                           WifiServiceImpl
//=====================================================================
const String WifiServiceImpl::TAG("WifiService");
const Boolean WifiServiceImpl::DBG = true;

CAR_INTERFACE_IMPL_2(WifiServiceImpl, Object, IIWifiManager, IBinder);

WifiServiceImpl::WifiServiceImpl()
{
}

ECode WifiServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context;
    //
    // mInterfaceName =  SystemProperties.get("wifi.interface", "wlan0");
    //
    // mTrafficPoller = new WifiTrafficPoller(mContext, mInterfaceName);
    // mWifiStateMachine = new WifiStateMachine(mContext, mInterfaceName, mTrafficPoller);
    // mWifiStateMachine.enableRssiPolling(true);
    // mBatteryStats = BatteryStatsService.getService();
    // mAppOps = (AppOpsManager)context.getSystemService(Context.APP_OPS_SERVICE);
    //
    // mNotificationController = new WifiNotificationController(mContext, mWifiStateMachine);
    // mSettingsStore = new WifiSettingsStore(mContext);
    //
    // HandlerThread wifiThread = new HandlerThread("WifiService");
    // wifiThread.start();
    // mClientHandler = new ClientHandler(wifiThread.getLooper());
    // mWifiStateMachineHandler = new WifiStateMachineHandler(wifiThread.getLooper());
    // mWifiController = new WifiController(mContext, this, wifiThread.getLooper());
    //
    // mBatchedScanSupported = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_batched_scan_supported);
    return NOERROR;
}

ECode WifiServiceImpl::CheckAndStartWifi()
{
    // ==================before translated======================
    // /* Check if wi-fi needs to be enabled */
    // boolean wifiEnabled = mSettingsStore.isWifiToggleEnabled();
    // Slog.i(TAG, "WifiService starting up with Wi-Fi " +
    //         (wifiEnabled ? "enabled" : "disabled"));
    //
    // registerForScanModeChange();
    // mContext.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 if (mSettingsStore.handleAirplaneModeToggled()) {
    //                     mWifiController.sendMessage(CMD_AIRPLANE_TOGGLED);
    //                 }
    //             }
    //         },
    //         new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED));
    //
    // // Adding optimizations of only receiving broadcasts when wifi is enabled
    // // can result in race conditions when apps toggle wifi in the background
    // // without active user involvement. Always receive broadcasts.
    // registerForBroadcasts();
    //
    // mWifiController.start();
    //
    // mIsControllerStarted = true;
    //
    // // If we are already disabled (could be due to airplane mode), avoid changing persist
    // // state here
    // if (wifiEnabled) setWifiEnabled(wifiEnabled);
    //
    // mWifiWatchdogStateMachine = WifiWatchdogStateMachine.
    //        makeWifiWatchdogStateMachine(mContext, mWifiStateMachine.getMessenger());
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::PingSupplicant(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncPingSupplicant(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetChannelList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncGetChannelList(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::StartScan(
    /* [in] */ IScanSettings* settings,
    /* [in] */ IWorkSource* workSource)
{
    VALIDATE_NOT_NULL(settings);
    VALIDATE_NOT_NULL(workSource);
    // ==================before translated======================
    // enforceChangePermission();
    // if (settings != null) {
    //     // TODO: should be removed once the startCustomizedScan API is opened up
    //     mContext.enforceCallingOrSelfPermission(android.Manifest.permission.LOCATION_HARDWARE,
    //             "LocationHardware");
    //     settings = new ScanSettings(settings);
    //     if (!settings.isValid()) {
    //         Slog.e(TAG, "invalid scan setting");
    //         return;
    //     }
    // }
    // if (workSource != null) {
    //     enforceWorkSourcePermission();
    //     // WifiManager currently doesn't use names, so need to clear names out of the
    //     // supplied WorkSource to allow future WorkSource combining.
    //     workSource.clearNames();
    // }
    // mWifiStateMachine.startScan(Binder.getCallingUid(), scanRequestCounter++,
    //         settings, workSource);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::IsBatchedScanSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBatchedScanSupported;
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::PollBatchedScan()
{
    // ==================before translated======================
    // enforceChangePermission();
    // if (mBatchedScanSupported == false) return;
    // mWifiStateMachine.requestBatchedScanPoll();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWpsNfcConfigurationToken(
    /* [in] */ Int32 netId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceConnectivityInternalPermission();
    // return mWifiStateMachine.syncGetWpsNfcConfigurationToken(netId);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::RequestBatchedScan(
    /* [in] */ IBatchedScanSettings* requested,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* workSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(requested);
    VALIDATE_NOT_NULL(binder);
    VALIDATE_NOT_NULL(workSource);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    // if (workSource != null) {
    //     enforceWorkSourcePermission();
    //     // WifiManager currently doesn't use names, so need to clear names out of the
    //     // supplied WorkSource to allow future WorkSource combining.
    //     workSource.clearNames();
    // }
    // if (mBatchedScanSupported == false) return false;
    // requested = new BatchedScanSettings(requested);
    // if (requested.isInvalid()) return false;
    // BatchedScanRequest r = new BatchedScanRequest(requested, binder, workSource);
    // synchronized(mBatchedScanners) {
    //     mBatchedScanners.add(r);
    //     resolveBatchedScannersLocked();
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetBatchedScanResults(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mBatchedScanSupported == false) return new ArrayList<BatchedScanResult>();
    // int uid = Binder.getCallingUid();
    // int userId = UserHandle.getCallingUserId();
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     if (mAppOps.noteOp(AppOpsManager.OP_WIFI_SCAN, uid, callingPackage)
    //             != AppOpsManager.MODE_ALLOWED) {
    //         return new ArrayList<BatchedScanResult>();
    //     }
    //     if (!isCurrentProfile(userId)) {
    //         return new ArrayList<BatchedScanResult>();
    //     }
    //     return mWifiStateMachine.syncGetBatchedScanResultsList();
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::StopBatchedScan(
    /* [in] */ IBatchedScanSettings* settings)
{
    VALIDATE_NOT_NULL(settings);
    // ==================before translated======================
    // enforceChangePermission();
    // if (mBatchedScanSupported == false) return;
    // stopBatchedScan(settings, getCallingUid(), getCallingPid());
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::SetWifiEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    // Slog.d(TAG, "setWifiEnabled: " + enable + " pid=" + Binder.getCallingPid()
    //             + ", uid=" + Binder.getCallingUid());
    // if (DBG) {
    //     Slog.e(TAG, "Invoking mWifiStateMachine.setWifiEnabled\n");
    // }
    //
    // /*
    // * Caller might not have WRITE_SECURE_SETTINGS,
    // * only CHANGE_WIFI_STATE is enforced
    // */
    //
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     if (! mSettingsStore.handleWifiToggled(enable)) {
    //         // Nothing to do if wifi cannot be toggled
    //         return true;
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    //
    // if (!mIsControllerStarted) {
    //     Slog.e(TAG,"WifiController is not yet started, abort setWifiEnabled");
    //     return false;
    // }
    //
    // mWifiController.sendMessage(CMD_WIFI_TOGGLED);
    // return true;
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiEnabledState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.syncGetWifiState();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SetWifiApEnabled(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enabled)
{
    VALIDATE_NOT_NULL(wifiConfig);
    // ==================before translated======================
    // enforceChangePermission();
    // ConnectivityManager.enforceTetherChangePermission(mContext);
    // UserManager um = UserManager.get(mContext);
    // if (um.hasUserRestriction(UserManager.DISALLOW_CONFIG_TETHERING)) {
    //     throw new SecurityException("DISALLOW_CONFIG_TETHERING is enabled for this user.");
    // }
    // // null wifiConfig is a meaningful input for CMD_SET_AP
    // if (wifiConfig == null || wifiConfig.isValid()) {
    //     mWifiController.obtainMessage(CMD_SET_AP, enabled ? 1 : 0, 0, wifiConfig).sendToTarget();
    // } else {
    //     Slog.e(TAG, "Invalid WifiConfiguration");
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiApEnabledState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.syncGetWifiApState();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiApConfiguration(
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.syncGetWifiApConfiguration();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* wifiConfig)
{
    VALIDATE_NOT_NULL(wifiConfig);
    // ==================before translated======================
    // enforceChangePermission();
    // if (wifiConfig == null)
    //     return;
    // if (wifiConfig.isValid()) {
    //     mWifiStateMachine.setWifiApConfiguration(wifiConfig);
    // } else {
    //     Slog.e(TAG, "Invalid WifiConfiguration");
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::IsScanAlwaysAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mSettingsStore.isScanAlwaysAvailable();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::Disconnect()
{
    // ==================before translated======================
    // enforceChangePermission();
    // mWifiStateMachine.disconnectCommand();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::Reconnect()
{
    // ==================before translated======================
    // enforceChangePermission();
    // mWifiStateMachine.reconnectCommand();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::Reassociate()
{
    // ==================before translated======================
    // enforceChangePermission();
    // mWifiStateMachine.reassociateCommand();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetSupportedFeatures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncGetSupportedFeatures(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return 0;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::ReportActivityInfo(
    /* [out] */ IWifiActivityEnergyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // WifiLinkLayerStats stats;
    // WifiActivityEnergyInfo energyInfo = null;
    // if (mWifiStateMachineChannel != null) {
    //     stats = mWifiStateMachine.syncGetLinkLayerStats(mWifiStateMachineChannel);
    //     if (stats != null) {
    //         // Convert the LinkLayerStats into EnergyActivity
    //         energyInfo = new WifiActivityEnergyInfo(
    //                 WifiActivityEnergyInfo.STACK_STATE_STATE_IDLE, stats.tx_time,
    //                 stats.rx_time, stats.on_time - stats.tx_time - stats.rx_time,
    //                 0 /* TBD */);
    //     }
    //     return energyInfo;
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetConfiguredNetworks(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncGetConfiguredNetworks(Binder.getCallingUid(),
    //             mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetPrivilegedConfiguredNetworks(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceReadCredentialPermission();
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncGetPrivilegedConfiguredNetwork(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    // if (config.isValid()) {
    //     //TODO: pass the Uid the WifiStateMachine as a message parameter
    //     Slog.e("addOrUpdateNetwork", " uid = " + Integer.toString(Binder.getCallingUid())
    //             + " SSID " + config.SSID
    //             + " nid=" + Integer.toString(config.networkId));
    //     if (config.networkId == WifiConfiguration.INVALID_NETWORK_ID) {
    //         config.creatorUid = Binder.getCallingUid();
    //     } else {
    //         config.lastUpdateUid = Binder.getCallingUid();
    //     }
    //     if (mWifiStateMachineChannel != null) {
    //         return mWifiStateMachine.syncAddOrUpdateNetwork(mWifiStateMachineChannel, config);
    //     } else {
    //         Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //         return -1;
    //     }
    // } else {
    //     Slog.e(TAG, "bad network configuration");
    //     return -1;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    //
    // if (!isOwner(Binder.getCallingUid())) {
    //     Slog.e(TAG, "Remove is not authorized for user");
    //     return false;
    // }
    //
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncRemoveNetwork(mWifiStateMachineChannel, netId);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncEnableNetwork(mWifiStateMachineChannel, netId,
    //             disableOthers);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceChangePermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncDisableNetwork(mWifiStateMachineChannel, netId);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetConnectionInfo(
    /* [out] */ IWifiInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // /*
    //  * Make sure we have the latest information, by sending
    //  * a status request to the supplicant.
    //  */
    // return mWifiStateMachine.syncRequestConnectionInfo();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetScanResults(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // int userId = UserHandle.getCallingUserId();
    // int uid = Binder.getCallingUid();
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     if (mAppOps.noteOp(AppOpsManager.OP_WIFI_SCAN, uid, callingPackage)
    //             != AppOpsManager.MODE_ALLOWED) {
    //         return new ArrayList<ScanResult>();
    //     }
    //     if (!isCurrentProfile(userId)) {
    //         return new ArrayList<ScanResult>();
    //     }
    //     return mWifiStateMachine.syncGetScanResultsList();
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SaveConfiguration(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean result = true;
    // enforceChangePermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncSaveConfig(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SetCountryCode(
    /* [in] */ const String& countryCode,
    /* [in] */ Boolean persist)
{
    // ==================before translated======================
    // Slog.i(TAG, "WifiService trying to set country code to " + countryCode +
    //         " with persist set to " + persist);
    // enforceConnectivityInternalPermission();
    // final long token = Binder.clearCallingIdentity();
    // try {
    //     mWifiStateMachine.setCountryCode(countryCode, persist);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    // ==================before translated======================
    // enforceChangePermission();
    // if (!isDualBandSupported()) return;
    // Slog.i(TAG, "WifiService trying to set frequency band to " + band +
    //         " with persist set to " + persist);
    // final long token = Binder.clearCallingIdentity();
    // try {
    //     mWifiStateMachine.setFrequencyBand(band, persist);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetFrequencyBand(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.getFrequencyBand();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::IsDualBandSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // //TODO: Should move towards adding a driver API that checks at runtime
    // return mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.config_wifi_dual_band_support);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::IsIbssSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return (mWifiStateMachine.syncIsIbssSupported(mWifiStateMachineChannel) == 1);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetDhcpInfo(
    /* [out] */ IDhcpInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // DhcpResults dhcpResults = mWifiStateMachine.syncGetDhcpResults();
    //
    // DhcpInfo info = new DhcpInfo();
    //
    // if (dhcpResults.ipAddress != null &&
    //         dhcpResults.ipAddress.getAddress() instanceof Inet4Address) {
    //     info.ipAddress = NetworkUtils.inetAddressToInt(
    //        (Inet4Address) dhcpResults.ipAddress.getAddress());
    //     info.netmask = NetworkUtils.prefixLengthToNetmaskInt(
    //        dhcpResults.ipAddress.getNetworkPrefixLength());
    // }
    //
    // if (dhcpResults.gateway != null) {
    //     info.gateway = NetworkUtils.inetAddressToInt((Inet4Address) dhcpResults.gateway);
    // }
    //
    // int dnsFound = 0;
    // for (InetAddress dns : dhcpResults.dnsServers) {
    //     if (dns instanceof Inet4Address) {
    //         if (dnsFound == 0) {
    //             info.dns1 = NetworkUtils.inetAddressToInt((Inet4Address)dns);
    //         } else {
    //             info.dns2 = NetworkUtils.inetAddressToInt((Inet4Address)dns);
    //         }
    //         if (++dnsFound > 1) break;
    //     }
    // }
    // InetAddress serverAddress = dhcpResults.serverAddress;
    // if (serverAddress instanceof Inet4Address) {
    //     info.serverAddress = NetworkUtils.inetAddressToInt((Inet4Address)serverAddress);
    // }
    // info.leaseDuration = dhcpResults.leaseDuration;
    //
    // return info;
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::StartWifi()
{
    // ==================before translated======================
    // enforceConnectivityInternalPermission();
    // /* TODO: may be add permissions for access only to connectivity service
    //  * TODO: if a start issued, keep wifi alive until a stop issued irrespective
    //  * of WifiLock & device idle status unless wifi enabled status is toggled
    //  */
    //
    // mWifiStateMachine.setDriverStart(true);
    // mWifiStateMachine.reconnectCommand();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::StopWifi()
{
    // ==================before translated======================
    // enforceConnectivityInternalPermission();
    // /*
    //  * TODO: if a stop is issued, wifi is brought up only by startWifi
    //  * unless wifi enabled status is toggled
    //  */
    // mWifiStateMachine.setDriverStart(false);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    // ==================before translated======================
    // enforceChangePermission();
    //
    // mWifiStateMachine.addToBlacklist(bssid);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::ClearBlacklist()
{
    // ==================before translated======================
    // enforceChangePermission();
    //
    // mWifiStateMachine.clearBlacklist();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnableTdls(
    /* [in] */ const String& remoteAddress,
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // if (remoteAddress == null) {
    //   throw new IllegalArgumentException("remoteAddress cannot be null");
    // }
    //
    // TdlsTaskParams params = new TdlsTaskParams();
    // params.remoteIpAddress = remoteAddress;
    // params.enable = enable;
    // new TdlsTask().execute(params);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnableTdlsWithMacAddress(
    /* [in] */ const String& remoteMacAddress,
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // if (remoteMacAddress == null) {
    //   throw new IllegalArgumentException("remoteMacAddress cannot be null");
    // }
    //
    // mWifiStateMachine.enableTdls(remoteMacAddress, enable);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiServiceMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // enforceChangePermission();
    // return new Messenger(mClientHandler);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.getConfigFile();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnforceWakeSourcePermission(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    // ==================before translated======================
    // if (uid == android.os.Process.myUid()) {
    //     return;
    // }
    // mContext.enforcePermission(android.Manifest.permission.UPDATE_DEVICE_STATS,
    //         pid, uid, null);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::AcquireWifiLock(
    /* [in] */ IBinder* binder,
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IWorkSource* ws,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(binder);
    VALIDATE_NOT_NULL(ws);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(android.Manifest.permission.WAKE_LOCK, null);
    // if (lockMode != WifiManager.WIFI_MODE_FULL &&
    //         lockMode != WifiManager.WIFI_MODE_SCAN_ONLY &&
    //         lockMode != WifiManager.WIFI_MODE_FULL_HIGH_PERF) {
    //     Slog.e(TAG, "Illegal argument, lockMode= " + lockMode);
    //     if (DBG) throw new IllegalArgumentException("lockMode=" + lockMode);
    //     return false;
    // }
    // if (ws != null && ws.size() == 0) {
    //     ws = null;
    // }
    // if (ws != null) {
    //     enforceWakeSourcePermission(Binder.getCallingUid(), Binder.getCallingPid());
    // }
    // if (ws == null) {
    //     ws = new WorkSource(Binder.getCallingUid());
    // }
    // WifiLock wifiLock = new WifiLock(lockMode, tag, binder, ws);
    // synchronized (mLocks) {
    //     return acquireWifiLockLocked(wifiLock);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::UpdateWifiLockWorkSource(
    /* [in] */ IBinder* lock,
    /* [in] */ IWorkSource* ws)
{
    VALIDATE_NOT_NULL(lock);
    VALIDATE_NOT_NULL(ws);
    // ==================before translated======================
    // int uid = Binder.getCallingUid();
    // int pid = Binder.getCallingPid();
    // if (ws != null && ws.size() == 0) {
    //     ws = null;
    // }
    // if (ws != null) {
    //     enforceWakeSourcePermission(uid, pid);
    // }
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     synchronized (mLocks) {
    //         int index = mLocks.findLockByBinder(lock);
    //         if (index < 0) {
    //             throw new IllegalArgumentException("Wifi lock not active");
    //         }
    //         WifiLock wl = mLocks.mList.get(index);
    //         noteReleaseWifiLock(wl);
    //         wl.mWorkSource = ws != null ? new WorkSource(ws) : new WorkSource(uid);
    //         noteAcquireWifiLock(wl);
    //     }
    // } catch (RemoteException e) {
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::ReleaseWifiLock(
    /* [in] */ IBinder* lock,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(lock);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(android.Manifest.permission.WAKE_LOCK, null);
    // synchronized (mLocks) {
    //     return releaseWifiLockLocked(lock);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::InitializeMulticastFiltering()
{
    // ==================before translated======================
    // enforceMulticastChangePermission();
    //
    // synchronized (mMulticasters) {
    //     // if anybody had requested filters be off, leave off
    //     if (mMulticasters.size() != 0) {
    //         return;
    //     } else {
    //         mWifiStateMachine.startFilteringMulticastV4Packets();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::AcquireMulticastLock(
    /* [in] */ IBinder* binder,
    /* [in] */ const String& tag)
{
    VALIDATE_NOT_NULL(binder);
    // ==================before translated======================
    // enforceMulticastChangePermission();
    //
    // synchronized (mMulticasters) {
    //     mMulticastEnabled++;
    //     mMulticasters.add(new Multicaster(tag, binder));
    //     // Note that we could call stopFilteringMulticastV4Packets only when
    //     // our new size == 1 (first call), but this function won't
    //     // be called often and by making the stopPacket call each
    //     // time we're less fragile and self-healing.
    //     mWifiStateMachine.stopFilteringMulticastV4Packets();
    // }
    //
    // int uid = Binder.getCallingUid();
    // final long ident = Binder.clearCallingIdentity();
    // try {
    //     mBatteryStats.noteWifiMulticastEnabled(uid);
    // } catch (RemoteException e) {
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::ReleaseMulticastLock()
{
    // ==================before translated======================
    // enforceMulticastChangePermission();
    //
    // int uid = Binder.getCallingUid();
    // synchronized (mMulticasters) {
    //     mMulticastDisabled++;
    //     int size = mMulticasters.size();
    //     for (int i = size - 1; i >= 0; i--) {
    //         Multicaster m = mMulticasters.get(i);
    //         if ((m != null) && (m.getUid() == uid)) {
    //             removeMulticasterLocked(i, uid);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::IsMulticastEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    //
    // synchronized (mMulticasters) {
    //     return (mMulticasters.size() > 0);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiMonitor(
    /* [out] */ WifiMonitor** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWifiStateMachine.getWifiMonitor();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    // ==================before translated======================
    // enforceAccessPermission();
    // mWifiStateMachine.enableVerboseLogging(verbose);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetVerboseLoggingLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.getVerboseLoggingLevel();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::EnableAggressiveHandover(
    /* [in] */ Int32 enabled)
{
    // ==================before translated======================
    // enforceAccessPermission();
    // mWifiStateMachine.enableAggressiveHandover(enabled);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetAggressiveHandover(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.getAggressiveHandover();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::SetAllowScansWithTraffic(
    /* [in] */ Int32 enabled)
{
    // ==================before translated======================
    // enforceAccessPermission();
    // mWifiStateMachine.setAllowScansWithTraffic(enabled);
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetAllowScansWithTraffic(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // return mWifiStateMachine.getAllowScansWithTraffic();
    assert(0);
    return NOERROR;
}

ECode WifiServiceImpl::GetConnectionStatistics(
    /* [out] */ IWifiConnectionStatistics** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // enforceAccessPermission();
    // enforceReadCredentialPermission();
    // if (mWifiStateMachineChannel != null) {
    //     return mWifiStateMachine.syncGetConnectionStatistics(mWifiStateMachineChannel);
    // } else {
    //     Slog.e(TAG, "mWifiStateMachineChannel is not initialized");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

void WifiServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump WifiService from from pid="
    //             + Binder.getCallingPid()
    //             + ", uid=" + Binder.getCallingUid());
    //     return;
    // }
    // pw.println("Wi-Fi is " + mWifiStateMachine.syncGetWifiStateByName());
    // pw.println("Stay-awake conditions: " +
    //         Settings.Global.getInt(mContext.getContentResolver(),
    //                                Settings.Global.STAY_ON_WHILE_PLUGGED_IN, 0));
    // pw.println("mMulticastEnabled " + mMulticastEnabled);
    // pw.println("mMulticastDisabled " + mMulticastDisabled);
    // mWifiController.dump(fd, pw, args);
    // mSettingsStore.dump(fd, pw, args);
    // mNotificationController.dump(fd, pw, args);
    // mTrafficPoller.dump(fd, pw, args);
    //
    // pw.println("Latest scan results:");
    // List<ScanResult> scanResults = mWifiStateMachine.syncGetScanResultsList();
    // long nowMs = System.currentTimeMillis();
    // if (scanResults != null && scanResults.size() != 0) {
    //     pw.println("    BSSID              Frequency  RSSI    Age      SSID " +
    //             "                                Flags");
    //     for (ScanResult r : scanResults) {
    //         long ageSec = 0;
    //         long ageMilli = 0;
    //         if (nowMs > r.seen && r.seen > 0) {
    //             ageSec = (nowMs - r.seen) / 1000;
    //             ageMilli = (nowMs - r.seen) % 1000;
    //         }
    //         String candidate = " ";
    //         if (r.isAutoJoinCandidate > 0) candidate = "+";
    //         pw.printf("  %17s  %9d  %5d  %3d.%03d%s   %-32s  %s\n",
    //                                  r.BSSID,
    //                                  r.frequency,
    //                                  r.level,
    //                                  ageSec, ageMilli,
    //                                  candidate,
    //                                  r.SSID == null ? "" : r.SSID,
    //                                  r.capabilities);
    //     }
    // }
    // pw.println();
    // pw.println("Locks acquired: " + mFullLocksAcquired + " full, " +
    //         mFullHighPerfLocksAcquired + " full high perf, " +
    //         mScanLocksAcquired + " scan");
    // pw.println("Locks released: " + mFullLocksReleased + " full, " +
    //         mFullHighPerfLocksReleased + " full high perf, " +
    //         mScanLocksReleased + " scan");
    // pw.println();
    // pw.println("Locks held:");
    // mLocks.dump(pw);
    //
    // mWifiWatchdogStateMachine.dump(fd, pw, args);
    // pw.println();
    // mWifiStateMachine.dump(fd, pw, args);
    // pw.println();
    assert(0);
}

void WifiServiceImpl::StopBatchedScan(
    /* [in] */ IBatchedScanSettings* settings,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    // ==================before translated======================
    // ArrayList<BatchedScanRequest> found = new ArrayList<BatchedScanRequest>();
    // synchronized(mBatchedScanners) {
    //     for (BatchedScanRequest r : mBatchedScanners) {
    //         if (r.isSameApp(uid, pid) && (settings == null || settings.equals(r.settings))) {
    //             found.add(r);
    //             if (settings != null) break;
    //         }
    //     }
    //     for (BatchedScanRequest r : found) {
    //         mBatchedScanners.remove(r);
    //     }
    //     if (found.size() != 0) {
    //         resolveBatchedScannersLocked();
    //     }
    // }
    assert(0);
}

void WifiServiceImpl::ResolveBatchedScannersLocked()
{
    // ==================before translated======================
    // BatchedScanSettings setting = new BatchedScanSettings();
    // WorkSource responsibleWorkSource = null;
    // int responsibleUid = 0;
    // double responsibleCsph = 0; // Channel Scans Per Hour
    //
    // if (mBatchedScanners.size() == 0) {
    //     mWifiStateMachine.setBatchedScanSettings(null, 0, 0, null);
    //     return;
    // }
    // for (BatchedScanRequest r : mBatchedScanners) {
    //     BatchedScanSettings s = r.settings;
    //
    //     // evaluate responsibility
    //     int currentChannelCount;
    //     int currentScanInterval;
    //     double currentCsph;
    //
    //     if (s.channelSet == null || s.channelSet.isEmpty()) {
    //         // all channels - 11 B and 9 A channels roughly.
    //         currentChannelCount = 9 + 11;
    //     } else {
    //         currentChannelCount = s.channelSet.size();
    //         // these are rough est - no real need to correct for reg-domain;
    //         if (s.channelSet.contains("A")) currentChannelCount += (9 - 1);
    //         if (s.channelSet.contains("B")) currentChannelCount += (11 - 1);
    //
    //     }
    //     if (s.scanIntervalSec == BatchedScanSettings.UNSPECIFIED) {
    //         currentScanInterval = BatchedScanSettings.DEFAULT_INTERVAL_SEC;
    //     } else {
    //         currentScanInterval = s.scanIntervalSec;
    //     }
    //     currentCsph = 60 * 60 * currentChannelCount / currentScanInterval;
    //
    //     if (currentCsph > responsibleCsph) {
    //         responsibleUid = r.uid;
    //         responsibleWorkSource = r.workSource;
    //         responsibleCsph = currentCsph;
    //     }
    //
    //     if (s.maxScansPerBatch != BatchedScanSettings.UNSPECIFIED &&
    //             s.maxScansPerBatch < setting.maxScansPerBatch) {
    //         setting.maxScansPerBatch = s.maxScansPerBatch;
    //     }
    //     if (s.maxApPerScan != BatchedScanSettings.UNSPECIFIED &&
    //             (setting.maxApPerScan == BatchedScanSettings.UNSPECIFIED ||
    //             s.maxApPerScan > setting.maxApPerScan)) {
    //         setting.maxApPerScan = s.maxApPerScan;
    //     }
    //     if (s.scanIntervalSec != BatchedScanSettings.UNSPECIFIED &&
    //             s.scanIntervalSec < setting.scanIntervalSec) {
    //         setting.scanIntervalSec = s.scanIntervalSec;
    //     }
    //     if (s.maxApForDistance != BatchedScanSettings.UNSPECIFIED &&
    //             (setting.maxApForDistance == BatchedScanSettings.UNSPECIFIED ||
    //             s.maxApForDistance > setting.maxApForDistance)) {
    //         setting.maxApForDistance = s.maxApForDistance;
    //     }
    //     if (s.channelSet != null && s.channelSet.size() != 0) {
    //         if (setting.channelSet == null || setting.channelSet.size() != 0) {
    //             if (setting.channelSet == null) setting.channelSet = new ArrayList<String>();
    //             for (String i : s.channelSet) {
    //                 if (setting.channelSet.contains(i) == false) setting.channelSet.add(i);
    //             }
    //         } // else, ignore the constraint - we already use all channels
    //     } else {
    //         if (setting.channelSet == null || setting.channelSet.size() != 0) {
    //             setting.channelSet = new ArrayList<String>();
    //         }
    //     }
    // }
    //
    // setting.constrain();
    // mWifiStateMachine.setBatchedScanSettings(setting, responsibleUid, (int)responsibleCsph,
    //         responsibleWorkSource);
    assert(0);
}

void WifiServiceImpl::EnforceAccessPermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(android.Manifest.permission.ACCESS_WIFI_STATE,
    //         "WifiService");
    assert(0);
}

void WifiServiceImpl::EnforceChangePermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(android.Manifest.permission.CHANGE_WIFI_STATE,
    //                                         "WifiService");
    assert(0);
}

void WifiServiceImpl::EnforceReadCredentialPermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(android.Manifest.permission.READ_WIFI_CREDENTIAL,
    //                                         "WifiService");
    assert(0);
}

void WifiServiceImpl::EnforceWorkSourcePermission()
{
    // ==================before translated======================
    // mContext.enforceCallingPermission(android.Manifest.permission.UPDATE_DEVICE_STATS,
    //         "WifiService");
    assert(0);
}

void WifiServiceImpl::EnforceMulticastChangePermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(
    //         android.Manifest.permission.CHANGE_WIFI_MULTICAST_STATE,
    //         "WifiService");
    assert(0);
}

void WifiServiceImpl::EnforceConnectivityInternalPermission()
{
    // ==================before translated======================
    // mContext.enforceCallingOrSelfPermission(
    //         android.Manifest.permission.CONNECTIVITY_INTERNAL,
    //         "ConnectivityService");
    assert(0);
}

Boolean WifiServiceImpl::IsCurrentProfile(
    /* [in] */ Int32 userId)
{
    // ==================before translated======================
    // int currentUser = ActivityManager.getCurrentUser();
    // if (userId == currentUser) {
    //     return true;
    // }
    // List<UserInfo> profiles = UserManager.get(mContext).getProfiles(currentUser);
    // for (UserInfo user : profiles) {
    //     if (userId == user.id) {
    //         return true;
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean WifiServiceImpl::IsOwner(
    /* [in] */ Int32 uid)
{
    // ==================before translated======================
    // long ident = Binder.clearCallingIdentity();
    // int userId = UserHandle.getUserId(uid);
    // try {
    //     int ownerUser = UserHandle.USER_OWNER;
    //     if (userId == ownerUser) {
    //         return true;
    //     }
    //     List<UserInfo> profiles = UserManager.get(mContext).getProfiles(ownerUser);
    //     for (UserInfo profile : profiles) {
    //         if (userId == profile.id) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }
    // finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return FALSE;
}

void WifiServiceImpl::RegisterForScanModeChange()
{
    // ==================before translated======================
    // ContentObserver contentObserver = new ContentObserver(null) {
    //     @Override
    //     public void onChange(boolean selfChange) {
    //         mSettingsStore.handleWifiScanAlwaysAvailableToggled();
    //         mWifiController.sendMessage(CMD_SCAN_ALWAYS_MODE_CHANGED);
    //     }
    // };
    //
    // mContext.getContentResolver().registerContentObserver(
    //         Settings.Global.getUriFor(Settings.Global.WIFI_SCAN_ALWAYS_AVAILABLE),
    //         false, contentObserver);
    assert(0);
}

void WifiServiceImpl::RegisterForBroadcasts()
{
    // ==================before translated======================
    // IntentFilter intentFilter = new IntentFilter();
    // intentFilter.addAction(Intent.ACTION_SCREEN_ON);
    // intentFilter.addAction(Intent.ACTION_USER_PRESENT);
    // intentFilter.addAction(Intent.ACTION_SCREEN_OFF);
    // intentFilter.addAction(Intent.ACTION_BATTERY_CHANGED);
    // intentFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
    // intentFilter.addAction(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
    // intentFilter.addAction(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED);
    // intentFilter.addAction(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    // mContext.registerReceiver(mReceiver, intentFilter);
    assert(0);
}

void WifiServiceImpl::NoteAcquireWifiLock(
    /* [in] */ WifiLock* wifiLock)
{
    // ==================before translated======================
    // switch(wifiLock.mMode) {
    //     case WifiManager.WIFI_MODE_FULL:
    //     case WifiManager.WIFI_MODE_FULL_HIGH_PERF:
    //     case WifiManager.WIFI_MODE_SCAN_ONLY:
    //         mBatteryStats.noteFullWifiLockAcquiredFromSource(wifiLock.mWorkSource);
    //         break;
    // }
    assert(0);
}

void WifiServiceImpl::NoteReleaseWifiLock(
    /* [in] */ WifiLock* wifiLock)
{
    // ==================before translated======================
    // switch(wifiLock.mMode) {
    //     case WifiManager.WIFI_MODE_FULL:
    //     case WifiManager.WIFI_MODE_FULL_HIGH_PERF:
    //     case WifiManager.WIFI_MODE_SCAN_ONLY:
    //         mBatteryStats.noteFullWifiLockReleasedFromSource(wifiLock.mWorkSource);
    //         break;
    // }
    assert(0);
}

Boolean WifiServiceImpl::AcquireWifiLockLocked(
    /* [in] */ WifiLock* wifiLock)
{
    // ==================before translated======================
    // if (DBG) Slog.d(TAG, "acquireWifiLockLocked: " + wifiLock);
    //
    // mLocks.addLock(wifiLock);
    //
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     noteAcquireWifiLock(wifiLock);
    //     switch(wifiLock.mMode) {
    //     case WifiManager.WIFI_MODE_FULL:
    //         ++mFullLocksAcquired;
    //         break;
    //     case WifiManager.WIFI_MODE_FULL_HIGH_PERF:
    //         ++mFullHighPerfLocksAcquired;
    //         break;
    //
    //     case WifiManager.WIFI_MODE_SCAN_ONLY:
    //         ++mScanLocksAcquired;
    //         break;
    //     }
    //     mWifiController.sendMessage(CMD_LOCKS_CHANGED);
    //     return true;
    // } catch (RemoteException e) {
    //     return false;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
    return FALSE;
}

Boolean WifiServiceImpl::ReleaseWifiLockLocked(
    /* [in] */ IBinder* lock)
{
    // ==================before translated======================
    // boolean hadLock;
    //
    // WifiLock wifiLock = mLocks.removeLock(lock);
    //
    // if (DBG) Slog.d(TAG, "releaseWifiLockLocked: " + wifiLock);
    //
    // hadLock = (wifiLock != null);
    //
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     if (hadLock) {
    //         noteReleaseWifiLock(wifiLock);
    //         switch(wifiLock.mMode) {
    //             case WifiManager.WIFI_MODE_FULL:
    //                 ++mFullLocksReleased;
    //                 break;
    //             case WifiManager.WIFI_MODE_FULL_HIGH_PERF:
    //                 ++mFullHighPerfLocksReleased;
    //                 break;
    //             case WifiManager.WIFI_MODE_SCAN_ONLY:
    //                 ++mScanLocksReleased;
    //                 break;
    //         }
    //         mWifiController.sendMessage(CMD_LOCKS_CHANGED);
    //     }
    // } catch (RemoteException e) {
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    //
    // return hadLock;
    assert(0);
    return FALSE;
}

void WifiServiceImpl::RemoveMulticasterLocked(
    /* [in] */ Int32 i,
    /* [in] */ Int32 uid)
{
    // ==================before translated======================
    // Multicaster removed = mMulticasters.remove(i);
    //
    // if (removed != null) {
    //     removed.unlinkDeathRecipient();
    // }
    // if (mMulticasters.size() == 0) {
    //     mWifiStateMachine.startFilteringMulticastV4Packets();
    // }
    //
    // final long ident = Binder.clearCallingIdentity();
    // try {
    //     mBatteryStats.noteWifiMulticastDisabled(uid);
    // } catch (RemoteException e) {
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


