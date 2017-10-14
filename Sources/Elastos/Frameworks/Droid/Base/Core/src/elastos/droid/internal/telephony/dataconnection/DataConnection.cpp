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

#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/R.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcRetryAlarmController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/DataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/DcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DcController.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailBringUp.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/DcRetryAlarmController.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTesterFailBringUpAll.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTrackerBase.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/NetworkAgent.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Patterns.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <Elastos.CoreLibrary.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::Patterns;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::CSystem;
using Elastos::Core::CThrowable;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::IThrowable;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DataConnection::ConnectionParams
//=============================================================================
ECode DataConnection::ConnectionParams::constructor(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Int32 initialMaxRetry,
    /* [in] */ Int32 profileId,
    /* [in] */ Int32 rilRadioTechnology,
    /* [in] */ Boolean retryWhenSSChange,
    /* [in] */ IMessage* onCompletedMsg)
{
    mApnContext = apnContext;
    mInitialMaxRetry = initialMaxRetry;
    mProfileId = profileId;
    mRilRat = rilRadioTechnology;
    mRetryWhenSSChange = retryWhenSSChange;
    mOnCompletedMsg = onCompletedMsg;
    return NOERROR;
}

ECode DataConnection::ConnectionParams::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    String msg;
    DataConnection::MsgToString(mOnCompletedMsg, &msg);
    rev.AppendFormat("{mTag=%d mApnContext=%s mInitialMaxRetry=%d mProfileId=%d mRat=%d mOnCompletedMsg=%s}",
            mTag, TO_CSTR(mApnContext), mInitialMaxRetry, mProfileId, mRilRat, msg.string());
    *result = rev;
    return NOERROR;
}

//=============================================================================
// DataConnection::DisconnectParams
//=============================================================================
ECode DataConnection::DisconnectParams::constructor(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ const String& reason,
    /* [in] */ IMessage* onCompletedMsg)
{
    mApnContext = apnContext;
    mReason = reason;
    mOnCompletedMsg = onCompletedMsg;
    return NOERROR;
}

ECode DataConnection::DisconnectParams::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    String msg;
    DataConnection::MsgToString(mOnCompletedMsg, &msg);
    rev.AppendFormat("{mTag=%d" " mApnContext=%s" " mReason=%s" " mOnCompletedMsg=%s" "}",
            mTag, TO_CSTR(mApnContext), mReason.string(), msg.string());
    *result = rev;
    return NOERROR;
}

//=============================================================================
// DataConnection::UpdateLinkPropertyResult
//=============================================================================
DataConnection::UpdateLinkPropertyResult::UpdateLinkPropertyResult(
    /* [in] */ ILinkProperties* curLp)
    : mSetupResult(new DataCallResponse::SetupResult())
{
    ((DataCallResponse::SetupResult*) mSetupResult.Get())->mEnumValue = DataCallResponse_SUCCESS;
    mOldLp = curLp;
    mNewLp = curLp;
}

//=============================================================================
// DataConnection::DcDefaultState
//=============================================================================
DataConnection::DcDefaultState::DcDefaultState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcDefaultState::Enter()
{
    if (DBG) mHost->Log("DcDefaultState: enter");
    // Register for DRS or RAT change
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mHost->mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);

    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    serviceStateTracker->RegisterForDataRegStateOrRatChanged(handler,
            DataConnection::EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED, NULL);
    serviceStateTracker->RegisterForRoamingOn(handler,
            DataConnection::EVENT_DATA_CONNECTION_ROAM_ON, NULL);
    serviceStateTracker->RegisterForRoamingOff(handler,
            DataConnection::EVENT_DATA_CONNECTION_ROAM_OFF, NULL);
    // Add ourselves to the list of data connections
    ((DcController*) (mHost->mDcController.Get()))->AddDc(mHost);
    return NOERROR;
}

ECode DataConnection::DcDefaultState::Exit()
{
    if (DBG) mHost->Log("DcDefaultState: exit");
    // Unregister for DRS or RAT change.
    AutoPtr<IServiceStateTracker> serviceStateTracker;
    mHost->mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    serviceStateTracker->UnregisterForDataRegStateOrRatChanged(handler);
    serviceStateTracker->UnregisterForRoamingOn(handler);
    serviceStateTracker->UnregisterForRoamingOff(handler);
    // Remove ourselves from the DC lists
    ((DcController*) (mHost->mDcController.Get()))->RemoveDc(mHost);
    if (mHost->mAc != NULL) {
        mHost->mAc->Disconnected();
        mHost->mAc = NULL;
    }
    ((DcRetryAlarmController*) (mHost->mDcRetryAlarmController.Get()))->Dispose();
    mHost->mDcRetryAlarmController = NULL;
    mHost->mApnContexts = NULL;
    mHost->mReconnectIntent = NULL;
    mHost->mDct = NULL;
    mHost->mApnSetting = NULL;
    mHost->mPhone = NULL;
    mHost->mLinkProperties = NULL;
    mHost->mLastFailCause = NULL;
    mHost->mUserData = NULL;
    mHost->mDcController = NULL;
    mHost->mDcTesterFailBringUpAll = NULL;
    return NOERROR;
}

