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

#include "elastos/droid/internal/telephony/dataconnection/DcTracker.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnContext.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnContext.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CCdmaApnProfileTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/CdmaApnProfileTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTracker.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CNetworkConfig.h"
#include "elastos/droid/net/CProxyInfo.h"
#include "elastos/droid/net/NetworkFactory.h"
#include "elastos/droid/net/NetworkRequest.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/ProxyInfo.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.Droid.Provider.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Service;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_DISCONNECTED;
using Elastos::Droid::Internal::Telephony::SubscriptionController;
using Elastos::Droid::Internal::Telephony::Cdma::CdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CNetworkConfig;
using Elastos::Droid::Net::CProxyInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::NetworkRequest;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::ITelephonyCarriers;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CObject;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::ICloseable;
using Elastos::IO::ByteOrder;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTracker::SubBroadcastReceiverDefaultDds
//=============================================================================
DcTracker::SubBroadcastReceiverDefaultDds::SubBroadcastReceiverDefaultDds(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::SubBroadcastReceiverDefaultDds::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mPhone->GetSubId(&mHost->mSubId);
    Int64 i64;
    intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY,
            ISubscriptionManager::INVALID_SUB_ID, &i64);
    mHost->Log("got ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, new DDS = %lld", i64);
    mHost->UpdateSubIdAndCapability();
    Int64 defaultDataSubId = 0;
    IISub::Probe(SubscriptionController::GetInstance())->GetDefaultDataSubId(&defaultDataSubId);
    if (mHost->mSubId == defaultDataSubId) {
        mHost->Log("Dct is default-DDS now, process any pending MMS requests");
    }
    return NOERROR;
}

//=============================================================================
// DcTracker::SubBroadcastReceiverSubInfo
//=============================================================================
DcTracker::SubBroadcastReceiverSubInfo::SubBroadcastReceiverSubInfo(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::SubBroadcastReceiverSubInfo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mPhone->GetSubId(&mHost->mSubId);
    mHost->Log("got ACTION_SUBINFO_RECORD_UPDATED, mySubId = %lld", mHost->mSubId);
    mHost->UpdateSubIdAndCapability();
    return NOERROR;
}

//=============================================================================
// DcTracker::ApnChangeObserver
//=============================================================================
DcTracker::ApnChangeObserver::ApnChangeObserver(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::ApnChangeObserver::constructor()
{
    return ContentObserver::constructor(mHost->mDataConnectionTracker);
}

ECode DcTracker::ApnChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(IDctConstants::EVENT_APN_CHANGED, (IMessage**)&msg);
    Boolean b;
    mHost->SendMessage(msg, &b);
    return NOERROR;
}

//=============================================================================
// DcTracker::TelephonyNetworkFactory
//=============================================================================
DcTracker::TelephonyNetworkFactory::TelephonyNetworkFactory(
    /* [in] */ DcTracker* host)
    : mHost(host)
{
    CSparseArray::New((ISparseArray**)&mDdsRequests);
}

ECode DcTracker::TelephonyNetworkFactory::constructor(
    /* [in] */ ILooper* l,
    /* [in] */ IContext* c,
    /* [in] */ const String& TAG,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ IPhoneBase* phone)
{
    NetworkFactory::constructor(l, c, TAG, nc);
    mPhone = phone;
    mNetworkCapabilities = nc;
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::ProcessPendingNetworkRequests(
    /* [in] */ INetworkRequest* n)
{
    Int32 size;
    mDdsRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDdsRequests->ValueAt(i, (IInterface**)&obj);
        AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(obj);
        Boolean isEquals;
        IObject::Probe(nr)->Equals(n, &isEquals);
        if (isEquals) {
            Log("Found pending request in ddsRequest list = %s", TO_CSTR(nr));
            AutoPtr<IApnContext> apnContext;
            mHost->ApnContextForNetworkRequest(nr, (IApnContext**)&apnContext);
            if (apnContext != NULL) {
                Log("Activating APN=%s", TO_CSTR(apnContext));
                apnContext->IncRefCount();
            }
        }
    }
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::RegisterOnDemandDdsCallback()
{
    AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
    Int64 subId = 0;
    mPhone->GetSubId(&subId);
    AutoPtr<OnDemandDdsLockNotifier> ln = new OnDemandDdsLockNotifier(mHost);
    subController->RegisterForOnDemandDdsLockNotification(subId, ln);
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::UpdateNetworkCapability(
    /* [in] */ Int64 subId)
{
    Log("update networkCapabilites for subId = %lld", subId);
    mNetworkCapabilities->SetNetworkSpecifier(String("") + StringUtils::ToString(subId));
    Int64 defaultDataSubId = 0;
    AutoPtr<ISubscriptionController> sc = SubscriptionController::GetInstance();
    IISub::Probe(sc)->GetDefaultDataSubId(&defaultDataSubId);
    Int32 ss = 0;
    if ((subId > 0 && (IISub::Probe(sc)->GetSubState(subId, &ss), ss) == ISubscriptionManager::ACTIVE) &&
            (subId == defaultDataSubId)) {
        Log("INTERNET capability is with subId = %lld", subId);
        //Only defaultDataSub provides INTERNET.
        mNetworkCapabilities->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    }
    else {
        Log("INTERNET capability is removed from subId = %lld", subId);
        mNetworkCapabilities->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    }
    SetScoreFilter(50);
    RegisterOnDemandDdsCallback();
    Log("Ready to handle network requests");
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::NeedNetworkFor(
    /* [in] */ INetworkRequest* networkRequest,
    /* [in] */ Int32 score)
{
    // figure out the apn type and enable it
    if (mHost->DBG) Log("Cellular needs Network for %s", TO_CSTR(networkRequest));
    AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
    Log("subController = %s", TO_CSTR(subController));
    Int64 currentDds;
    IISub::Probe(subController)->GetDefaultDataSubId(&currentDds);
    Int64 subId;
    mPhone->GetSubId(&subId);
    Int64 requestedSpecifier;
    subController->GetSubIdFromNetworkRequest(networkRequest, &requestedSpecifier);
    Log("CurrentDds = %lld", currentDds);
    Log("mySubId = %lld", subId);
    Log("Requested networkSpecifier = %lld", requestedSpecifier);
    String networkSpecifier;
    mNetworkCapabilities->GetNetworkSpecifier(&networkSpecifier);
    Log("my networkSpecifier = %s", networkSpecifier.string());
    if (subId < 0) {
        Log("Can't handle any network request now, subId not ready.");
        return NOERROR;
    }
    // For clients that do not send subId in NetworkCapabilities,
    // Connectivity will send to all network factories. Accept only
    // when requestedSpecifier is same as current factory's subId
    if (requestedSpecifier != subId) {
        Log("requestedSpecifier is not same as mysubId. Bail out.");
        return NOERROR;
    }
    if (currentDds != requestedSpecifier) {
        Log("This request would result in DDS switch");
        Log("Requested DDS switch to subId = %lld", requestedSpecifier);
        //Queue this request and initiate temp DDS switch.
        //Once the DDS switch is done we will revist the pending requests.
        mDdsRequests->Put(((NetworkRequest*)networkRequest)->mRequestId, networkRequest);
        RequestOnDemandDataSubscriptionLock(networkRequest);
        return NOERROR;
    }
    else {
        Boolean isNetworkRequestForInternet;
        IsNetworkRequestForInternet(networkRequest, &isNetworkRequestForInternet);
        if (isNetworkRequestForInternet) {
            Log("Activating internet request on subId = %lld", subId);
            AutoPtr<IApnContext> apnContext;
            mHost->ApnContextForNetworkRequest(networkRequest, (IApnContext**)&apnContext);
            if (apnContext != NULL) {
                mHost->Log("Activating APN=%s", TO_CSTR(apnContext));
                apnContext->IncRefCount();
            }
        }
        else {
            Boolean isValidRequest;
            IsValidRequest(networkRequest, &isValidRequest);
            if (isValidRequest) {
                //non-default APN requests for this subscription.
                mDdsRequests->Put(((NetworkRequest*)networkRequest)->mRequestId, networkRequest);
                RequestOnDemandDataSubscriptionLock(networkRequest);
            }
            else {
                Log("Bogus request req = %s", TO_CSTR(networkRequest));
            }
        }
    }
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::IsValidRequest(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkCapabilities> nc;
    n->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    AutoPtr<ArrayOf<Int32> > types;
    nc->GetCapabilities((ArrayOf<Int32>**)&types);
    *result = (types->GetLength() > 0);
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::IsNetworkRequestForInternet(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkCapabilities> nc;
    n->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    Boolean flag;
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET, &flag);
    Log("Is the request for Internet = %d", flag);
    *result = flag;
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::RequestOnDemandDataSubscriptionLock(
    /* [in] */ INetworkRequest* n)
{
    Boolean isNetworkRequestForInternet;
    IsNetworkRequestForInternet(n, &isNetworkRequestForInternet);
    if (!isNetworkRequestForInternet) {
        //Request tempDDS lock only for non-default PDP requests
        AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
        Log("requestOnDemandDataSubscriptionLock for request = %s", TO_CSTR(n));
        subController->StartOnDemandDataSubscriptionRequest(n);
    }
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::RemoveRequestFromList(
    /* [in] */ ISparseArray* list,
    /* [in] */ INetworkRequest* n)
{
    Int32 requestId;
    n->GetRequestId(&requestId);
    AutoPtr<IInterface> obj;
    list->Get(requestId, (IInterface**)&obj);
    AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(obj);
    if (nr != NULL) {
        mHost->Log("Removing request = %s", TO_CSTR(nr));
        list->Remove(requestId);
        AutoPtr<IApnContext> apnContext;
        mHost->ApnContextForNetworkRequest(n, (IApnContext**)&apnContext);
        if (apnContext != NULL) {
            mHost->Log("Deactivating APN=%s", TO_CSTR(apnContext));
            apnContext->DecRefCount();
        }
    }
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::RemoveRequestIfFound(
    /* [in] */ INetworkRequest* n)
{
    mHost->Log("Release the request from dds queue, if found");
    RemoveRequestFromList(mDdsRequests, n);
    Boolean isNetworkRequestForInternet;
    IsNetworkRequestForInternet(n, &isNetworkRequestForInternet);
    if (!isNetworkRequestForInternet) {
        AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
        subController->StopOnDemandDataSubscriptionRequest(n);
    }
    else {
        // Internet requests are not queued in DDS list. So deactivate here explicitly.
        AutoPtr<IApnContext> apnContext;
        mHost->ApnContextForNetworkRequest(n, (IApnContext**)&apnContext);
        if (apnContext != NULL) {
            mHost->Log("Deactivating APN=%s", TO_CSTR(apnContext));
            apnContext->DecRefCount();
        }
    }
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::ReleaseNetworkFor(
    /* [in] */ INetworkRequest* networkRequest)
{
    if (mHost->DBG) Log("Cellular releasing Network for %s", TO_CSTR(networkRequest));
    RemoveRequestIfFound(networkRequest);
    return NOERROR;
}

ECode DcTracker::TelephonyNetworkFactory::ReleaseAllNetworkRequests()
{
    Log("releaseAllNetworkRequests");
    AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
    Int32 size;
    mDdsRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDdsRequests->ValueAt(i, (IInterface**)&obj);
        AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(obj);
        if (nr != NULL) {
            Log("Removing request = %s", TO_CSTR(nr));
            subController->StopOnDemandDataSubscriptionRequest(nr);
            Int32 requestId;
            nr->GetRequestId(&requestId);
            mDdsRequests->Remove(requestId);
        }
    }
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcTracker::TelephonyNetworkFactory::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    Int64 subId;
    mPhone->GetSubId(&subId);
    String tag;
    tag.AppendFormat("TelephonyNetworkFactory %lld", subId);
    return Logger::D(tag, msgBuf);
}

//=============================================================================
// DcTracker::ProvisionNotificationBroadcastReceiver
//=============================================================================
DcTracker::ProvisionNotificationBroadcastReceiver::ProvisionNotificationBroadcastReceiver(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::constructor(
    /* [in] */ const String& provisionUrl,
    /* [in] */ const String& networkOperator)
{
    mNetworkOperator = networkOperator;
    mProvisionUrl = provisionUrl;
    return NOERROR;
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::SetEnableFailFastMobileData(
    /* [in] */ Int32 enabled)
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(IDctConstants::CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA, enabled, 0, (IMessage**)&msg);
    Boolean b;
    mHost->SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::EnableMobileProvisioning()
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(IDctConstants::CMD_ENABLE_MOBILE_PROVISIONING, (IMessage**)&msg);
    AutoPtr<IBundle> bundle = CBundle::ForPair(IDctConstants::PROVISIONING_URL_KEY, mProvisionUrl);
    msg->SetData(bundle);
    Boolean b;
    mHost->SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Turning back on the radio can take time on the order of a minute, so show user a
    // spinner so they know something is going on.
    CProgressDialog::New(context, (IProgressDialog**)&mHost->mProvisioningSpinner);
    IDialog::Probe(mHost->mProvisioningSpinner)->SetTitle(StringUtils::ParseInt32(mNetworkOperator));
    // TODO: Don't borrow "Connecting..." i18n string; give Telephony a version.
    AutoPtr<ICharSequence> cs;
    context->GetText(R::string::media_route_status_connecting, (ICharSequence**)&cs);
    IAlertDialog::Probe(mHost->mProvisioningSpinner)->SetMessage(cs);
    mHost->mProvisioningSpinner->SetIndeterminate(TRUE);
    IDialog::Probe(mHost->mProvisioningSpinner)->SetCancelable(TRUE);
    // Allow non-Activity Service Context to create a View.
    AutoPtr<IWindow> w;
    IDialog::Probe(mHost->mProvisioningSpinner)->GetWindow((IWindow**)&w);
    w->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    IDialog::Probe(mHost->mProvisioningSpinner)->Show();
    // After timeout, hide spinner so user can at least use their device.
    // TODO: Indicate to user that it is taking an unusually long time to connect?
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(IDctConstants::CMD_CLEAR_PROVISIONING_SPINNER, mHost->mProvisioningSpinner, (IMessage**)&msg);
    Boolean b;
    mHost->SendMessageDelayed(msg, PROVISIONING_SPINNER_TIMEOUT_MILLIS, &b);
    // This code is almost identical to the old
    // ConnectivityService.handleMobileProvisioningAction code.
    mHost->SetRadio(TRUE);
    SetEnableFailFastMobileData(IDctConstants::ENABLED);
    EnableMobileProvisioning();
    return NOERROR;
}

CAR_INTERFACE_IMPL(DcTracker::OnDemandDdsLockNotifier, Object, ISubscriptionControllerOnDemandDdsLockNotifier)

DcTracker::OnDemandDdsLockNotifier::OnDemandDdsLockNotifier(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::OnDemandDdsLockNotifier::NotifyOnDemandDdsLockGranted(
    /* [in] */ INetworkRequest* n)
{
    mHost->Log("Got the tempDds lock for the request = %s", TO_CSTR(n));
    mHost->ProcessPendingNetworkRequests(n);
    return NOERROR;
}

//=============================================================================
// DcTracker
//=============================================================================
CAR_INTERFACE_IMPL(DcTracker, DcTrackerBase, IDcTracker)

const String DcTracker::PUPPET_MASTER_RADIO_STRESS_TEST("gsm.defaultpdpcontext.active");
const Int32 DcTracker::POLL_PDP_MILLIS = 5 * 1000;
const Int32 DcTracker::PROVISIONING_SPINNER_TIMEOUT_MILLIS = 120 * 1000;
AutoPtr<IUri> DcTracker::PREFERAPN_NO_UPDATE_URI = InitPREFERAPN_NO_UPDATE_URI();
const String DcTracker::APN_ID("apn_id");
const String DcTracker::PROPERTY_CDMA_IPPROTOCOL = InitPROPERTY_CDMA_IPPROTOCOL();
const String DcTracker::PROPERTY_CDMA_ROAMING_IPPROTOCOL = InitPROPERTY_CDMA_ROAMING_IPPROTOCOL();

DcTracker::DcTracker()
    : mDisconnectPendingCount(0)
    , mReregisterOnReconnectFailure(FALSE)
    , mCanSetPreferApn(FALSE)
    , mImsRegistrationState(FALSE)
    , mWaitCleanUpApnContext(NULL)
    , mDeregistrationAlarmState(FALSE)
    , mImsDeregistrationDelayIntent(NULL)
    , mWwanIwlanCoexistFlag(FALSE)
{
    Boolean mOosIsDisconnect;
    SystemProperties::GetBoolean(IPhoneBase::PROPERTY_OOS_IS_DISCONNECT, TRUE, &mOosIsDisconnect);
    CArrayList::New((IArrayList**)&mDisconnectAllCompleteMsgList);
    CRegistrantList::New((IRegistrantList**)&mAllDataDisconnectedRegistrants);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mAttached);
}

ECode DcTracker::constructor(
    /* [in] */ IPhoneBase* p)
{
    DcTrackerBase::constructor(p);
    Int32 phoneType;
    IPhone::Probe(p)->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
        LOG__TAG = "GsmDCT";
    } else if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        LOG__TAG = "CdmaDCT";
    } else {
        LOG__TAG = "DCT";
        Loge("unexpected phone type [%d]", phoneType);
    }
    if (DBG) Log(LOG__TAG + ".constructor");
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<IResources> res;
        AutoPtr<IContext> context;
        IPhone::Probe(p)->GetContext((IContext**)&context);
        context->GetResources((IResources**)&res);
        Boolean fetchApnFromOmhCard;
        res->GetBoolean(R::bool_::config_fetch_apn_from_omh_card, &fetchApnFromOmhCard);
        Log(LOG__TAG + " fetchApnFromOmhCard: %d", fetchApnFromOmhCard);
        if (fetchApnFromOmhCard) {
            CCdmaApnProfileTracker::New(ICDMAPhone::Probe(p), (ICdmaApnProfileTracker**)&mOmhApt);
            mOmhApt->RegisterForModemProfileReady(this,
                    IDctConstants::EVENT_MODEM_DATA_PROFILE_READY, NULL);
        }
    }
    mDataConnectionTracker = this;
    Update();
    mApnObserver = new ApnChangeObserver(this);
    mApnObserver->constructor();
    AutoPtr<IContext> context;
    IPhone::Probe(p)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    contentResolver->RegisterContentObserver(
            Elastos::Droid::Provider::Telephony::Carriers::CONTENT_URI, TRUE, mApnObserver);
    InitApnContexts();
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        // Register the reconnect and restart actions.
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        String apnType;
        apnContext->GetApnType(&apnType);
        StringBuilder s(INTENT_RECONNECT_ALARM);
        s += ".";
        s += apnType;
        filter->AddAction(s.ToString());
        s.Reset();
        s += INTENT_RESTART_TRYSETUP_ALARM;
        s += ".";
        s += apnType;
        filter->AddAction(s.ToString());
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        AutoPtr<IIntent> intent;
        context->RegisterReceiver(mIntentReceiver, filter, String(NULL), IHandler::Probe(mPhone), (IIntent**)&intent);
    }
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkFilter);
    mNetworkFilter->AddTransportType(INetworkCapabilities::TRANSPORT_CELLULAR);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_MMS);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_SUPL);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_DUN);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_FOTA);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_IMS);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_CBS);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_IA);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_RCS);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_XCAP);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_EIMS);
    mNetworkFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
    //Dont add INTERNET capability, only defaultDataSubscription provides INTERNET.
    AutoPtr<ILooper> looper;
    GetLooper((ILooper**)&looper);
    mNetworkFactory = new TelephonyNetworkFactory(this);
    ((TelephonyNetworkFactory*) mNetworkFactory.Get())->constructor(looper, context,
            String("TelephonyNetworkFactory"), mNetworkFilter, mPhone);
    mNetworkFactory->SetScoreFilter(-1);
    CMessenger::New(IHandler::Probe(mNetworkFactory), (IMessenger**)&mNetworkFactoryMessenger);
    cm->RegisterNetworkFactory(mNetworkFactoryMessenger, String("Telephony"));
    // Add Emergency APN to APN setting list by default to support EPDN in sim absent cases
    InitEmergencyApnSetting();
    AddEmergencyApnSetting();
    Int32 phoneId;
    p->GetPhoneId(&phoneId);
    mProvisionActionName = String("com.android.internal.telephony.PROVISION") + StringUtils::ToString(phoneId);
    context = NULL;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mSubInfoBroadcastReceiver, filter, (IIntent**)&intent);
    filter = NULL;
    CIntentFilter::New(ITelephonyIntents::ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, (IIntentFilter**)&filter);
    intent = NULL;
    context->RegisterReceiver(mDefaultDdsBroadcastReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode DcTracker::ProcessPendingNetworkRequests(
    /* [in] */ INetworkRequest* n)
{
    return ((TelephonyNetworkFactory*) mNetworkFactory.Get())->ProcessPendingNetworkRequests(n);
}

ECode DcTracker::UpdateSubIdAndCapability()
{
    return ((TelephonyNetworkFactory*) mNetworkFactory.Get())->UpdateNetworkCapability(mSubId);
}

ECode DcTracker::ReleaseAllNetworkRequests()
{
    return ((TelephonyNetworkFactory*) mNetworkFactory.Get())->ReleaseAllNetworkRequests();
}

ECode DcTracker::RegisterForAllEvents()
{
    ((PhoneBase*) mPhone.Get())->mCi->RegisterForAvailable(this, IDctConstants::EVENT_RADIO_AVAILABLE, NULL);
    ((PhoneBase*) mPhone.Get())->mCi->RegisterForOffOrNotAvailable(this,
            IDctConstants::EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
    ((PhoneBase*) mPhone.Get())->mCi->RegisterForWwanIwlanCoexistence(this,
            IDctConstants::EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE, NULL);
    ((PhoneBase*) mPhone.Get())->mCi->RegisterForDataNetworkStateChanged(this,
            IDctConstants::EVENT_DATA_STATE_CHANGED, NULL);
    AutoPtr<ICallTracker> ct;
    mPhone->GetCallTracker((ICallTracker**)&ct);
    ct->RegisterForVoiceCallEnded(this,
             IDctConstants::EVENT_VOICE_CALL_ENDED, NULL);
    ct->RegisterForVoiceCallStarted (this,
             IDctConstants::EVENT_VOICE_CALL_STARTED, NULL);
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    serviceStateTracker->RegisterForDataConnectionAttached(this,
             IDctConstants::EVENT_DATA_CONNECTION_ATTACHED, NULL);
    serviceStateTracker->RegisterForDataConnectionDetached(this,
             IDctConstants::EVENT_DATA_CONNECTION_DETACHED, NULL);
    serviceStateTracker->RegisterForRoamingOn(this,
             IDctConstants::EVENT_ROAMING_ON, NULL);
    serviceStateTracker->RegisterForRoamingOff(this,
             IDctConstants::EVENT_ROAMING_OFF, NULL);
    serviceStateTracker->RegisterForPsRestrictedEnabled(this,
             IDctConstants::EVENT_PS_RESTRICT_ENABLED, NULL);
    serviceStateTracker->RegisterForPsRestrictedDisabled(this,
             IDctConstants::EVENT_PS_RESTRICT_DISABLED, NULL);
    // SubscriptionManager::RegisterForDdsSwitch(this,
    //          IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS, NULL);
    serviceStateTracker->RegisterForDataRegStateOrRatChanged(this,
             IDctConstants::EVENT_DATA_RAT_CHANGED, NULL);
    Int32 phoneType = 0;
    IPhone::Probe(mPhone)->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        mCdmaSsm = CdmaSubscriptionSourceManager::GetInstance(context, ((PhoneBase*) mPhone.Get())->mCi, this,
                IDctConstants::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        // CdmaSsm doesn't send this event whenever you register - fake it ourselves
        AutoPtr<IMessage> msg;
        ObtainMessage(IDctConstants::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
    }
    return NOERROR;
}

ECode DcTracker::Dispose()
{
    if (DBG) Log("GsmDCT.dispose");
    ReleaseAllNetworkRequests();
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    if (mProvisionBroadcastReceiver != NULL) {
        context->UnregisterReceiver(mProvisionBroadcastReceiver);
        mProvisionBroadcastReceiver = NULL;
    }
    if (mProvisioningSpinner != NULL) {
        IDialogInterface::Probe(mProvisioningSpinner)->Dismiss();
        mProvisioningSpinner = NULL;
    }
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    cm->UnregisterNetworkFactory(mNetworkFactoryMessenger);
    context->UnregisterReceiver(mDefaultDdsBroadcastReceiver);
    context->UnregisterReceiver(mSubInfoBroadcastReceiver);
    mNetworkFactoryMessenger = NULL;
    Boolean b;
    CleanUpAllConnections(TRUE, String(NULL), &b);
    DcTrackerBase::Dispose();
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    contentResolver->UnregisterContentObserver(mApnObserver);
    mApnContexts->Clear();
    mPrioritySortedApnContexts->Clear();
    if (mCdmaSsm != NULL) {
        mCdmaSsm->Dispose(this);
    }
    if (mOmhApt != NULL) {
        mOmhApt->UnregisterForModemProfileReady(this);
    }
    DestroyDataConnections();
    return NOERROR;
}

ECode DcTracker::UnregisterForAllEvents()
{
     //Unregister for all events
    ((PhoneBase*) mPhone.Get())->mCi->UnregisterForAvailable(this);
    ((PhoneBase*) mPhone.Get())->mCi->UnregisterForOffOrNotAvailable(this);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != NULL) {
        r->UnregisterForRecordsLoaded(this);
        mIccRecords->Set(NULL);
    }
    ((PhoneBase*) mPhone.Get())->mCi->UnregisterForDataNetworkStateChanged(this);
    AutoPtr<ICallTracker> ct;
    mPhone->GetCallTracker((ICallTracker**)&ct);
    ct->UnregisterForVoiceCallEnded(this);
    ct->UnregisterForVoiceCallStarted(this);
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    serviceStateTracker->UnregisterForDataConnectionAttached(this);
    serviceStateTracker->UnregisterForDataConnectionDetached(this);
    serviceStateTracker->UnregisterForRoamingOn(this);
    serviceStateTracker->UnregisterForRoamingOff(this);
    serviceStateTracker->UnregisterForPsRestrictedEnabled(this);
    serviceStateTracker->UnregisterForPsRestrictedDisabled(this);
    //SubscriptionManager.unregisterForDdsSwitch(this);
    return NOERROR;
}

ECode DcTracker::ApnContextForNetworkRequest(
    /* [in] */ INetworkRequest* nr,
    /* [out] */ IApnContext** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkCapabilities> nc;
    nr->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    // for now, ignore the bandwidth stuff
    AutoPtr<ArrayOf<Int32> > types;
    nc->GetTransportTypes((ArrayOf<Int32>**)&types);
    Boolean hasTransport;
    nc->HasTransport(INetworkCapabilities::TRANSPORT_CELLULAR, &hasTransport);
    if (types->GetLength() > 0 && hasTransport == FALSE) {
        *result = NULL;
        return NOERROR;
    }
    // in the near term just do 1-1 matches.
    // TODO - actually try to match the set of capabilities
    Int32 type = -1;
    String name(NULL);
    Boolean error = FALSE;
    Boolean hasCapability;
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_DEFAULT;
        type = IConnectivityManager::TYPE_MOBILE;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_MMS, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_MMS;
        type = IConnectivityManager::TYPE_MOBILE_MMS;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_SUPL, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_SUPL;
        type = IConnectivityManager::TYPE_MOBILE_SUPL;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_DUN, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_DUN;
        type = IConnectivityManager::TYPE_MOBILE_DUN;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_FOTA, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_FOTA;
        type = IConnectivityManager::TYPE_MOBILE_FOTA;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_IMS, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_IMS;
        type = IConnectivityManager::TYPE_MOBILE_IMS;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_CBS, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_CBS;
        type = IConnectivityManager::TYPE_MOBILE_CBS;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_IA, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = IPhoneConstants::APN_TYPE_IA;
        type = IConnectivityManager::TYPE_MOBILE_IA;
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_RCS, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = NULL;
        Loge("RCS APN type not yet supported");
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_XCAP, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = NULL;
        Loge("XCAP APN type not yet supported");
    }
    nc->HasCapability(INetworkCapabilities::NET_CAPABILITY_EIMS, &hasCapability);
    if (hasCapability) {
        if (name != NULL) error = TRUE;
        name = NULL;
        Loge("EIMS APN type not yet supported");
    }
    if (error) {
        Loge("Multiple apn types specified in request - result is unspecified!");
    }
    if (type == -1 || name == NULL) {
        Loge("Unsupported NetworkRequest in Telephony: %s", TO_CSTR(nr));
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(name), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        Loge("Request for unsupported mobile type: %d", type);
    }
    *result = apnContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTracker::SetRadio(
    /* [in] */ Boolean on)
{
    AutoPtr<IInterface> service = ServiceManager::CheckService(String("phone"));
    AutoPtr<IITelephony> phone = IITelephony::Probe(service);
    // try {
    Boolean b;
    phone->SetRadio(on, &b);
    // } catch (Exception e) {
        // Ignore.
    // }
    return NOERROR;
}

ECode DcTracker::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(type), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IDcAsyncChannel> dcAsyncChannel;
    apnContext->GetDcAc((IDcAsyncChannel**)&dcAsyncChannel);
    *result = (dcAsyncChannel != NULL);
    return NOERROR;
}

ECode DcTracker::IsOnDemandDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * Check if APN enabled
     * Check if MobileData is ON
     * Check if MobileData UI override present
     */
    Boolean flag = FALSE;
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Boolean apnContextIsEnabled;
    apnContext->IsEnabled(&apnContextIsEnabled);
    DctConstantsState apnContextState;
    apnContext->GetState(&apnContextState);
    Boolean apnTypePossible = !(apnContextIsEnabled &&
            (apnContextState == DctConstantsState_FAILED));
    Boolean userDataEnabled = mUserDataEnabled;
    if (IPhoneConstants::APN_TYPE_MMS.Equals(apnType)) {
        AutoPtr<IResources> res;
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        context->GetResources((IResources**)&res);
        Boolean mobileDataOffOveride;
        res->GetBoolean(R::bool_::config_enable_mms_with_mobile_data_off, &mobileDataOffOveride);
        Log("isOnDemandDataPossible MobileDataEnabled override = %d", mobileDataOffOveride);
        userDataEnabled = (mUserDataEnabled || mobileDataOffOveride);
    }
    flag = apnTypePossible && userDataEnabled;
    Log("isOnDemandDataPossible, possible =%d, apnContext = %s", flag, TO_CSTR(apnContext));
    *result = flag;
    return NOERROR;
}

