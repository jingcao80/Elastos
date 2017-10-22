//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/ProxyDataTracker.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/net/BaseNetworkStateTracker.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkInfo.h"
#include "elastos/droid/net/NetworkState.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(ProxyDataTracker, BaseNetworkStateTracker, IProxyDataTracker)

const String ProxyDataTracker::TAG("ProxyDataTracker");
const String ProxyDataTracker::NETWORK_TYPE("PROXY");
const String ProxyDataTracker::DNS1("8.8.8.8");
const String ProxyDataTracker::DNS2("8.8.4.4");
const String ProxyDataTracker::INTERFACE_NAME("ifb0");
const String ProxyDataTracker::REASON_ENABLED("enabled");
const String ProxyDataTracker::REASON_DISABLED("disabled");
const String ProxyDataTracker::REASON_PROXY_DOWN("proxy_down");
const Int32 ProxyDataTracker::MSG_TEAR_DOWN_REQUEST = 1;
const Int32 ProxyDataTracker::MSG_SETUP_REQUEST = 2;
const String ProxyDataTracker::PERMISSION_PROXY_STATUS_SENDER("android.permission.ACCESS_NETWORK_CONDITIONS");
const String ProxyDataTracker::ACTION_PROXY_STATUS_CHANGE("com.android.net.PROXY_STATUS_CHANGE");
const String ProxyDataTracker::KEY_IS_PROXY_AVAILABLE("is_proxy_available");
const String ProxyDataTracker::KEY_REPLY_TO_MESSENGER_BINDER("reply_to_messenger_binder");
const String ProxyDataTracker::KEY_REPLY_TO_MESSENGER_BINDER_BUNDLE("reply_to_messenger_binder_bundle");

ProxyDataTracker::ProxyDataTracker()
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mReconnectRequested);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mIsProxyAvailable);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mDefaultGatewayAddr);
    mProxyStatusServiceListener = new InnerSub_BroadcastReceiver(this);
}

ECode ProxyDataTracker::constructor()
{
    CNetworkInfo::New(IConnectivityManager::TYPE_PROXY, 0, NETWORK_TYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilities);
    mNetworkInfo->SetIsAvailable(TRUE);
    // try {
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr<IInetAddress> dns1, dns2;
    ECode ec = helper->GetByName(DNS1, (IInetAddress**)&dns1);
    if (FAILED(ec)) {
        if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
            Logger::E(TAG, "Could not add DNS address %d", ec);
        }
        else return ec;
    }
    Boolean b;
    mLinkProperties->AddDnsServer(dns1, &b);
    ec = helper->GetByName(DNS2, (IInetAddress**)&dns2);
    if (FAILED(ec)) {
        if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
            Logger::E(TAG, "Could not add DNS address %d", ec);
        }
        else return ec;
    }
    mLinkProperties->AddDnsServer(dns2, &b);
    mLinkProperties->SetInterfaceName(INTERFACE_NAME);
    // } catch (UnknownHostException e) {
    // }
    return NOERROR;
}

ECode ProxyDataTracker::Clone(
    /* [out] */ IInterface** result)
{
    return E_CLONE_NOT_SUPPORTED_EXCEPTION;
}

ECode ProxyDataTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    mContext = context;
    mTarget = target;
    AutoPtr<IIntentFilter> newIntentFilter;
    CIntentFilter::New(ACTION_PROXY_STATUS_CHANGE, (IIntentFilter**)&newIntentFilter);
    AutoPtr<IIntent> out;
    mContext->RegisterReceiver(mProxyStatusServiceListener,
            newIntentFilter,
            PERMISSION_PROXY_STATUS_SENDER,
            NULL,
            (IIntent**)&out);
    return NOERROR;
}