ECode DataConnection::DcDefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    Boolean retVal = HANDLED;
    if (VDBG) {
        Int32 size;
        mHost->mApnContexts->GetSize(&size);
        String s = mHost->GetWhatToString(msgWhat);
        mHost->Log("DcDefault msg= %s RefCount=%d", s.string(), size);
    }
    if (msgWhat == IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION) {
        if (mHost->mAc != NULL) {
            if (VDBG) mHost->Log("Disconnecting to previous connection mHost->mAc=%s", TO_CSTR(mHost->mAc));
            mHost->mAc->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                    IAsyncChannel::STATUS_FULL_CONNECTION_REFUSED_ALREADY_CONNECTED);
        } else {
            CAsyncChannel::New((IAsyncChannel**)&mHost->mAc);
            AutoPtr<IHandler> handler;
            mHost->GetHandler((IHandler**)&handler);
            AutoPtr<IMessenger> messenger;
            msg->GetReplyTo((IMessenger**)&messenger);
            mHost->mAc->Connected(NULL, handler, messenger);
            if (VDBG) mHost->Log("DcDefaultState: FULL_CONNECTION reply connected");
            mHost->mAc->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                    IAsyncChannel::STATUS_SUCCESSFUL, mHost->mId, StringUtils::ParseCharSequence(String("hi")));
        }
    }
    else if (msgWhat == IAsyncChannel::CMD_CHANNEL_DISCONNECTED) {
        if (VDBG) mHost->Log("CMD_CHANNEL_DISCONNECTED");
        mHost->Quit();
    }
    else if (msgWhat == IDcAsyncChannel::REQ_IS_INACTIVE) {
        Boolean val;
        mHost->GetIsInactive(&val);
        if (VDBG) mHost->Log("REQ_IS_INACTIVE  isInactive=%d", val);
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_IS_INACTIVE, val ? 1 : 0);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_GET_CID) {
        Int32 cid;
        mHost->GetCid(&cid);
        if (VDBG) mHost->Log("REQ_GET_CID  cid=%d", cid);
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_GET_CID, cid);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_GET_APNSETTING) {
        AutoPtr<IApnSetting> apnSetting;
        mHost->GetApnSetting((IApnSetting**)&apnSetting);
        if (VDBG) mHost->Log("REQ_GET_APNSETTING  mApnSetting=%s", TO_CSTR(apnSetting));
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_GET_APNSETTING, apnSetting);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_GET_LINK_PROPERTIES) {
        AutoPtr<ILinkProperties> lp;
        mHost->GetCopyLinkProperties((ILinkProperties**)&lp);
        if (VDBG) mHost->Log("REQ_GET_LINK_PROPERTIES linkProperties %s", TO_CSTR(lp));
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_GET_LINK_PROPERTIES, lp);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_SET_LINK_PROPERTIES_HTTP_PROXY) {
        AutoPtr<IProxyInfo> proxy = IProxyInfo::Probe(msgObj);
        if (VDBG) mHost->Log("REQ_SET_LINK_PROPERTIES_HTTP_PROXY proxy=%s", TO_CSTR(proxy));
        mHost->SetLinkPropertiesHttpProxy(proxy);
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_SET_LINK_PROPERTIES_HTTP_PROXY);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_GET_NETWORK_CAPABILITIES) {
        AutoPtr<INetworkCapabilities> nc;
        mHost->GetCopyNetworkCapabilities((INetworkCapabilities**)&nc);
        if (VDBG) mHost->Log("REQ_GET_NETWORK_CAPABILITIES networkCapabilities%s", TO_CSTR(nc));
        mHost->mAc->ReplyToMessage(msg, IDcAsyncChannel::RSP_GET_NETWORK_CAPABILITIES, nc);
    }
    else if (msgWhat == IDcAsyncChannel::REQ_RESET) {
        if (VDBG) mHost->Log("DcDefaultState: msg.what=REQ_RESET");
        mHost->TransitionTo(mHost->mInactiveState);
    }
    else if (msgWhat == EVENT_CONNECT) {
        if (DBG) mHost->Log("DcDefaultState: msg.what=EVENT_CONNECT, fail not expected");
        AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(msgObj);
        AutoPtr<IDcFailCause> dfc;
        DcFailCause::FromInt32(DcFailCause_UNKNOWN, (IDcFailCause**)&dfc);
        mHost->NotifyConnectCompleted(cp, dfc, FALSE);
    }
    else if (msgWhat == EVENT_DISCONNECT) {
        if (DBG) {
            Int32 size;
            mHost->mApnContexts->GetSize(&size);
            mHost->Log("DcDefaultState deferring msg.what=EVENT_DISCONNECT RefCount=%d", size);
        }
        mHost->DeferMessage(msg);
    }
    else if (msgWhat == EVENT_DISCONNECT_ALL) {
        if (DBG) {
            Int32 size;
            mHost->mApnContexts->GetSize(&size);
            mHost->Log("DcDefaultState deferring msg.what=EVENT_DISCONNECT_ALL RefCount=%d",
                    size);
        }
        mHost->DeferMessage(msg);
    }
    else if (msgWhat == EVENT_TEAR_DOWN_NOW) {
        if (DBG) mHost->Log("DcDefaultState EVENT_TEAR_DOWN_NOW");
        ((PhoneBase*) mHost->mPhone.Get())->mCi->DeactivateDataCall(mHost->mCid, 0,  NULL);
    }
    else if (msgWhat == EVENT_LOST_CONNECTION) {
        if (DBG) {
            String s;
            s.AppendFormat("DcDefaultState ignore EVENT_LOST_CONNECTION"
                    " tag=%d:mTag=%d", msgArg1, mHost->mTag);
            mHost->LogAndAddLogRec(s);
        }
    }
    else if (msgWhat == EVENT_RETRY_CONNECTION) {
        if (DBG) {
            String s;
            s.AppendFormat("DcDefaultState ignore EVENT_RETRY_CONNECTION"
                    " tag=%d:mTag=%d", msgArg1, mHost->mTag);
            mHost->LogAndAddLogRec(s);
        }
    }
    else if (msgWhat == EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED) {
        AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
        AutoPtr<IPair> drsRatPair = IPair::Probe(((AsyncResult*) ar.Get())->mResult);
        AutoPtr<IInterface> obj;
        drsRatPair->GetFirst((IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&mHost->mDataRegState);
        AutoPtr<IInterface > second;
        drsRatPair->GetSecond((IInterface**)&second);
        Int32 iSecond;
        IInteger32::Probe(second)->GetValue(&iSecond);
        if (mHost->mRilRat != iSecond) {
            AutoPtr<IInterface> obj;
            drsRatPair->GetSecond((IInterface**)&obj);
            Int32 i32;
            IInteger32::Probe(obj)->GetValue(&i32);
            mHost->UpdateTcpBufferSizes(i32);
        }
        mHost->mRilRat = iSecond;
        if (DBG) {
            mHost->Log("DcDefaultState: EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                    " drs=%d mRilRat=%d", mHost->mDataRegState, mHost->mRilRat);
        }
        AutoPtr<IServiceState> ss;
        IPhone::Probe(mHost->mPhone)->GetServiceState((IServiceState**)&ss);
        Int32 networkType;
        ss->GetDataNetworkType(&networkType);
        String name;
        CTelephonyManager::GetNetworkTypeName(networkType, &name);
        mHost->mNetworkInfo->SetSubtype(networkType, name);
        if (mHost->mNetworkAgent != NULL) {
            AutoPtr<INetworkCapabilities> networkCapabilities;
            mHost->MakeNetworkCapabilities((INetworkCapabilities**)&networkCapabilities);
            mHost->mNetworkAgent->SendNetworkCapabilities(networkCapabilities);
            mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);
            mHost->mNetworkAgent->SendLinkProperties(mHost->mLinkProperties);
        }
    }
    else if (msgWhat == EVENT_DATA_CONNECTION_ROAM_ON) {
        mHost->mNetworkInfo->SetRoaming(TRUE);
    }
    else if (msgWhat == EVENT_DATA_CONNECTION_ROAM_OFF) {
        mHost->mNetworkInfo->SetRoaming(FALSE);
    }
    else {
        if (DBG) {
            String s = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcDefaultState: shouldn't happen but ignore msg.what=", s.string());
        }
    }
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcDefaultState::GetName()
{
    return String("DcDefaultState");
}

//=============================================================================
// DataConnection::DcInactiveState
//=============================================================================
DataConnection::DcInactiveState::DcInactiveState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcInactiveState::SetEnterNotificationParams(
    /* [in] */ ConnectionParams* cp,
    /* [in] */ IDcFailCause* cause)
{
    if (VDBG) mHost->Log("DcInactiveState: setEnterNoticationParams cp,cause");
    mHost->mConnectionParams = cp;
    mHost->mDisconnectParams = NULL;
    mHost->mDcFailCause = cause;
    return NOERROR;
}

ECode DataConnection::DcInactiveState::SetEnterNotificationParams(
    /* [in] */ DisconnectParams* dp)
{
    if (VDBG) mHost->Log("DcInactiveState: setEnterNoticationParams dp");
    mHost->mConnectionParams = NULL;
    mHost->mDisconnectParams = dp;
    DcFailCause::FromInt32(DcFailCause_NONE, (IDcFailCause**)&mHost->mDcFailCause);
    return NOERROR;
}

ECode DataConnection::DcInactiveState::SetEnterNotificationParams(
    /* [in] */ IDcFailCause* cause)
{
    mHost->mConnectionParams = NULL;
    mHost->mDisconnectParams = NULL;
    mHost->mDcFailCause = cause;
    return NOERROR;
}

ECode DataConnection::DcInactiveState::Enter()
{
    mHost->mTag += 1;
    if (DBG) mHost->Log("DcInactiveState: enter() mTag=%d", mHost->mTag);
    if (mHost->mConnectionParams != NULL) {
        if (DBG) {
            mHost->Log("DcInactiveState: enter notifyConnectCompleted +ALL failCause=%s", TO_CSTR(mHost->mDcFailCause));
        }
        mHost->NotifyConnectCompleted(mHost->mConnectionParams, mHost->mDcFailCause, TRUE);
    }
    if (mHost->mDisconnectParams != NULL) {
        if (DBG) {
            mHost->Log("DcInactiveState: enter notifyDisconnectCompleted +ALL failCause=%d"
                    , TO_CSTR(mHost->mDcFailCause));
        }
        mHost->NotifyDisconnectCompleted(mHost->mDisconnectParams, TRUE);
    }
    if (mHost->mDisconnectParams == NULL && mHost->mConnectionParams == NULL && mHost->mDcFailCause != NULL) {
        if (DBG) {
            mHost->Log("DcInactiveState: enter notifyAllDisconnectCompleted failCause=%s"
                    , TO_CSTR(mHost->mDcFailCause));
        }
        mHost->NotifyAllDisconnectCompleted(mHost->mDcFailCause);
    }
    // Remove ourselves from cid mapping, before clearSettings
    ((DcController*) (mHost->mDcController.Get()))->RemoveActiveDcByCid(mHost);
    mHost->ClearSettings();
    return NOERROR;
}

ECode DataConnection::DcInactiveState::Exit()
{
    return NOERROR;
}

ECode DataConnection::DcInactiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Boolean retVal;
    if (msgWhat == IDcAsyncChannel::REQ_RESET) {
        if (DBG) {
            mHost->Log("DcInactiveState: msg.what=RSP_RESET, ignore we're already reset");
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CONNECT) {
        if (DBG) mHost->Log("DcInactiveState: mag.what=EVENT_CONNECT");
        AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(msgObj);
        Boolean isInitConnectionOk;
        mHost->InitConnection(cp, &isInitConnectionOk);
        if (isInitConnectionOk) {
            mHost->OnConnect(mHost->mConnectionParams);
            mHost->TransitionTo(mHost->mActivatingState);
        } else {
            if (DBG) {
                mHost->Log("DcInactiveState: msg.what=EVENT_CONNECT initConnection failed");
            }
            AutoPtr<IDcFailCause> dfc;
            DcFailCause::FromInt32(DcFailCause_UNACCEPTABLE_NETWORK_PARAMETER, (IDcFailCause**)&dfc);
            mHost->NotifyConnectCompleted(cp, dfc, FALSE);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DISCONNECT) {
        if (DBG) mHost->Log("DcInactiveState: msg.what=EVENT_DISCONNECT");
        mHost->NotifyDisconnectCompleted((DisconnectParams*) IObject::Probe(msgObj), FALSE);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DISCONNECT_ALL) {
        if (DBG) mHost->Log("DcInactiveState: msg.what=EVENT_DISCONNECT_ALL");
        mHost->NotifyDisconnectCompleted((DisconnectParams*) IObject::Probe(msgObj), FALSE);
        retVal = HANDLED;
    } else {
        if (VDBG) {
            String whatToString = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcInactiveState nothandled msg.what=%s", whatToString.string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcInactiveState::GetName()
{
    return String("DataConnection::DcInactiveState");
}

//=============================================================================
// DataConnection::DcRetryingState
//=============================================================================
DataConnection::DcRetryingState::DcRetryingState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcRetryingState::Enter()
{
    if ((mHost->mConnectionParams->mRilRat != mHost->mRilRat)
            || (mHost->mDataRegState != IServiceState::STATE_IN_SERVICE)){
        // RAT has changed or we're not in service so don't even begin retrying.
        if (DBG) {
            String s;
            s.AppendFormat("DcRetryingState: enter() not retrying rat changed"
                    ", mConnectionParams.mRilRat=%d != mRilRat:%d transitionTo(mInactiveState)",
                    mHost->mConnectionParams->mRilRat, mHost->mRilRat);
            mHost->LogAndAddLogRec(s);
        }
        AutoPtr<IDcFailCause> dfc;
        DcFailCause::FromInt32(DcFailCause_LOST_CONNECTION, (IDcFailCause**)&dfc);
        mHost->mInactiveState->SetEnterNotificationParams(dfc);
        mHost->TransitionTo(mHost->mInactiveState);
    } else {
        if (DBG) {
            mHost->Log("DcRetryingState: enter() mTag=%d"
                ", call notifyAllOfDisconnectDcRetrying lostConnection", mHost->mTag);
        }
        mHost->NotifyAllOfDisconnectDcRetrying(IPhone::REASON_LOST_DATA_CONNECTION);
        // Remove ourselves from cid mapping
        ((DcController*) (mHost->mDcController.Get()))->RemoveActiveDcByCid(mHost);
        mHost->mCid = -1;
    }
    return NOERROR;
}

ECode DataConnection::DcRetryingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    Boolean retVal;
    do {
        if (msgWhat == EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED) {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<IPair> drsRatPair = IPair::Probe(((AsyncResult*) ar.Get())->mResult);
            AutoPtr<IInterface > obj;
            drsRatPair->GetFirst((IInterface**)&obj);
            Int32 drs;
            IInteger32::Probe(obj)->GetValue(&drs);
            obj = NULL;
            drsRatPair->GetSecond((IInterface**)&obj);
            Int32 rat;
            IInteger32::Probe(obj)->GetValue(&rat);
            if ((rat == mHost->mRilRat) && (drs == mHost->mDataRegState)) {
                if (DBG) {
                    mHost->Log("DcRetryingState: EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                            " strange no change in drs=%d"
                            " rat=%d" " ignoring",
                            drs, rat);
                }
            } else {
                // have to retry connecting since no attach event will come
                if (mHost->mConnectionParams->mRetryWhenSSChange) {
                    retVal = NOT_HANDLED;
                    break;
                }
                if (drs != IServiceState::STATE_IN_SERVICE) {
                    // We've lost the connection and we're retrying but DRS or RAT changed
                    // so we may never succeed, might as well give up.
                    AutoPtr<IDcFailCause> dfc;
                    DcFailCause::FromInt32(DcFailCause_LOST_CONNECTION, (IDcFailCause**)&dfc);
                    mHost->mInactiveState->SetEnterNotificationParams(dfc);
                    mHost->DeferMessage(msg);
                    mHost->TransitionTo(mHost->mInactiveState);
                    if (DBG) {
                        String s;
                        s.AppendFormat("DcRetryingState: "
                                "EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                                " giving up changed from %d"
                                " to rat=%d"
                                " or drs changed from %d" " to drs=%d", mHost->mRilRat, rat, mHost->mDataRegState, drs);
                        mHost->LogAndAddLogRec(s);
                    }
                }
                mHost->mDataRegState = drs;
                mHost->mRilRat = rat;
                // TODO - pass the other type here too?
                AutoPtr<IServiceState> ss;
                IPhone::Probe(mHost->mPhone)->GetServiceState((IServiceState**)&ss);
                Int32 networkType;
                ss->GetDataNetworkType(&networkType);
                String name;
                CTelephonyManager::GetNetworkTypeName(networkType, &name);
                mHost->mNetworkInfo->SetSubtype(networkType, name);
            }
            retVal = HANDLED;
        }
        else if (msgWhat == EVENT_RETRY_CONNECTION) {
            if (msgArg1 == mHost->mTag) {
                mHost->mRetryManager->IncreaseRetryCount();
                if (DBG) {
                    Int32 retryCount;
                    mHost->mRetryManager->GetRetryCount(&retryCount);
                    mHost->Log("DcRetryingState EVENT_RETRY_CONNECTION"
                            " RetryCount=%d"
                            " mConnectionParams=%s", retryCount, TO_CSTR(TO_IINTERFACE(mHost->mConnectionParams)));
                }
                mHost->OnConnect(mHost->mConnectionParams);
                mHost->TransitionTo(mHost->mActivatingState);
            } else {
                if (DBG) {
                    mHost->Log("DcRetryingState stale EVENT_RETRY_CONNECTION"
                            " tag:%d" " != mTag:%d", msgArg1, mHost->mTag);
                }
            }
            retVal = HANDLED;
        }
        else if (msgWhat == IDcAsyncChannel::REQ_RESET) {
            if (DBG) {
                mHost->Log("DcRetryingState: msg.what=RSP_RESET, ignore we're already reset");
            }
            AutoPtr<IDcFailCause> dfc;
            DcFailCause::FromInt32(DcFailCause_RESET_BY_FRAMEWORK, (IDcFailCause**)&dfc);
            mHost->mInactiveState->SetEnterNotificationParams(mHost->mConnectionParams, dfc);
            mHost->TransitionTo(mHost->mInactiveState);
            retVal = HANDLED;
        }
        else if (msgWhat == EVENT_CONNECT) {
            AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(msgObj);
            if (DBG) {
                Int32 size;
                mHost->mApnContexts->GetSize(&size);
                mHost->Log("DcRetryingState: msg.what=EVENT_CONNECT RefCount=%d cp=%s"
                        " mConnectionParams=%s", size, TO_CSTR(cp), TO_CSTR(mHost->mConnectionParams));
            }
            Boolean isInitConnectionOk;
            mHost->InitConnection(cp, &isInitConnectionOk);
            if (isInitConnectionOk) {
                mHost->OnConnect(mHost->mConnectionParams);
                mHost->TransitionTo(mHost->mActivatingState);
            } else {
                if (DBG) {
                    mHost->Log("DcRetryingState: msg.what=EVENT_CONNECT initConnection failed");
                }
                AutoPtr<IDcFailCause> dfc;
                DcFailCause::FromInt32(DcFailCause_UNACCEPTABLE_NETWORK_PARAMETER, (IDcFailCause**)&dfc);
                mHost->NotifyConnectCompleted(cp, dfc, FALSE);
            }
            retVal = HANDLED;
        }
        else if (msgWhat == EVENT_DISCONNECT) {
            AutoPtr<DisconnectParams> dp = (DisconnectParams*) IObject::Probe(msgObj);
            Int32 size;
            mHost->mApnContexts->GetSize(&size);
            if (mHost->mApnContexts->Remove(dp->mApnContext) && size == 0) {
                if (DBG) {
                    mHost->Log("DcRetryingState msg.what=EVENT_DISCONNECT  RefCount=%d"
                            " dp=%s", size, TO_CSTR(dp));
                }
                mHost->mInactiveState->SetEnterNotificationParams(dp);
                mHost->TransitionTo(mHost->mInactiveState);
            } else {
                if (DBG) mHost->Log("DcRetryingState: msg.what=EVENT_DISCONNECT");
                mHost->NotifyDisconnectCompleted(dp, FALSE);
            }
            retVal = HANDLED;
        }
        else if (msgWhat == EVENT_DISCONNECT_ALL) {
            if (DBG) {
                Int32 size;
                mHost->mApnContexts->GetSize(&size);
                mHost->Log("DcRetryingState msg.what=EVENT_DISCONNECT/DISCONNECT_ALL "
                        "RefCount=%d", size);
            }
            AutoPtr<IDcFailCause> dfc;
            DcFailCause::FromInt32(DcFailCause_LOST_CONNECTION, (IDcFailCause**)&dfc);
            mHost->mInactiveState->SetEnterNotificationParams(dfc);
            mHost->TransitionTo(mHost->mInactiveState);
            retVal = HANDLED;
        }
        else {
            if (VDBG) {
                String whatToString = mHost->GetWhatToString(msgWhat);
                mHost->Log("DcRetryingState nothandled msg.what=%s", whatToString.string());
            }
            retVal = NOT_HANDLED;
        }
    } while(FALSE);
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcRetryingState::GetName()
{
    return String("DataConnection::DcRetryingState");
}

//=============================================================================
// DataConnection::DcActivatingState
//=============================================================================
DataConnection::DcActivatingState::DcActivatingState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcActivatingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)

    Boolean retVal;
    AutoPtr<IAsyncResult> ar;
    AutoPtr<ConnectionParams> cp;
    String s;
    MsgToString(msg, &s);
    if (DBG) mHost->Log("DcActivatingState: msg=%s", s.string());
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED || msgWhat == EVENT_CONNECT) {
        // Activating can't process until we're done.
        mHost->DeferMessage(msg);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_SETUP_DATA_CONNECTION_DONE) {
        ar = IAsyncResult::Probe(msgObj);
        cp = (ConnectionParams*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
        AutoPtr<IDataCallResponseSetupResult> result;
        mHost->OnSetupConnectionCompleted(ar, (IDataCallResponseSetupResult**)&result);
        if (((DataCallResponse::SetupResult*) result.Get())->mEnumValue != DataCallResponse_ERR_Stale) {
            if (mHost->mConnectionParams != cp) {
                mHost->Loge("DcActivatingState: WEIRD mConnectionsParams:%s != cp:%s",
                        TO_CSTR(mHost->mConnectionParams), TO_CSTR(cp));
            }
        }
        if (DBG) {
            mHost->Log("DcActivatingState onSetupConnectionCompleted result=%s dc=%s",
                    TO_CSTR(result), TO_CSTR(mHost));
        }
        switch (((DataCallResponse::SetupResult*) result.Get())->mEnumValue) {
            case DataCallResponse_SUCCESS:
                // All is well
                DcFailCause::FromInt32(DcFailCause_NONE, (IDcFailCause**)&mHost->mDcFailCause);
                mHost->TransitionTo(mHost->mActiveState);
                break;
            case DataCallResponse_ERR_BadCommand:
                // Vendor ril rejected the command and didn't connect.
                // Transition to inactive but send notifications after
                // we've entered the mInactive state.
                mHost->mInactiveState->SetEnterNotificationParams(cp, ((DataCallResponse::SetupResult*) result.Get())->mFailCause);
                mHost->TransitionTo(mHost->mInactiveState);
                break;
            case DataCallResponse_ERR_UnacceptableParameter:
                // The addresses given from the RIL are bad
                mHost->TearDownData(TO_IINTERFACE(cp));
                mHost->TransitionTo(mHost->mDisconnectingErrorCreatingConnection);
                break;
            case DataCallResponse_ERR_GetLastErrorFromRil: {
                // Request failed and this is an old RIL
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(EVENT_GET_LAST_FAIL_DONE, TO_IINTERFACE(cp), (IMessage**)&msg);
                ((PhoneBase*) mHost->mPhone.Get())->mCi->GetLastDataCallFailCause(msg);
                break;
            }
            case DataCallResponse_ERR_RilError: {
                Int32 delay;
                ((DcRetryAlarmController*) (mHost->mDcRetryAlarmController.Get()))->GetSuggestedRetryTime(mHost, ar, &delay);
                Boolean isRestartRadioFail;
                ((DataCallResponse::SetupResult*) result.Get())->mFailCause->IsRestartRadioFail(&isRestartRadioFail);
                if (DBG) {
                    Boolean isRetryNeeded;
                    mHost->mRetryManager->IsRetryNeeded(&isRetryNeeded);
                    Boolean isPermanentFail;
                    mHost->mDct->IsPermanentFail(((DataCallResponse::SetupResult*) result.Get())->mFailCause, &isPermanentFail);
                    mHost->Log("DcActivatingState: ERR_RilError  delay=%d isRetryNeeded=%d result=%s result.isRestartRadioFail=%d result.isPermanentFail=%d",
                            delay, isRetryNeeded, TO_CSTR(result), isRestartRadioFail, isPermanentFail);
                }
                Boolean isPermanentFail;
                mHost->mDct->IsPermanentFail(((DataCallResponse::SetupResult*) result.Get())->mFailCause, &isPermanentFail);
                if (isRestartRadioFail) {
                    if (DBG) mHost->Log("DcActivatingState: ERR_RilError restart radio");
                    ((DcTrackerBase*) mHost->mDct.Get())->SendRestartRadio();
                    mHost->mInactiveState->SetEnterNotificationParams(cp, ((DataCallResponse::SetupResult*) result.Get())->mFailCause);
                    mHost->TransitionTo(mHost->mInactiveState);
                } else if (isPermanentFail) {
                    if (DBG) mHost->Log("DcActivatingState: ERR_RilError perm error");
                    mHost->mInactiveState->SetEnterNotificationParams(cp, ((DataCallResponse::SetupResult*) result.Get())->mFailCause);
                    mHost->TransitionTo(mHost->mInactiveState);
                } else if (delay >= 0) {
                    if (DBG) mHost->Log("DcActivatingState: ERR_RilError retry");
                    mHost->mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION, mHost->mTag, delay);
                    mHost->TransitionTo(mHost->mRetryingState);
                } else {
                    if (DBG) mHost->Log("DcActivatingState: ERR_RilError no retry");
                    mHost->mInactiveState->SetEnterNotificationParams(cp, ((DataCallResponse::SetupResult*) result.Get())->mFailCause);
                    mHost->TransitionTo(mHost->mInactiveState);
                }
                break;
            }
            case DataCallResponse_ERR_Stale:
                mHost->Loge("DcActivatingState: stale EVENT_SETUP_DATA_CONNECTION_DONE"
                        " tag:%d != mTag:%d", cp->mTag, mHost->mTag);
                break;
            default:
                Logger::E("DataConnection::DcActivatingState", "Unknown SetupResult, should not happen");
                return E_RUNTIME_EXCEPTION;
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_GET_LAST_FAIL_DONE) {
        ar = IAsyncResult::Probe(msgObj);
        cp = (ConnectionParams*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
        if (cp->mTag == mHost->mTag) {
            if (mHost->mConnectionParams != cp) {
                mHost->Loge("DcActivatingState: WEIRD mConnectionsParams:%s"
                        " != cp:%s", TO_CSTR(mHost->mConnectionParams), TO_CSTR(cp));
            }
            AutoPtr<IDcFailCause> cause;
            DcFailCause::FromInt32(DcFailCause_UNKNOWN, (IDcFailCause**)&cause);
            if (((AsyncResult*) ar.Get())->mException == NULL) {
                AutoPtr<IInterface> obj;
                IArrayOf::Probe(((AsyncResult*) ar.Get())->mResult)->Get(0, (IInterface**)&obj);
                Int32 rilFailCause;
                IInteger32::Probe(obj)->GetValue(&rilFailCause);
                DcFailCause::FromInt32(rilFailCause, (IDcFailCause**)&cause);
                Int32 errCode;
                cause->GetErrorCode(&errCode);
                if (errCode == DcFailCause_NONE) {
                    if (DBG) {
                        mHost->Log("DcActivatingState msg.what=EVENT_GET_LAST_FAIL_DONE"
                                " BAD: error was NONE, change to UNKNOWN");
                    }
                    cause = NULL;
                    DcFailCause::FromInt32(DcFailCause_UNKNOWN, (IDcFailCause**)&cause);
                }
            }
            mHost->mDcFailCause = cause;
            Int32 retryDelay;
            mHost->mRetryManager->GetRetryTimer(&retryDelay);
            if (DBG) {
                Boolean isRetryNeeded;
                mHost->mRetryManager->IsRetryNeeded(&isRetryNeeded);
                mHost->Log("DcActivatingState msg.what=EVENT_GET_LAST_FAIL_DONE"
                        " cause=%s"
                        " retryDelay=%d"
                        " isRetryNeeded=%d"
                        " dc=%s",
                        TO_CSTR(cause), retryDelay, isRetryNeeded, TO_CSTR(mHost));
            }
            Boolean isRestartRadioFail;
            cause->IsRestartRadioFail(&isRestartRadioFail);
            Boolean isRetryNeeded;
            mHost->mRetryManager->IsRetryNeeded(&isRetryNeeded);
            Boolean isPermanentFail;
            mHost->mDct->IsPermanentFail(cause, &isPermanentFail);
            if (isRestartRadioFail) {
                if (DBG) {
                    mHost->Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE"
                            " restart radio");
                }
                ((DcTrackerBase*) mHost->mDct.Get())->SendRestartRadio();
                mHost->mInactiveState->SetEnterNotificationParams(cp, cause);
                mHost->TransitionTo(mHost->mInactiveState);
            } else if (isPermanentFail) {
                if (DBG) mHost->Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE perm er");
                mHost->mInactiveState->SetEnterNotificationParams(cp, cause);
                mHost->TransitionTo(mHost->mInactiveState);
            } else if ((retryDelay >= 0) && (isRetryNeeded)) {
                if (DBG) mHost->Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE retry");
                mHost->mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION, mHost->mTag,
                                                retryDelay);
                mHost->TransitionTo(mHost->mRetryingState);
            } else {
                if (DBG) mHost->Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE no retry");
                mHost->mInactiveState->SetEnterNotificationParams(cp, cause);
                mHost->TransitionTo(mHost->mInactiveState);
            }
        } else {
            mHost->Loge("DcActivatingState: stale EVENT_GET_LAST_FAIL_DONE"
                    " tag:%d != mTag:%d", cp->mTag, mHost->mTag);
        }
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            Int32 size;
            mHost->mApnContexts->GetSize(&size);
            String whatToString = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcActivatingState not handled msg.what=%s RefCount=%d", whatToString.string(), size);
        }
        retVal = NOT_HANDLED;
    }
    *_result = retVal;
    return NOERROR;
}

String DataConnection::DcActivatingState::GetName()
{
    return String("DataConnection::DcActivatingState");
}

//=============================================================================
// DataConnection::DcActiveState
//=============================================================================
DataConnection::DcActiveState::DcActiveState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcActiveState::Enter()
{
    if (DBG) mHost->Log("DcActiveState: enter dc=%s", TO_CSTR(mHost));
    Int32 retryCount;
    mHost->mRetryManager->GetRetryCount(&retryCount);
    if (retryCount != 0) {
        mHost->Log("DcActiveState: connected after retrying call notifyAllOfConnected");
        mHost->mRetryManager->SetRetryCount(0);
    }
    // If we were retrying there maybe more than one, otherwise they'll only be one.
    mHost->NotifyAllOfConnected(IPhone::REASON_CONNECTED);
    // If the EVENT_CONNECT set the current max retry restore it here
    // if it didn't then this is effectively a NOP.
    mHost->mRetryManager->RestoreCurMaxRetryCount();
    ((DcController*) (mHost->mDcController.Get()))->AddActiveDcByCid(mHost);
    String networkInfoReason;
    mHost->mNetworkInfo->GetReason(&networkInfoReason);
    mHost->mNetworkInfo->SetDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED, networkInfoReason, String(NULL));
    String apn;
    mHost->mApnSetting->GetApn(&apn);
    mHost->mNetworkInfo->SetExtraInfo(apn);
    mHost->UpdateTcpBufferSizes(mHost->mRilRat);
    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    AutoPtr<INetworkCapabilities> networkCapabilities;
    mHost->MakeNetworkCapabilities((INetworkCapabilities**)&networkCapabilities);
    AutoPtr<IContext> context;
    IPhone::Probe(mHost->mPhone)->GetContext((IContext**)&context);
    Int64 subId;
    mHost->mPhone->GetSubId(&subId);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHost->mNetworkAgent = new DcNetworkAgent(mHost);
    StringBuilder sb("DcNetworkAgent"); sb += subId;
    ((DcNetworkAgent*) mHost->mNetworkAgent.Get())->constructor(
        looper, context, sb.ToString(), mHost->mNetworkInfo, networkCapabilities, mHost->mLinkProperties, 50);
    return NOERROR;
}