ECode DcTracker::IsDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Boolean apnContextIsEnabled;
    apnContext->IsEnabled(&apnContextIsEnabled);
    DctConstantsState apnContextState;
    apnContext->GetState(&apnContextState);
    Boolean apnTypePossible = !(apnContextIsEnabled &&
            (apnContextState == DctConstantsState_FAILED));
    String apnContextType;
    apnContext->GetApnType(&apnContextType);
    Boolean isEmergencyApn = apnContextType.Equals(IPhoneConstants::APN_TYPE_EMERGENCY);
    // Set the emergency APN availability status as TRUE irrespective of conditions checked in
    // isDataAllowed() like IN_SERVICE, MOBILE DATA status etc.
    Boolean isDataAllowed;
    IsDataAllowed(&isDataAllowed);
    Boolean dataAllowed = isEmergencyApn || isDataAllowed;
    Boolean possible = dataAllowed && apnTypePossible;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilDataRadioTechnology;
    serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    if ((apnType.Equals(IPhoneConstants::APN_TYPE_DEFAULT)
                || apnType.Equals(IPhoneConstants::APN_TYPE_IA))
            && (rilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN)
            && (!mWwanIwlanCoexistFlag)) {
        Log("Default data call activation not possible in iwlan.");
        possible = FALSE;
    }
    if (VDBG) {
        Log("isDataPossible(%s): possible=%d isDataAllowed=%d "
                "apnTypePossible=%d apnContextisEnabled=%d apnContextState()=%d",
                apnType.string(), possible, dataAllowed, apnTypePossible,
                apnContextIsEnabled, apnContextState);
    }
    *result = possible;
    return NOERROR;
}

ECode DcTracker::Finalize()
{
    if (DBG) Log("finalize");
    return NOERROR;
}

ECode DcTracker::SupplyMessenger()
{
    // Supply the data connection tracker messenger only if
    // this is corresponding to the current DDS.
    Boolean isActiveDataSubscription;
    IsActiveDataSubscription(&isActiveDataSubscription);
    if (!isActiveDataSubscription) {
        return NOERROR;
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    AutoPtr<IMessenger> msg;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_MMS, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_SUPL, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_DUN, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_HIPRI, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_FOTA, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_IMS, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_CBS, msg);
    msg = NULL;
    CMessenger::New(this, (IMessenger**)&msg);
    cm->SupplyMessenger(IConnectivityManager::TYPE_MOBILE_EMERGENCY, msg);
    return NOERROR;
}

