#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/server/wifi/WifiTrafficPoller.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::CTrafficStats;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

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
}

ECode WifiTrafficPoller::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcel;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcel);
        mOwner->mNetworkInfo = INetworkInfo::Probe(parcel);
    } else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mOwner->mScreenOn->Set(FALSE);
    } else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mOwner->mScreenOn->Set(TRUE);
    }
    mOwner->EvaluateTrafficStatsPolling();
    return NOERROR;
}

//=====================================================================
//                  WifiTrafficPoller::TrafficHandler
//=====================================================================
ECode WifiTrafficPoller::TrafficHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ENABLE_TRAFFIC_STATS_POLL: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mEnableTrafficStatsPoll = (arg1 == 1);
            if (mOwner->DBG) {
                Logger::E(mOwner->TAG, "ENABLE_TRAFFIC_STATS_POLL %d, Token %s",
                        mOwner->mEnableTrafficStatsPoll,
                        StringUtils::ToString(mOwner->mTrafficStatsPollToken).string());
            }
            mOwner->mTrafficStatsPollToken++;
            if (mOwner->mEnableTrafficStatsPoll) {
                mOwner->NotifyOnDataActivity();
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> newMsg;
                helper->Obtain(this, TRAFFIC_STATS_POLL, mOwner->mTrafficStatsPollToken, 0, (IMessage**)&newMsg);
                Boolean r;
                SendMessageDelayed(newMsg, POLL_TRAFFIC_STATS_INTERVAL_MSECS, &r);
            }
            break;
        }
        case TRAFFIC_STATS_POLL: {
            if (mOwner->VDBG) {
                Int32 size;
                mOwner->mClients->GetSize(&size);
                Logger::E(mOwner->TAG, "TRAFFIC_STATS_POLL %d, Token:%s, num clients:%d",
                        mOwner->mEnableTrafficStatsPoll, StringUtils::ToString(mOwner->mTrafficStatsPollToken).string(),
                        size);
            }
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mTrafficStatsPollToken) {
                mOwner->NotifyOnDataActivity();
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> newMsg;
                helper->Obtain(this, TRAFFIC_STATS_POLL, mOwner->mTrafficStatsPollToken, 0, (IMessage**)&newMsg);
                Boolean r;
                SendMessageDelayed(newMsg, POLL_TRAFFIC_STATS_INTERVAL_MSECS, &r);
            }
            break;
        }
        case ADD_CLIENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->mClients->Add(obj);
            if (mOwner->DBG) {
                Int32 size;
                mOwner->mClients->GetSize(&size);
                Logger::E(mOwner->TAG, "ADD_CLIENT: %d", size);
            }
            break;
        }
        case REMOVE_CLIENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->mClients->Remove(obj);
            break;
        }
    }
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
    /* [in] */ const String& iface)
    : DBG(FALSE)
    , VDBG(FALSE)
{
    CArrayList::New((IList**)&mClients);
    CAtomicBoolean::New(TRUE, (IAtomicBoolean**)&mScreenOn);
    mInterface = iface;
    mTrafficHandler = new TrafficHandler(this);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);

    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);

    AutoPtr<IBroadcastReceiver> receiver = new InnerBroadcastReceiver1(this);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, filter, (IIntent**)&intent);
}

ECode WifiTrafficPoller::AddClient(
    /* [in] */ IMessenger* client)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> newMsg;
    helper->Obtain(mTrafficHandler, ADD_CLIENT, client, (IMessage**)&newMsg);
    newMsg->SendToTarget();
    return NOERROR;
}

ECode WifiTrafficPoller::RemoveClient(
    /* [in] */ IMessenger* client)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> newMsg;
    helper->Obtain(mTrafficHandler, REMOVE_CLIENT, TO_IINTERFACE(client), (IMessage**)&newMsg);
    newMsg->SendToTarget();
    return NOERROR;
}

ECode WifiTrafficPoller::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0 ) {
        DBG = TRUE;
    } else {
        DBG = FALSE;
    }
    return NOERROR;
}

ECode WifiTrafficPoller::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Logger::E(TAG, "WifiTrafficPoller::Dump TODO");
    // ==================before translated======================
    // pw.println("mEnableTrafficStatsPoll " + mEnableTrafficStatsPoll);
    // pw.println("mTrafficStatsPollToken " + mTrafficStatsPollToken);
    // pw.println("mTxPkts " + mTxPkts);
    // pw.println("mRxPkts " + mRxPkts);
    // pw.println("mDataActivity " + mDataActivity);
    return NOERROR;
}

void WifiTrafficPoller::EvaluateTrafficStatsPolling()
{
    AutoPtr<IMessage> msg;
    if (mNetworkInfo == NULL) return;
    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    Boolean screenOn;
    mScreenOn->Get(&screenOn);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    if (state == NetworkInfoDetailedState_CONNECTED && screenOn) {
        helper->Obtain(mTrafficHandler, ENABLE_TRAFFIC_STATS_POLL, 1, 0, (IMessage**)&msg);
    } else {
        helper->Obtain(mTrafficHandler, ENABLE_TRAFFIC_STATS_POLL, 0, 0, (IMessage**)&msg);
    }
    msg->SendToTarget();
}

void WifiTrafficPoller::NotifyOnDataActivity()
{
    Int64 sent, received;
    Int64 preTxPkts = mTxPkts, preRxPkts = mRxPkts;
    Int32 dataActivity = IWifiManager::DATA_ACTIVITY_NONE;

    AutoPtr<ITrafficStats> trafficStats;
    CTrafficStats::AcquireSingleton((ITrafficStats**)&trafficStats);
    trafficStats->GetTxPackets(mInterface, &mTxPkts);
    trafficStats->GetRxPackets(mInterface, &mRxPkts);

    if (VDBG) {
        Logger::E(TAG, " packet count Tx=%ld, Rx=%ld", mTxPkts, mRxPkts);
    }

    if (preTxPkts > 0 || preRxPkts > 0) {
        sent = mTxPkts - preTxPkts;
        received = mRxPkts - preRxPkts;
        if (sent > 0) {
            dataActivity |= IWifiManager::DATA_ACTIVITY_OUT;
        }
        if (received > 0) {
            dataActivity |= IWifiManager::DATA_ACTIVITY_IN;
        }

        Boolean screenOn;
        mScreenOn->Get(&screenOn);
        if (dataActivity != mDataActivity && screenOn) {
            mDataActivity = dataActivity;
            if (DBG) {
                Logger::E(TAG, "notifying of data activity %d", mDataActivity);
            }
            Int32 size;
            mClients->GetSize(&size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> msg;
                helper->Obtain((IMessage**)&msg);
                msg->SetWhat(IWifiManager::DATA_ACTIVITY_NOTIFICATION);
                msg->SetArg1(mDataActivity);
                //try {
                    AutoPtr<IInterface> obj;
                    mClients->Get(i, (IInterface**)&obj);
                    IMessenger* client = IMessenger::Probe(obj);
                    if (client != NULL)
                        client->Send(msg);
                //} catch (RemoteException e) {
                    // Failed to reach, skip
                    // Client removal is handled in WifiService
                //}
            }
        }
    }
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