ECode DataConnection::DcActiveState::Exit()
{
    if (DBG) mHost->Log("DcActiveState: exit dc=%s", TO_CSTR(TO_IINTERFACE(this)));
    String reason;
    mHost->mNetworkInfo->GetReason(&reason);
    String extraInfo;
    mHost->mNetworkInfo->GetExtraInfo(&extraInfo);
    mHost->mNetworkInfo->SetDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED, reason, extraInfo);
    mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);
    mHost->mNetworkAgent = NULL;
    return NOERROR;
}

ECode DataConnection::DcActiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean retVal;
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_CONNECT) {
        AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(msgObj);
        if (DBG) {
            mHost->Log("DcActiveState: EVENT_CONNECT cp=%s dc=%s", TO_CSTR(TO_IINTERFACE(cp)), TO_CSTR(mHost));
        }
        Boolean isContains;
        mHost->mApnContexts->Contains(cp->mApnContext, &isContains);
        if (isContains) {
            mHost->Log("DcActiveState ERROR already added apnContext=%s", TO_CSTR(cp->mApnContext));
        } else {
            mHost->mApnContexts->Add(cp->mApnContext);
            if (DBG) {
                Int32 size;
                mHost->mApnContexts->GetSize(&size);
                mHost->Log("DcActiveState msg.what=EVENT_CONNECT RefCount=%d", size);
            }
        }
        AutoPtr<IDcFailCause> dfc;
        DcFailCause::FromInt32(DcFailCause_NONE, (IDcFailCause**)&dfc);
        mHost->NotifyConnectCompleted(cp, dfc, FALSE);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DISCONNECT) {
        AutoPtr<DisconnectParams> dp = (DisconnectParams*) IObject::Probe(msgObj);
        if (DBG) {
            mHost->Log("DcActiveState: EVENT_DISCONNECT dp=%s dc=%s",
                    TO_CSTR(dp), TO_CSTR(mHost));
        }
        Boolean isContains;
        mHost->mApnContexts->Contains(dp->mApnContext, &isContains);
        if (isContains) {
            Int32 size;
            mHost->mApnContexts->GetSize(&size);
            if (DBG) {
                mHost->Log("DcActiveState msg.what=EVENT_DISCONNECT RefCount=%d", size);
            }
            if (size == 1) {
                mHost->mApnContexts->Clear();
                mHost->mDisconnectParams = dp;
                mHost->mConnectionParams = NULL;
                dp->mTag = mHost->mTag;
                mHost->TearDownData(TO_IINTERFACE(dp));
                mHost->TransitionTo(mHost->mDisconnectingState);
            } else {
                mHost->mApnContexts->Remove(dp->mApnContext);
                mHost->NotifyDisconnectCompleted(dp, FALSE);
            }
        } else {
            mHost->Log("DcActiveState ERROR no such apnContext=%s in this dc=%s",
                    TO_CSTR(dp->mApnContext), TO_CSTR(mHost));
            mHost->NotifyDisconnectCompleted(dp, FALSE);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DISCONNECT_ALL) {
        if (DBG) {
            mHost->Log("DcActiveState EVENT_DISCONNECT clearing apn contexts,"
                    " dc=%s", TO_CSTR(mHost));
        }
        AutoPtr<DisconnectParams> dp = (DisconnectParams*) IObject::Probe(msgObj);
        mHost->mDisconnectParams = dp;
        mHost->mConnectionParams = NULL;
        dp->mTag = mHost->mTag;
        mHost->TearDownData(TO_IINTERFACE(dp));
        mHost->TransitionTo(mHost->mDisconnectingState);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_LOST_CONNECTION) {
        if (DBG) {
            mHost->Log("DcActiveState EVENT_LOST_CONNECTION dc=%s", TO_CSTR(mHost));
        }
        Boolean isRetryNeeded;
        mHost->mRetryManager->IsRetryNeeded(&isRetryNeeded);
        if (isRetryNeeded) {
            // We're going to retry
            Int32 delayMillis;
            mHost->mRetryManager->GetRetryTimer(&delayMillis);
            if (DBG) {
                mHost->Log("DcActiveState EVENT_LOST_CONNECTION startRetryAlarm"
                        " mTag=%d delay=%lldms", mHost->mTag, delayMillis);
            }
            mHost->mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION, mHost->mTag,
                    delayMillis);
            mHost->TransitionTo(mHost->mRetryingState);
        } else {
            AutoPtr<IDcFailCause> dfc;
            DcFailCause::FromInt32(DcFailCause_LOST_CONNECTION, (IDcFailCause**)&dfc);
            mHost->mInactiveState->SetEnterNotificationParams(dfc);
            mHost->TransitionTo(mHost->mInactiveState);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DATA_CONNECTION_ROAM_ON) {
        mHost->mNetworkInfo->SetRoaming(TRUE);
        mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DATA_CONNECTION_ROAM_OFF) {
        mHost->mNetworkInfo->SetRoaming(FALSE);
        mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            String whatToString = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcActiveState not handled msg.what=%s", whatToString.string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcActiveState::GetName()
{
    return String("DataConnection::DcActiveState");
}

//=============================================================================
// DataConnection::DcDisconnectingState
//=============================================================================
DataConnection::DcDisconnectingState::DcDisconnectingState(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcDisconnectingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean retVal;
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_CONNECT) {
        Int32 size;
        mHost->mApnContexts->GetSize(&size);
        if (DBG) mHost->Log("DcDisconnectingState msg.what=EVENT_CONNECT. Defer. RefCount = %d",
                size);
        mHost->DeferMessage(msg);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DEACTIVATE_DONE) {
        Int32 size;
        mHost->mApnContexts->GetSize(&size);
        if (DBG) mHost->Log("DcDisconnectingState msg.what=EVENT_DEACTIVATE_DONE RefCount=%d",
                size);
        AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
        AutoPtr<DisconnectParams> dp = (DisconnectParams*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
        if (dp->mTag == mHost->mTag) {
            // Transition to inactive but send notifications after
            // we've entered the mInactive state.
            mHost->mInactiveState->SetEnterNotificationParams((DisconnectParams*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj));
            mHost->TransitionTo(mHost->mInactiveState);
        } else {
            if (DBG) mHost->Log("DcDisconnectState stale EVENT_DEACTIVATE_DONE"
                    " dp.tag=%d mTag=%d", dp->mTag, mHost->mTag);
        }
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            String whatToString = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcDisconnectingState not handled msg.what=%s", whatToString.string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcDisconnectingState::GetName()
{
    return String("DataConnection::DcDisconnectingState");
}

//=============================================================================
// DataConnection::DcDisconnectionErrorCreatingConnection
//=============================================================================
DataConnection::DcDisconnectionErrorCreatingConnection::DcDisconnectionErrorCreatingConnection(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcDisconnectionErrorCreatingConnection::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean retVal;
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_DEACTIVATE_DONE) {
        AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
        AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
        if (cp->mTag == mHost->mTag) {
            if (DBG) {
                mHost->Log("DcDisconnectionErrorCreatingConnection"
                    " msg.what=EVENT_DEACTIVATE_DONE");
            }
            // Transition to inactive but send notifications after
            // we've entered the mInactive state.
            AutoPtr<IDcFailCause> dfc;
            DcFailCause::FromInt32(DcFailCause_UNACCEPTABLE_NETWORK_PARAMETER, (IDcFailCause**)&dfc);
            mHost->mInactiveState->SetEnterNotificationParams(cp, dfc);
            mHost->TransitionTo(mHost->mInactiveState);
        } else {
            if (DBG) {
                mHost->Log("DcDisconnectionErrorCreatingConnection stale EVENT_DEACTIVATE_DONE"
                        " dp.tag=%d, mTag=%d", cp->mTag, mHost->mTag);
            }
        }
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            String whatToString = mHost->GetWhatToString(msgWhat);
            mHost->Log("DcDisconnectionErrorCreatingConnection not handled msg.what=%s", whatToString.string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DataConnection::DcDisconnectionErrorCreatingConnection::GetName()
{
    return String("DataConnection::DcDisconnectionErrorCreatingConnection");
}

//=============================================================================
// DataConnection::DcNetworkAgent
//=============================================================================
DataConnection::DcNetworkAgent::DcNetworkAgent(
    /* [in] */ DataConnection* host)
    : mHost(host)
{}

ECode DataConnection::DcNetworkAgent::constructor(
    /* [in] */ ILooper* l,
    /* [in] */ IContext* c,
    /* [in] */ const String& TAG,
    /* [in] */ INetworkInfo* ni,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Int32 score)
{
    return NetworkAgent::constructor(l, c, TAG, ni, nc, lp, score);
}

ECode DataConnection::DcNetworkAgent::Unwanted()
{
    if (mHost->mNetworkAgent != this) {
        mHost->Log("unwanted found mNetworkAgent=%s"
                ", which isn't me.  Aborting unwanted", TO_CSTR(mHost->mNetworkAgent));
        return NOERROR;
    }
    // this can only happen if our exit has been called - we're already disconnected
    if (mHost->mApnContexts == NULL) return NOERROR;
    AutoPtr<IIterator> it;
    mHost->mApnContexts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        AutoPtr<IMessage> msg;
        IHandler::Probe(mHost->mDct)->ObtainMessage(IDctConstants::EVENT_DISCONNECT_DONE, apnContext, (IMessage**)&msg);
        AutoPtr<DisconnectParams> dp = new DisconnectParams();
        String reason;
        apnContext->GetReason(&reason);
        dp->constructor(apnContext, reason, msg);
        msg = NULL;
        mHost->ObtainMessage(EVENT_DISCONNECT, TO_IINTERFACE(dp), (IMessage**)&msg);
        mHost->SendMessage(msg);
    }
    return NOERROR;
}

//=============================================================================
// DataConnection
//=============================================================================
CAR_INTERFACE_IMPL(DataConnection, StateMachine, IDataConnection)

const Boolean DataConnection::DBG = TRUE;
const Boolean DataConnection::VDBG = TRUE;
const String DataConnection::DEFAULT_DATA_RETRY_CONFIG("default_randomization=2000,5000,10000,20000,40000,80000:5000,160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000");
const String DataConnection::SECONDARY_DATA_RETRY_CONFIG("max_retries=3, 5000, 5000, 5000");
const String DataConnection::NETWORK_TYPE("MOBILE");
AutoPtr<IAtomicInteger32> DataConnection::mInstanceNumber = InitInstanceNumber();
const String DataConnection::NULL_IP("0.0.0.0");
const Int32 DataConnection::BASE = IProtocol::BASE_DATA_CONNECTION;
const Int32 DataConnection::EVENT_CONNECT = BASE + 0;
const Int32 DataConnection::EVENT_SETUP_DATA_CONNECTION_DONE = BASE + 1;
const Int32 DataConnection::EVENT_GET_LAST_FAIL_DONE = BASE + 2;
const Int32 DataConnection::EVENT_DEACTIVATE_DONE = BASE + 3;
const Int32 DataConnection::EVENT_DISCONNECT = BASE + 4;
const Int32 DataConnection::EVENT_RIL_CONNECTED = BASE + 5;
const Int32 DataConnection::EVENT_DISCONNECT_ALL = BASE + 6;
const Int32 DataConnection::EVENT_DATA_STATE_CHANGED = BASE + 7;
const Int32 DataConnection::EVENT_TEAR_DOWN_NOW = BASE + 8;
const Int32 DataConnection::EVENT_LOST_CONNECTION = BASE + 9;
const Int32 DataConnection::EVENT_RETRY_CONNECTION = BASE + 10;
const Int32 DataConnection::EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED = BASE + 11;
const Int32 DataConnection::EVENT_DATA_CONNECTION_ROAM_ON = BASE + 12;
const Int32 DataConnection::EVENT_DATA_CONNECTION_ROAM_OFF = BASE + 13;
const Int32 DataConnection::CMD_TO_STRING_COUNT = EVENT_DATA_CONNECTION_ROAM_OFF - BASE + 1;
AutoPtr<ArrayOf<String> > DataConnection::sCmdToString = InitCmdToString();
const String DataConnection::TCP_BUFFER_SIZES_GPRS("4092,8760,48000,4096,8760,48000");
const String DataConnection::TCP_BUFFER_SIZES_EDGE("4093,26280,70800,4096,16384,70800");
const String DataConnection::TCP_BUFFER_SIZES_UMTS("58254,349525,1048576,58254,349525,1048576");
const String DataConnection::TCP_BUFFER_SIZES_1XRTT("16384,32768,131072,4096,16384,102400");
const String DataConnection::TCP_BUFFER_SIZES_EVDO("4094,87380,262144,4096,16384,262144");
const String DataConnection::TCP_BUFFER_SIZES_EHRPD("131072,262144,1048576,4096,16384,524288");
const String DataConnection::TCP_BUFFER_SIZES_HSDPA("61167,367002,1101005,8738,52429,262114");
const String DataConnection::TCP_BUFFER_SIZES_HSPA("40778,244668,734003,16777,100663,301990");
const String DataConnection::TCP_BUFFER_SIZES_LTE("524288,1048576,2097152,262144,524288,1048576");
const String DataConnection::TCP_BUFFER_SIZES_HSPAP("122334,734003,2202010,32040,192239,576717");

ECode DataConnection::CmdToString(
    /* [in] */ Int32 _cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String value;
    Int32 cmd = _cmd;
    cmd -= BASE;
    if ((cmd >= 0) && (cmd < sCmdToString->GetLength())) {
        value = (*sCmdToString)[cmd];
    } else {
        DcAsyncChannel::CmdToString(cmd + BASE, &value);
    }
    if (value == NULL) {
        value = String("0x") + StringUtils::ToHexString(cmd + BASE);
    }
    *result = value;
    return NOERROR;
}

ECode DataConnection::MakeDataConnection(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ Int32 id,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IDcTesterFailBringUpAll* failBringUpAll,
    /* [in] */ IDcController* dcc,
    /* [out] */ IDataConnection** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 instNumber;
    mInstanceNumber->IncrementAndGet(&instNumber);
    AutoPtr<IDataConnection> dc = new DataConnection();
    ((DataConnection*)dc.Get())->constructor(phone, String("DC-") + StringUtils::ToString(instNumber), id, dct, failBringUpAll, dcc);
    assert(0 && "TODO: IStateMachine");
    // IStateMachine::Probe(dc)->Start();
    String name;
    ((DataConnection*)dc.Get())->GetName(&name);
    if (DBG) ((DataConnection*)dc.Get())->Log("Made %s", name.string());
    *result = dc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataConnection::Dispose()
{
    Log("dispose: call quiteNow()");
    QuitNow();
    return NOERROR;
}

ECode DataConnection::GetCopyNetworkCapabilities(
    /* [out] */ INetworkCapabilities** result)
{
    return MakeNetworkCapabilities(result);
}

ECode DataConnection::GetCopyLinkProperties(
    /* [out] */ ILinkProperties** result)
{
    return CLinkProperties::New(mLinkProperties, result);
}

ECode DataConnection::GetIsInactive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)


    *result = (GetCurrentState().Get() == IState::Probe(TO_IINTERFACE(mInactiveState)));
    return NOERROR;
}

ECode DataConnection::GetCid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCid;
    return NOERROR;
}

ECode DataConnection::GetApnSetting(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mApnSetting;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataConnection::SetLinkPropertiesHttpProxy(
    /* [in] */ IProxyInfo* proxy)
{
    mLinkProperties->SetHttpProxy(proxy);
    return NOERROR;
}

ECode DataConnection::IsIpv4Connected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = FALSE;
    AutoPtr<IList> list;
    mLinkProperties->GetAddresses((IList**)&list);
    AutoPtr<ICollection> addresses = ICollection::Probe(list);
    AutoPtr<IIterator> it;
    addresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInetAddress> addr = IInetAddress::Probe(obj);
        if (Elastos::Net::IInet4Address::Probe(addr) != NULL) {
            AutoPtr<Elastos::Net::IInet4Address> i4addr = Elastos::Net::IInet4Address::Probe(addr);
            Boolean isLinkLocalAddress;
            IInetAddress::Probe(i4addr)->IsLinkLocalAddress(&isLinkLocalAddress);
            Boolean isAnyLocalAddress;
            IInetAddress::Probe(i4addr)->IsAnyLocalAddress(&isAnyLocalAddress);
            Boolean isMulticastAddress;
            IInetAddress::Probe(i4addr)->IsMulticastAddress(&isMulticastAddress);
            Boolean isLoopbackAddress;
            IInetAddress::Probe(i4addr)->IsLoopbackAddress(&isLoopbackAddress);
            if (!isAnyLocalAddress && !isLinkLocalAddress && !isLoopbackAddress && !isMulticastAddress) {
                ret = TRUE;
                break;
            }
        }
    }
    *result = ret;
    return NOERROR;
}

ECode DataConnection::IsIpv6Connected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = FALSE;
    AutoPtr<IList> list;
    mLinkProperties->GetAddresses((IList**)&list);
    AutoPtr<ICollection> addresses = ICollection::Probe(list);
    AutoPtr<IIterator> it;
    addresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInetAddress> addr = IInetAddress::Probe(obj);
        if (Elastos::Net::IInet6Address::Probe(addr) != NULL) {
            AutoPtr<Elastos::Net::IInet6Address> i6addr = Elastos::Net::IInet6Address::Probe(addr);
            Boolean isMulticastAddress;
            IInetAddress::Probe(i6addr)->IsMulticastAddress(&isMulticastAddress);
            Boolean isLoopbackAddress;
            IInetAddress::Probe(i6addr)->IsLoopbackAddress(&isLoopbackAddress);
            Boolean isLinkLocalAddress;
            IInetAddress::Probe(i6addr)->IsLinkLocalAddress(&isLinkLocalAddress);
            Boolean isAnyLocalAddress;
            IInetAddress::Probe(i6addr)->IsAnyLocalAddress(&isAnyLocalAddress);
            if (!isAnyLocalAddress && !isLinkLocalAddress && !isLoopbackAddress && !isMulticastAddress) {
                ret = TRUE;
                break;
            }
        }
    }
    *result = ret;
    return NOERROR;
}