ECode DcTracker::AddApnContext(
    /* [in] */ const String& type,
    /* [in] */ INetworkConfig* networkConfig,
    /* [out] */ IApnContext** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IApnContext> apnContext;
    CApnContext::New(context, type, LOG__TAG, networkConfig,
            this, (IApnContext**)&apnContext);
    mApnContexts->Put(StringUtils::ParseCharSequence(type), apnContext);
    mPrioritySortedApnContexts->Add(apnContext);
    *result = apnContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTracker::InitApnContexts()
{
    Log("initApnContexts: E");
    // Load device network attributes from resources
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > networkConfigStrings;
    res->GetStringArray(R::array::networkAttributes, (ArrayOf<String>**)&networkConfigStrings);
    for (Int32 i = 0; i < networkConfigStrings->GetLength(); ++i) {
        String networkConfigString = (*networkConfigStrings)[i];
        AutoPtr<INetworkConfig> networkConfig;
        CNetworkConfig::New((INetworkConfig**)&networkConfig);
        AutoPtr<IApnContext> apnContext;
        Int32 type;
        networkConfig->GetType(&type);
        switch (type) {
        case IConnectivityManager::TYPE_MOBILE:
            AddApnContext(IPhoneConstants::APN_TYPE_DEFAULT, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_MMS:
            AddApnContext(IPhoneConstants::APN_TYPE_MMS, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_SUPL:
            AddApnContext(IPhoneConstants::APN_TYPE_SUPL, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_DUN:
            AddApnContext(IPhoneConstants::APN_TYPE_DUN, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_HIPRI:
            AddApnContext(IPhoneConstants::APN_TYPE_HIPRI, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_FOTA:
            AddApnContext(IPhoneConstants::APN_TYPE_FOTA, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_IMS:
            AddApnContext(IPhoneConstants::APN_TYPE_IMS, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_CBS:
            AddApnContext(IPhoneConstants::APN_TYPE_CBS, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_IA:
            AddApnContext(IPhoneConstants::APN_TYPE_IA, networkConfig, (IApnContext**)&apnContext);
            break;
        case IConnectivityManager::TYPE_MOBILE_EMERGENCY:
            AddApnContext(IPhoneConstants::APN_TYPE_EMERGENCY, networkConfig, (IApnContext**)&apnContext);
            break;
        default:
            Log("initApnContexts: skipping unknown type=%d", type);
            continue;
        }
        Log("initApnContexts: apnContext=%s", TO_CSTR(apnContext));
    }
    Log("initApnContexts: X mApnContexts=%s", TO_CSTR(mApnContexts));
    return NOERROR;
}

ECode DcTracker::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext != NULL) {
        AutoPtr<IDcAsyncChannel> dcac;
        apnContext->GetDcAc((IDcAsyncChannel**)&dcac);
        if (dcac != NULL) {
            if (DBG) Log("return link properites for %s", apnType.string());
            return dcac->GetLinkPropertiesSync(result);
        }
    }
    if (DBG) Log("return new LinkProperties");
    return CLinkProperties::New(result);
}

ECode DcTracker::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext!=NULL) {
        AutoPtr<IDcAsyncChannel> dataConnectionAc;
        apnContext->GetDcAc((IDcAsyncChannel**)&dataConnectionAc);
        if (dataConnectionAc != NULL) {
            if (DBG) {
                Log("get active pdp is not null, return NetworkCapabilities for %s", apnType.string());
            }
            return dataConnectionAc->GetNetworkCapabilitiesSync(result);
        }
    }
    if (DBG) Log("return new NetworkCapabilities");
    return CNetworkCapabilities::New(result);
}

ECode DcTracker::GetActiveApnTypes(
    /* [out, callee] */ ArrayOf<String>** _result)
{
    VALIDATE_NOT_NULL(_result)

    if (DBG) Log("get all active apn types");
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isReady;
        apnContext->IsReady(&isReady);
        Boolean b;
        mAttached->Get(&b);
        if (b && isReady) {
            String apnType;
            apnContext->GetApnType(&apnType);
            result->Add(StringUtils::ParseCharSequence(apnType));
        }
    }
    AutoPtr<ArrayOf<IInterface*> > array;
    result->ToArray(ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&array);
    Int32 size = array->GetLength();
    *_result = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        (*_result)->Set(i, TO_STR((*array)[i]));
    }
    return NOERROR;
}

ECode DcTracker::GetActiveApnString(
    /* [in] */ const String& apnType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (VDBG) Log( "get active apn string for type:%s", apnType.string());
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext != NULL) {
        AutoPtr<IApnSetting> apnSetting;
        apnContext->GetApnSetting((IApnSetting**)&apnSetting);
        if (apnSetting != NULL) {
            return apnSetting->GetApn(result);
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode DcTracker::IsApnTypeEnabled(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Boolean isEnabled;
    apnContext->IsEnabled(&isEnabled);
    *result = isEnabled;
    return NOERROR;
}

ECode DcTracker::SetState(
    /* [in] */ DctConstantsState s)
{
    if (DBG) Log("setState should not be used in GSM %d", s);
    return NOERROR;
}

ECode DcTracker::GetState(
    /* [in] */ const String& apnType,
    /* [out] */ DctConstantsState* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext != NULL) {
        return apnContext->GetState(result);
    }
    *result = DctConstantsState_FAILED;
    return NOERROR;
}

ECode DcTracker::IsProvisioningApn(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext != NULL) {
        Boolean isProvisioningApn;
        apnContext->IsProvisioningApn(&isProvisioningApn);
        *result = isProvisioningApn;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::GetOverallState(
    /* [out] */ DctConstantsState* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isConnecting = FALSE;
    Boolean isFailed = TRUE; // All enabled Apns should be FAILED.
    Boolean isAnyEnabled = FALSE;
    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isEnabled;
        apnContext->IsEnabled(&isEnabled);
        if (isEnabled) {
            isAnyEnabled = TRUE;
            DctConstantsState state;
            apnContext->GetState(&state);
            switch (state) {
            case DctConstantsState_CONNECTED:
            case DctConstantsState_DISCONNECTING:
                if (DBG) Log("overall state is CONNECTED");
                *result = DctConstantsState_CONNECTED;
                return NOERROR;
            case DctConstantsState_RETRYING:
            case DctConstantsState_CONNECTING:
                isConnecting = TRUE;
                isFailed = FALSE;
                break;
            case DctConstantsState_IDLE:
            case DctConstantsState_SCANNING:
                isFailed = FALSE;
                break;
            default:
                isAnyEnabled = TRUE;
                break;
            }
        }
    }
    if (!isAnyEnabled) { // Nothing enabled. return IDLE.
        if (DBG) Log( "overall state is IDLE");
        *result = DctConstantsState_IDLE;
        return NOERROR;
    }
    if (isConnecting) {
        if (DBG) Log( "overall state is CONNECTING");
        return DctConstantsState_CONNECTING;
    } else if (!isFailed) {
        if (DBG) Log( "overall state is IDLE");
        *result = DctConstantsState_IDLE;
        return NOERROR;
    } else {
        if (DBG) Log( "overall state is FAILED");
        *result = DctConstantsState_FAILED;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTracker::IsApnTypeAvailable(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IApnSetting> dunApn;
    FetchDunApn((IApnSetting**)&dunApn);
    if (type.Equals(IPhoneConstants::APN_TYPE_DUN) && dunApn != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    if (mAllApnSettings != NULL) {
        AutoPtr<IIterator> it;
        mAllApnSettings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
            Boolean canHandleType;
            apn->CanHandleType(type, &canHandleType);
            if (canHandleType) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::GetAnyDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(mDataEnabledLock);
    if (!(mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        // Make sure we don't have a context that is going down
        // and is explicitly disabled.
        Boolean isDataAllowed;
        IsDataAllowed(apnContext, &isDataAllowed);
        if (isDataAllowed) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::GetAnyDataEnabled(
    /* [in] */ Boolean checkUserDataEnabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(mDataEnabledLock);
    if (!(mInternalDataEnabled && (!checkUserDataEnabled || mUserDataEnabled)
                && (!checkUserDataEnabled || sPolicyDataEnabled)))
        *result = FALSE;
        return NOERROR;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        // Make sure we dont have a context that going down
        // and is explicitly disabled.
        Boolean isDataAllowed;
        IsDataAllowed(apnContext, &isDataAllowed);
        if (isDataAllowed) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::IsDataAllowed(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    //If RAT is iwlan then dont allow default/IA PDP at all.
    //Rest of APN types can be evaluated for remaining conditions.
    String apnType;
    apnContext->GetApnType(&apnType);
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilDataRadioTechnology;
    serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    if ((apnType.Equals(IPhoneConstants::APN_TYPE_DEFAULT)
                || apnType.Equals(IPhoneConstants::APN_TYPE_IA))
            && (rilDataRadioTechnology
                == IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN)
            && (!mWwanIwlanCoexistFlag)) {
        Log("Default data call activation not allowed in iwlan.");
        *result = FALSE;
        return NOERROR;
    } else {
        Boolean isReady;
        apnContext->IsReady(&isReady);
        Boolean isDataAllowed;
        IsDataAllowed(&isDataAllowed);
        *result = isReady && isDataAllowed;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTracker::OnDataConnectionDetached()
{
    /*
     * We presently believe it is unnecessary to tear down the PDP context
     * when GPRS detaches, but we should stop the network polling.
     */
    if (DBG) Log ("onDataConnectionDetached: stop polling and notify detached");
    StopNetStatPoll();
    StopDataStallAlarm();
    NotifyDataConnection(IPhone::REASON_DATA_DETACHED);
    mAttached->Set(FALSE);
    return NOERROR;
}

ECode DcTracker::OnDataConnectionAttached()
{
    if (DBG) Log("onDataConnectionAttached");
    mAttached->Set(TRUE);
    DctConstantsState overallState;
    GetOverallState(&overallState);
    if (overallState == DctConstantsState_CONNECTED) {
        if (DBG) Log("onDataConnectionAttached: start polling notify attached");
        StartNetStatPoll();
        StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
        NotifyDataConnection(IPhone::REASON_DATA_ATTACHED);
    } else {
        // update APN availability so that APN can be enabled.
        NotifyOffApnsOfAvailability(IPhone::REASON_DATA_ATTACHED);
    }
    if (mAutoAttachOnCreationConfig) {
        mAutoAttachOnCreation = TRUE;
    }
    SetupDataOnConnectableApns(IPhone::REASON_DATA_ATTACHED);
    return NOERROR;
}

ECode DcTracker::IsDataAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean internalDataEnabled;
    {
        AutoLock lock(mDataEnabledLock);
        internalDataEnabled = mInternalDataEnabled;
    }
    Boolean attachedState;
    mAttached->Get(&attachedState);
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    Boolean desiredPowerState;
    serviceStateTracker->GetDesiredPowerState(&desiredPowerState);
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 radioTech;
    serviceState->GetRilDataRadioTechnology(&radioTech);
    if (radioTech == IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
            && desiredPowerState == FALSE) {
        desiredPowerState = TRUE;
        attachedState = TRUE;
    }
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    Boolean recordsLoaded;
    if (r != NULL) {
        r->GetRecordsLoaded(&recordsLoaded);
    }
    else {
        recordsLoaded = FALSE;
    }
    Boolean subscriptionFromNv;
    IsNvSubscription(&subscriptionFromNv);
    Boolean isConcurrentVoiceAndDataAllowed;
    serviceStateTracker->IsConcurrentVoiceAndDataAllowed(&isConcurrentVoiceAndDataAllowed);
    serviceState = NULL;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Boolean roaming;
    serviceState->GetRoaming(&roaming);
    PhoneConstantsState state;
    IPhone::Probe(mPhone)->GetState(&state);
    Boolean isDataOnRoamingEnabled;
    GetDataOnRoamingEnabled(&isDataOnRoamingEnabled);
    Boolean allowed =
                (attachedState || mAutoAttachOnCreation) &&
                (subscriptionFromNv || recordsLoaded) &&
                (state == PhoneConstantsState_IDLE ||
                isConcurrentVoiceAndDataAllowed) &&
                internalDataEnabled &&
                (!roaming || isDataOnRoamingEnabled) &&
                !mIsPsRestricted &&
                desiredPowerState;
    if (!allowed && DBG) {
        String reason("");
        if (!(attachedState || mAutoAttachOnCreation)) {
            reason.AppendFormat(" - Attached= %d", attachedState);
        }
        if (!(subscriptionFromNv || recordsLoaded)) {
            reason += " - SIM not loaded and not NV subscription";
        }
        Boolean isConcurrentVoiceAndDataAllowed;
        serviceStateTracker->IsConcurrentVoiceAndDataAllowed(&isConcurrentVoiceAndDataAllowed);
        if (state != PhoneConstantsState_IDLE && !isConcurrentVoiceAndDataAllowed) {
            reason.AppendFormat(" - PhoneState= %d", state);
            reason += " - Concurrent voice and data not allowed";
        }
        if (!internalDataEnabled) reason += " - mInternalDataEnabled= false";
        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Boolean roaming;
        serviceState->GetRoaming(&roaming);
        if (roaming && !isDataOnRoamingEnabled) {
            reason += " - Roaming and data roaming not enabled";
        }
        if (mIsPsRestricted) reason += " - mIsPsRestricted= true";
        if (!desiredPowerState) reason += " - desiredPowerState= false";
        if (DBG) Log("isDataAllowed: not allowed due to %s", reason.string());
    }
    *result = allowed;
    return NOERROR;
}

ECode DcTracker::SetupDataOnConnectableApns(
    /* [in] */ const String& reason)
{
    if (DBG) Log("setupDataOnConnectableApns: %s", reason.string());
    AutoPtr<IIterator> it;
    mPrioritySortedApnContexts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        if (DBG) Log("setupDataOnConnectableApns: apnContext %s", TO_CSTR(apnContext));
        DctConstantsState state;
        apnContext->GetState(&state);
        if (state == DctConstantsState_FAILED) {
            apnContext->SetState(DctConstantsState_IDLE);
        }
        Boolean isConnectable;
        apnContext->IsConnectable(&isConnectable);
        if (isConnectable) {
            Log("setupDataOnConnectableApns: isConnectable() call trySetupData");
            apnContext->SetReason(reason);
            Boolean b;
            TrySetupData(apnContext, &b);
        }
    }
    return NOERROR;
}

ECode DcTracker::TrySetupData(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) {
        String apnType;
        apnContext->GetApnType(&apnType);
        String reason;
        apnContext->GetReason(&reason);
        Log("trySetupData for type:%s due to %s apnContext=%s", apnType.string(), reason.string(), TO_CSTR(apnContext));
        Log("trySetupData with mIsPsRestricted=%d", mIsPsRestricted);
    }
    assert(0 && "TODO: ISimulatedRadioControl");
    // AutoPtr<ISimulatedRadioControl> control;
    // mPhone->GetSimulatedRadioControl((ISimulatedRadioControl**)&control);
    // if (control != NULL) {
    //     // Assume data is connected on the simulator
    //     // FIXME  this can be improved
    //     apnContext->SetState(DctConstantsState_CONNECTED);
    //     String apnType;
    //     apnContext->GetApnType(&apnType);
    //     String reason;
    //     apnContext->GetReason(&reason);
    //     mPhone->NotifyDataConnection(reason, apnType);
    //     Log("trySetupData: X We're on the simulator; assuming connected retValue=true");
    //     *result = TRUE;
    //     return NOERROR;
    // }
    // Allow SETUP_DATA request for E-APN to be completed during emergency call
    // and MOBILE DATA On/Off cases as well.
    String apnType;
    apnContext->GetApnType(&apnType);
    Boolean isEmergencyApn = apnType.Equals(IPhoneConstants::APN_TYPE_EMERGENCY);
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    Boolean desiredPowerState;
    serviceStateTracker->GetDesiredPowerState(&desiredPowerState);
    Boolean checkUserDataEnabled =
                !(apnType.Equals(IPhoneConstants::APN_TYPE_IMS));
    // If set the special property, enable mms data even if mobile data is turned off.
    if (apnType.Equals(IPhoneConstants::APN_TYPE_MMS)) {
        AutoPtr<IResources> res;
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        context->GetResources((IResources**)&res);
        Boolean b;
        res->GetBoolean(R::bool_::config_enable_mms_with_mobile_data_off, &b);
        checkUserDataEnabled = checkUserDataEnabled && !b;
    }
    Boolean isConnectable;
    apnContext->IsConnectable(&isConnectable);
    Boolean isEmergency;
    IsEmergency(&isEmergency);
    Boolean isDataAllowed;
    IsDataAllowed(apnContext, &isDataAllowed);
    Boolean isAnyDataEnabled;
    GetAnyDataEnabled(checkUserDataEnabled, &isAnyDataEnabled);
    if (isConnectable && (isEmergencyApn ||
            (isDataAllowed &&
            isAnyDataEnabled && !isEmergency))) {
        DctConstantsState apnContextState;
        apnContext->GetState(&apnContextState);
        if (apnContextState == DctConstantsState_FAILED) {
            if (DBG) Log("trySetupData: make a FAILED ApnContext IDLE so its reusable");
            apnContext->SetState(DctConstantsState_IDLE);
        }
        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Int32 radioTech;
        serviceState->GetRilDataRadioTechnology(&radioTech);
        if (apnContextState == DctConstantsState_IDLE) {
            String apnType;
            apnContext->GetApnType(&apnType);
            AutoPtr<IArrayList> waitingApns;
            BuildWaitingApns(apnType, radioTech, (IArrayList**)&waitingApns);
            Boolean isEmpty;
            waitingApns->IsEmpty(&isEmpty);
            if (isEmpty) {
                AutoPtr<IDcFailCause> dfc;
                CDcFailCause::New(DcFailCause_MISSING_UNKNOWN_APN, (IDcFailCause**)&dfc);
                NotifyNoData(dfc, apnContext);
                String reason;
                apnContext->GetReason(&reason);
                NotifyOffApnsOfAvailability(reason);
                if (DBG) Log("trySetupData: X No APN found retValue=false");
                *result = FALSE;
                return NOERROR;
            } else {
                apnContext->SetWaitingApns(waitingApns);
                if (DBG) {
                    String s;
                    ApnListToString(mAllApnSettings, &s);
                    Log("trySetupData: Create from mAllApnSettings : ", s.string());
                }
            }
        }
        if (DBG) {
            AutoPtr<IArrayList> waitingApns;
            apnContext->GetWaitingApns((IArrayList**)&waitingApns);
            if (waitingApns == NULL) {
                Log("trySetupData: call setupData, waitingApns : null");
            }
            else {
                String s;
                ApnListToString(waitingApns, &s);
                Log("trySetupData: call setupData, waitingApns : ", s.string());
            }
        }
        Boolean retValue;
        SetupData(apnContext, radioTech, &retValue);
        String reason;
        apnContext->GetReason(&reason);
        NotifyOffApnsOfAvailability(reason);
        if (DBG) Log("trySetupData: X retValue=%d", retValue);
        *result = retValue;
        return NOERROR;
    } else {
        Boolean isConnectable;
        apnContext->IsConnectable(&isConnectable);
        String apnType;
        apnContext->GetApnType(&apnType);
        if (!apnType.Equals(IPhoneConstants::APN_TYPE_DEFAULT) && isConnectable) {
            String reason;
            apnContext->GetReason(&reason);
            mPhone->NotifyDataConnectionFailed(reason, apnType);
        }
        String reason;
        apnContext->GetReason(&reason);
        NotifyOffApnsOfAvailability(reason);
        if (DBG) Log("trySetupData: X apnContext not 'ready' retValue=false");
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTracker::NotifyOffApnsOfAvailability(
    /* [in] */ const String& reason)
{
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isReady;
        apnContext->IsReady(&isReady);
        Boolean b;
        mAttached->Get(&b);
        if ((!b && mOosIsDisconnect) || !isReady) {
            String apnType;
            apnContext->GetApnType(&apnType);
            if (VDBG) Log("notifyOffApnOfAvailability type:%s", apnType.string());
            String apnContextReason;
            apnContext->GetReason(&apnContextReason);
            mPhone->NotifyDataConnection(!reason.IsNull() ? reason : apnContextReason,
                                        apnType,
                                        PhoneConstantsDataState_DISCONNECTED);
        } else {
            if (VDBG) {
                Log("notifyOffApnsOfAvailability skipped apn due to attached && isReady %s",
                        TO_CSTR(apnContext));
            }
        }
    }
    return NOERROR;
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ Boolean tearDown,
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("cleanUpAllConnections: tearDown=%d reason=%s", tearDown, reason.string());
    Boolean didDisconnect = FALSE;
    Boolean specificdisable = FALSE;
    if (!TextUtils::IsEmpty(reason)) {
        specificdisable = reason.Equals(IPhone::REASON_DATA_SPECIFIC_DISABLED);
    }
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isDisconnected;
        apnContext->IsDisconnected(&isDisconnected);
        if (isDisconnected == FALSE) didDisconnect = TRUE;
        if (specificdisable) {
            String apnType;
            apnContext->GetApnType(&apnType);
            if (!apnType.Equals(IPhoneConstants::APN_TYPE_IMS)) {
                if (DBG) Log("ApnConextType: %s", apnType.string());
                apnContext->SetReason(reason);
                CleanUpConnection(tearDown, apnContext);
            }
        } else {
            // TODO - only do cleanup if not disconnected
            apnContext->SetReason(reason);
            CleanUpConnection(tearDown, apnContext);
        }
    }
    StopNetStatPoll();
    StopDataStallAlarm();
    // TODO: Do we need mRequestedApnType?
    mRequestedApnType = IPhoneConstants::APN_TYPE_DEFAULT;
    Log("cleanUpConnection: mDisconnectPendingCount = %d", mDisconnectPendingCount);
    if (tearDown && mDisconnectPendingCount == 0) {
        NotifyDataDisconnectComplete();
        NotifyAllDataDisconnected();
    }
    *result = didDisconnect;
    return NOERROR;
}

ECode DcTracker::OnCleanUpAllConnections(
    /* [in] */ const String& cause)
{
    Boolean b;
    CleanUpAllConnections(TRUE, cause, &b);
    return NOERROR;
}

ECode DcTracker::CleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ IApnContext* apnContext)
{
    if (apnContext == NULL) {
        if (DBG) Log("cleanUpConnection: apn context is null");
        return NOERROR;
    }
    AutoPtr<IDcAsyncChannel> dcac;
    apnContext->GetDcAc((IDcAsyncChannel**)&dcac);
    if (DBG) {
        String reason;
        apnContext->GetReason(&reason);
        Log("cleanUpConnection: E tearDown=%d reason=%s apnContext=%s", tearDown, reason.string(), TO_CSTR(apnContext));
    }
    if (tearDown) {
        Boolean isDisconnected;
        apnContext->IsDisconnected(&isDisconnected);
        if (isDisconnected) {
            // The request is tearDown and but ApnContext is not connected.
            // If apnContext is not enabled anymore, break the linkage to the DCAC/DC.
            apnContext->SetState(DctConstantsState_IDLE);
            Boolean isReady;
            apnContext->IsReady(&isReady);
            if (!isReady) {
                if (dcac != NULL) {
                    dcac->TearDown(apnContext, String(""), NULL);
                }
                apnContext->SetDataConnectionAc(NULL);
            }
        } else {
            // Connection is still there. Try to clean up.
            if (dcac != NULL) {
                DctConstantsState state;
                apnContext->GetState(&state);
                if (state != DctConstantsState_DISCONNECTING) {
                    Boolean disconnectAll = FALSE;
                    String apnType;
                    apnContext->GetApnType(&apnType);
                    if (IPhoneConstants::APN_TYPE_DUN.Equals(apnType)) {
                        // CAF_MSIM is this below condition required.
                        // if (PhoneConstants.APN_TYPE_DUN.equals(PhoneConstants.APN_TYPE_DEFAULT)) {
                        Boolean isTeardownForDun;
                        TeardownForDun(&isTeardownForDun);
                        if (isTeardownForDun) {
                            if (DBG) Log("tearing down dedicated DUN connection");
                            // we need to tear it down - we brought it up just for dun and
                            // other people are camped on it and now dun is done.  We need
                            // to stop using it and let the normal apn list get used to find
                            // connections for the remaining desired connections
                            disconnectAll = TRUE;
                        }
                    }
                    if (DBG) {
                        Log("cleanUpConnection: tearing down%s", (disconnectAll ? " all" :""));
                    }
                    AutoPtr<IMessage> msg;
                    ObtainMessage(IDctConstants::EVENT_DISCONNECT_DONE, apnContext, (IMessage**)&msg);
                    if (disconnectAll) {
                        String reason;
                        apnContext->GetReason(&reason);
                        AutoPtr<IDcAsyncChannel> dcAsyncChannel;
                        apnContext->GetDcAc((IDcAsyncChannel**)&dcAsyncChannel);
                        dcAsyncChannel->TearDownAll(reason, msg);
                    } else {
                        String reason;
                        apnContext->GetReason(&reason);
                        AutoPtr<IDcAsyncChannel> dcAsyncChannel;
                        apnContext->GetDcAc((IDcAsyncChannel**)&dcAsyncChannel);
                        dcAsyncChannel->TearDown(apnContext, reason, msg);
                    }
                    apnContext->SetState(DctConstantsState_DISCONNECTING);
                    mDisconnectPendingCount++;
                }
            } else {
                // apn is connected but no reference to dcac.
                // Should not be happen, but reset the state in case.
                apnContext->SetState(DctConstantsState_IDLE);
                String apnType;
                apnContext->GetApnType(&apnType);
                String reason;
                apnContext->GetReason(&reason);
                mPhone->NotifyDataConnection(reason, apnType);
            }
        }
    } else {
        // force clean up the data connection.
        if (dcac != NULL) dcac->ReqReset();
        apnContext->SetState(DctConstantsState_IDLE);
        String apnType;
        apnContext->GetApnType(&apnType);
        String reason;
        apnContext->GetReason(&reason);
        mPhone->NotifyDataConnection(reason, apnType);
        apnContext->SetDataConnectionAc(NULL);
    }
    if (mOmhApt != NULL) {
        mOmhApt->ClearActiveApnProfile();
    }
    // Make sure reconnection alarm is cleaned up if there is no ApnContext
    // associated to the connection.
    if (dcac != NULL) {
        CancelReconnectAlarm(apnContext);
    }
    String reason;
    apnContext->GetReason(&reason);
    SetupDataForSinglePdnArbitration(reason);
    if (DBG) {
        AutoPtr<IDcAsyncChannel> dcAsyncChannel;
        apnContext->GetDcAc((IDcAsyncChannel**)&dcAsyncChannel);
        Log("cleanUpConnection: X tearDown=%d reason=%s apnContext=%s dcac=%s", tearDown, reason.string(), TO_CSTR(apnContext), TO_CSTR(dcAsyncChannel));
    }
    return NOERROR;
}

ECode DcTracker::SetupDataForSinglePdnArbitration(
    /* [in] */ const String& reason)
{
    // In single pdn case, if a higher priority call which was scheduled for retry gets
    // cleaned up due to say apn disabled, we need to try setup data on connectable apns
    // as there won't be any EVENT_DISCONNECT_DONE call back.
    if (DBG) {
        Boolean isDisconnected;
        IsDisconnected(&isDisconnected);
        Log("setupDataForSinglePdn: reason = %s isDisconnected = %d", reason.string(), isDisconnected);
    }
    Boolean isDisconnected;
    IsDisconnected(&isDisconnected);
    Boolean isOnlySingleDcAllowed;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilDataRadioTechnology;
    serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    IsOnlySingleDcAllowed(rilDataRadioTechnology, &isOnlySingleDcAllowed);
    if (isOnlySingleDcAllowed && isDisconnected
            && !IPhone::REASON_SINGLE_PDN_ARBITRATION.Equals(reason)) {
        SetupDataOnConnectableApns(IPhone::REASON_SINGLE_PDN_ARBITRATION);
    }
    return NOERROR;
}

ECode DcTracker::TeardownForDun(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // CDMA always needs to do this the profile id is correct
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilRat;
    serviceState->GetRilDataRadioTechnology(&rilRat);
    Boolean isCdma;
    CServiceState::IsCdma(rilRat, &isCdma);
    if (isCdma) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IApnSetting> dunApn;
    FetchDunApn((IApnSetting**)&dunApn);
    *result = (dunApn != NULL);
    return NOERROR;
}

ECode DcTracker::CancelReconnectAlarm(
    /* [in] */ IApnContext* apnContext)
{
    if (apnContext == NULL) return NOERROR;
    AutoPtr<IPendingIntent> intent;
    apnContext->GetReconnectIntent((IPendingIntent**)&intent);
    if (intent != NULL) {
            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
            AutoPtr<IAlarmManager> am = IAlarmManager::Probe(obj);
            am->Cancel(intent);
            apnContext->SetReconnectIntent(NULL);
    }
    return NOERROR;
}

ECode DcTracker::ParseTypes(
    /* [in] */ const String& types,
    /* [out, callee] */ ArrayOf<String>** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<ArrayOf<String> > result;
    // If unset, set to DEFAULT.
    if (types == NULL || types.Equals("")) {
        result = ArrayOf<String>::Alloc(1);
        (*result)[0] = IPhoneConstants::APN_TYPE_ALL;
    } else {
        StringUtils::Split(types, ",", (ArrayOf<String>**)&result);
    }
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DcTracker::ImsiMatches(
    /* [in] */ const String& imsiDB,
    /* [in] */ const String& imsiSIM,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Note: imsiDB value has digit number or 'x' character for seperating USIM information
    // for MVNO operator. And then digit number is matched at same order and 'x' character
    // could replace by any digit number.
    // ex) if imsiDB inserted '310260x10xxxxxx' for GG Operator,
    //     that means first 6 digits, 8th and 9th digit
    //     should be set in USIM for GG Operator.
    Int32 len = imsiDB.GetLength();
    // Int32 idxCompare = 0;
    if (len <= 0) {
        *result = FALSE;
        return NOERROR;
    }
    if (len > imsiSIM.GetLength()) {
        *result = FALSE;
        return NOERROR;
    }
    for (Int32 idx=0; idx<len; idx++) {
        Char32 c = imsiDB.GetChar(idx);
        if ((c == 'x') || (c == 'X') || (c == imsiSIM.GetChar(idx))) {
            continue;
        } else {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::MvnoMatches(
    /* [in] */ IIccRecords* r,
    /* [in] */ const String& mvnoType,
    /* [in] */ const String& mvnoMatchData,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mvnoType.EqualsIgnoreCase("spn")) {
        String serviceProviderName;
        r->GetServiceProviderName(&serviceProviderName);
        if ((!serviceProviderName.IsNull()) &&
                serviceProviderName.EqualsIgnoreCase(mvnoMatchData)) {
            *result = TRUE;
            return NOERROR;
        }
    } else if (mvnoType.EqualsIgnoreCase("imsi")) {
        String imsiSIM;
        r->GetIMSI(&imsiSIM);
        Boolean isImsiMatches;
        ImsiMatches(mvnoMatchData, imsiSIM, &isImsiMatches);
        if ((imsiSIM != NULL) && isImsiMatches) {
            *result = TRUE;
            return NOERROR;
        }
    } else if (mvnoType.EqualsIgnoreCase("gid")) {
        String gid1;
        r->GetGid1(&gid1);
        Int32 mvno_match_data_length = mvnoMatchData.GetLength();
        if ((gid1 != NULL) && (gid1.GetLength() >= mvno_match_data_length) &&
                gid1.Substring(0, mvno_match_data_length).EqualsIgnoreCase(mvnoMatchData)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::IsPermanentFail(
    /* [in] */ IDcFailCause* dcFailCause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isPermanentFail;
    dcFailCause->IsPermanentFail(&isPermanentFail);
    Boolean b;
    mAttached->Get(&b);
    Int32 errCode;
    dcFailCause->GetErrorCode(&errCode);
    *result = (isPermanentFail && (b == FALSE || errCode != DcFailCause_SIGNAL_LOST));
    return NOERROR;
}

ECode DcTracker::MakeApnSetting(
    /* [in] */ ICursor* cursor,
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 columnIndexOrThrowTYPE;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::TYPE, &columnIndexOrThrowTYPE);
    String strColumnIndexOrThrowTYPE;
    cursor->GetString(columnIndexOrThrowTYPE, &strColumnIndexOrThrowTYPE);
    AutoPtr<ArrayOf<String> > types;
    ParseTypes(strColumnIndexOrThrowTYPE, (ArrayOf<String>**)&types);
    Int32 columnIndexOrThrow_ID;
    cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &columnIndexOrThrow_ID);
    Int32 columnIndexOrThrowNUMERIC;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::NUMERIC, &columnIndexOrThrowNUMERIC);
    Int32 columnIndexOrThrowNAME;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::NAME, &columnIndexOrThrowNAME);
    Int32 columnIndexOrThrowAPN;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::APN, &columnIndexOrThrowAPN);
    Int32 columnIndexOrThrowPROXY;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::PROXY, &columnIndexOrThrowPROXY);
    Int32 columnIndexOrThrowPORT;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::PORT, &columnIndexOrThrowPORT);
    Int32 columnIndexOrThrowMMSC;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MMSC, &columnIndexOrThrowMMSC);
    Int32 columnIndexOrThrowMMSPROXY;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MMSPROXY, &columnIndexOrThrowMMSPROXY);
    Int32 columnIndexOrThrowMMSPORT;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MMSPORT, &columnIndexOrThrowMMSPORT);
    Int32 columnIndexOrThrowUSER;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::USER, &columnIndexOrThrowUSER);
    Int32 columnIndexOrThrowPASSWORD;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::PASSWORD, &columnIndexOrThrowPASSWORD);
    Int32 columnIndexOrThrowAUTH_TYPE;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::AUTH_TYPE, &columnIndexOrThrowAUTH_TYPE);
    Int32 columnIndexOrThrowPROTOCOL;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::PROTOCOL, &columnIndexOrThrowPROTOCOL);
    Int32 columnIndexOrThrowBEARER;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::BEARER, &columnIndexOrThrowBEARER);
    Int32 columnIndexOrThrowPROFILE_ID;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::PROFILE_ID, &columnIndexOrThrowPROFILE_ID);
    Int32 columnIndexOrThrowMAX_CONNS;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MAX_CONNS, &columnIndexOrThrowMAX_CONNS);
    Int32 columnIndexOrThrowMAX_CONNS_TIME;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MAX_CONNS_TIME, &columnIndexOrThrowMAX_CONNS_TIME);
    Int32 columnIndexOrThrowMTU;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MTU, &columnIndexOrThrowMTU);
    Int32 columnIndexOrThrowMVNO_TYPE;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MVNO_TYPE, &columnIndexOrThrowMVNO_TYPE);
    Int32 columnIndexOrThrowMVNO_MATCH_DATA;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MVNO_MATCH_DATA, &columnIndexOrThrowMVNO_MATCH_DATA);
    Int32 iColumnIndexOrThrow_ID;
    cursor->GetInt32(columnIndexOrThrow_ID, &iColumnIndexOrThrow_ID);
    String strColumnIndexOrThrowNUMERIC;
    cursor->GetString(columnIndexOrThrowNUMERIC, &strColumnIndexOrThrowNUMERIC);
    String strColumnIndexOrThrowNAME;
    cursor->GetString(columnIndexOrThrowNAME, &strColumnIndexOrThrowNAME);
    String strColumnIndexOrThrowAPN;
    cursor->GetString(columnIndexOrThrowAPN, &strColumnIndexOrThrowAPN);
    String strColumnIndexOrThrowPORT;
    cursor->GetString(columnIndexOrThrowPORT, &strColumnIndexOrThrowPORT);
    String strColumnIndexOrThrowMMSPORT;
    cursor->GetString(columnIndexOrThrowMMSPORT, &strColumnIndexOrThrowMMSPORT);
    String strColumnIndexOrThrowUSER;
    cursor->GetString(columnIndexOrThrowUSER, &strColumnIndexOrThrowUSER);
    String strColumnIndexOrThrowPASSWORD;
    cursor->GetString(columnIndexOrThrowPASSWORD, &strColumnIndexOrThrowPASSWORD);
    Int32 iColumnIndexOrThrowAUTH_TYPE;
    cursor->GetInt32(columnIndexOrThrowAUTH_TYPE, &iColumnIndexOrThrowAUTH_TYPE);
    String strColumnIndexOrThrowPROTOCOL;
    cursor->GetString(columnIndexOrThrowPROTOCOL, &strColumnIndexOrThrowPROTOCOL);
    Int32 iColumnIndexOrThrowBEARER;
    cursor->GetInt32(columnIndexOrThrowBEARER, &iColumnIndexOrThrowBEARER);
    Int32 iColumnIndexOrThrowPROFILE_ID;
    cursor->GetInt32(columnIndexOrThrowPROFILE_ID, &iColumnIndexOrThrowPROFILE_ID);
    Int32 iColumnIndexOrThrowMAX_CONNS;
    cursor->GetInt32(columnIndexOrThrowMAX_CONNS, &iColumnIndexOrThrowMAX_CONNS);
    Int32 iColumnIndexOrThrowMAX_CONNS_TIME;
    cursor->GetInt32(columnIndexOrThrowMAX_CONNS_TIME, &iColumnIndexOrThrowMAX_CONNS_TIME);
    Int32 iColumnIndexOrThrowMTU;
    cursor->GetInt32(columnIndexOrThrowMTU, &iColumnIndexOrThrowMTU);
    String strColumnIndexOrThrowMVNO_TYPE;
    cursor->GetString(columnIndexOrThrowMVNO_TYPE, &strColumnIndexOrThrowMVNO_TYPE);
    String strColumnIndexOrThrowMVNO_MATCH_DATA;
    cursor->GetString(columnIndexOrThrowMVNO_MATCH_DATA, &strColumnIndexOrThrowMVNO_MATCH_DATA);
    String strColumnIndexOrThrowPROXY;
    cursor->GetString(columnIndexOrThrowPROXY, &strColumnIndexOrThrowPROXY);
    String strColumnIndexOrThrowMMSC;
    cursor->GetString(columnIndexOrThrowMMSC, &strColumnIndexOrThrowMMSC);
    String strColumnIndexOrThrowMMSPROXY;
    cursor->GetString(columnIndexOrThrowMMSPROXY, &strColumnIndexOrThrowMMSPROXY);
    Int32 columnIndexOrThrowROAMING_PROTOCOL;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::ROAMING_PROTOCOL, &columnIndexOrThrowROAMING_PROTOCOL);
    Int32 columnIndexOrThrowCARRIER_ENABLED;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::CARRIER_ENABLED, &columnIndexOrThrowCARRIER_ENABLED);
    Int32 columnIndexOrThrowMODEM_COGNITIVE;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::MODEM_COGNITIVE, &columnIndexOrThrowMODEM_COGNITIVE);
    Int32 columnIndexOrThrowWAIT_TIME;
    cursor->GetColumnIndexOrThrow(ITelephonyCarriers::WAIT_TIME, &columnIndexOrThrowWAIT_TIME);
    String strColumnIndexOrThrowROAMING_PROTOCOL;
    cursor->GetString(columnIndexOrThrowROAMING_PROTOCOL, &strColumnIndexOrThrowROAMING_PROTOCOL);
    Int32 iColumnIndexOrThrowCARRIER_ENABLED;
    cursor->GetInt32(columnIndexOrThrowCARRIER_ENABLED, &iColumnIndexOrThrowCARRIER_ENABLED);
    Int32 iColumnIndexOrThrowMODEM_COGNITIVE;
    cursor->GetInt32(columnIndexOrThrowMODEM_COGNITIVE, &iColumnIndexOrThrowMODEM_COGNITIVE);
    Int32 iColumnIndexOrThrowWAIT_TIME;
    cursor->GetInt32(columnIndexOrThrowWAIT_TIME, &iColumnIndexOrThrowWAIT_TIME);
    String strTrimColumnIndexOrThrowPROXY;
    NetworkUtils::TrimV4AddrZeros(strColumnIndexOrThrowPROXY, &strTrimColumnIndexOrThrowPROXY);
    String strTrimColumnIndexOrThrowMMSC;
    NetworkUtils::TrimV4AddrZeros(strColumnIndexOrThrowMMSC, &strTrimColumnIndexOrThrowMMSC);
    String strTrimColumnIndexOrThrowMMSPROXY;
    NetworkUtils::TrimV4AddrZeros(strColumnIndexOrThrowMMSPROXY, &strTrimColumnIndexOrThrowMMSPROXY);
    AutoPtr<IApnSetting> apn;
    CApnSetting::New(
            iColumnIndexOrThrow_ID,
            strColumnIndexOrThrowNUMERIC,
            strColumnIndexOrThrowNAME,
            strColumnIndexOrThrowAPN,
            strTrimColumnIndexOrThrowPROXY,
            strColumnIndexOrThrowPORT,
            strTrimColumnIndexOrThrowMMSC,
            strTrimColumnIndexOrThrowMMSPROXY,
            strColumnIndexOrThrowMMSPORT,
            strColumnIndexOrThrowUSER,
            strColumnIndexOrThrowPASSWORD,
            iColumnIndexOrThrowAUTH_TYPE,
            types,
            strColumnIndexOrThrowPROTOCOL,
            strColumnIndexOrThrowROAMING_PROTOCOL,
            iColumnIndexOrThrowCARRIER_ENABLED == 1,
            iColumnIndexOrThrowBEARER,
            iColumnIndexOrThrowPROFILE_ID,
            iColumnIndexOrThrowMODEM_COGNITIVE == 1,
            iColumnIndexOrThrowMAX_CONNS,
            iColumnIndexOrThrowWAIT_TIME,
            iColumnIndexOrThrowMAX_CONNS_TIME,
            iColumnIndexOrThrowMTU,
            strColumnIndexOrThrowMVNO_TYPE,
            strColumnIndexOrThrowMVNO_MATCH_DATA,
            (IApnSetting**)&apn);
    *result = apn;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTracker::CreateApnList(
    /* [in] */ ICursor* cursor,
    /* [out] */ IArrayList** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<IArrayList> mnoApns;
    CArrayList::New((IArrayList**)&mnoApns);
    AutoPtr<IArrayList> mvnoApns;
    CArrayList::New((IArrayList**)&mvnoApns);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    Boolean isMoveToFirst;
    cursor->MoveToFirst(&isMoveToFirst);
    if (isMoveToFirst) {
        Boolean isMoveToNextOk;
        do {
            AutoPtr<IApnSetting> apn;
            MakeApnSetting(cursor, (IApnSetting**)&apn);
            if (apn == NULL) {
                continue;
            }
            Boolean hasMvnoParams;
            apn->HasMvnoParams(&hasMvnoParams);
            if (hasMvnoParams) {
                Boolean isMvnoMatches;
                String mvnoType;
                apn->GetMvnoType(&mvnoType);
                String mvnoMatchData;
                apn->GetMvnoMatchData(&mvnoMatchData);
                MvnoMatches(r, mvnoType, mvnoMatchData, &isMvnoMatches);
                if (r != NULL && isMvnoMatches) {
                    mvnoApns->Add(apn);
                }
            } else {
                mnoApns->Add(apn);
            }
        } while (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk);
    }
    Boolean isEmpty;
    mvnoApns->IsEmpty(&isEmpty);
    AutoPtr<IArrayList> result = isEmpty ? mnoApns : mvnoApns;
    if (DBG) Log("createApnList: X result=%s", TO_CSTR(result));
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DcTracker::DataConnectionNotInUse(
    /* [in] */ IDcAsyncChannel* dcac,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("dataConnectionNotInUse: check if dcac is inuse dcac=%s", TO_CSTR(dcac));
    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        AutoPtr<IDcAsyncChannel> dcAsyncChannel;
        apnContext->GetDcAc((IDcAsyncChannel**)&dcAsyncChannel);
        if (dcAsyncChannel.Get() == dcac) {
            if (DBG) Log("dataConnectionNotInUse: in use by apnContext=%s", TO_CSTR(apnContext));
            *result = FALSE;
            return NOERROR;
        }
    }
    // TODO: Fix retry handling so free DataConnections have empty apnlists.
    // Probably move retry handling into DataConnections and reduce complexity
    // of DCT.
    if (DBG) Log("dataConnectionNotInUse: tearDownAll");
    dcac->TearDownAll(String("No connection"), NULL);
    if (DBG) Log("dataConnectionNotInUse: not in use return true");
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::FindFreeDataConnection(
    /* [out] */ IDcAsyncChannel** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollection> values;
    mDataConnectionAcHashMap->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(obj);
        Boolean isInactiveSync;
        dcac->IsInactiveSync(&isInactiveSync);
        Boolean isDataConnectionNotInUse;
        DataConnectionNotInUse(dcac, &isDataConnectionNotInUse);
        if (isInactiveSync && isDataConnectionNotInUse) {
            if (DBG) {
                Log("findFreeDataConnection: found free DataConnection="
                    " dcac=%s", TO_CSTR(dcac));
            }
            *result = dcac;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    Log("findFreeDataConnection: NO free DataConnection");
    *result = NULL;
    return NOERROR;
}

ECode DcTracker::SetupData(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Int32 radioTech,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("setupData: apnContext=%s", TO_CSTR(apnContext));
    AutoPtr<IApnSetting> apnSetting;
    AutoPtr<IDcAsyncChannel> dcac;
    apnContext->GetNextWaitingApn((IApnSetting**)&apnSetting);
    if (apnSetting == NULL) {
        if (DBG) Log("setupData: return for no apn found!");
        *result = FALSE;
        return NOERROR;
    }
    Int32 profileId;
    apnSetting->GetProfileId(&profileId);
    if (profileId == 0) {
        String apnType;
        apnContext->GetApnType(&apnType);
        GetApnProfileID(apnType, &profileId);
    }
    // On CDMA, if we're explicitly asking for DUN, we need have
    // a dun-profiled connection so we can't share an existing one
    // On GSM/LTE we can share existing apn connections provided they support
    // this type.
    String apnType;
    apnContext->GetApnType(&apnType);
    Boolean isTeardownForDun;
    TeardownForDun(&isTeardownForDun);
    if (apnType != IPhoneConstants::APN_TYPE_DUN ||
            isTeardownForDun == FALSE) {
        CheckForCompatibleConnectedApnContext(apnContext, (IDcAsyncChannel**)&dcac);
        if (dcac != NULL) {
            // Get the dcacApnSetting for the connection we want to share.
            AutoPtr<IApnSetting> dcacApnSetting;
            dcac->GetApnSettingSync((IApnSetting**)&dcacApnSetting);
            if (dcacApnSetting != NULL) {
                // Setting is good, so use it.
                apnSetting = dcacApnSetting;
            }
        }
    }
    if (dcac == NULL) {
        Boolean isOnlySingleDcAllowed;
        IsOnlySingleDcAllowed(radioTech, &isOnlySingleDcAllowed);
        if (isOnlySingleDcAllowed) {
            Boolean isHigherPriorityApnContextActive;
            IsHigherPriorityApnContextActive(apnContext, &isHigherPriorityApnContextActive);
            if (isHigherPriorityApnContextActive) {
                if (DBG) {
                    Log("setupData: Higher priority ApnContext active.  Ignoring call");
                }
                *result = FALSE;
                return NOERROR;
            }
            // Only lower priority calls left.  Disconnect them all in this single PDP case
            // so that we can bring up the requested higher priority call (once we receive
            // repsonse for deactivate request for the calls we are about to disconnect
            Boolean isCleanUpAllConnectionsOk;
            CleanUpAllConnections(TRUE, IPhone::REASON_SINGLE_PDN_ARBITRATION, &isCleanUpAllConnectionsOk);
            if (isCleanUpAllConnectionsOk) {
                // If any call actually requested to be disconnected, means we can't
                // bring up this connection yet as we need to wait for those data calls
                // to be disconnected.
                if (DBG) Log("setupData: Some calls are disconnecting first.  Wait and retry");
                *result = FALSE;
                return NOERROR;
            }
            // No other calls are active, so proceed
            if (DBG) Log("setupData: Single pdp. Continue setting up data call.");
        }
        FindFreeDataConnection((IDcAsyncChannel**)&dcac);
        if (dcac == NULL) {
            CreateDataConnection((IDcAsyncChannel**)&dcac);
        }
        if (dcac == NULL) {
            if (DBG) Log("setupData: No free DataConnection and couldn't create one, WEIRD");
            *result = FALSE;
            return NOERROR;
        }
    }
    if (DBG) Log("setupData: dcac=%s apnSetting=%s", TO_CSTR(dcac), TO_CSTR(apnSetting));
    apnContext->SetDataConnectionAc(dcac);
    apnContext->SetApnSetting(apnSetting);
    apnContext->SetState(DctConstantsState_CONNECTING);
    String reason;
    apnContext->GetReason(&reason);
    mPhone->NotifyDataConnection(reason, apnType);
    AutoPtr<IMessage> msg;
    ObtainMessage((IMessage**)&msg);
    msg->SetWhat(IDctConstants::EVENT_DATA_SETUP_COMPLETE);
    msg->SetObj(apnContext);
    Int32 initialMaxRetry;
    GetInitialMaxRetry(&initialMaxRetry);
    dcac->BringUp(apnContext, initialMaxRetry, profileId, radioTech, mAutoAttachOnCreation, msg);
    if (DBG) Log("setupData: initing!");
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::OnApnChanged()
{
    if (DBG) Log("onApnChanged: tryRestartDataConnections");
    TryRestartDataConnections(TRUE, IPhone::REASON_APN_CHANGED);
    return NOERROR;
}

ECode DcTracker::TryRestartDataConnections(
    /* [in] */ Boolean isCleanupNeeded,
    /* [in] */ const String& reason)
{
    DctConstantsState overallState;
    GetOverallState(&overallState);
    Boolean isDisconnected = (overallState == DctConstantsState_IDLE ||
            overallState == DctConstantsState_FAILED);
    if (IGSMPhone::Probe(mPhone) != NULL) {
        // The "current" may no longer be valid.  MMS depends on this to send properly. TBD
        Boolean b;
        IGSMPhone::Probe(mPhone)->UpdateCurrentCarrierInProvider(&b);
    }
    // TODO: It'd be nice to only do this if the changed entrie(s)
    // match the current operator.
    if (DBG) Log("tryRestartDataConnections: createAllApnList and cleanUpAllConnections");
    CreateAllApnList();
    SetInitialAttachApn();
    if (isCleanupNeeded) {
        Boolean b;
        CleanUpAllConnections(!isDisconnected, reason, &b);
    }
    // If the state is already connected don't setup data now.
    if (isDisconnected) {
        SetupDataOnConnectableApns(reason);
    }
    return NOERROR;
}

ECode DcTracker::OnWwanIwlanCoexistenceDone(
    /* [in] */ IAsyncResult* ar)
{
    if (((AsyncResult*) ar)->mException != NULL) {
        Log("onWwanIwlanCoexistenceDone: error = %s", TO_CSTR(((AsyncResult*) ar)->mException));
    } else {
        AutoPtr<IArrayOf> iArray = IArrayOf::Probe(((AsyncResult*) ar)->mResult);
        Int32 size;
        iArray->GetLength(&size);
        AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            iArray->Get(i, (IInterface**)&obj);
            Byte b;
            IByte::Probe(obj)->GetValue(&b);
            (*array)[i] = b;
        }
        Log("onWwanIwlanCoexistenceDone, payload hexdump = "
                , IccUtils::BytesToHexString(array).string());
        AutoPtr<IByteBufferHelper> helper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
        AutoPtr<IByteBuffer> oemHookResponse;
        helper->Wrap(array, (IByteBuffer**)&oemHookResponse);
        AutoPtr<IByteOrderHelper> byteOrderHelper;
        CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&byteOrderHelper);
        ByteOrder order;
        byteOrderHelper->GetNativeOrder(&order);
        oemHookResponse->SetOrder(order);
        Byte resp;
        oemHookResponse->Get(&resp);
        Log("onWwanIwlanCoexistenceDone: resp = %d", resp);
        Boolean tempStatus = (resp > 0)? TRUE : FALSE;
        if (mWwanIwlanCoexistFlag == tempStatus) {
            Log("onWwanIwlanCoexistenceDone: no change in status, ignore.");
            return NOERROR;
        }
        mWwanIwlanCoexistFlag = tempStatus;
        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Int32 rilDataRadioTechnology;
        serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        if (rilDataRadioTechnology
                == IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN) {
            Log("notifyDataConnection IWLAN_AVAILABLE");
            NotifyDataConnection(IPhone::REASON_IWLAN_AVAILABLE);
        }
    }
    return NOERROR;
}

ECode DcTracker::OnModemApnProfileReady()
{
    if (mState == DctConstantsState_FAILED) {
        Boolean b;
        CleanUpAllConnections(FALSE, IPhone::REASON_PS_RESTRICT_ENABLED, &b);
    }
    if (DBG) Log("OMH: onModemApnProfileReady(): Setting up data call");
    TryRestartDataConnections(FALSE, IPhone::REASON_SIM_LOADED);
    return NOERROR;
}

ECode DcTracker::FindDataConnectionAcByCid(
    /* [in] */ Int32 cid,
    /* [out] */ IDcAsyncChannel** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollection> values;
    mDataConnectionAcHashMap->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(obj);
        Int32 cidSync;
        dcac->GetCidSync(&cidSync);
        if (cidSync == cid) {
            *result = dcac;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode DcTracker::GotoIdleAndNotifyDataConnection(
    /* [in] */ const String& reason)
{
    if (DBG) Log("gotoIdleAndNotifyDataConnection: reason=%s", reason.string());
    NotifyDataConnection(reason);
    mActiveApn = NULL;
    return NOERROR;
}

ECode DcTracker::IsHigherPriorityApnContextActive(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    mPrioritySortedApnContexts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> otherContext = IApnContext::Probe(obj);
        String otherContextApnType;
        otherContext->GetApnType(&otherContextApnType);
        String apnContextApnType;
        apnContext->GetApnType(&apnContextApnType);
        if (apnContextApnType.EqualsIgnoreCase(otherContextApnType)) {
            *result = FALSE;
            return NOERROR;
        }
        Boolean isEnabled;
        otherContext->IsEnabled(&isEnabled);
        DctConstantsState state;
        otherContext->GetState(&state);
        if (isEnabled && state != DctConstantsState_FAILED) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::IsOnlySingleDcAllowed(
    /* [in] */ Int32 rilRadioTech,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<Int32> > singleDcRats;
    res->GetInt32Array(
            R::array::config_onlySingleDcAllowed, (ArrayOf<Int32>**)&singleDcRats);
    Boolean onlySingleDcAllowed = FALSE;
    Boolean b;
    SystemProperties::GetBoolean(String("persist.telephony.test.singleDc"), FALSE, &b);
    if (Build::IS_DEBUGGABLE && b) {
        onlySingleDcAllowed = TRUE;
    }
    if (singleDcRats != NULL) {
        for (Int32 i=0; i < singleDcRats->GetLength() && onlySingleDcAllowed == FALSE; i++) {
            if (rilRadioTech == (*singleDcRats)[i]) onlySingleDcAllowed = TRUE;
        }
    }
    if (DBG) Log("isOnlySingleDcAllowed(%d): %d", rilRadioTech, onlySingleDcAllowed);
    *result = onlySingleDcAllowed;
    return NOERROR;
}

ECode DcTracker::RestartRadio()
{
    if (DBG) Log("restartRadio: ************TURN OFF RADIO**************");
    Boolean b;
    CleanUpAllConnections(TRUE, IPhone::REASON_RADIO_TURNED_OFF, &b);
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    serviceStateTracker->PowerOffRadioSafely(this);
    /* Note: no need to call setRadioPower(true).  Assuming the desired
     * radio power state is still ON (as tracked by ServiceStateTracker),
     * ServiceStateTracker will call setRadioPower when it receives the
     * RADIO_STATE_CHANGED notification for the power off.  And if the
     * desired power state has changed in the interim, we don't want to
     * override it with an unconditional power on.
     */
    String s;
    SystemProperties::Get(String("net.ppp.reset-by-timeout"), String("0"), &s);
    Int32 reset = StringUtils::ParseInt32(s);
    SystemProperties::Set(String("net.ppp.reset-by-timeout"), StringUtils::ToString(reset+1));
    return NOERROR;
}

ECode DcTracker::RetryAfterDisconnected(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean retry = TRUE;
    String reason;
    apnContext->GetReason(&reason);
    Boolean isOnlySingleDcAllowed;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilDataRadioTechnology;
    serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    IsOnlySingleDcAllowed(rilDataRadioTechnology, &isOnlySingleDcAllowed);
    Boolean isHigherPriorityApnContextActive;
    IsHigherPriorityApnContextActive(apnContext, &isHigherPriorityApnContextActive);
    if (IPhone::REASON_RADIO_TURNED_OFF.Equals(reason) ||
            (isOnlySingleDcAllowed && isHigherPriorityApnContextActive)) {
        retry = FALSE;
    }
    *result = retry;
    return NOERROR;
}

ECode DcTracker::StartAlarmForReconnect(
    /* [in] */ Int32 delay,
    /* [in] */ IApnContext* apnContext)
{
    String apnType;
    apnContext->GetApnType(&apnType);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    String reason;
    apnContext->GetReason(&reason);
    intent->PutExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON, reason);
    intent->PutExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE, apnType);
    // Get current sub id.
    Int64 subId;
    SubscriptionManager::GetDefaultDataSubId(&subId);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    if (DBG) {
        String action;
        intent->GetAction(&action);
        Log("startAlarmForReconnect: delay=%d action=%s apn=%s", delay, action.string(), TO_CSTR(apnContext));
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IPendingIntent> alarmIntent;
    CPendingIntent::GetBroadcast(context, 0,
            intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&alarmIntent);
    apnContext->SetReconnectIntent(alarmIntent);
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
            SystemClock::GetElapsedRealtime() + delay, alarmIntent);
    return NOERROR;
}

ECode DcTracker::StartAlarmForRestartTrySetup(
    /* [in] */ Int32 delay,
    /* [in] */ IApnContext* apnContext)
{
    String apnType;
    apnContext->GetApnType(&apnType);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE, apnType);
    if (DBG) {
        String action;
        intent->GetAction(&action);
        Log("startAlarmForRestartTrySetup: delay=%d action=%s apn=%s", delay, action.string(), TO_CSTR(apnContext));
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IPendingIntent> alarmIntent;
    CPendingIntent::GetBroadcast(context, 0,
            intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&alarmIntent);
    apnContext->SetReconnectIntent(alarmIntent);
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
            SystemClock::GetElapsedRealtime() + delay, alarmIntent);
    return NOERROR;
}

ECode DcTracker::NotifyNoData(
    /* [in] */ IDcFailCause* lastFailCauseCode,
    /* [in] */ IApnContext* apnContext)
{
    String apnContextApnType;
    apnContext->GetApnType(&apnContextApnType);
    if (DBG) Log( "notifyNoData: type=%s", apnContextApnType.string());
    Boolean isPermanentFail;
    IsPermanentFail(lastFailCauseCode, &isPermanentFail);
    if (isPermanentFail
        && (!apnContextApnType.Equals(IPhoneConstants::APN_TYPE_DEFAULT))) {
        String reason;
        apnContext->GetReason(&reason);
        mPhone->NotifyDataConnectionFailed(reason, apnContextApnType);
    }
    return NOERROR;
}

ECode DcTracker::OnRecordsLoaded()
{
    if (mOmhApt != NULL) {
        Log("OMH: onRecordsLoaded(): calling loadProfiles()");
        /* query for data profiles stored in the modem */
        ((CdmaApnProfileTracker*) mOmhApt.Get())->LoadProfiles();
        ICommandsInterfaceRadioState commandsInterfaceRadioState;
        ((PhoneBase*) mPhone.Get())->mCi->GetRadioState(&commandsInterfaceRadioState);
        Boolean isOn = commandsInterfaceRadioState == Elastos::Droid::Internal::Telephony::RADIO_ON;
        if (isOn) {
            if (DBG) Log("onRecordsLoaded: notifying data availability");
            NotifyOffApnsOfAvailability(IPhone::REASON_SIM_LOADED);
        }
    } else {
        if (DBG) Log("onRecordsLoaded: createAllApnList");
        ICommandsInterfaceRadioState commandsInterfaceRadioState;
        ((PhoneBase*) mPhone.Get())->mCi->GetRadioState(&commandsInterfaceRadioState);
        Boolean isOn = commandsInterfaceRadioState == Elastos::Droid::Internal::Telephony::RADIO_ON;
        if (isOn) {
            if (DBG) Log("onRecordsLoaded: notifying data availability");
            NotifyOffApnsOfAvailability(IPhone::REASON_SIM_LOADED);
        }
        TryRestartDataConnections(FALSE, IPhone::REASON_SIM_LOADED);
   }
    return NOERROR;
}

ECode DcTracker::OnNvReady()
{
    if (DBG) Log("onNvReady");
    CreateAllApnList();
    SetupDataOnConnectableApns(IPhone::REASON_NV_READY);
    return NOERROR;
}

ECode DcTracker::OnSetDependencyMet(
    /* [in] */ const String& apnType,
    /* [in] */ Boolean met)
{
    // don't allow users to tweak hipri to work around default dependency not met
    if (IPhoneConstants::APN_TYPE_HIPRI.Equals(apnType)) return NOERROR;
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        Loge("onSetDependencyMet: ApnContext not found in onSetDependencyMet(%s, %d)", apnType.string(), met);
        return NOERROR;
    }
    Boolean isEnabled;
    apnContext->IsEnabled(&isEnabled);
    ApplyNewState(apnContext, isEnabled, met);
    if (IPhoneConstants::APN_TYPE_DEFAULT.Equals(apnType)) {
        // tie actions on default to similar actions on HIPRI regarding dependencyMet
        AutoPtr<IInterface> obj;
        mApnContexts->Get(StringUtils::ParseCharSequence(IPhoneConstants::APN_TYPE_HIPRI), (IInterface**)&obj);
        apnContext = IApnContext::Probe(obj);
        Boolean isEnabled;
        apnContext->IsEnabled(&isEnabled);
        if (apnContext != NULL) ApplyNewState(apnContext, isEnabled, met);
    }
    return NOERROR;
}