ECode ProxyDataTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    SetTeardownRequested(TRUE);
    mReconnectRequested->Set(FALSE);
    // try {
    Boolean available;
    if ((mIsProxyAvailable->Get(&available), available) && mProxyStatusService != NULL) {
        AutoPtr<IMessage> msg = CMessage::Obtain(NULL, MSG_TEAR_DOWN_REQUEST);
        ECode ec = mProxyStatusService->Send(msg);
        if (FAILED(ec)) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::E(TAG, "Unable to connect to proxy status service %d", ec);
                *result = FALSE;
                return NOERROR;
            }
            else return ec;
        }
    }
    // } catch (RemoteException e) {
    // }
    SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, REASON_DISABLED, String(NULL));
    *result = TRUE;
    return NOERROR;
}

ECode ProxyDataTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mReconnectRequested->Set(TRUE);
    SetTeardownRequested(FALSE);
    Boolean available;
    if (mIsProxyAvailable->Get(&available), !available) {
        Logger::W(TAG, "Reconnect requested even though proxy service is not up. Bailing.");
        *result = FALSE;
        return NOERROR;
    }
    SetDetailedState(NetworkInfoDetailedState_CONNECTING, REASON_ENABLED, String(NULL));
    // try {
    AutoPtr<IMessage> msg = CMessage::Obtain(NULL, MSG_SETUP_REQUEST);
    ECode ec = mProxyStatusService->Send(msg);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E(TAG, "Unable to connect to proxy status service %d", ec);
            SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, REASON_PROXY_DOWN, String(NULL));
            *result = FALSE;
            return NOERROR;
        }
        else return ec;
    }
    // }
    // We'll assume proxy is set up successfully. If not, a status change broadcast will be
    // received afterwards to indicate any failure.
    SetDetailedState(NetworkInfoDetailedState_CONNECTED, REASON_ENABLED, String(NULL));
    *result = TRUE;
    return NOERROR;
}

ECode ProxyDataTracker::GetDefaultGatewayAddr(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return mDefaultGatewayAddr->Get(result);
}

ECode ProxyDataTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = String("net.tcp.buffersize.wifi");
    return NOERROR;
}

ECode ProxyDataTracker::SetDetailedState(
    /* [in] */ NetworkInfoDetailedState state,
    /* [in] */ const String& reason,
    /* [in] */ const String& extraInfo)
{
    mNetworkInfo->SetDetailedState(state, reason, extraInfo);
    AutoPtr<IMessage> msg;
    mTarget->ObtainMessage(EVENT_STATE_CHANGED, mNetworkInfo, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//===================================================
// ProxyDataTracker::InnerSub_BroadcastReceiver
//===================================================
ProxyDataTracker::InnerSub_BroadcastReceiver::InnerSub_BroadcastReceiver(ProxyDataTracker* host)
    : mHost(host)
{}
ECode ProxyDataTracker::InnerSub_BroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent->GetAction(&action), action.Equals(ACTION_PROXY_STATUS_CHANGE)) {
        Boolean b;
        intent->GetBooleanExtra(KEY_IS_PROXY_AVAILABLE, FALSE, &b);
        mHost->mIsProxyAvailable->Set(b);
        if (mHost->mIsProxyAvailable->Get(&b), b) {
            AutoPtr<IBundle> bundle;
            intent->GetBundleExtra(KEY_REPLY_TO_MESSENGER_BINDER_BUNDLE, (IBundle**)&bundle);
            b = bundle == NULL;
            AutoPtr<IBinder> binder;
            if (!b) {
                bundle->GetBinder(KEY_REPLY_TO_MESSENGER_BINDER, (IBinder**)&binder);
                b = binder == NULL;
            }
            if (b) {
                Logger::E(TAG, "no messenger binder in the intent to send future requests");
                mHost->mIsProxyAvailable->Set(FALSE);
                return NOERROR;
            }
            CMessenger::New(IIMessenger::Probe(binder), (IMessenger**)&(mHost->mProxyStatusService));
            // If there is a pending reconnect request, do it now.
            if (mHost->mReconnectRequested->Get(&b), b) {
                Boolean tmp;
                mHost->Reconnect(&tmp);
            }
        } else {
            mHost->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED,
                    REASON_PROXY_DOWN, String(NULL));
        }
    } else {
        Logger::D(TAG, "Unrecognized broadcast intent");
    }
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