ECode DataConnection::UpdateLinkProperty(
    /* [in] */ IDataCallResponse* newState,
    /* [out] */ UpdateLinkPropertyResult** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<UpdateLinkPropertyResult> result = new UpdateLinkPropertyResult(mLinkProperties);
    if (newState == NULL) {
        *_result = result;
        REFCOUNT_ADD(*_result)
        return NOERROR;
    }
    AutoPtr<IDataCallResponseSetupResult> setupResult;
    CLinkProperties::New((ILinkProperties**)&(result->mNewLp));
    // set link properties based on data call response
    SetLinkProperties(newState, result->mNewLp, (IDataCallResponseSetupResult**)&(result->mSetupResult));
    if (((DataCallResponse::SetupResult*) result->mSetupResult.Get())->mEnumValue != DataCallResponse_SUCCESS) {
        if (DBG) Log("updateLinkProperty failed : %s", TO_CSTR(result->mSetupResult));
        *_result = result;
        REFCOUNT_ADD(*_result)
        return NOERROR;
    }
    // copy HTTP proxy as it is not part DataCallResponse.
    AutoPtr<IProxyInfo> proxyInfo;
    mLinkProperties->GetHttpProxy((IProxyInfo**)&proxyInfo);
    result->mNewLp->SetHttpProxy(proxyInfo);
    CheckSetMtu(mApnSetting, result->mNewLp);
    mLinkProperties = result->mNewLp;
    UpdateTcpBufferSizes(mRilRat);
    Boolean isEquals;
    IObject::Probe(result->mOldLp)->Equals(result->mNewLp, &isEquals);
    if (DBG && (! isEquals)) {
        Log("updateLinkProperty old LP=%s", TO_CSTR(result->mOldLp));
        Log("updateLinkProperty new LP=%s", TO_CSTR(result->mNewLp));
    }
    IObject::Probe(result->mNewLp)->Equals(result->mOldLp, &isEquals);
    if (isEquals == FALSE &&
            mNetworkAgent != NULL) {
        mNetworkAgent->SendLinkProperties(mLinkProperties);
    }
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DataConnection::CheckSetMtu(
    /* [in] */ IApnSetting* apn,
    /* [in] */ ILinkProperties* lp)
{
    if (lp == NULL) return NOERROR;
    if (apn == NULL || lp == NULL) return NOERROR;
    Int32 lpMtu;
    lp->GetMtu(&lpMtu);
    if (lpMtu != IPhoneConstants::UNSET_MTU) {
        if (DBG) Log("MTU set by call response to: %d", lpMtu);
        return NOERROR;
    }
    Int32 apnMtu;
    apn->GetMtu(&apnMtu);
    if (apn != NULL && apnMtu != IPhoneConstants::UNSET_MTU) {
        lp->SetMtu(apnMtu);
        if (DBG) Log("MTU set by APN to: %d", apnMtu);
        return NOERROR;
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 mtu;
    res->GetInteger(R::integer::config_mobile_mtu, &mtu);
    if (mtu != IPhoneConstants::UNSET_MTU) {
        lp->SetMtu(mtu);
        if (DBG) Log("MTU set by config resource to: %d", mtu);
    }
    return NOERROR;
}

DataConnection::DataConnection()
    : mTag(0)
    , mCid(0)
    , mCreateTime(0)
    , mLastFailTime(0)
    , mRilRat(Elastos::Core::Math::INT32_MAX_VALUE)
    , mDataRegState(Elastos::Core::Math::INT32_MAX_VALUE)
    , mId(0)
{
    assert(0 && "TODO: CRetryManager");
    // CRetryManager::New((IRetryManager**)&mRetryManager);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    mDefaultState = new DcDefaultState(this);
    mInactiveState = new DcInactiveState(this);
    mRetryingState = new DcRetryingState(this);
    mActivatingState = new DcActivatingState(this);
    mActiveState = new DcActiveState(this);
    mDisconnectingState = new DcDisconnectingState(this);
    mDisconnectingErrorCreatingConnection = new DcDisconnectionErrorCreatingConnection(this);
}

ECode DataConnection::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ const String& name,
    /* [in] */ Int32 id,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IDcTesterFailBringUpAll* failBringUpAll,
    /* [in] */ IDcController* dcc)
{
    AutoPtr<IHandler> handler;
    IStateMachine::Probe(dcc)->GetHandler((IHandler**)&handler);
    StateMachine::constructor(name, handler);
    SetLogRecSize(300);
    SetLogOnlyTransitions(TRUE);
    if (DBG) Log("DataConnection constructor E");
    mPhone = phone;
    mDct = dct;
    mDcTesterFailBringUpAll = failBringUpAll;
    mDcController = dcc;
    mId = id;
    mCid = -1;
    CDcRetryAlarmController::New(mPhone, this, (IDcRetryAlarmController**)&mDcRetryAlarmController);
    AutoPtr<IServiceState> ss;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&ss);
    Int32 rilDataRadioTechnology;
    ss->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    mRilRat = rilDataRadioTechnology;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    serviceState->GetDataRegState(&mDataRegState);
    Int32 networkType;
    ss->GetDataNetworkType(&networkType);
    String networkTypeName;
    CTelephonyManager::GetNetworkTypeName(networkType, &networkTypeName);
    CNetworkInfo::New(IConnectivityManager::TYPE_MOBILE,
            networkType, NETWORK_TYPE, networkTypeName, (INetworkInfo**)&mNetworkInfo);
    Boolean roaming;
    ss->GetRoaming(&roaming);
    mNetworkInfo->SetRoaming(roaming);
    mNetworkInfo->SetIsAvailable(TRUE);
    AddState(mDefaultState);
    AddState(mInactiveState, mDefaultState);
    AddState(mActivatingState, mDefaultState);
    AddState(mRetryingState, mDefaultState);
    AddState(mActiveState, mDefaultState);
    AddState(mDisconnectingState, mDefaultState);
    AddState(mDisconnectingErrorCreatingConnection, mDefaultState);
    SetInitialState(mInactiveState);
    CArrayList::New((IList**)&mApnContexts);
    if (DBG) Log("DataConnection constructor X");
    return NOERROR;
}