ECode DcTracker::ApplyNewState(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean met)
{
    Boolean cleanup = FALSE;
    Boolean trySetup = FALSE;
    if (DBG) {
        Boolean isEnabled;
        apnContext->IsEnabled(&isEnabled);
        String apnContextApnType;
        apnContext->GetApnType(&apnContextApnType);
        Boolean dependencyMet;
        apnContext->GetDependencyMet(&dependencyMet);
        Log("applyNewState(%s, %d(%d), %d(%d))", apnContextApnType.string(), enabled, isEnabled, met, dependencyMet);
    }
    Boolean isReady;
    apnContext->IsReady(&isReady);
    if (isReady) {
        cleanup = TRUE;
        if (enabled && met) {
            DctConstantsState state;
            apnContext->GetState(&state);
            switch(state) {
                case DctConstantsState_CONNECTING:
                case DctConstantsState_SCANNING:
                case DctConstantsState_CONNECTED:
                case DctConstantsState_DISCONNECTING:
                    // We're "READY" and active so just return
                    if (DBG) Log("applyNewState: 'ready' so return");
                    return NOERROR;
                case DctConstantsState_IDLE:
                    // fall through: this is unexpected but if it happens cleanup and try setup
                case DctConstantsState_FAILED:
                case DctConstantsState_RETRYING: {
                    // We're "READY" but not active so disconnect (cleanup = true) and
                    // connect (trySetup = true) to be sure we retry the connection.
                    trySetup = TRUE;
                    apnContext->SetReason(IPhone::REASON_DATA_ENABLED);
                    break;
                }
            }
        } else if (met) {
            apnContext->SetReason(IPhone::REASON_DATA_DISABLED);
            // If ConnectivityService has disabled this network, stop trying to bring
            // it up, but do not tear it down - ConnectivityService will do that
            // directly by talking with the DataConnection.
            //
            // This doesn't apply to DUN, however.  Those connections have special
            // requirements from carriers and we need stop using them when the dun
            // request goes away.  This applies to both CDMA and GSM because they both
            // can declare the DUN APN sharable by default traffic, thus still satisfying
            // those requests and not torn down organically.
            String apnContextApnType;
            apnContext->GetApnType(&apnContextApnType);
            Boolean isTeardownForDun;
            TeardownForDun(&isTeardownForDun);
            if ((apnContextApnType == IPhoneConstants::APN_TYPE_DUN && isTeardownForDun) ||
                    (apnContextApnType == IPhoneConstants::APN_TYPE_MMS)) {
                cleanup = TRUE;
            } else {
                cleanup = FALSE;
            }
        } else {
            apnContext->SetReason(IPhone::REASON_DATA_DEPENDENCY_UNMET);
        }
    } else {
        if (enabled && met) {
            Boolean isEnabled;
            apnContext->IsEnabled(&isEnabled);
            if (isEnabled) {
                apnContext->SetReason(IPhone::REASON_DATA_DEPENDENCY_MET);
            } else {
                apnContext->SetReason(IPhone::REASON_DATA_ENABLED);
            }
            DctConstantsState state;
            apnContext->GetState(&state);
            if (state == DctConstantsState_FAILED) {
                apnContext->SetState(DctConstantsState_IDLE);
            }
            trySetup = TRUE;
        }
    }
    apnContext->SetEnabled(enabled);
    apnContext->SetDependencyMet(met);
    if (cleanup) CleanUpConnection(TRUE, apnContext);
    Boolean b;
    if (trySetup) TrySetupData(apnContext, &b);
    return NOERROR;
}

