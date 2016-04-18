
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/server/wifi/WifiTrafficPoller.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//              WifiTrafficPoller::InnerBroadcastReceiver1
//=====================================================================
WifiTrafficPoller::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiTrafficPoller* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiTrafficPoller::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // if (intent.getAction().equals(
    //         WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //             WifiManager.EXTRA_NETWORK_INFO);
    // } else if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF)) {
    //     mScreenOn.set(false);
    // } else if (intent.getAction().equals(Intent.ACTION_SCREEN_ON)) {
    //     mScreenOn.set(true);
    // }
    // evaluateTrafficStatsPolling();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                  WifiTrafficPoller::TrafficHandler
//=====================================================================
ECode WifiTrafficPoller::TrafficHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch (msg.what) {
    //     case ENABLE_TRAFFIC_STATS_POLL:
    //         mEnableTrafficStatsPoll = (msg.arg1 == 1);
    //         if (DBG) {
    //             Log.e(TAG, "ENABLE_TRAFFIC_STATS_POLL "
    //                     + mEnableTrafficStatsPoll + " Token "
    //                     + Integer.toString(mTrafficStatsPollToken));
    //         }
    //         mTrafficStatsPollToken++;
    //         if (mEnableTrafficStatsPoll) {
    //             notifyOnDataActivity();
    //             sendMessageDelayed(Message.obtain(this, TRAFFIC_STATS_POLL,
    //                     mTrafficStatsPollToken, 0), POLL_TRAFFIC_STATS_INTERVAL_MSECS);
    //         }
    //         break;
    //     case TRAFFIC_STATS_POLL:
    //         if (VDBG) {
    //             Log.e(TAG, "TRAFFIC_STATS_POLL "
    //                     + mEnableTrafficStatsPoll + " Token "
    //                     + Integer.toString(mTrafficStatsPollToken)
    //                     + " num clients " + mClients.size());
    //         }
    //         if (msg.arg1 == mTrafficStatsPollToken) {
    //             notifyOnDataActivity();
    //             sendMessageDelayed(Message.obtain(this, TRAFFIC_STATS_POLL,
    //                     mTrafficStatsPollToken, 0), POLL_TRAFFIC_STATS_INTERVAL_MSECS);
    //         }
    //         break;
    //     case ADD_CLIENT:
    //         mClients.add((Messenger) msg.obj);
    //         if (DBG) {
    //             Log.e(TAG, "ADD_CLIENT: "
    //                     + Integer.toString(mClients.size()));
    //         }
    //         break;
    //     case REMOVE_CLIENT:
    //         mClients.remove(msg.obj);
    //         break;
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          WifiTrafficPoller
//=====================================================================
const Int32 WifiTrafficPoller::POLL_TRAFFIC_STATS_INTERVAL_MSECS;
const Int32 WifiTrafficPoller::ENABLE_TRAFFIC_STATS_POLL;
const Int32 WifiTrafficPoller::TRAFFIC_STATS_POLL;
const Int32 WifiTrafficPoller::ADD_CLIENT;
const Int32 WifiTrafficPoller::REMOVE_CLIENT;

WifiTrafficPoller::WifiTrafficPoller(
    /* [in] */ IContext* context,
    /* [in] */ String iface)
{
    CArrayList::New((IList**)&mClients);
    // ==================before translated======================
    // mInterface = iface;
    // mTrafficHandler = new TrafficHandler();
    //
    // IntentFilter filter = new IntentFilter();
    // filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
    // filter.addAction(Intent.ACTION_SCREEN_OFF);
    // filter.addAction(Intent.ACTION_SCREEN_ON);
    //
    // context.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 if (intent.getAction().equals(
    //                         WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //                     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //                             WifiManager.EXTRA_NETWORK_INFO);
    //                 } else if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF)) {
    //                     mScreenOn.set(false);
    //                 } else if (intent.getAction().equals(Intent.ACTION_SCREEN_ON)) {
    //                     mScreenOn.set(true);
    //                 }
    //                 evaluateTrafficStatsPolling();
    //             }
    //         }, filter);
}

ECode WifiTrafficPoller::AddClient(
    /* [in] */ IMessenger* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // Message.obtain(mTrafficHandler, ADD_CLIENT, client).sendToTarget();
    assert(0);
    return NOERROR;
}

ECode WifiTrafficPoller::RemoveClient(
    /* [in] */ IMessenger* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // Message.obtain(mTrafficHandler, REMOVE_CLIENT, client).sendToTarget();
    assert(0);
    return NOERROR;
}

ECode WifiTrafficPoller::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    // ==================before translated======================
    // if (verbose > 0 ) {
    //     DBG = true;
    // } else {
    //     DBG = false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiTrafficPoller::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // pw.println("mEnableTrafficStatsPoll " + mEnableTrafficStatsPoll);
    // pw.println("mTrafficStatsPollToken " + mTrafficStatsPollToken);
    // pw.println("mTxPkts " + mTxPkts);
    // pw.println("mRxPkts " + mRxPkts);
    // pw.println("mDataActivity " + mDataActivity);
    assert(0);
    return NOERROR;
}

void WifiTrafficPoller::EvaluateTrafficStatsPolling()
{
    // ==================before translated======================
    // Message msg;
    // if (mNetworkInfo == null) return;
    // if (mNetworkInfo.getDetailedState() == CONNECTED && mScreenOn.get()) {
    //     msg = Message.obtain(mTrafficHandler,
    //             ENABLE_TRAFFIC_STATS_POLL, 1, 0);
    // } else {
    //     msg = Message.obtain(mTrafficHandler,
    //             ENABLE_TRAFFIC_STATS_POLL, 0, 0);
    // }
    // msg.sendToTarget();
    assert(0);
}

void WifiTrafficPoller::NotifyOnDataActivity()
{
    // ==================before translated======================
    // long sent, received;
    // long preTxPkts = mTxPkts, preRxPkts = mRxPkts;
    // int dataActivity = WifiManager.DATA_ACTIVITY_NONE;
    //
    // mTxPkts = TrafficStats.getTxPackets(mInterface);
    // mRxPkts = TrafficStats.getRxPackets(mInterface);
    //
    // if (VDBG) {
    //     Log.e(TAG, " packet count Tx="
    //             + Long.toString(mTxPkts)
    //             + " Rx="
    //             + Long.toString(mRxPkts));
    // }
    //
    // if (preTxPkts > 0 || preRxPkts > 0) {
    //     sent = mTxPkts - preTxPkts;
    //     received = mRxPkts - preRxPkts;
    //     if (sent > 0) {
    //         dataActivity |= WifiManager.DATA_ACTIVITY_OUT;
    //     }
    //     if (received > 0) {
    //         dataActivity |= WifiManager.DATA_ACTIVITY_IN;
    //     }
    //
    //     if (dataActivity != mDataActivity && mScreenOn.get()) {
    //         mDataActivity = dataActivity;
    //         if (DBG) {
    //             Log.e(TAG, "notifying of data activity "
    //                     + Integer.toString(mDataActivity));
    //         }
    //         for (Messenger client : mClients) {
    //             Message msg = Message.obtain();
    //             msg.what = WifiManager.DATA_ACTIVITY_NOTIFICATION;
    //             msg.arg1 = mDataActivity;
    //             try {
    //                 client.send(msg);
    //             } catch (RemoteException e) {
    //                 // Failed to reach, skip
    //                 // Client removal is handled in WifiService
    //             }
    //         }
    //     }
    // }
    assert(0);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