ECode DataConnection::GetRetryConfig(
    /* [in] */ Boolean forDefault,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 nt;
    serviceState->GetNetworkType(&nt);
    if (Build::IS_DEBUGGABLE) {
        String config;
        SystemProperties::Get(String("test.data_retry_config"), &config);
        if (! TextUtils::IsEmpty(config)) {
            *result = config;
            return NOERROR;
        }
    }
    if ((nt == ITelephonyManager::NETWORK_TYPE_CDMA) ||
        (nt == ITelephonyManager::NETWORK_TYPE_1xRTT) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_0) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_A) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_B) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EHRPD)) {
        // CDMA variant
        return SystemProperties::Get(String("ro.cdma.data_retry_config"), result);
    } else {
        // Use GSM variant for all others.
        if (forDefault) {
            return SystemProperties::Get(String("ro.gsm.data_retry_config"), result);
        } else {
            return SystemProperties::Get(String("ro.gsm.2nd_data_retry_config"), result);
        }
    }
    return NOERROR;
}

ECode DataConnection::ConfigureRetry(
    /* [in] */ Boolean forDefault)
{
    String retryConfig;
    GetRetryConfig(forDefault, &retryConfig);
    Boolean isConfigure;
    mRetryManager->Configure(retryConfig, &isConfigure);
    if (!isConfigure) {
        if (forDefault) {
            mRetryManager->Configure(DEFAULT_DATA_RETRY_CONFIG, &isConfigure);
            if (!isConfigure) {
                // Should never happen, log an error and default to a simple linear sequence.
                Loge("configureRetry: Could not configure using "
                        "DEFAULT_DATA_RETRY_CONFIG=%s", DEFAULT_DATA_RETRY_CONFIG.string());
                Boolean b;
                mRetryManager->Configure(5, 2000, 1000, &b);
            }
        } else {
            mRetryManager->Configure(SECONDARY_DATA_RETRY_CONFIG, &isConfigure);
            if (!isConfigure) {
                // Should never happen, log an error and default to a simple sequence.
                Loge("configureRetry: Could note configure using "
                        "SECONDARY_DATA_RETRY_CONFIG=%s", SECONDARY_DATA_RETRY_CONFIG.string());
                Boolean b;
                mRetryManager->Configure(5, 2000, 1000, &b);
            }
        }
    }
    if (DBG) {
        Log("configureRetry: forDefault=%d mRetryManager=%s", forDefault, TO_CSTR(mRetryManager));
    }
    return NOERROR;
}