ECode DcTracker::CheckForCompatibleConnectedApnContext(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ IDcAsyncChannel** result)
{
    VALIDATE_NOT_NULL(result)

    String apnType;
    apnContext->GetApnType(&apnType);
    AutoPtr<IApnSetting> dunSetting;
    if (IPhoneConstants::APN_TYPE_DUN.Equals(apnType)) {
        FetchDunApn((IApnSetting**)&dunSetting);
    }
    if (DBG) {
        Log("checkForCompatibleConnectedApnContext: apnContext=%s", TO_CSTR(apnContext) );
    }
    AutoPtr<IDcAsyncChannel> potentialDcac;
    AutoPtr<IApnContext> potentialApnCtx;
    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> curApnCtx = IApnContext::Probe(obj);
        AutoPtr<IDcAsyncChannel> curDcac;
        curApnCtx->GetDcAc((IDcAsyncChannel**)&curDcac);
        Log("curDcac: %s", TO_CSTR(curDcac));
        if (curDcac != NULL) {
            AutoPtr<IApnSetting> apnSetting;
            curApnCtx->GetApnSetting((IApnSetting**)&apnSetting);
            Log("apnSetting: %s", TO_CSTR(apnSetting));
            Boolean canHandleType;
            apnSetting->CanHandleType(apnType, &canHandleType);
            if (dunSetting != NULL) {
                Boolean isEquals;
                IObject::Probe(dunSetting)->Equals(apnSetting, &isEquals);
                if (isEquals) {
                    DctConstantsState state;
                    curApnCtx->GetState(&state);
                    switch (state) {
                        case DctConstantsState_CONNECTED:
                            if (DBG) {
                                Log("checkForCompatibleConnectedApnContext:"
                                        " found dun conn=%s"
                                        " curApnCtx=%s", TO_CSTR(curDcac), TO_CSTR(curApnCtx));
                            }
                            *result = curDcac;
                            REFCOUNT_ADD(*result)
                            return NOERROR;
                        case DctConstantsState_RETRYING:
                        case DctConstantsState_CONNECTING:
                            potentialDcac = curDcac;
                            potentialApnCtx = curApnCtx;
                            break;
                        case DctConstantsState_DISCONNECTING:
                            //Update for DISCONNECTING only if there is no other potential match
                            if (potentialDcac == NULL) {
                                potentialDcac = curDcac;
                                potentialApnCtx = curApnCtx;
                            }
                        default:
                            // Not connected, potential unchanged
                            break;
                    }
                }
            } else if (apnSetting != NULL && canHandleType) {
                DctConstantsState state;
                curApnCtx->GetState(&state);
                switch (state) {
                    case DctConstantsState_CONNECTED:
                        if (DBG) {
                            Log("checkForCompatibleConnectedApnContext:"
                                    " found canHandle conn=%s"
                                    " curApnCtx=%s", TO_CSTR(curDcac), TO_CSTR(curApnCtx));
                        }
                        *result = curDcac;
                        REFCOUNT_ADD(*result)
                        return NOERROR;
                    case DctConstantsState_RETRYING:
                    case DctConstantsState_CONNECTING:
                        potentialDcac = curDcac;
                        potentialApnCtx = curApnCtx;
                        break;
                    case DctConstantsState_DISCONNECTING:
                        // Update for DISCONNECTING only if there is no other potential match
                        if (potentialDcac == NULL) {
                            potentialDcac = curDcac;
                            potentialApnCtx = curApnCtx;
                        }
                    default:
                        // Not connected, potential unchanged
                        break;
                }
            }
        } else {
            if (VDBG) {
                Log("checkForCompatibleConnectedApnContext: not conn curApnCtx=%s", TO_CSTR(curApnCtx));
            }
        }
    }
    if (potentialDcac != NULL) {
        if (DBG) {
            Log("checkForCompatibleConnectedApnContext: found potential conn=%s"
                    " curApnCtx=%s", TO_CSTR(potentialDcac), TO_CSTR(potentialApnCtx));
        }
        *result = potentialDcac;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    if (DBG) Log("checkForCompatibleConnectedApnContext: NO conn apnContext=%s", TO_CSTR(apnContext));
    *result = NULL;
    return NOERROR;
}

ECode DcTracker::OnEnableApn(
    /* [in] */ Int32 apnId,
    /* [in] */ Int32 enabled)
{
    String type;
    ApnIdToType(apnId, &type);
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(type), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext == NULL) {
        Loge("onEnableApn(%d, %d): NO ApnContext", apnId, enabled);
        return NOERROR;
    }
    // TODO change our retry manager to use the appropriate numbers for the new APN
    if (DBG) Log("onEnableApn: apnContext=%s call applyNewState", TO_CSTR(apnContext));
    Boolean dependencyMet;
    apnContext->GetDependencyMet(&dependencyMet);
    ApplyNewState(apnContext, enabled == IDctConstants::ENABLED, dependencyMet);
    return NOERROR;
}

ECode DcTracker::OnTrySetupData(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("onTrySetupData: reason=%s", reason.string());
    SetupDataOnConnectableApns(reason);
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::OnTrySetupData(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("onTrySetupData: apnContext=%s", TO_CSTR(apnContext));
    return TrySetupData(apnContext, result);
}

ECode DcTracker::OnRoamingOff()
{
    if (DBG) Log("onRoamingOff");
    if (mUserDataEnabled == FALSE) return NOERROR;
    Boolean isDataOnRoamingEnabled;
    GetDataOnRoamingEnabled(&isDataOnRoamingEnabled);
    if (isDataOnRoamingEnabled == FALSE) {
        NotifyOffApnsOfAvailability(IPhone::REASON_ROAMING_OFF);
        SetupDataOnConnectableApns(IPhone::REASON_ROAMING_OFF);
    } else {
        NotifyDataConnection(IPhone::REASON_ROAMING_OFF);
    }
    return NOERROR;
}

ECode DcTracker::OnRoamingOn()
{
    if (mUserDataEnabled == FALSE) return NOERROR;
    Boolean isDataOnRoamingEnabled;
    GetDataOnRoamingEnabled(&isDataOnRoamingEnabled);
    if (isDataOnRoamingEnabled) {
        if (DBG) Log("onRoamingOn: setup data on roaming");
        SetupDataOnConnectableApns(IPhone::REASON_ROAMING_ON);
        NotifyDataConnection(IPhone::REASON_ROAMING_ON);
    } else {
        if (DBG) Log("onRoamingOn: Tear down data connection on roaming.");
        Boolean b;
        CleanUpAllConnections(TRUE, IPhone::REASON_ROAMING_ON, &b);
        NotifyOffApnsOfAvailability(IPhone::REASON_ROAMING_ON);
    }
    return NOERROR;
}

ECode DcTracker::OnRadioAvailable()
{
    if (DBG) Log("onRadioAvailable");
    //assert(0 && "TODO: ISimulatedRadioControl");
    Logger::E("DcTracker", "TODO DcTracker::OnRadioAvailable ISimulatedRadioControl is not ready!");
    // AutoPtr<ISimulatedRadioControl> control;
    // mPhone->GetSimulatedRadioControl((ISimulatedRadioControl**)&control);
    // if (control != NULL) {
    //     // Assume data is connected on the simulator
    //     // FIXME  this can be improved
    //     // SetState(DctConstants.State.CONNECTED);
    //     NotifyDataConnection(NULL);
    //     Log("onRadioAvailable: We're on the simulator; assuming data is connected");
    // }
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    Boolean isRecordsLoaded;
    if (r != NULL)
        r->GetRecordsLoaded(&isRecordsLoaded);
    if (r != NULL && isRecordsLoaded) {
        NotifyOffApnsOfAvailability(String(NULL));
    }
    DctConstantsState overallState;
    GetOverallState(&overallState);
    if (overallState != DctConstantsState_IDLE) {
        CleanUpConnection(TRUE, NULL);
    }
    return NOERROR;
}

ECode DcTracker::OnRadioOffOrNotAvailable()
{
    Logger::E("DcTracker", "TODO OnRadioOffOrNotAvailable not implemented");
    return NOERROR;
    // Make sure our reconnect delay starts at the initial value
    // next time the radio comes on
    mReregisterOnReconnectFailure = FALSE;
    assert(0 && "TODO: ISimulatedRadioControl");
    // AutoPtr<ISimulatedRadioControl> control;
    // mPhone->GetSimulatedRadioControl((ISimulatedRadioControl**)&control);
    // if (control != NULL) {
    //     // Assume data is connected on the simulator
    //     // FIXME  this can be improved
    //     Log("We're on the simulator; assuming radio off is meaningless");
    // } else {
    //     if (DBG) Log("onRadioOffOrNotAvailable: is off and clean up all connections");
    //     Boolean b;
    //     CleanUpAllConnections(FALSE, IPhone::REASON_RADIO_TURNED_OFF, &b);
    // }
    NotifyOffApnsOfAvailability(String(NULL));
    return NOERROR;
}

ECode DcTracker::CompleteConnection(
    /* [in] */ IApnContext* apnContext)
{
    Boolean isProvApn;
    apnContext->IsProvisioningApn(&isProvApn);
    if (DBG) Log("completeConnection: successful, notify the world apnContext=%s", TO_CSTR(apnContext));
    if (mIsProvisioning && !TextUtils::IsEmpty(mProvisioningUrl)) {
        if (DBG) {
            Log("completeConnection: MOBILE_PROVISIONING_ACTION url=%s", mProvisioningUrl.string());
        }
        AutoPtr<IIntent> newIntent = Intent::MakeMainSelectorActivity(IIntent::ACTION_MAIN,
                IIntent::CATEGORY_APP_BROWSER);
        AutoPtr<IUri> uri;
        Uri::Parse(mProvisioningUrl, (IUri**)&uri);
        newIntent->SetData(uri);
        newIntent->SetFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT |
                IIntent::FLAG_ACTIVITY_NEW_TASK);
        // try {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        ECode ec = context->StartActivity(newIntent);
        // } catch (ActivityNotFoundException e) {
        if ((ECode) E_ACTIVITY_NOT_FOUND_EXCEPTION == ec) {
            Loge("completeConnection: startActivityAsUser failed %d", ec);
        }
        // }
    }
    mIsProvisioning = FALSE;
    mProvisioningUrl = NULL;
    if (mProvisioningSpinner != NULL) {
        AutoPtr<IMessage> msg;
        ObtainMessage(IDctConstants::CMD_CLEAR_PROVISIONING_SPINNER,
                mProvisioningSpinner, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
    }
    String apnContextApnType;
    apnContext->GetApnType(&apnContextApnType);
    String reason;
    apnContext->GetReason(&reason);
    mPhone->NotifyDataConnection(reason, apnContextApnType);
    StartNetStatPoll();
    StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
    return NOERROR;
}

ECode DcTracker::OnDataSetupComplete(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IDcFailCause> cause;
    CDcFailCause::New(DcFailCause_UNKNOWN, (IDcFailCause**)&cause);
    Boolean handleError = FALSE;
    AutoPtr<IApnContext> apnContext;
    if (IApnContext::Probe(((AsyncResult*) ar)->mUserObj) != NULL){
        apnContext = IApnContext::Probe(((AsyncResult*) ar)->mUserObj);
    } else {
        Logger::E(LOG__TAG, "onDataSetupComplete: No apnContext");
        return E_RUNTIME_EXCEPTION;
    }
    if (((AsyncResult*) ar)->mException == NULL) {
        AutoPtr<IDcAsyncChannel> dcac;
        apnContext->GetDcAc((IDcAsyncChannel**)&dcac);
        if (RADIO_TESTS) {
            // Note: To change radio.test.onDSC.null.dcac from command line you need to
            // adb root and adb remount and from the command line you can only change the
            // value to 1 once. To change it a second time you can reboot or execute
            // adb shell stop and then adb shell start. The command line to set the value is:
            // adb shell sqlite3 /data/data/com.android.providers.settings/databases/settings.db "insert into system (name,value) values ('radio.test.onDSC.null.dcac', '1');"
            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> cr;
            context->GetContentResolver((IContentResolver**)&cr);
            String radioTestProperty("radio.test.onDSC.null.dcac");
            Int32 i32;
            Settings::System::GetInt32(cr, radioTestProperty, 0, &i32);
            if (i32 == 1) {
                Log("onDataSetupComplete: %s is true, set dcac to null and reset property to false", radioTestProperty.string());
                dcac = NULL;
                Boolean b;
                Settings::System::PutInt32(cr, radioTestProperty, 0, &b);
                AutoPtr<IContext> context;
                IPhone::Probe(mPhone)->GetContext((IContext**)&context);
                AutoPtr<IContentResolver> contentResolver;
                context->GetContentResolver((IContentResolver**)&contentResolver);
                Settings::System::GetInt32(contentResolver, radioTestProperty, -1, &i32);
                Log("onDataSetupComplete: %s=%d", radioTestProperty.string(), i32);
            }
        }
        if (dcac == NULL) {
            Log("onDataSetupComplete: no connection to DC, handle as error");
            cause = NULL;
            CDcFailCause::New(DcFailCause_CONNECTION_TO_DATACONNECTIONAC_BROKEN, (IDcFailCause**)&cause);
            handleError = TRUE;
        } else {
            AutoPtr<IApnSetting> apn;
            apnContext->GetApnSetting((IApnSetting**)&apn);
            if (DBG) {
                String apnApn;
                apn->GetApn(&apnApn);
                Log("onDataSetupComplete: success apn=%s", (apn == NULL ? "unknown" : apnApn.string()));
            }
            String apnProxy;
            apn->GetProxy(&apnProxy);
            if (apn != NULL && apnProxy != NULL && apnProxy.GetLength() != 0) {
                // try {
                ECode ec;
                do {
                    String port;
                    apn->GetPort(&port);
                    if (TextUtils::IsEmpty(port)) port = "8080";
                    String apnProxy;
                    ec = apn->GetProxy(&apnProxy);
                    if (FAILED(ec)) break;
                    AutoPtr<IProxyInfo> proxy;
                    ec = CProxyInfo::New(apnProxy,
                            StringUtils::ParseInt32(port), String(NULL), (IProxyInfo**)&proxy);
                    if (FAILED(ec)) break;
                    ec = dcac->SetLinkPropertiesHttpProxySync(proxy);
                } while(FALSE);
                // } catch (NumberFormatException e) {
                if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
                    String apnPort;
                    apn->GetPort(&apnPort);
                    Loge("onDataSetupComplete: NumberFormatException making ProxyProperties (%s" "): %d", apnPort.string(), ec);
                }
                // }
            }
            // everything is setup
            String apnContextApnType;
            apnContext->GetApnType(&apnContextApnType);
            if (TextUtils::Equals(apnContextApnType,IPhoneConstants::APN_TYPE_DEFAULT)) {
                SystemProperties::Set(PUPPET_MASTER_RADIO_STRESS_TEST, String("true"));
                if (mCanSetPreferApn && mPreferredApn == NULL) {
                    if (DBG) Log("onDataSetupComplete: PREFERED APN is null");
                    mPreferredApn = apn;
                    if (mPreferredApn != NULL) {
                        Int32 mPreferredApnId;
                        mPreferredApn->GetId(&mPreferredApnId);
                        SetPreferredApn(mPreferredApnId);
                    }
                }
            } else {
                SystemProperties::Set(PUPPET_MASTER_RADIO_STRESS_TEST, String("false"));
            }
            // A connection is setup
            apnContext->SetState(DctConstantsState_CONNECTED);
            Boolean isProvApn;
            apnContext->IsProvisioningApn(&isProvApn);
            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IConnectivityManager> cm;
            CConnectivityManager::From(context, (IConnectivityManager**)&cm);
            if (mProvisionBroadcastReceiver != NULL) {
                context->UnregisterReceiver(mProvisionBroadcastReceiver);
                mProvisionBroadcastReceiver = NULL;
            }
            if ((!isProvApn) || mIsProvisioning) {
                // Hide any provisioning notification.
                cm->SetProvisioningNotificationVisible(FALSE, IConnectivityManager::TYPE_MOBILE,
                        mProvisionActionName);
                // Complete the connection normally notifying the world we're connected.
                // We do this if this isn't a special provisioning apn or if we've been
                // told its time to provision.
                CompleteConnection(apnContext);
            } else {
                // This is a provisioning APN that we're reporting as connected. Later
                // when the user desires to upgrade this to a "default" connection,
                // mIsProvisioning == true, we'll go through the code path above.
                // mIsProvisioning becomes true when CMD_ENABLE_MOBILE_PROVISIONING
                // is sent to the DCT.
                if (DBG) {
                    Log("onDataSetupComplete: successful, BUT send connected to prov apn as"
                            " mIsProvisioning:%d == false"
                            " && (isProvisioningApn:%d == true", mIsProvisioning, isProvApn);
                }
                // While radio is up, grab provisioning URL.  The URL contains ICCID which
                // disappears when radio is off.
                String mobileProvisioningUrl;
                cm->GetMobileProvisioningUrl(&mobileProvisioningUrl);
                AutoPtr<ITelephonyManager> tm;
                CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
                String networkOperatorName;
                tm->GetNetworkOperatorName(&networkOperatorName);
                mProvisionBroadcastReceiver = new ProvisionNotificationBroadcastReceiver(this);
                ((ProvisionNotificationBroadcastReceiver*) mProvisionBroadcastReceiver.Get())->constructor(mobileProvisioningUrl, networkOperatorName);
                AutoPtr<IIntentFilter> intentFilter;
                CIntentFilter::New(mProvisionActionName, (IIntentFilter**)&intentFilter);
                AutoPtr<IIntent> intent;
                context->RegisterReceiver(mProvisionBroadcastReceiver, intentFilter, (IIntent**)&intent);
                // Put up user notification that sign-in is required.
                cm->SetProvisioningNotificationVisible(TRUE, IConnectivityManager::TYPE_MOBILE,
                        mProvisionActionName);
                // Turn off radio to save battery and avoid wasting carrier resources.
                // The network isn't usable and network validation will just fail anyhow.
                SetRadio(FALSE);
                intent = NULL;
                CIntent::New(ITelephonyIntents::ACTION_DATA_CONNECTION_CONNECTED_TO_PROVISIONING_APN, (IIntent**)&intent);
                AutoPtr<IApnSetting> apnSetting;
                apnContext->GetApnSetting((IApnSetting**)&apnSetting);
                String apn;
                apnSetting->GetApn(&apn);
                intent->PutExtra(IPhoneConstants::DATA_APN_KEY, apn);
                String apnContextApnType;
                apnContext->GetApnType(&apnContextApnType);
                intent->PutExtra(IPhoneConstants::DATA_APN_TYPE_KEY, apnContextApnType);
                String apnType;
                apnContext->GetApnType(&apnType);
                AutoPtr<ILinkProperties> linkProperties;
                GetLinkProperties(apnType, (ILinkProperties**)&linkProperties);
                if (linkProperties != NULL) {
                    intent->PutExtra(IPhoneConstants::DATA_LINK_PROPERTIES_KEY, IParcelable::Probe(linkProperties));
                    String iface;
                    linkProperties->GetInterfaceName(&iface);
                    if (iface != NULL) {
                        intent->PutExtra(IPhoneConstants::DATA_IFACE_NAME_KEY, iface);
                    }
                }
                AutoPtr<INetworkCapabilities> networkCapabilities;
                GetNetworkCapabilities(apnType, (INetworkCapabilities**)&networkCapabilities);
                if (networkCapabilities != NULL) {
                    intent->PutExtra(IPhoneConstants::DATA_NETWORK_CAPABILITIES_KEY,
                            IParcelable::Probe(networkCapabilities));
                }
                context->SendBroadcastAsUser(intent, UserHandle::ALL);
            }
            if (DBG) {
                String apnContextApnType;
                apnContext->GetApnType(&apnContextApnType);
                String reason;
                apnContext->GetReason(&reason);
                Log("onDataSetupComplete: SETUP complete type=%s, reason:%s", apnContextApnType.string(), reason.string());
            }
        }
    } else {
        cause = IDcFailCause::Probe(((AsyncResult*) ar)->mResult);
        if (DBG) {
            AutoPtr<IApnSetting> apn;
            apnContext->GetApnSetting((IApnSetting**)&apn);
            String apnApn;
            apn->GetApn(&apnApn);
            Log("onDataSetupComplete: error apn=%s cause=%s", (apn == NULL ? "unknown" : apnApn.string()), TO_CSTR(cause));
        }
        if (cause == NULL) {
            CDcFailCause::New(DcFailCause_UNKNOWN, (IDcFailCause**)&cause);
        }
        Boolean isEventLoggable;
        cause->IsEventLoggable(&isEventLoggable);
        if (isEventLoggable) {
            // Log this failure to the Event Logs.
            Int32 cid;
            GetCellLocationId(&cid);
            Int32 networkType;
            AutoPtr<ITelephonyManager> tm;
            CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
            tm->GetNetworkType(&networkType);
            Logger::D(LOG__TAG, "TODO EventLog");
            // EventLog::WriteEvent(IEventLogTags::PDP_SETUP_FAIL,
            //         cause, cid, networkType);
        }
        AutoPtr<IApnSetting> apn;
        apnContext->GetApnSetting((IApnSetting**)&apn);
        String apnContextApnType;
        apnContext->GetApnType(&apnContextApnType);
        String reason;
        apnContext->GetReason(&reason);
        String apnApn;
        apn->GetApn(&apnApn);
        mPhone->NotifyPreciseDataConnectionFailed(reason, apnContextApnType, apn != NULL ? apnApn : String("unknown"), TO_STR(cause));
        // Count permanent failures and remove the APN we just tried
        Boolean isPermanentFail;
        IsPermanentFail(cause, &isPermanentFail);
        if (isPermanentFail) {
            apnContext->DecWaitingApnsPermFailCount();
        }
        AutoPtr<IApnSetting> apnSetting;
        apnContext->GetApnSetting((IApnSetting**)&apnSetting);
        apnContext->RemoveWaitingApn(apnSetting);
        if (DBG) {
            AutoPtr<IArrayList> waitingApns;
            apnContext->GetWaitingApns((IArrayList**)&waitingApns);
            if (waitingApns == NULL){
                Int32 waitingApnsPermFailCount;
                apnContext->GetWaitingApnsPermFailCount(&waitingApnsPermFailCount);
                Log("onDataSetupComplete: WaitingApns.size = null"
                        " WaitingApnsPermFailureCountDown=%d",
                        waitingApnsPermFailCount);
            }
            else {
                Int32 size;
                waitingApns->GetSize(&size);
                Int32 waitingApnsPermFailCount;
                apnContext->GetWaitingApnsPermFailCount(&waitingApnsPermFailCount);
                Log("onDataSetupComplete: WaitingApns.size=%d"
                        " WaitingApnsPermFailureCountDown=%d",
                        size,
                        waitingApnsPermFailCount);
            }
        }
        handleError = TRUE;
    }
    if (handleError) {
        OnDataSetupCompleteError(ar);
    }
    /* If flag is set to false after SETUP_DATA_CALL is invoked, we need
     * to clean data connections.
     */
    if (!mInternalDataEnabled) {
        CleanUpAllConnections(String(NULL));
    }
    return NOERROR;
}

ECode DcTracker::GetApnDelay(
    /* [in] */ const String& reason,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mFailFast || IPhone::REASON_NW_TYPE_CHANGED.Equals(reason) ||
            IPhone::REASON_APN_CHANGED.Equals(reason)) {
        return SystemProperties::GetInt32(String("persist.radio.apn_ff_delay"),
                APN_FAIL_FAST_DELAY_DEFAULT_MILLIS, result);
    } else {
        return SystemProperties::GetInt32(String("persist.radio.apn_delay"), APN_DELAY_DEFAULT_MILLIS, result);
    }
    return NOERROR;
}

ECode DcTracker::OnDataSetupCompleteError(
    /* [in] */ IAsyncResult* ar)
{
    String reason("");
    AutoPtr<IApnContext> apnContext;
    if (IApnContext::Probe(((AsyncResult*) ar)->mUserObj) != NULL){
        apnContext = IApnContext::Probe(((AsyncResult*) ar)->mUserObj);
    } else {
        Logger::E(LOG__TAG, "onDataSetupCompleteError: No apnContext");
        return E_RUNTIME_EXCEPTION;
    }
    // See if there are more APN's to try
    AutoPtr<IArrayList> waitingApns;
    apnContext->GetWaitingApns((IArrayList**)&waitingApns);
    Boolean isEmpty;
    waitingApns->IsEmpty(&isEmpty);
    if (waitingApns == NULL || isEmpty) {
        apnContext->SetState(DctConstantsState_FAILED);
        String apnContextApnType;
        apnContext->GetApnType(&apnContextApnType);
        mPhone->NotifyDataConnection(IPhone::REASON_APN_FAILED, apnContextApnType);
        apnContext->SetDataConnectionAc(NULL);
        Int32 waitingApnsPermFailCount;
        apnContext->GetWaitingApnsPermFailCount(&waitingApnsPermFailCount);
        if (waitingApnsPermFailCount == 0) {
            if (DBG) {
                Log("onDataSetupCompleteError: All APN's had permanent failures, stop retrying");
            }
        } else {
            Int32 delay;
            GetApnDelay(IPhone::REASON_APN_FAILED, &delay);
            if (DBG) {
                Log("onDataSetupCompleteError: Not all APN's had permanent failures delay=%d", delay);
            }
            StartAlarmForRestartTrySetup(delay, apnContext);
        }
    } else {
        if (DBG) Log("onDataSetupCompleteError: Try next APN");
        apnContext->SetState(DctConstantsState_SCANNING);
        // Wait a bit before trying the next APN, so that
        // we're not tying up the RIL command channel
        Int32 delay;
        GetApnDelay(IPhone::REASON_APN_FAILED, &delay);
        StartAlarmForReconnect(delay, apnContext);
    }
    return NOERROR;
}

ECode DcTracker::OnDisconnectDone(
    /* [in] */ Int32 connId,
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IApnContext> apnContext;
    if (IApnContext::Probe(((AsyncResult*) ar)->mUserObj) != NULL) {
        apnContext = IApnContext::Probe(((AsyncResult*) ar)->mUserObj);
    } else {
        Loge("onDisconnectDone: Invalid ar in onDisconnectDone, ignore");
        return NOERROR;
    }
    if (DBG) Log("onDisconnectDone: EVENT_DISCONNECT_DONE apnContext=%s", TO_CSTR(apnContext));
    apnContext->SetState(DctConstantsState_IDLE);
    String apnContextApnType;
    apnContext->GetApnType(&apnContextApnType);
    String reason;
    apnContext->GetReason(&reason);
    mPhone->NotifyDataConnection(reason, apnContextApnType);
    // if all data connection are gone, check whether Airplane mode request was
    // pending.
    Boolean isDisconnected;
    IsDisconnected(&isDisconnected);
    if (isDisconnected) {
        AutoPtr<IServiceStateTracker> serviceStateTracker;
        mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
        Boolean isProcessPendingRadioPowerOffAfterDataOff;
        serviceStateTracker->ProcessPendingRadioPowerOffAfterDataOff(&isProcessPendingRadioPowerOffAfterDataOff);
        if (isProcessPendingRadioPowerOffAfterDataOff) {
            if (DBG) Log("onDisconnectDone: radio will be turned off, no retries");
            // Radio will be turned off. No need to retry data setup
            apnContext->SetApnSetting(NULL);
            apnContext->SetDataConnectionAc(NULL);
            // Need to notify disconnect as well, in the case of switching Airplane mode.
            // Otherwise, it would cause 30s delayed to turn on Airplane mode.
            if (mDisconnectPendingCount > 0)
                mDisconnectPendingCount--;
            if (mDisconnectPendingCount == 0) {
                NotifyDataDisconnectComplete();
                NotifyAllDataDisconnected();
            }
            return NOERROR;
        }
    }
    // If APN is still enabled, try to bring it back up automatically
    Boolean isReady;
    apnContext->IsReady(&isReady);
    Boolean isRetryAfterDisconnected;
    RetryAfterDisconnected(apnContext, &isRetryAfterDisconnected);
    Boolean b;
    mAttached->Get(&b);
    if (b && isReady && isRetryAfterDisconnected) {
        SystemProperties::Set(PUPPET_MASTER_RADIO_STRESS_TEST, String("false"));
        // Wait a bit before trying the next APN, so that
        // we're not tying up the RIL command channel.
        // This also helps in any external dependency to turn off the context.
        if (DBG) Log("onDisconnectDone: attached, ready and retry after disconnect");
        String reason;
        apnContext->GetReason(&reason);
        Int32 apnDelay;
        GetApnDelay(reason, &apnDelay);
        StartAlarmForReconnect(apnDelay, apnContext);
    } else {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        Boolean restartRadioAfterProvisioning;
        res->GetBoolean(R::bool_::config_restartRadioAfterProvisioning, &restartRadioAfterProvisioning);
        Boolean isProvisioningApn;
        apnContext->IsProvisioningApn(&isProvisioningApn);
        if (isProvisioningApn && restartRadioAfterProvisioning) {
            Log("onDisconnectDone: restartRadio after provisioning");
            RestartRadio();
        }
        apnContext->SetApnSetting(NULL);
        apnContext->SetDataConnectionAc(NULL);
        Boolean isOnlySingleDcAllowed;
        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Int32 rilDataRadioTechnology;
        serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        IsOnlySingleDcAllowed(rilDataRadioTechnology, &isOnlySingleDcAllowed);
        if (isOnlySingleDcAllowed) {
            if (DBG) Log("onDisconnectDone: isOnlySigneDcAllowed true so setup single apn");
            SetupDataOnConnectableApns(IPhone::REASON_SINGLE_PDN_ARBITRATION);
        } else {
            if (DBG) Log("onDisconnectDone: not retrying");
        }
    }
    if (mDisconnectPendingCount > 0)
        mDisconnectPendingCount--;
    if (mDisconnectPendingCount == 0) {
        NotifyDataDisconnectComplete();
        NotifyAllDataDisconnected();
    }
    return NOERROR;
}

ECode DcTracker::OnDisconnectDcRetrying(
    /* [in] */ Int32 connId,
    /* [in] */ IAsyncResult* ar)
{
    // We could just do this in DC!!!
    AutoPtr<IApnContext> apnContext;
    if (IApnContext::Probe(((AsyncResult*) ar)->mUserObj) != NULL) {
        apnContext = IApnContext::Probe(((AsyncResult*) ar)->mUserObj);
    } else {
        Loge("onDisconnectDcRetrying: Invalid ar in onDisconnectDone, ignore");
        return NOERROR;
    }
    apnContext->SetState(DctConstantsState_RETRYING);
    if (DBG) Log("onDisconnectDcRetrying: apnContext=%s", TO_CSTR(apnContext));
    String apnContextApnType;
    apnContext->GetApnType(&apnContextApnType);
    String reason;
    apnContext->GetReason(&reason);
    mPhone->NotifyDataConnection(reason, apnContextApnType);
    return NOERROR;
}

ECode DcTracker::OnVoiceCallStarted()
{
    if (DBG) Log("onVoiceCallStarted");
    mInVoiceCall = TRUE;
    Boolean isConcurrentVoiceAndDataAllowed;
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    serviceStateTracker->IsConcurrentVoiceAndDataAllowed(&isConcurrentVoiceAndDataAllowed);
    Boolean isConnected;
    IsConnected(&isConnected);
    if (isConnected && ! isConcurrentVoiceAndDataAllowed) {
        if (DBG) Log("onVoiceCallStarted stop polling");
        StopNetStatPoll();
        StopDataStallAlarm();
        NotifyDataConnection(IPhone::REASON_VOICE_CALL_STARTED);
    }
    return NOERROR;
}

ECode DcTracker::OnVoiceCallEnded()
{
    if (DBG) Log("onVoiceCallEnded");
    mInVoiceCall = FALSE;
    Boolean isConnected;
    IsConnected(&isConnected);
    if (isConnected) {
        AutoPtr<IServiceStateTracker> serviceStateTracker;
        mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
        Boolean isConcurrentVoiceAndDataAllowed;
        serviceStateTracker->IsConcurrentVoiceAndDataAllowed(&isConcurrentVoiceAndDataAllowed);
        if (!isConcurrentVoiceAndDataAllowed) {
            StartNetStatPoll();
            StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
            NotifyDataConnection(IPhone::REASON_VOICE_CALL_ENDED);
        } else {
            // clean slate after call end.
            ResetPollStats();
        }
    }
    // reset reconnect timer
    SetupDataOnConnectableApns(IPhone::REASON_VOICE_CALL_ENDED);
    return NOERROR;
}

ECode DcTracker::OnCleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ Int32 apnId,
    /* [in] */ const String& reason)
{
    if (DBG) Log("onCleanUpConnection");
    String apnType;
    ApnIdToType(apnId, &apnType);
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (apnContext != NULL) {
        apnContext->SetReason(reason);
        CleanUpConnection(tearDown, apnContext);
    }
    return NOERROR;
}

ECode DcTracker::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        DctConstantsState state;
        apnContext->GetState(&state);
        if (state == DctConstantsState_CONNECTED) {
            // At least one context is connected, return true
            *result = TRUE;
            return NOERROR;
        }
    }
    // There are not any contexts connected, return false
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::IsDisconnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isDisconnected;
        apnContext->IsDisconnected(&isDisconnected);
        if (!isDisconnected) {
            // At least one context was not disconnected return false
            *result = FALSE;
            return NOERROR;
        }
    }
    // All contexts were disconnected so return true
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::NotifyDataConnection(
    /* [in] */ const String& reason)
{
    if (DBG) Log("notifyDataConnection: reason=%s", reason.string());
    AutoPtr<IIterator> it;
    AutoPtr<ICollection> values;
    mApnContexts->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        Boolean isReady;
        apnContext->IsReady(&isReady);
        Boolean b;
        mAttached->Get(&b);
        if ((b || !mOosIsDisconnect) && isReady) {
            String apnContextApnType;
            apnContext->GetApnType(&apnContextApnType);
            if (DBG) Log("notifyDataConnection: type:%s", apnContextApnType.string());
            String apnContextReason;
            apnContext->GetReason(&apnContextReason);
            mPhone->NotifyDataConnection(reason != NULL ? reason : apnContextReason,
                    apnContextApnType);
        }
    }
    NotifyOffApnsOfAvailability(reason);
    return NOERROR;
}