ECode DataConnection::OnConnect(
    /* [in] */ ConnectionParams* cp)
{
    String apn;
    mApnSetting->GetApn(&apn);
    String carrier;
    mApnSetting->GetCarrier(&carrier);
    String proxy;
    mApnSetting->GetProxy(&proxy);
    String port;
    mApnSetting->GetPort(&port);
    if (DBG) Log("onConnect: carrier='%s' APN='%s' proxy='%s' port='%s'",
            carrier.string(), apn.string(),
            proxy.string(), port.string());
    // Check if we should fake an error.
    AutoPtr<DcFailBringUp> dcFailBringUp;
    ((DcTesterFailBringUpAll*) mDcTesterFailBringUpAll.Get())->GetDcFailBringUp((DcFailBringUp**)&dcFailBringUp);
    if (dcFailBringUp->mCounter  > 0) {
        AutoPtr<IDataCallResponse> response;
        CDataCallResponse::New((IDataCallResponse**)&response);
        Int32 version;
        response->GetVersion(&version);
        ((PhoneBase*) mPhone.Get())->mCi->GetRilVersion(&version);
        Int32 status;
        response->GetStatus(&status);
        dcFailBringUp->mFailCause->GetErrorCode(&(status));
        response->SetCid(0);
        response->SetActive(0);
        response->SetType(String(""));
        response->SetIfname(String(""));
        response->SetAddresses(ArrayOf<String>::Alloc(0));
        response->SetDnses(ArrayOf<String>::Alloc(0));
        response->SetGateways(ArrayOf<String>::Alloc(0));
        response->SetSuggestedRetryTime(dcFailBringUp->mSuggestedRetryTime);
        response->SetPcscf(ArrayOf<String>::Alloc(0));
        response->SetMtu(IPhoneConstants::UNSET_MTU);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_SETUP_DATA_CONNECTION_DONE, TO_IINTERFACE(cp), (IMessage**)&msg);
        AsyncResult::ForMessage(msg, response, NULL);
        SendMessage(msg);
        if (DBG) {
            Log("onConnect: FailBringUpAll=%s send error response=%s",
                    TO_CSTR(dcFailBringUp),
                    TO_CSTR(response));
        }
        dcFailBringUp->mCounter -= 1;
        return NOERROR;
    }
    mCreateTime = -1;
    mLastFailTime = -1;
    DcFailCause::FromInt32(DcFailCause_NONE, (IDcFailCause**)&mLastFailCause);
    // The data profile's profile ID must be set when it is created.
    Int32 dataProfileId;
    ApnProfileType apnProfiletype;
    mApnSetting->GetApnProfileType(&apnProfiletype);
    Int32 profileId;
    mApnSetting->GetProfileId(&profileId);
    if (apnProfiletype == PROFILE_TYPE_OMH) {
        dataProfileId = profileId + IRILConstants::DATA_PROFILE_OEM_BASE;
        Log("OMH profile, dataProfile id = %d", dataProfileId);
    } else {
        dataProfileId = cp->mProfileId;
    }
    // msg.obj will be returned in AsyncResult.userObj;
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SETUP_DATA_CONNECTION_DONE, TO_IINTERFACE(cp), (IMessage**)&msg);
    msg->SetObj(TO_IINTERFACE(cp));
    Int32 authType;
    mApnSetting->GetAuthType(&authType);
    String user;
    mApnSetting->GetUser(&user);
    if (authType == -1) {
        authType = TextUtils::IsEmpty(user) ? IRILConstants::SETUP_DATA_AUTH_NONE
                : IRILConstants::SETUP_DATA_AUTH_PAP_CHAP;
    }
    String protocol;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Boolean roaming;
    serviceState->GetRoaming(&roaming);
    if (roaming) {
        mApnSetting->GetRoamingProtocol(&protocol);
    } else {
        mApnSetting->GetProtocol(&protocol);
    }
    String dataTechnology;
    GetDataTechnology(cp->mRilRat, &dataTechnology);
    String password;
    mApnSetting->GetPassword(&password);
    ((PhoneBase*) mPhone.Get())->mCi->SetupDataCall(
            dataTechnology,
            StringUtils::ToString(dataProfileId),
            apn, user, password,
            StringUtils::ToString(authType),
            protocol, msg);
    return NOERROR;
}

ECode DataConnection::GetDataTechnology(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 dataTechnology = radioTechnology + 2;
    Int32 rilVersion;
    ((PhoneBase*) mPhone.Get())->mCi->GetRilVersion(&rilVersion);
    if (rilVersion < 5) {
        Boolean isGsm;
        CServiceState::IsGsm(radioTechnology, &isGsm);
        Boolean isCdma;
        CServiceState::IsCdma(radioTechnology, &isCdma);
        if (isGsm) {
            dataTechnology = IRILConstants::SETUP_DATA_TECH_GSM;
        } else if (isCdma) {
            dataTechnology = IRILConstants::SETUP_DATA_TECH_CDMA;
        }
    }
    *result = StringUtils::ToString(dataTechnology);
    return NOERROR;
}

ECode DataConnection::TearDownData(
    /* [in] */ IInterface* o)
{
    Int32 discReason = IRILConstants::DEACTIVATE_REASON_NONE;
    if ((o != NULL) && ((DisconnectParams*) IObject::Probe(o) != NULL)) {
        AutoPtr<DisconnectParams> dp = (DisconnectParams*) IObject::Probe(o);
        if (TextUtils::Equals(dp->mReason, IPhone::REASON_RADIO_TURNED_OFF)) {
            discReason = IRILConstants::DEACTIVATE_REASON_RADIO_OFF;
        } else if (TextUtils::Equals(dp->mReason, IPhone::REASON_PDP_RESET)) {
            discReason = IRILConstants::DEACTIVATE_REASON_PDP_RESET;
        }
    }
    ICommandsInterfaceRadioState commandsInterfaceRadioState;
    ((PhoneBase*) mPhone.Get())->mCi->GetRadioState(&commandsInterfaceRadioState);
    Boolean isOn = commandsInterfaceRadioState == Elastos::Droid::Internal::Telephony::RADIO_ON;
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 rilDataRadioTechnology;
    serviceState->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    if (isOn || (rilDataRadioTechnology
                    == IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN )) {
        if (DBG) Log("tearDownData radio is on, call deactivateDataCall");
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_DEACTIVATE_DONE, mTag, 0, o, (IMessage**)&msg);
        ((PhoneBase*) mPhone.Get())->mCi->DeactivateDataCall(mCid, discReason, msg);
    } else {
        if (DBG) Log("tearDownData radio is off sendMessage EVENT_DEACTIVATE_DONE immediately");
        AutoPtr<IAsyncResult> ar = new AsyncResult(o, NULL, NULL);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_DEACTIVATE_DONE, mTag, 0, ar, (IMessage**)&msg);
        SendMessage(msg);
    }
    return NOERROR;
}