ECode DcTracker::IsNvSubscription(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 radioTech;
    serviceState->GetRilVoiceRadioTechnology(&radioTech);
    if (mCdmaSsm == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 cdmaSubscriptionSource = 0;
    mCdmaSsm->GetCdmaSubscriptionSource(&cdmaSubscriptionSource);
    AutoPtr<IUiccControllerHelper> ucHelper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&ucHelper);
    Int32 flag = 0;
    ucHelper->GetFamilyFromRadioTechnology(radioTech, &flag);
    if (flag == IUiccController::APP_FAM_3GPP2
            && cdmaSubscriptionSource == ICdmaSubscriptionSourceManager::SUBSCRIPTION_FROM_NV) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::GetOperatorNumeric(
    /* [out] */ String* _result)
{
    VALIDATE_NOT_NULL(_result)

    String result;
    Boolean isNvSubscription;
    IsNvSubscription(&isNvSubscription);
    if (isNvSubscription) {
        SystemProperties::Get(ICDMAPhone::PROPERTY_CDMA_HOME_OPERATOR_NUMERIC, &result);
        Log("getOperatorNumberic - returning from NV: %s", result.string());
    } else {
        AutoPtr<IInterface> obj;
        mIccRecords->Get((IInterface**)&obj);
        AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
        if (r != NULL) {
            r->GetOperatorNumeric(&result);
        }
        else {
            result = "";
        }
        Log("getOperatorNumberic - returning from card: %s", result.string());
    }
    if (result == NULL) result = "";
    *_result = result;
    return NOERROR;
}

ECode DcTracker::CreateAllApnList()
{
    mAllApnSettings->Clear();
    String _operator;
    GetOperatorNumeric(&_operator);
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 radioTech;
    serviceState->GetRilDataRadioTechnology(&radioTech);
    Boolean isCdma;
    CServiceState::IsCdma(radioTech, &isCdma);
    if (mOmhApt != NULL && (isCdma &&
            IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD != radioTech)) {
        AutoPtr<IArrayList> mOmhApnsList;
        CArrayList::New((IArrayList**)&mOmhApnsList);
        mOmhApt->GetOmhApnProfilesList((IArrayList**)&mOmhApnsList);
        Boolean isEmpty;
        mOmhApnsList->IsEmpty(&isEmpty);
        if (!isEmpty) {
            if (DBG) Log("createAllApnList: Copy Omh profiles");
            mAllApnSettings->AddAll(ICollection::Probe(mOmhApnsList));
        }
    }
    Boolean isEmpty;
    mAllApnSettings->IsEmpty(&isEmpty);
    if (isEmpty) {
        Boolean _operatorIsEmpty = _operator.IsEmpty();
        if (!_operator.IsNull() && !_operatorIsEmpty) {
            String selection = String("numeric = '") + _operator + "'";
            // query only enabled apn.
            // carrier_enabled : 1 means enabled apn, 0 disabled apn.
            selection += " and carrier_enabled = 1";
            if (DBG) Log("createAllApnList: selection=%s", selection.string());
            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> contentResolver;
            context->GetContentResolver((IContentResolver**)&contentResolver);
            AutoPtr<ICursor> cursor;
            contentResolver->Query(
                    Elastos::Droid::Provider::Telephony::Carriers::CONTENT_URI, NULL, selection, NULL, String(NULL), (ICursor**)&cursor);
            if (cursor != NULL) {
                Int32 count;
                cursor->GetCount(&count);
                if (count > 0) {
                    CreateApnList(cursor, (IArrayList**)&mAllApnSettings);
                }
                ICloseable::Probe(cursor)->Close();
            }
        }
    }
    DedupeApnSettings();
    Boolean isDummyProfileNeeded;
    IsDummyProfileNeeded(&isDummyProfileNeeded);
    if (isEmpty && isDummyProfileNeeded) {
        AddDummyApnSettings(_operator);
    }
    AddEmergencyApnSetting();
    if (isEmpty) {
        if (DBG) Log("createAllApnList: No APN found for carrier: %s", _operator.string());
        mPreferredApn = NULL;
        // TODO: What is the right behavior?
        //notifyNoData(DataConnection.FailCause.MISSING_UNKNOWN_APN);
    } else {
        GetPreferredApn((IApnSetting**)&mPreferredApn);
        String numeric;
        mPreferredApn->GetNumeric(&numeric);
        if (mPreferredApn != NULL && !numeric.Equals(_operator)) {
            mPreferredApn = NULL;
            SetPreferredApn(-1);
        }
        if (DBG) Log("createAllApnList: mPreferredApn=%s", TO_CSTR(mPreferredApn));
    }
    if (DBG) Log("createAllApnList: X mAllApnSettings=%s", TO_CSTR(mAllApnSettings));
    SetDataProfilesAsNeeded();
    return NOERROR;
}

ECode DcTracker::DedupeApnSettings()
{
    AutoPtr<IArrayList> resultApns;
    CArrayList::New((IArrayList**)&resultApns);
    // coalesce APNs if they are similar enough to prevent
    // us from bringing up two data calls with the same interface
    Int32 i = 0;
    Int32 size;
    mAllApnSettings->GetSize(&size);
    while (i < size - 1) {
        AutoPtr<IInterface> obj;
        mAllApnSettings->Get(i, (IInterface**)&obj);
        AutoPtr<IApnSetting> first = IApnSetting::Probe(obj);
        AutoPtr<IApnSetting> second;
        Int32 j = i + 1;
        while (j < size) {
            AutoPtr<IInterface> obj;
            mAllApnSettings->Get(j, (IInterface**)&obj);
            second = IApnSetting::Probe(obj);
            Boolean isApnsSimilar;
            ApnsSimilar(first, second, &isApnsSimilar);
            if (isApnsSimilar) {
                AutoPtr<IApnSetting> newApn;
                MergeApns(first, second, (IApnSetting**)&newApn);
                mAllApnSettings->Set(i, newApn);
                first = newApn;
                mAllApnSettings->Remove(j);
            } else {
                j++;
            }
        }
        i++;
    }
    return NOERROR;
}

ECode DcTracker::ApnTypeSameAny(
    /* [in] */ IApnSetting* first,
    /* [in] */ IApnSetting* second,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<String> > types;
    first->GetTypes((ArrayOf<String>**)&types);
    AutoPtr<ArrayOf<String> > secondTypes;
    second->GetTypes((ArrayOf<String>**)&secondTypes);
    if (VDBG) {
        String apn;
        first->GetApn(&apn);
        StringBuilder apnType1(apn + ": ");
        for (Int32 index1 = 0; index1 < types->GetLength(); index1++) {
            apnType1.Append((*(types))[index1]);
            apnType1.Append(",");
        }
        String secondApn;
        second->GetApn(&secondApn);
        StringBuilder apnType2(secondApn + ": ");
        for (Int32 index1 = 0; index1 < secondTypes->GetLength(); index1++) {
            apnType2.Append((*(secondTypes))[index1]);
            apnType2.Append(",");
        }
        Log("APN1: is %s", apnType1.ToString().string());
        Log("APN2: is %s", apnType2.ToString().string());
    }
    for (Int32 index1 = 0; index1 < types->GetLength(); index1++) {
        for (Int32 index2 = 0; index2 < secondTypes->GetLength(); index2++) {
            if ((*(types))[index1].Equals(IPhoneConstants::APN_TYPE_ALL) ||
                    (*(secondTypes))[index2].Equals(IPhoneConstants::APN_TYPE_ALL) ||
                    (*(types))[index1].Equals((*(secondTypes))[index2])) {
                if (VDBG) Log("apnTypeSameAny: return true");
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    if (VDBG) Log("apnTypeSameAny: return false");
    *result = FALSE;
    return NOERROR;
}

ECode DcTracker::ApnsSimilar(
    /* [in] */ IApnSetting* first,
    /* [in] */ IApnSetting* second,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean canHandleTypeFirst;
    first->CanHandleType(IPhoneConstants::APN_TYPE_DUN, &canHandleTypeFirst);
    Boolean canHandleTypeSecond;
    second->CanHandleType(IPhoneConstants::APN_TYPE_DUN, &canHandleTypeSecond);
    Boolean isApnTypeSameAny;
    ApnTypeSameAny(first, second, &isApnTypeSameAny);
    Boolean isXorEqualsMmsc;
    String firstMmsc;
    first->GetMmsc(&firstMmsc);
    String secondMmsc;
    second->GetMmsc(&secondMmsc);
    XorEquals(firstMmsc, secondMmsc, &isXorEqualsMmsc);
    Boolean isXorEqualsProxy;
    String secondMmsProxy;
    second->GetMmsProxy(&secondMmsProxy);
    String firstMmsProxy;
    first->GetMmsProxy(&firstMmsProxy);
    XorEquals(firstMmsProxy, secondMmsProxy, &isXorEqualsProxy);
    Boolean isXorEqualsPort;
    String secondMmsPort;
    second->GetMmsPort(&secondMmsPort);
    String firstMmsPort;
    first->GetMmsPort(&firstMmsPort);
    XorEquals(firstMmsPort, secondMmsPort, &isXorEqualsPort);
    String fMvnoType;
    first->GetMvnoType(&fMvnoType);
    String sMvnoType;
    second->GetMvnoType(&sMvnoType);
    String fMvnoMatchData;
    first->GetMvnoMatchData(&fMvnoMatchData);
    String sMvnoMatchData;
    second->GetMvnoMatchData(&sMvnoMatchData);
    String secondApn;
    second->GetApn(&secondApn);
    String firstApn;
    first->GetApn(&firstApn);
    String firstProxy;
    first->GetProxy(&firstProxy);
    String secondProxy;
    second->GetProxy(&secondProxy);
    String firstPort;
    first->GetPort(&firstPort);
    String secondPort;
    second->GetPort(&secondPort);
    Boolean firstCarrierEnabled;
    first->GetCarrierEnabled(&firstCarrierEnabled);
    Boolean secondCarrierEnabled;
    second->GetCarrierEnabled(&secondCarrierEnabled);
    Int32 secondBearer;
    second->GetBearer(&secondBearer);
    Int32 firstBearer;
    first->GetBearer(&firstBearer);
    Int32 secondProfileId;
    second->GetProfileId(&secondProfileId);
    Int32 firstProfileId;
    first->GetProfileId(&firstProfileId);
    Boolean isProxyEquals;
    XorEquals(firstProxy, secondProxy, &isProxyEquals);
    Boolean isPortEquals;
    XorEquals(firstPort, secondPort, &isPortEquals);
    *result = (canHandleTypeFirst == FALSE &&
            canHandleTypeSecond == FALSE &&
            firstApn.Equals(secondApn) &&
            !isApnTypeSameAny &&
            isProxyEquals &&
            isPortEquals &&
            firstCarrierEnabled == secondCarrierEnabled &&
            firstBearer == secondBearer &&
            firstProfileId == secondProfileId &&
            fMvnoType.Equals(sMvnoType) &&
            fMvnoMatchData.Equals(sMvnoMatchData) &&
            isXorEqualsMmsc &&
            isXorEqualsProxy &&
            isXorEqualsPort);
    return NOERROR;
}

ECode DcTracker::XorEquals(
    /* [in] */ const String& first,
    /* [in] */ const String& second,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (first.Equals(second) ||
            TextUtils::IsEmpty(first) ||
            TextUtils::IsEmpty(second));
    return NOERROR;
}

ECode DcTracker::MergeApns(
    /* [in] */ IApnSetting* dest,
    /* [in] */ IApnSetting* src,
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> resultTypes;
    CArrayList::New((IArrayList**)&resultTypes);
    AutoPtr<ArrayOf<String> > destTypes;
    dest->GetTypes((ArrayOf<String>**)&destTypes);
    AutoPtr<IList> outList;
    Arrays::AsList(destTypes, (IList**)&outList);
    resultTypes->AddAll(ICollection::Probe(outList));
    AutoPtr<ArrayOf<String> > srcTypes;
    src->GetTypes((ArrayOf<String>**)&srcTypes);
    for (Int32 i = 0; i < srcTypes->GetLength(); ++i) {
        String srcType = (*srcTypes)[i];
        Boolean isContains;
        resultTypes->Contains(StringUtils::ParseCharSequence(srcType), &isContains);
        if (isContains == FALSE) resultTypes->Add(StringUtils::ParseCharSequence(srcType));
    }
    String destMmsc;
    dest->GetMmsc(&destMmsc);
    String srcMmsc;
    src->GetMmsc(&srcMmsc);
    String mmsc = (TextUtils::IsEmpty(destMmsc) ? srcMmsc : destMmsc);
    String destMmsProxy;
    dest->GetMmsProxy(&destMmsProxy);
    String srcMmsProxy;
    src->GetMmsProxy(&srcMmsProxy);
    String mmsProxy = (TextUtils::IsEmpty(destMmsProxy) ? srcMmsProxy : destMmsProxy);
    String destMmsPort;
    dest->GetMmsPort(&destMmsPort);
    String srcMmsPort;
    src->GetMmsPort(&srcMmsPort);
    String mmsPort = (TextUtils::IsEmpty(destMmsPort) ? srcMmsPort : destMmsPort);
    String destProxy;
    dest->GetProxy(&destProxy);
    String srcProxy;
    src->GetProxy(&srcProxy);
    String proxy = (TextUtils::IsEmpty(destProxy) ? srcProxy : destProxy);
    String destPort;
    dest->GetPort(&destPort);
    String srcPort;
    src->GetPort(&srcPort);
    String port = (TextUtils::IsEmpty(destPort) ? srcPort : destPort);
    String srcProtocol;
    src->GetProtocol(&srcProtocol);
    String destProtocol;
    dest->GetProtocol(&destProtocol);
    String protocol = srcProtocol.Equals("IPV4V6") ? srcProtocol : destProtocol;
    String srcRoamingProtocol;
    src->GetRoamingProtocol(&srcRoamingProtocol);
    String destRoamingProtocol;
    dest->GetRoamingProtocol(&destRoamingProtocol);
    String roamingProtocol = srcRoamingProtocol.Equals("IPV4V6") ? srcRoamingProtocol : destRoamingProtocol;
    String destNumeric;
    dest->GetNumeric(&destNumeric);
    String destCarrier;
    dest->GetCarrier(&destCarrier);
    String destApn;
    dest->GetApn(&destApn);
    String destMvnoMatchData;
    dest->GetMvnoMatchData(&destMvnoMatchData);
    String destMvnoType;
    dest->GetMvnoType(&destMvnoType);
    Int32 destId;
    dest->GetId(&destId);
    Int32 destAuthType;
    dest->GetAuthType(&destAuthType);
    Int32 destProfileId;
    dest->GetProfileId(&destProfileId);
    Int32 destBearer;
    dest->GetBearer(&destBearer);
    Int32 destWaitTime;
    dest->GetWaitTime(&destWaitTime);
    Int32 destMaxConns;
    dest->GetMaxConns(&destMaxConns);
    Int32 destMtu;
    dest->GetMtu(&destMtu);
    String destUser;
    dest->GetUser(&destUser);
    String destPassword;
    dest->GetPassword(&destPassword);
    AutoPtr<ArrayOf<IInterface*> > array;
    resultTypes->ToArray(ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&array);
    AutoPtr<ArrayOf<String> > resultTypesArray = ArrayOf<String>::Alloc(array->GetLength());
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        resultTypesArray->Set(i, TO_STR((*array)[i]));
    }
    Boolean isDestCarrierEnabled;
    dest->GetCarrierEnabled(&isDestCarrierEnabled);
    Boolean isDestModemCognitive;
    dest->GetModemCognitive(&isDestModemCognitive);
    Boolean isSrcModemCognitive;
    src->GetModemCognitive(&isSrcModemCognitive);
    Int32 maxConnsTime;
    dest->GetMaxConnsTime(&maxConnsTime);
    return CApnSetting::New(destId, destNumeric, destCarrier, destApn,
            proxy, port, mmsc, mmsProxy, mmsPort, destUser, destPassword,
            destAuthType, resultTypesArray, protocol,
            roamingProtocol, isDestCarrierEnabled, destBearer, destProfileId,
            (isDestModemCognitive || isSrcModemCognitive), destMaxConns, destWaitTime,
            maxConnsTime, destMtu, destMvnoType, destMvnoMatchData, result);
}

ECode DcTracker::IsDummyProfileNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 radioTech;
    serviceState->GetRilDataRadioTechnology(&radioTech);

    AutoPtr<IUiccControllerHelper> ucHelper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&ucHelper);
    Int32 radioTechFam = 0;
    ucHelper->GetFamilyFromRadioTechnology(radioTech, &radioTechFam);
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (DBG) Log(String("isDummyProfileNeeded: radioTechFam = ") + StringUtils::ToString(radioTechFam));
    // If uicc app family based on data rat is unknown,
    // check if records selected is RuimRecords.
    *result = (radioTechFam == IUiccController::APP_FAM_3GPP2 ||
            ((radioTechFam == IUiccController::APP_FAM_UNKNOWN) &&
            (r != NULL) && (IRuimRecords::Probe(r) != NULL)));
    return NOERROR;
}

ECode DcTracker::AddDummyApnSettings(
    /* [in] */ const String& _operator)
{
    // Create dummy data profiles.
    if (DBG) Log("createAllApnList: Creating dummy apn for cdma operator:%s", _operator.string());
    AutoPtr<ArrayOf<String> > defaultApnTypes = ArrayOf<String>::Alloc(7);
    (*defaultApnTypes)[0] = IPhoneConstants::APN_TYPE_DEFAULT;
    (*defaultApnTypes)[1] = IPhoneConstants::APN_TYPE_MMS;
    (*defaultApnTypes)[2] = IPhoneConstants::APN_TYPE_SUPL;
    (*defaultApnTypes)[3] = IPhoneConstants::APN_TYPE_HIPRI;
    (*defaultApnTypes)[4] = IPhoneConstants::APN_TYPE_FOTA;
    (*defaultApnTypes)[5] = IPhoneConstants::APN_TYPE_IMS;
    (*defaultApnTypes)[6] = IPhoneConstants::APN_TYPE_CBS;
    AutoPtr<ArrayOf<String> > dunApnTypes = ArrayOf<String>::Alloc(1);
    (*dunApnTypes)[0] = IPhoneConstants::APN_TYPE_DUN;
    AutoPtr<IApnSetting> apn;
    CApnSetting::New(IDctConstants::APN_DEFAULT_ID, _operator, String(NULL), String(NULL),
            String(NULL), String(NULL), String(NULL), String(NULL), String(NULL), String(NULL), String(NULL),
            IRILConstants::SETUP_DATA_AUTH_PAP_CHAP, defaultApnTypes,
            PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, TRUE, 0,
            0, FALSE, 0, 0, 0, IPhoneConstants::UNSET_MTU, String(""), String(""), (IApnSetting**)&apn);
    mAllApnSettings->Add(apn);
    apn = NULL;
    CApnSetting::New(IDctConstants::APN_DUN_ID, _operator, String(NULL), String(NULL),
            String(NULL), String(NULL), String(NULL), String(NULL), String(NULL), String(NULL), String(NULL),
            IRILConstants::SETUP_DATA_AUTH_PAP_CHAP, dunApnTypes,
            PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, TRUE, 0,
            0, FALSE, 0, 0, 0, IPhoneConstants::UNSET_MTU, String(""), String(""), (IApnSetting**)&apn);
    mAllApnSettings->Add(apn);
    return NOERROR;
}

ECode DcTracker::CreateDataConnection(
    /* [out] */ IDcAsyncChannel** result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("createDataConnection E");
    Int32 id;
    mUniqueIdGenerator->GetAndIncrement(&id);
    AutoPtr<IDataConnection> conn;
    DataConnection::MakeDataConnection(mPhone, id,
            this, mDcTesterFailBringUpAll, mDcc, (IDataConnection**)&conn);
    AutoPtr<IInteger32> iId;
    CInteger32::New(id, (IInteger32**)&iId);
    mDataConnections->Put(iId, conn);
    AutoPtr<IDcAsyncChannel> dcac;
    CDcAsyncChannel::New(conn, LOG__TAG, (IDcAsyncChannel**)&dcac);
    AutoPtr<IHandler> handler;
    IStateMachine::Probe(conn)->GetHandler((IHandler**)&handler);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    Int32 status;
    IAsyncChannel::Probe(dcac)->FullyConnectSync(context, this, handler, &status);
    if (status == IAsyncChannel::STATUS_SUCCESSFUL) {
        Int32 dataConnectionIdSync;
        dcac->GetDataConnectionIdSync(&dataConnectionIdSync);
        AutoPtr<IInteger32> iDataConnectionIdSync;
        CInteger32::New(dataConnectionIdSync, (IInteger32**)&iDataConnectionIdSync);
        mDataConnectionAcHashMap->Put(iDataConnectionIdSync, dcac);
    } else {
        Loge("createDataConnection: Could not connect to dcac=%s status=%d", TO_CSTR(dcac), status);
    }
    if (DBG) Log("createDataConnection() X id=%d dc=%s", id, TO_CSTR(conn));
    *result = dcac;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTracker::DestroyDataConnections()
{
    if (mDataConnections != NULL) {
        if (DBG) Log("destroyDataConnections: clear mDataConnectionList");
        mDataConnections->Clear();
    } else {
        if (DBG) Log("destroyDataConnections: mDataConnecitonList is empty, ignore");
    }
    return NOERROR;
}

ECode DcTracker::BuildWaitingApns(
    /* [in] */ const String& requestedApnType,
    /* [in] */ Int32 radioTech,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log("buildWaitingApns: E requestedApnType=%s", requestedApnType.string());
    AutoPtr<IArrayList> apnList;
    CArrayList::New((IArrayList**)&apnList);
    if (requestedApnType.Equals(IPhoneConstants::APN_TYPE_DUN)) {
        AutoPtr<IApnSetting> dun;
        FetchDunApn((IApnSetting**)&dun);
        if (dun != NULL) {
            apnList->Add(dun);
            if (DBG) Log("buildWaitingApns: X added APN_TYPE_DUN apnList=%s", TO_CSTR(apnList));
            *result = apnList;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    String _operator;
    GetOperatorNumeric(&_operator);
    // This is a workaround for a bug (7305641) where we don't failover to other
    // suitable APNs if our preferred APN fails.  On prepaid ATT sims we need to
    // failover to a provisioning APN, but once we've used their default data
    // connection we are locked to it for life.  This change allows ATT devices
    // to say they don't want to use preferred at all.
    Boolean usePreferred = TRUE;
    // try {
    ECode ec;
    do {
        AutoPtr<IContext> context;
        ec = IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        if (FAILED(ec)) break;
        AutoPtr<IResources> res;
        ec = context->GetResources((IResources**)&res);
        if (FAILED(ec)) break;
        ec = res->GetBoolean(R::bool_::config_dontPreferApn, &usePreferred);
        if (FAILED(ec)) break;
        usePreferred = ! usePreferred;
    } while(FALSE);
    // } catch (Resources.NotFoundException e) {
    if ((ECode) E_RESOURCES_NOT_FOUND_EXCEPTION == ec) {
        if (DBG) Log("buildWaitingApns: usePreferred NotFoundException set to true");
        usePreferred = TRUE;
    }
    // }
    if (usePreferred) {
        GetPreferredApn((IApnSetting**)&mPreferredApn);
    }
    if (DBG) {
        Log("buildWaitingApns: usePreferred=%d canSetPreferApn=%d mPreferredApn=%s operator=%s radioTech=%d",
                usePreferred, mCanSetPreferApn, TO_CSTR(mPreferredApn), _operator.string(), radioTech);
    }
    Boolean canHandleType;
    mPreferredApn->CanHandleType(requestedApnType, &canHandleType);
    if (usePreferred && mCanSetPreferApn && mPreferredApn != NULL && canHandleType) {
        String preferredApnNumeric;
        mPreferredApn->GetNumeric(&preferredApnNumeric);
        if (DBG) {
            Log("buildWaitingApns: Preferred APN:%s:%s:%s", _operator.string(),
                    preferredApnNumeric.string(), TO_CSTR(mPreferredApn));
        }
        if (preferredApnNumeric != NULL && preferredApnNumeric.Equals(_operator)) {
            Int32 preferredApnBearer;
            mPreferredApn->GetBearer(&preferredApnBearer);
            if (preferredApnBearer == 0 || preferredApnBearer == radioTech) {
                apnList->Add(mPreferredApn);
                if (DBG) Log("buildWaitingApns: X added preferred apnList=%s", TO_CSTR(apnList));
                *result = apnList;
                REFCOUNT_ADD(*result)
                return NOERROR;
            } else {
                if (DBG) Log("buildWaitingApns: no preferred APN");
                SetPreferredApn(-1);
                mPreferredApn = NULL;
            }
        } else {
            if (DBG) Log("buildWaitingApns: no preferred APN");
            SetPreferredApn(-1);
            mPreferredApn = NULL;
        }
    }
    Boolean isEmpty;
    mAllApnSettings->IsEmpty(&isEmpty);
    if (mAllApnSettings != NULL && !isEmpty) {
        if (DBG) Log("buildWaitingApns: mAllApnSettings=%s", TO_CSTR(mAllApnSettings));
        AutoPtr<IIterator> it;
        mAllApnSettings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
            if (DBG) Log("buildWaitingApns: apn=%s", TO_CSTR(apn));
            Boolean canHandleType;
            apn->CanHandleType(requestedApnType, &canHandleType);
            if (canHandleType) {
                Int32 apnBearer;
                apn->GetBearer(&apnBearer);
                if (apnBearer == 0 || apnBearer == radioTech) {
                    if (DBG) Log("buildWaitingApns: adding apn=%s", TO_CSTR(apn));
                    apnList->Add(apn);
                } else {
                    if (DBG) {
                        Log("buildWaitingApns: bearer:%d != radioTech:%d", apnBearer, radioTech);
                    }
                }
            } else {
                if (DBG) {
                    Log("buildWaitingApns: couldn't handle requesedApnType=%s", requestedApnType.string());
                }
            }
        }
    } else {
        Loge("mAllApnSettings is null!");
    }
    if (DBG) Log("buildWaitingApns: X apnList=%s", TO_CSTR(apnList));
    *result = apnList;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTracker::ApnListToString(
    /* [in] */ IArrayList* apns,
    /* [out] */ String* _result)
{
    VALIDATE_NOT_NULL(_result)

    if (apns == NULL) {
        Log("apnListToString: apns = null.");
        *_result = "";
        return NOERROR;
    }
    else {
        StringBuilder result;
        Int32 size;
        apns->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            apns->Get(i, (IInterface**)&obj);
            result.Append('[');
            result.Append(TO_STR(obj));
            result.Append(']');
        }
        return result.ToString(_result);
    }
    return NOERROR;
}

ECode DcTracker::SetPreferredApn(
    /* [in] */ Int32 pos)
{
    if (!mCanSetPreferApn) {
        Log("setPreferredApn: X !canSEtPreferApn");
        return NOERROR;
    }
    Log("setPreferredApn: delete");
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    GetUri(PREFERAPN_NO_UPDATE_URI, (IUri**)&uri);
    Int32 i32;
    resolver->Delete(uri, String(NULL), NULL, &i32);
    if (pos >= 0) {
        Log("setPreferredApn: insert");
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(APN_ID, pos);
        AutoPtr<IUri> iNoUse;
        resolver->Insert(uri, values, (IUri**)&iNoUse);
    }
    return NOERROR;
}

ECode DcTracker::GetPreferredApn(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isEmpty;
    mAllApnSettings->IsEmpty(&isEmpty);
    if (mAllApnSettings == NULL || isEmpty) {
        Log("getPreferredApn: mAllApnSettings is %s", ((mAllApnSettings == NULL) ? "null" : "empty"));
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(3);
    (*strs)[0] = String("_id");
    (*strs)[1] = String("name");
    (*strs)[2] = String("apn");
    AutoPtr<ICursor> cursor;
    AutoPtr<IUri> uri;
    GetUri(PREFERAPN_NO_UPDATE_URI, (IUri**)&uri);
    contentResolver->Query(
            uri, strs,
            String(NULL), NULL, ITelephonyCarriers::DEFAULT_SORT_ORDER, (ICursor**)&cursor);
    if (cursor != NULL) {
        mCanSetPreferApn = TRUE;
    } else {
        mCanSetPreferApn = FALSE;
    }
    Int32 count;
    cursor->GetCount(&count);
    Log("getPreferredApn: mRequestedApnType=%s cursor=%s cursor.count=%d",
            mRequestedApnType.string(), TO_CSTR(cursor),  ((cursor != NULL) ? count : 0));
    if (mCanSetPreferApn && count > 0) {
        Int32 pos;
        Boolean isMoveToFirst;
        cursor->MoveToFirst(&isMoveToFirst);
        Int32 columnIndexOrThrow_ID;
        cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &columnIndexOrThrow_ID);
        cursor->GetInt32(columnIndexOrThrow_ID, &pos);
        AutoPtr<IIterator> it;
        mAllApnSettings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IApnSetting> p = IApnSetting::Probe(obj);
            Log("getPreferredApn: apnSetting=%s", TO_CSTR(p));
            Boolean canHandleType;
            p->CanHandleType(mRequestedApnType, &canHandleType);
            Int32 pId;
            p->GetId(&pId);
            if (pId == pos && canHandleType) {
                Log("getPreferredApn: X found apnSetting%s", TO_CSTR(p));
                ICloseable::Probe(cursor)->Close();
                *result = p;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Log("getPreferredApn: X not found");
    *result = NULL;
    return NOERROR;
}

ECode DcTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) Log("handleMessage msg=%s", TO_CSTR(msg));
    if (!((PhoneBase*) mPhone.Get())->mIsTheCurrentActivePhone || mIsDisposed) {
        Loge("handleMessage: Ignore GSM msgs since GSM phone is inactive");
        return NOERROR;
    }
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    switch (msgWhat) {
        case IDctConstants::EVENT_RECORDS_LOADED:
            OnRecordsLoaded();
            break;
        case IDctConstants::EVENT_DATA_CONNECTION_DETACHED:
            OnDataConnectionDetached();
            break;
        case IDctConstants::EVENT_DATA_CONNECTION_ATTACHED:
            OnDataConnectionAttached();
            break;
        case IDctConstants::EVENT_DO_RECOVERY:
            DoRecovery();
            break;
        case IDctConstants::EVENT_APN_CHANGED:
            OnApnChanged();
            break;
        case IDctConstants::EVENT_PS_RESTRICT_ENABLED:
            /**
             * We don't need to explicitly to tear down the PDP context
             * when PS restricted is enabled. The base band will deactive
             * PDP context and notify us with PDP_CONTEXT_CHANGED.
             * But we should stop the network polling and prevent reset PDP.
             */
            if (DBG) Log("EVENT_PS_RESTRICT_ENABLED %d", mIsPsRestricted);
            StopNetStatPoll();
            StopDataStallAlarm();
            mIsPsRestricted = TRUE;
            break;
        case IDctConstants::EVENT_PS_RESTRICT_DISABLED:
            /**
             * When PS restrict is removed, we need setup PDP connection if
             * PDP connection is down.
             */
            if (DBG) Log("EVENT_PS_RESTRICT_DISABLED %d", mIsPsRestricted);
            mIsPsRestricted  = FALSE;
            Boolean isConnected;
            IsConnected(&isConnected);
            if (isConnected) {
                StartNetStatPoll();
                StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
            } else {
                // TODO: Should all PDN states be checked to fail?
                if (mState == DctConstantsState_FAILED) {
                    Boolean b;
                    CleanUpAllConnections(FALSE, IPhone::REASON_PS_RESTRICT_ENABLED, &b);
                    mReregisterOnReconnectFailure = FALSE;
                }
                AutoPtr<IInterface> obj;
                mApnContexts->Get(StringUtils::ParseCharSequence(IPhoneConstants::APN_TYPE_DEFAULT), (IInterface**)&obj);
                AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
                if (apnContext != NULL) {
                    apnContext->SetReason(IPhone::REASON_PS_RESTRICT_ENABLED);
                    Boolean b;
                    TrySetupData(apnContext, &b);
                } else {
                    Loge("**** Default ApnContext not found ****");
                    if (Build::IS_DEBUGGABLE) {
                        Logger::E(LOG__TAG, "Default ApnContext not found");
                        return E_RUNTIME_EXCEPTION;
                    }
                }
            }
            break;
        case IDctConstants::EVENT_TRY_SETUP_DATA:
            if (IApnContext::Probe(msgObj) != NULL) {
                Boolean b;
                OnTrySetupData(IApnContext::Probe(msgObj), &b);
            } else if (ICharSequence::Probe(msgObj) != NULL) {
                Boolean b;
                OnTrySetupData(TO_STR(msgObj), &b);
            } else {
                Loge("EVENT_TRY_SETUP request w/o apnContext or String");
            }
            break;
        case IDctConstants::EVENT_CLEAN_UP_CONNECTION: {
            Boolean tearDown = (msgArg1 == 0) ? FALSE : TRUE;
            if (DBG) Log("EVENT_CLEAN_UP_CONNECTION tearDown=%d", tearDown);
            if (IApnContext::Probe(msgObj) != NULL) {
                CleanUpConnection(tearDown, IApnContext::Probe(msgObj));
            } else {
                Loge("EVENT_CLEAN_UP_CONNECTION request w/o apn context, call super");
                DcTrackerBase::HandleMessage(msg);
            }
            break;
        }
        case IDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE: {
            Boolean enabled = (msgArg1 == IDctConstants::ENABLED) ? TRUE : FALSE;
            OnSetInternalDataEnabled(enabled, IMessage::Probe(msgObj));
            break;
        }
        case IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS: {
            AutoPtr<IMessage> mCause;
            ObtainMessage(IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS, NULL, (IMessage**)&mCause);
            if ((msgObj != NULL) && (ICharSequence::Probe(msgObj) != NULL)) {
                mCause->SetObj(msgObj);
            }
            DcTrackerBase::HandleMessage(mCause);
            break;
        }
        case IDctConstants::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED: // fall thru
        case IDctConstants::EVENT_DATA_RAT_CHANGED: {
            // When data rat changes we might need to load different
            // set of apns (example, LTE->1x)
            Boolean isNvSubscription;
            IsNvSubscription(&isNvSubscription);
            Boolean b;
            OnUpdateIcc(&b);
            if (b) {
                Log("onUpdateIcc: tryRestartDataConnections %s", IPhone::REASON_NW_TYPE_CHANGED.string());
                TryRestartDataConnections(TRUE, IPhone::REASON_NW_TYPE_CHANGED);
            } else if (isNvSubscription){
                // If cdma subscription source changed to NV or data rat changed to cdma
                // (while subscription source was NV) - we need to trigger NV ready
                OnNvReady();
            }
            break;
        }
        case IDctConstants::CMD_CLEAR_PROVISIONING_SPINNER:
            // Check message sender intended to clear the current spinner.
            if (mProvisioningSpinner == msgObj) {
                IDialogInterface::Probe(mProvisioningSpinner)->Dismiss();
                mProvisioningSpinner = NULL;
            }
            break;
        case IDctConstants::EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE:
            OnWwanIwlanCoexistenceDone(IAsyncResult::Probe(msgObj));
            break;
        case IDctConstants::EVENT_MODEM_DATA_PROFILE_READY:
            OnModemApnProfileReady();
            break;
        default:
            // handle the message in the super class DataConnectionTracker
            DcTrackerBase::HandleMessage(msg);
            break;
    }
    return NOERROR;
}

ECode DcTracker::GetApnProfileID(
    /* [in] */ const String& apnType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_IMS)) {
        *result = IRILConstants::DATA_PROFILE_IMS;
        return NOERROR;
    } else if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_FOTA)) {
        *result = IRILConstants::DATA_PROFILE_FOTA;
        return NOERROR;
    } else if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_CBS)) {
        *result = IRILConstants::DATA_PROFILE_CBS;
        return NOERROR;
    } else if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_IA)) {
        *result = IRILConstants::DATA_PROFILE_DEFAULT;
        return NOERROR; // DEFAULT for now
    } else if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_DUN)) {
        *result = IRILConstants::DATA_PROFILE_TETHERED;
        return NOERROR;
    } else {
        *result = IRILConstants::DATA_PROFILE_DEFAULT;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTracker::GetCellLocationId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 cid = -1;
    AutoPtr<ICellLocation> loc;
    IPhone::Probe(mPhone)->GetCellLocation((ICellLocation**)&loc);
    if (loc != NULL) {
        if (IGsmCellLocation::Probe(loc) != NULL) {
            IGsmCellLocation::Probe(loc)->GetCid(&cid);
        } else if (ICdmaCellLocation::Probe(loc) != NULL) {
            ICdmaCellLocation::Probe(loc)->GetBaseStationId(&cid);
        }
    }
    *result = cid;
    return NOERROR;
}