ECode DataConnection::NotifyAllWithEvent(
    /* [in] */ IApnContext* alreadySent,
    /* [in] */ Int32 event,
    /* [in] */ const String& reason)
{
    String extraInfo;
    mNetworkInfo->GetExtraInfo(&extraInfo);
    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    mNetworkInfo->SetDetailedState(state, reason,
            extraInfo);
    AutoPtr<IIterator> it;
    mApnContexts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        if (apnContext.Get() == alreadySent) continue;
        if (reason != NULL) apnContext->SetReason(reason);
        AutoPtr<IMessage> msg;
        IHandler::Probe(mDct)->ObtainMessage(event, apnContext, (IMessage**)&msg);
        AsyncResult::ForMessage(msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode DataConnection::NotifyAllOfConnected(
    /* [in] */ const String& reason)
{
    return NotifyAllWithEvent(NULL, IDctConstants::EVENT_DATA_SETUP_COMPLETE, reason);
}

ECode DataConnection::NotifyAllOfDisconnectDcRetrying(
    /* [in] */ const String& reason)
{
    return NotifyAllWithEvent(NULL, IDctConstants::EVENT_DISCONNECT_DC_RETRYING, reason);
}

ECode DataConnection::NotifyAllDisconnectCompleted(
    /* [in] */ IDcFailCause* cause)
{
    return NotifyAllWithEvent(NULL, IDctConstants::EVENT_DISCONNECT_DONE, TO_STR(cause));
}

ECode DataConnection::NotifyConnectCompleted(
    /* [in] */ ConnectionParams* cp,
    /* [in] */ IDcFailCause* cause,
    /* [in] */ Boolean sendAll)
{
    AutoPtr<IApnContext> alreadySent;
    if (cp != NULL && cp->mOnCompletedMsg != NULL) {
        // Get the completed message but only use it once
        AutoPtr<IMessage> connectionCompletedMsg = cp->mOnCompletedMsg;
        cp->mOnCompletedMsg = NULL;
        AutoPtr<IInterface> obj;
        connectionCompletedMsg->GetObj((IInterface**)&obj);
        if (IApnContext::Probe(obj) != NULL) {
            alreadySent = IApnContext::Probe(obj);
        }
        AutoPtr<ISystem> systemHelper;
        CSystem::AcquireSingleton((ISystem**)&systemHelper);
        Int64 timeStamp;
        systemHelper->GetCurrentTimeMillis(&timeStamp);
        connectionCompletedMsg->SetArg1(mCid);
        Int32 errCode;
        cause->GetErrorCode(&errCode);
        if (errCode == DcFailCause_NONE) {
            mCreateTime = timeStamp;
            AsyncResult::ForMessage(connectionCompletedMsg);
        } else {
            mLastFailCause = cause;
            mLastFailTime = timeStamp;
            // Return message with a Throwable exception to signify an error.
            if (cause == NULL) DcFailCause::FromInt32(DcFailCause_UNKNOWN, (IDcFailCause**)&cause);
            AutoPtr<IThrowable> t;
            CThrowable::New(TO_STR(cause), (IThrowable**)&t);
            AsyncResult::ForMessage(connectionCompletedMsg, cause, t);
        }
        if (DBG) {
            String msg;
            MsgToString(connectionCompletedMsg, &msg);
            Log("notifyConnectCompleted at %lld cause=%s connectionCompletedMsg=%s",
                    timeStamp, TO_CSTR(cause), msg.string());
        }
        connectionCompletedMsg->SendToTarget();
    }
    if (sendAll) {
        NotifyAllWithEvent(alreadySent, IDctConstants::EVENT_DATA_SETUP_COMPLETE_ERROR,
                TO_STR(cause));
    }
    return NOERROR;
}

ECode DataConnection::NotifyDisconnectCompleted(
    /* [in] */ DisconnectParams* dp,
    /* [in] */ Boolean sendAll)
{
    if (VDBG) Log("NotifyDisconnectCompleted");
    AutoPtr<IApnContext> alreadySent;
    String reason(NULL);
    if (dp != NULL && dp->mOnCompletedMsg != NULL) {
        // Get the completed message but only use it once
        AutoPtr<IMessage> msg = dp->mOnCompletedMsg;
        dp->mOnCompletedMsg = NULL;
        AutoPtr<IInterface> msgObj;
        msg->GetObj((IInterface**)&msgObj);
        if (IApnContext::Probe(msgObj) != NULL) {
            alreadySent = IApnContext::Probe(msgObj);
        }
        reason = dp->mReason;
        if (VDBG) {
            Log("msg=%s msg.obj=%s", TO_CSTR(msg),
                ((ICharSequence::Probe(msgObj) != NULL) ? TO_CSTR(msgObj) : "<no-reason>"));
        }
        AsyncResult::ForMessage(msg);
        msg->SendToTarget();
    }
    if (sendAll) {
        if (reason == NULL) {
            reason = "DcFailCause_UNKNOWN";
        }
        NotifyAllWithEvent(alreadySent, IDctConstants::EVENT_DISCONNECT_DONE, reason);
    }
    if (DBG) Log("NotifyDisconnectCompleted DisconnectParams=%s", TO_CSTR(dp));
    return NOERROR;
}

ECode DataConnection::GetDataConnectionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode DataConnection::ClearSettings()
{
    if (DBG) Log("clearSettings");
    mCreateTime = -1;
    mLastFailTime = -1;
    DcFailCause::FromInt32(DcFailCause_NONE, (IDcFailCause**)&mLastFailCause);
    mCid = -1;
    mPcscfAddr = ArrayOf<String>::Alloc(5);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    mApnContexts->Clear();
    mApnSetting = NULL;
    mDcFailCause = NULL;
    return NOERROR;
}

ECode DataConnection::OnSetupConnectionCompleted(
    /* [in] */ IAsyncResult* ar,
    /* [out] */ IDataCallResponseSetupResult** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<IDataCallResponse> response = IDataCallResponse::Probe(((AsyncResult*) ar)->mResult);
    AutoPtr<ConnectionParams> cp = (ConnectionParams*) IObject::Probe(((AsyncResult*) ar)->mUserObj);
    AutoPtr<DataCallResponse::SetupResult> result;
    Int32 status;
    response->GetStatus(&status);
    if (cp->mTag != mTag) {
        if (DBG) {
            Log("onSetupConnectionCompleted stale cp.tag=%d, mtag=%d", cp->mTag, mTag);
        }
        result = new DataCallResponse::SetupResult();
        result->mEnumValue = DataCallResponse_ERR_Stale;
    } else if (((AsyncResult*) ar)->mException != NULL) {
        if (DBG) {
            Log("onSetupConnectionCompleted failed, ar.exception=%s response=%s",
                    TO_CSTR(((AsyncResult*) ar)->mException), TO_CSTR(response));
        }
        assert(0 && "TODO: CommandException");
        // if (IThrowable::Probe(((AsyncResult*) ar)->mException) != NULL
        //         && ((CommandException) (((AsyncResult*) ar)->mException))->GetCommandError()
        //         == CommandException.Error.RADIO_NOT_AVAILABLE) {
        //     result = DataCallResponse::SetupResult(DataCallResponse_ERR_BadCommand);
        //     result->mFailCause = DcFailCause::FromInt32((DcFailCause_RADIO_NOT_AVAILABLE);
        // } else if ((response == NULL) || (response->mVersion < 4)) {
        //     result = DataCallResponse::SetupResult(DataCallResponse_ERR_GetLastErrorFromRil);
        // } else {
        //     result = DataCallResponse::SetupResult(DataCallResponse_ERR_RilError);
        //     result->mFailCause = DcFailCause::FromInt32(response->mStatus);
        // }
    } else if (status != 0) {
        ((DataCallResponse::SetupResult*) result.Get())->mEnumValue = DataCallResponse_ERR_RilError;
        DcFailCause::FromInt32(status, (IDcFailCause**)&(((DataCallResponse::SetupResult*) result.Get())->mFailCause));
    } else {
        if (DBG) Log("onSetupConnectionCompleted received DataCallResponse: %s", TO_CSTR(response));
        response->GetCid(&mCid);
        response->GetPcscf((ArrayOf<Elastos::String>**)&mPcscfAddr);
        AutoPtr<UpdateLinkPropertyResult> updateLinkPropertyResult;
        UpdateLinkProperty(response, (UpdateLinkPropertyResult**)&updateLinkPropertyResult);
        result = (DataCallResponse::SetupResult*) updateLinkPropertyResult->mSetupResult.Get();
    }
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DataConnection::IsDnsOk(
    /* [in] */ ArrayOf<String>* domainNameServers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isDnsCheckDisabled;
    IPhone::Probe(mPhone)->IsDnsCheckDisabled(&isDnsCheckDisabled);
    if (NULL_IP.Equals((*domainNameServers)[0]) && NULL_IP.Equals((*domainNameServers)[1]) && !isDnsCheckDisabled) {
        // Work around a race condition where QMI does not fill in DNS:
        // Deactivate PDP and let DataConnectionTracker retry.
        // Do not apply the race condition workaround for MMS APN
        // if Proxy is an IP-address.
        // Otherwise, the default APN will not be restored anymore.
        String mmsProxy;
        mApnSetting->GetMmsProxy(&mmsProxy);
        Boolean isIpAddress;
        IsIpAddress(mmsProxy, &isIpAddress);
        AutoPtr<ArrayOf<String> > types;
        mApnSetting->GetTypes((ArrayOf<String>**)&types);
        if (!(*types)[0].Equals(IPhoneConstants::APN_TYPE_MMS)
            || !isIpAddress) {
            Log("isDnsOk: return false apn.types[0]=%s APN_TYPE_MMS=%s isIpAddress(%s)=%d",
                    (*types)[0].string(), IPhoneConstants::APN_TYPE_MMS.string(), mmsProxy.string(),
                    isIpAddress);
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode DataConnection::UpdateTcpBufferSizes(
    /* [in] */ Int32 rilRat)
{
    String sizes(NULL);
    String s;
    CServiceState::RilRadioTechnologyToString(rilRat, &s);
    String ratName = s.ToLowerCase();
    // ServiceState gives slightly different names for EVDO tech ("evdo-rev.0" for ex)
    // - patch it up:
    if (rilRat == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0 ||
            rilRat == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A ||
            rilRat == IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B) {
        ratName = "evdo";
    }
    // in the form: "ratname:rmem_min,rmem_def,rmem_max,wmem_min,wmem_def,wmem_max"
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > configOverride;
    res->GetStringArray(
            R::array::config_mobile_tcp_buffers, (ArrayOf<String>**)&configOverride);
    for (Int32 i = 0; i < configOverride->GetLength(); i++) {
        AutoPtr<ArrayOf<String> > split;
        StringUtils::Split((*configOverride)[i], ":", (ArrayOf<String>**)&split);
        if (ratName.Equals((*split)[0]) && split->GetLength() == 2) {
            sizes = (*split)[1];
            break;
        }
    }
    if (sizes == NULL) {
        // no override - use telephony defaults
        // doing it this way allows device or carrier to just override the types they
        // care about and inherit the defaults for the others.
        switch (rilRat) {
            case IServiceState::RIL_RADIO_TECHNOLOGY_GPRS:
                sizes = TCP_BUFFER_SIZES_GPRS;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_EDGE:
                sizes = TCP_BUFFER_SIZES_EDGE;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_UMTS:
                sizes = TCP_BUFFER_SIZES_UMTS;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT:
                sizes = TCP_BUFFER_SIZES_1XRTT;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0:
            case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A:
            case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B:
                sizes = TCP_BUFFER_SIZES_EVDO;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD:
                sizes = TCP_BUFFER_SIZES_EHRPD;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA:
                sizes = TCP_BUFFER_SIZES_HSDPA;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_HSPA:
            case IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA:
                sizes = TCP_BUFFER_SIZES_HSPA;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_LTE:
                sizes = TCP_BUFFER_SIZES_LTE;
                break;
            case IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP:
                sizes = TCP_BUFFER_SIZES_HSPAP;
                break;
            default:
                // Leave empty - this will let ConnectivityService use the system default.
                break;
        }
    }
    mLinkProperties->SetTcpBufferSizes(sizes);
    return NOERROR;
}

ECode DataConnection::MakeNetworkCapabilities(
    /* [out] */ INetworkCapabilities** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<INetworkCapabilities> result;
    CNetworkCapabilities::New((INetworkCapabilities**)&result);
    result->AddTransportType(INetworkCapabilities::TRANSPORT_CELLULAR);
    if (mApnSetting != NULL) {
        AutoPtr<ArrayOf<String> > types;
        mApnSetting->GetTypes((ArrayOf<String>**)&types);
        for (Int32 i = 0; i < types->GetLength(); ++i) {
            String type = (*types)[i];
            if (type == IPhoneConstants::APN_TYPE_ALL) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_MMS);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_SUPL);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_FOTA);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_IMS);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_CBS);
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_IA);
            }
            else if (type == IPhoneConstants::APN_TYPE_DEFAULT) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
            }
            else if (type == IPhoneConstants::APN_TYPE_MMS) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_MMS);
            }
            else if (type == IPhoneConstants::APN_TYPE_SUPL) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_SUPL);
            }
            else if (type == IPhoneConstants::APN_TYPE_DUN) {
                AutoPtr<IApnSetting> securedDunApn;
                mDct->FetchDunApn((IApnSetting**)&securedDunApn);
                Boolean isEquals;
                IObject::Probe(securedDunApn)->Equals(mApnSetting, &isEquals);
                if (securedDunApn == NULL || isEquals) {
                    result->AddCapability(INetworkCapabilities::NET_CAPABILITY_DUN);
                }
            }
            else if (type == IPhoneConstants::APN_TYPE_FOTA) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_FOTA);
            }
            else if (type == IPhoneConstants::APN_TYPE_IMS) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_IMS);
            }
            else if (type == IPhoneConstants::APN_TYPE_CBS) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_CBS);
            }
            else if (type == IPhoneConstants::APN_TYPE_IA) {
                result->AddCapability(INetworkCapabilities::NET_CAPABILITY_IA);
            }
            Int64 subId;
            mPhone->GetSubId(&subId);
            Int64 defaultDataSubId;
            SubscriptionManager::GetDefaultDataSubId(&defaultDataSubId);
            if (subId != defaultDataSubId) {
                Log("DataConnection on non-dds does not have INTERNET capability.");
                result->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
            }
        }
        CConnectivityManager::MaybeMarkCapabilitiesRestricted(result);
    }
    Int32 up = 14;
    Int32 down = 14;
    switch (mRilRat) {
        case IServiceState::RIL_RADIO_TECHNOLOGY_GPRS:
            up = 80;
            down = 80;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EDGE:
            up = 59;
            down = 236;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_UMTS:
            up = 384;
            down = 384;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95A: // fall through
        case IServiceState::RIL_RADIO_TECHNOLOGY_IS95B:
            up = 14;
            down = 14;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0:
            up = 153;
            down = 2457;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A:
            up = 1843;
            down = 3174;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT:
            up = 100;
            down = 100;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA:
            up = 2048;
            down = 14336;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA:
            up = 5898;
            down = 14336;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPA:
            up = 5898;
            down = 14336;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B:
            up = 1843;
            down = 5017;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_LTE:
            up = 51200;
            down = 102400;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD:
            up = 153;
            down = 2516;
            break;
        case IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP:
            up = 11264;
            down = 43008;
            break;
        default:
            break;
    }
    result->SetLinkUpstreamBandwidthKbps(up);
    result->SetLinkDownstreamBandwidthKbps(down);
    Int64 subId;
    mPhone->GetSubId(&subId);
    result->SetNetworkSpecifier(StringUtils::ToString(subId));
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DataConnection::IsIpAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (address == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IMatcher> matcher;
    Patterns::IP_ADDRESS->Matcher(address, (IMatcher**)&matcher);
    return matcher->Matches(result);
}

ECode DataConnection::SetLinkProperties(
    /* [in] */ IDataCallResponse* response,
    /* [in] */ ILinkProperties* lp,
    /* [out] */ IDataCallResponseSetupResult** result)
{
    VALIDATE_NOT_NULL(result)

    // Check if system property dns usable
    Boolean okToUseSystemPropertyDns = FALSE;
    String ifname;
    response->GetIfname(&ifname);
    String propertyPrefix = String("net.") + ifname + ".";
    AutoPtr<ArrayOf<String> > dnsServers = ArrayOf<String>::Alloc(2);
    SystemProperties::Get(propertyPrefix + "dns1", &(*dnsServers)[0]);
    SystemProperties::Get(propertyPrefix + "dns2", &(*dnsServers)[1]);
    IsDnsOk(dnsServers, &okToUseSystemPropertyDns);
    // set link properties based on data call response
    return response->SetLinkProperties(lp, okToUseSystemPropertyDns, result);
}

ECode DataConnection::InitConnection(
    /* [in] */ ConnectionParams* cp,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IApnContext> apnContext = cp->mApnContext;
    String apnType;
    apnContext->GetApnType(&apnType);
    Boolean canHandleType;
    mApnSetting->CanHandleType(apnType, &canHandleType);
    if (mApnSetting == NULL) {
        // Only change apn setting if it isn't set, it will
        // only NOT be set only if we're in DcInactiveState.
        apnContext->GetApnSetting((IApnSetting**)&mApnSetting);
    } else if (canHandleType) {
        // All is good.
    } else {
        if (DBG) {
            Log("initConnection: incompatible apnSetting in ConnectionParams cp=%s" " dc=%s", TO_CSTR(cp),
                    TO_CSTR(TO_IINTERFACE(this)));
        }
        *result = FALSE;
        return NOERROR;
    }
    mTag += 1;
    mConnectionParams = cp;
    mConnectionParams->mTag = mTag;
    Boolean isCantains;
    mApnContexts->Contains(apnContext, &isCantains);
    if (!isCantains) {
        mApnContexts->Add(apnContext);
    }
    mApnSetting->CanHandleType(IPhoneConstants::APN_TYPE_DEFAULT, &canHandleType);
    ConfigureRetry(canHandleType);
    mRetryManager->SetRetryCount(0);
    mRetryManager->SetCurMaxRetryCount(mConnectionParams->mInitialMaxRetry);
    mRetryManager->SetRetryForever(FALSE);
    if (DBG) {
        Int32 size;
        mApnContexts->GetSize(&size);
        Log("initConnection:  RefCount=%d" " mApnList=%s" " mConnectionParams=%s",
                size, TO_CSTR(mApnContexts), TO_CSTR(mConnectionParams));
    }
    *result = TRUE;
    return NOERROR;
}

ECode DataConnection::TearDownNow()
{
    if (DBG) Log("tearDownNow()");
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_TEAR_DOWN_NOW, (IMessage**)&msg);
    SendMessage(msg);
    return NOERROR;
}

String DataConnection::GetWhatToString(
    /* [in] */ Int32 what)
{
    String cmdStr;
    CmdToString(what, &cmdStr);
    return cmdStr;
}

ECode DataConnection::MsgToString(
    /* [in] */ IMessage* msg,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String retVal;
    if (msg == NULL) {
        retVal = "null";
    } else {
        StringBuilder b;
        b.Append("{what=");
        Int32 msgWhat;
        msg->GetWhat(&msgWhat);
        String cmd;
        CmdToString(msgWhat, &cmd);
        b.Append(cmd);
        b.Append(" when=");
        Int64 msgWhen;
        msg->GetWhen(&msgWhen);
        TimeUtils::FormatDuration(msgWhen - SystemClock::GetUptimeMillis(), b);
        Int32 msgArg1;
        msg->GetArg1(&msgArg1);
        if (msgArg1 != 0) {
            b.Append(" arg1=");
            b.Append(msgArg1);
        }
        Int32 msgArg2;
        msg->GetArg2(&msgArg2);
        if (msgArg2 != 0) {
            b.Append(" arg2=");
            b.Append(msgArg2);
        }
        AutoPtr<IInterface> msgObj;
        msg->GetObj((IInterface**)&msgObj);
        if (msgObj != NULL) {
            b.Append(" obj=");
            b.Append(msgObj);
        }
        b.Append(" target=");
        AutoPtr<IHandler> msgTarget;
        msg->GetTarget((IHandler**)&msgTarget);
        b.Append(msgTarget);
        b.Append(" replyTo=");
        AutoPtr<IMessenger> messenger;
        msg->GetReplyTo((IMessenger**)&messenger);
        b.Append(messenger);
        b.Append("}");
        retVal = b.ToString();
    }
    *result = retVal;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DataConnection::Slog(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D("DC", msgBuf);
}

ECode DataConnection::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::D(name, msgBuf);
}

ECode DataConnection::Logd(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::D(name, msgBuf);
}

ECode DataConnection::Logv(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::V(name, msgBuf);
}

ECode DataConnection::Logi(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::I(name, msgBuf);
}

ECode DataConnection::Logw(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::W(name, msgBuf);
}

ECode DataConnection::Loge(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::E(name, msgBuf);
}

ECode DataConnection::ToStringSimple(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 size;
    mApnContexts->GetSize(&size);
    AutoPtr<INetworkCapabilities> networkCapabilities;
    MakeNetworkCapabilities((INetworkCapabilities**)&networkCapabilities);
    String name;
    GetName(&name);
    String stateName;
    GetCurrentState()->GetName(&stateName);
    String rev;
    rev.AppendFormat("%s: State=%s mApnSetting=%s RefCount=%d mCid=%d"
            " mCreateTime=%lld mLastastFailTime=%lld mLastFailCause=%s"
            " mTag=%d mRetryManager=%s mLinkProperties=%s linkCapabilities=",
            name.string(), stateName.string(), TO_CSTR(mApnSetting), size,
            mCid, mCreateTime, mLastFailTime, TO_CSTR(mLastFailCause), mTag, TO_CSTR(mRetryManager),
            TO_CSTR(mLinkProperties), TO_CSTR(networkCapabilities));
    *result = rev;
    return NOERROR;
}

ECode DataConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String simple;
    ToStringSimple(&simple);
    String rev;
    rev.AppendFormat("{%s mApnContexts=%s}", simple.string(), TO_CSTR(mApnContexts));
    *result = rev;
    return NOERROR;
}

ECode DataConnection::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Print(String("DataConnection "));
    StateMachine::Dump(fd, pw, args);
    Int32 size;
    mApnContexts->GetSize(&size);
    pw->Println(String(" mApnContexts.size=") + StringUtils::ToString(size));
    pw->Println(String(" mApnContexts=") + TO_STR(mApnContexts));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mDataConnectionTracker=") + TO_STR(mDct));
    pw->Println(String(" mApnSetting=") + TO_STR(mApnSetting));
    pw->Println(String(" mTag=") + StringUtils::ToString(mTag));
    pw->Println(String(" mCid=") + StringUtils::ToString(mCid));
    pw->Println(String(" mRetryManager=") + TO_STR(mRetryManager));
    pw->Println(String(" mConnectionParams=") + TO_STR(mConnectionParams));
    pw->Println(String(" mDisconnectParams=") + TO_STR(mDisconnectParams));
    pw->Println(String(" mDcFailCause=") + TO_STR(mDcFailCause));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mPhone=") + TO_STR(mPhone));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mLinkProperties=") + TO_STR(mLinkProperties));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mDataRegState=") + StringUtils::ToString(mDataRegState));
    pw->Println(String(" mRilRat=") + StringUtils::ToString(mRilRat));
    AutoPtr<INetworkCapabilities> networkCapabilities;
    MakeNetworkCapabilities((INetworkCapabilities**)&networkCapabilities);
    pw->Println(String(" mNetworkCapabilities=") + TO_STR(networkCapabilities));
    pw->Println(String(" mCreateTime=") + TimeUtils::LogTimeOfDay(mCreateTime));
    pw->Println(String(" mLastFailTime=") + TimeUtils::LogTimeOfDay(mLastFailTime));
    pw->Println(String(" mLastFailCause=") + TO_STR(mLastFailCause));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mUserData=") + TO_STR(mUserData));
    pw->Println(String(" mInstanceNumber=") + TO_STR(mInstanceNumber));
    pw->Println(String(" mAc=") + TO_STR(mAc));
    pw->Println(String(" mDcRetryAlarmController=") + TO_STR(mDcRetryAlarmController));
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

AutoPtr<IAtomicInteger32> DataConnection::InitInstanceNumber()
{
    AutoPtr<IAtomicInteger32> rev;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&rev);
    return rev;
}

AutoPtr<ArrayOf<String> > DataConnection::InitCmdToString()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(CMD_TO_STRING_COUNT);
    (*rev)[EVENT_CONNECT - BASE] = "EVENT_CONNECT";
    (*rev)[EVENT_SETUP_DATA_CONNECTION_DONE - BASE] = "EVENT_SETUP_DATA_CONNECTION_DONE";
    (*rev)[EVENT_GET_LAST_FAIL_DONE - BASE] = "EVENT_GET_LAST_FAIL_DONE";
    (*rev)[EVENT_DEACTIVATE_DONE - BASE] = "EVENT_DEACTIVATE_DONE";
    (*rev)[EVENT_DISCONNECT - BASE] = "EVENT_DISCONNECT";
    (*rev)[EVENT_RIL_CONNECTED - BASE] = "EVENT_RIL_CONNECTED";
    (*rev)[EVENT_DISCONNECT_ALL - BASE] = "EVENT_DISCONNECT_ALL";
    (*rev)[EVENT_DATA_STATE_CHANGED - BASE] = "EVENT_DATA_STATE_CHANGED";
    (*rev)[EVENT_TEAR_DOWN_NOW - BASE] = "EVENT_TEAR_DOWN_NOW";
    (*rev)[EVENT_LOST_CONNECTION - BASE] = "EVENT_LOST_CONNECTION";
    (*rev)[EVENT_RETRY_CONNECTION - BASE] = "EVENT_RETRY_CONNECTION";
    (*rev)[EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED - BASE] = "EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED";
    (*rev)[EVENT_DATA_CONNECTION_ROAM_ON - BASE] = "EVENT_DATA_CONNECTION_ROAM_ON";
    (*rev)[EVENT_DATA_CONNECTION_ROAM_OFF - BASE] = "EVENT_DATA_CONNECTION_ROAM_OFF";
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