ECode DcTracker::GetUiccRecords(
    /* [in] */ Int32 appFamily,
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    return mUiccController->GetIccRecords(phoneId, appFamily, result);
}

ECode DcTracker::OnUpdateIcc(
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)

    Boolean result = FALSE;
    if (mUiccController == NULL ) {
        Loge("onUpdateIcc: mUiccController is null. Error!");
        *_result = FALSE;
        return NOERROR;
    }
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 dataRat;
    serviceState->GetRilDataRadioTechnology(&dataRat);

    AutoPtr<IUiccControllerHelper> ucHelper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&ucHelper);
    Int32 appFamily;
    ucHelper->GetFamilyFromRadioTechnology(dataRat, &appFamily);

    AutoPtr<IIccRecords> newIccRecords;
    GetUiccRecords(appFamily, (IIccRecords**)&newIccRecords);
    AutoPtr<IClassInfo> clsInfo;
    CObject::ReflectClassInfo(newIccRecords, (IClassInfo**)&clsInfo);
    String name;
    if (clsInfo != NULL)
        clsInfo->GetName(&name);
    Log("onUpdateIcc: newIccRecords %s, dataRat:%d", name.string(), dataRat);
    if (dataRat == IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN) {
        // Ignore this. This could be due to data not registered
        // We want to ignore RADIO_TECHNOLOGY_UNKNOWN so that we do not tear down data
        // call in case we are out of service.
        *_result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
    if (r != newIccRecords) {
        if (r != NULL) {
            AutoPtr<IClassInfo> clsInfo;
            CObject::ReflectClassInfo(r, (IClassInfo**)&clsInfo);
            String name;
            if (clsInfo != NULL)
                clsInfo->GetName(&name);
            Log("Removing stale icc objects. %s", name.string());
            r->UnregisterForRecordsLoaded(this);
            mIccRecords->Set(NULL);
        }
        if (newIccRecords != NULL) {
            AutoPtr<IClassInfo> clsInfo;
            CObject::ReflectClassInfo(newIccRecords, (IClassInfo**)&clsInfo);
            String name;
            if (clsInfo != NULL)
                clsInfo->GetName(&name);
            Log("New records found %s", name.string());
            mIccRecords->Set(newIccRecords);
            newIccRecords->RegisterForRecordsLoaded(
                    this, IDctConstants::EVENT_RECORDS_LOADED, NULL);
        }
        // Records changed -> return true
        result = TRUE;
    }
    *_result = result;
    return NOERROR;
}

ECode DcTracker::Update()
{
    Int64 subId;
    mPhone->GetSubId(&subId);
    Log("update sub = %lld", subId);
    Log("update(): Active DDS, register for all events now!");
    RegisterForAllEvents();
    Boolean b;
    OnUpdateIcc(&b);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    Int32 i32;
    Settings::Global::GetInt32(contentResolver,
            ISettingsGlobal::MOBILE_DATA + StringUtils::ToString(phoneId), 1, &i32);
    mUserDataEnabled = i32 == 1;
    if (ICDMALTEPhone::Probe(mPhone) != NULL) {
        assert(0 && "Need UpdateCurrentCarrierInProvider in ICDMALTEPhone");
        // ICDMALTEPhone::Probe(mPhone)->UpdateCurrentCarrierInProvider();
        SupplyMessenger();
    }
    else if (IGSMPhone::Probe(mPhone) != NULL) {
        Boolean b;
        IGSMPhone::Probe(mPhone)->UpdateCurrentCarrierInProvider(&b);
        SupplyMessenger();
    }
    else {
        Log("Phone object is not MultiSim. This should not hit!!!!");
    }
    return NOERROR;
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ const String& cause)
{
    return CleanUpAllConnections(cause, NULL);
}

ECode DcTracker::UpdateRecords()
{
    Boolean b;
    return OnUpdateIcc(&b);
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ const String& cause,
    /* [in] */ IMessage* disconnectAllCompleteMsg)
{
    Log("cleanUpAllConnections");
    if (disconnectAllCompleteMsg != NULL) {
        mDisconnectAllCompleteMsgList->Add(disconnectAllCompleteMsg);
    }
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS, (IMessage**)&msg);
    msg->SetObj(StringUtils::ParseCharSequence(cause));
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTracker::NotifyDataDisconnectComplete()
{
    Log("notifyDataDisconnectComplete");
    AutoPtr<IIterator> it;
    mDisconnectAllCompleteMsgList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMessage> m = IMessage::Probe(obj);
        m->SendToTarget();
    }
    mDisconnectAllCompleteMsgList->Clear();
    return NOERROR;
}

ECode DcTracker::NotifyAllDataDisconnected()
{
    sEnableFailFastRefCounter = 0;
    mFailFast = FALSE;
    ((RegistrantList*) mAllDataDisconnectedRegistrants.Get())->NotifyRegistrants();
    return NOERROR;
}

ECode DcTracker::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    ((RegistrantList*) mAllDataDisconnectedRegistrants.Get())->AddUnique(h, what, obj);
    Boolean isDisconnected;
    IsDisconnected(&isDisconnected);
    if (isDisconnected) {
        Log("notify All Data Disconnected");
        NotifyAllDataDisconnected();
    }
    return NOERROR;
}

ECode DcTracker::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    ((RegistrantList*) mAllDataDisconnectedRegistrants.Get())->Remove(h);
    return NOERROR;
}

ECode DcTracker::OnSetInternalDataEnabled(
    /* [in] */ Boolean enable)
{
    return OnSetInternalDataEnabled(enable, NULL);
}

ECode DcTracker::OnSetInternalDataEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ IMessage* onCompleteMsg)
{
    Boolean sendOnComplete = TRUE;
    {
        AutoLock lock(mDataEnabledLock);
        mInternalDataEnabled = enabled;
        if (enabled) {
            Log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
            Boolean b;
            OnTrySetupData(IPhone::REASON_DATA_ENABLED, &b);
        } else {
            sendOnComplete = FALSE;
            Log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
            CleanUpAllConnections(String(NULL), onCompleteMsg);
        }
    }
    if (sendOnComplete) {
        if (onCompleteMsg != NULL) {
            onCompleteMsg->SendToTarget();
        }
    }
    return NOERROR;
}

ECode DcTracker::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG)
        Log("setInternalDataEnabledFlag(%d)", enable);
    if (mInternalDataEnabled != enable) {
        mInternalDataEnabled = enable;
    }
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    return SetInternalDataEnabled(enable, NULL, result);
}

ECode DcTracker::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG)
        Log("setInternalDataEnabled(%d)", enable);
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE, onCompleteMsg, (IMessage**)&msg);
    msg->SetArg1(enable ? IDctConstants::ENABLED : IDctConstants::DISABLED);
    Boolean b;
    SendMessage(msg, &b);
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::IsActiveDataSubscription(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // FIXME This should have code like
    // return (mPhone.getSubId() == SubscriptionManager.getDefaultDataSubId());
    *result = TRUE;
    return NOERROR;
}

ECode DcTracker::SetDataAllowed(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* response)
{
    mIsCleanupRequired = !enable;
    ((PhoneBase*) mPhone.Get())->mCi->SetDataAllowed(enable, response);
    mInternalDataEnabled = enable;
    return NOERROR;
}

ECode DcTracker::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    return Logger::D(LOG__TAG, "[%d]%s", phoneId, msgBuf);
}

ECode DcTracker::Loge(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    return Logger::E(LOG__TAG, "[%d]%s", phoneId, msgBuf);
}

ECode DcTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("DataConnectionTracker extends:"));
    DcTrackerBase::Dump(fd, pw, args);
    pw->Println(String(" mReregisterOnReconnectFailure=") + StringUtils::ToString(mReregisterOnReconnectFailure));
    pw->Println(String(" canSetPreferApn=") + StringUtils::ToString(mCanSetPreferApn));
    pw->Println(String(" mApnObserver=") + TO_STR(TO_IINTERFACE(mApnObserver)));
    DctConstantsState overallState;
    GetOverallState(&overallState);
    pw->Println(String(" getOverallState=") + StringUtils::ToString(overallState));
    pw->Println(String(" mDataConnectionAsyncChannels=%s\n") + TO_STR(mDataConnectionAcHashMap));
    Boolean b;
    mAttached->Get(&b);
    pw->Println(String(" mAttached=") + StringUtils::ToString(b));
    return NOERROR;
}

ECode DcTracker::GetPcscfAddress(
    /* [in] */ const String& apnType,
    /* [out, callee] */ ArrayOf<String>** _result)
{
    VALIDATE_NOT_NULL(_result)

    Log("getPcscfAddress()");
    AutoPtr<IApnContext> apnContext;
    if (apnType == NULL){
        Log("apnType is null, return null");
        *_result = NULL;
        return NOERROR;
    }
    if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_EMERGENCY)) {
        AutoPtr<IInterface> obj;
        mApnContexts->Get(StringUtils::ParseCharSequence(IPhoneConstants::APN_TYPE_EMERGENCY), (IInterface**)&obj);
        apnContext = IApnContext::Probe(obj);
    } else if (TextUtils::Equals(apnType, IPhoneConstants::APN_TYPE_IMS)) {
        AutoPtr<IInterface> obj;
        mApnContexts->Get(StringUtils::ParseCharSequence(IPhoneConstants::APN_TYPE_IMS), (IInterface**)&obj);
        apnContext = IApnContext::Probe(obj);
    } else {
        Log("apnType is invalid, return null");
        *_result = NULL;
        return NOERROR;
    }
    if (apnContext == NULL) {
        Log("apnContext is null, return null");
        *_result = NULL;
        return NOERROR;
    }
    AutoPtr<IDcAsyncChannel> dcac;
    apnContext->GetDcAc((IDcAsyncChannel**)&dcac);
    AutoPtr<ArrayOf<String> > result;
    if (dcac != NULL) {
        dcac->GetPcscfAddr((ArrayOf<String>**)&result);
        for (Int32 i = 0; i < result->GetLength(); i++) {
            Log("Pcscf[%d]: %s", i, (*result)[i].string());
        }
        *_result = result;
        REFCOUNT_ADD(*_result)
        return NOERROR;
    }
    *_result = NULL;
    return NOERROR;
}

ECode DcTracker::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    Log("setImsRegistrationState - mImsRegistrationState(before): %d"
            ", registered(current) : %d", mImsRegistrationState, registered);
    if (mPhone == NULL) return NOERROR;
    AutoPtr<IServiceStateTracker> sst;
    mPhone->GetServiceStateTracker((IServiceStateTracker**)&sst);
    if (sst == NULL) return NOERROR;
    sst->SetImsRegistrationState(registered);
    return NOERROR;
}

ECode DcTracker::InitEmergencyApnSetting()
{
    // Operator Numeric is not available when sim records are not loaded.
    // Query Telephony.db with APN type as EPDN request does not
    // require APN name, plmn and all operators support same APN config.
    // DB will contain only one entry for Emergency APN
    String selection("type=\"emergency\"");
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ICursor> cursor;
    contentResolver->Query(
            Elastos::Droid::Provider::Telephony::Carriers::CONTENT_URI, NULL, selection, NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        Int32 count;
        cursor->GetCount(&count);
        if (count > 0) {
            Boolean isMoveToFirst;
            if (cursor->MoveToFirst(&isMoveToFirst)) {
                MakeApnSetting(cursor, (IApnSetting**)&mEmergencyApn);
            }
        }
        ICloseable::Probe(cursor)->Close();
    }
    return NOERROR;
}

ECode DcTracker::AddEmergencyApnSetting()
{
    if (mEmergencyApn != NULL) {
        if (mAllApnSettings == NULL) {
            CArrayList::New((IArrayList**)&mAllApnSettings);
        } else {
            Boolean hasEmergencyApn = FALSE;
            AutoPtr<IIterator> it;
            mAllApnSettings->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
                AutoPtr<ArrayOf<String> > types;
                apn->GetTypes((ArrayOf<String>**)&types);
                if (ArrayUtils::Contains(types.Get(), IPhoneConstants::APN_TYPE_EMERGENCY)) {
                    hasEmergencyApn = TRUE;
                    break;
                }
            }
            if (hasEmergencyApn == FALSE) {
                mAllApnSettings->Add(mEmergencyApn);
            } else {
                Log("addEmergencyApnSetting - E-APN setting is already present");
            }
        }
    }
    return NOERROR;
}

ECode DcTracker::GetUri(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return Uri::WithAppendedPath(uri, String("/subId/") + StringUtils::ToString(mSubId), result);
}

ECode DcTracker::GetImsRegistrationState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mImsRegistrationState;
    return NOERROR;
}

AutoPtr<IUri> DcTracker::InitPREFERAPN_NO_UPDATE_URI()
{
    AutoPtr<IUri> rev;
    Uri::Parse(String("content://telephony/carriers/preferapn_no_update"), (IUri**)&rev);
    return rev;
}

String DcTracker::InitPROPERTY_CDMA_IPPROTOCOL()
{
    String rev;
    SystemProperties::Get(
            String("persist.telephony.cdma.protocol"), String("IP"), &rev);
    return rev;
}

String DcTracker::InitPROPERTY_CDMA_ROAMING_IPPROTOCOL()
{
    String rev;
    SystemProperties::Get(
            String("persist.telephony.cdma.rproto"), String("IP"), &rev);
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
