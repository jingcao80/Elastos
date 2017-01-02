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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/server/wifi/SupplicantStateTracker.h"
#include "elastos/droid/server/wifi/WifiMonitor.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"
#ifdef DROID_CORE
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/wifi/CStateChangeResult.h"
#include "elastos/droid/wifi/CSupplicantState.h"
#endif
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::ISupplicantState;
using Elastos::Droid::Wifi::ISupplicantStateHelper;
using Elastos::Droid::Wifi::CSupplicantStateHelper;
using Elastos::Droid::Wifi::CSupplicantState;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

const String SupplicantStateTracker::TAG("SupplicantStateTracker");
Boolean SupplicantStateTracker::DBG = FALSE;
const Int32 SupplicantStateTracker::MAX_RETRIES_ON_AUTHENTICATION_FAILURE;


//==============================================================
// SupplicantStateTracker::DefaultState
//==============================================================
ECode SupplicantStateTracker::DefaultState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    return NOERROR;
}

ECode SupplicantStateTracker::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);

    if (SupplicantStateTracker::DBG)
        Logger::D(SupplicantStateTracker::TAG, "DefaultState msg: %08x", what);

    switch (what) {
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            mOwner->mAuthenticationFailuresCount++;
            mOwner->mAuthFailureInSupplicantBroadcast = TRUE;
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject::Probe(obj.Get()));
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            mOwner->SendSupplicantStateChangedBroadcast(state, mOwner->mAuthFailureInSupplicantBroadcast);
            mOwner->mAuthFailureInSupplicantBroadcast = FALSE;
            mOwner->TransitionOnSupplicantStateChange(stateChangeResult);
            break;
        }
        // case WifiStateMachine::CMD_RESET_SUPPLICANT_STATE:
        //     mOwner->TransitionTo(mOwner->mUninitializedState);
        //     break;
        case IWifiManager::CONNECT_NETWORK:
            if (DBG) Logger::D("SupplicantStateTracker::DefaultState", "ProcessMessage CONNECT_NETWORK");
            mOwner->mNetworksDisabledDuringConnect = TRUE;
            mOwner->mAssociationRejectCount = 0;
            break;
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT:
            mOwner->mAssociationRejectCount++;
            break;
        default:
            Logger::E(TAG, "Ignoring %p", msg);
            break;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::UninitializedState
//==============================================================
ECode SupplicantStateTracker::UninitializedState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::InactiveState
//==============================================================
ECode SupplicantStateTracker::InactiveState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::DisconnectedState
//==============================================================
ECode SupplicantStateTracker::DisconnectedState::Enter()
{
    if (DBG) Logger::D("SupplicantStateTracker::DisconnectedState", "Enter");
    /* If a disconnect event happens after authentication failure
     * exceeds maximum retries, disable the network
     */
    AutoPtr<IMessage> message = mOwner->GetCurrentMessage();
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject::Probe(obj.Get()));
    if (mOwner->mAuthenticationFailuresCount >= MAX_RETRIES_ON_AUTHENTICATION_FAILURE) {
        Int32 networkId;
        stateChangeResult->GetNetworkId(&networkId);
        Logger::D(TAG, "Failed to authenticate, disabling network %d", networkId);
        mOwner->HandleNetworkConnectionFailure(networkId, IWifiConfiguration::DISABLED_AUTH_FAILURE);
        mOwner->mAuthenticationFailuresCount = 0;
    }
    else if (mOwner->mAssociationRejectCount >= MAX_RETRIES_ON_ASSOCIATION_REJECT) {
        Int32 networkId;
        stateChangeResult->GetNetworkId(&networkId);
        Logger::D(TAG, "Association getting rejected, disabling network %d", networkId);
        mOwner->HandleNetworkConnectionFailure(networkId,
                IWifiConfiguration::DISABLED_ASSOCIATION_REJECT);
        mOwner->mAssociationRejectCount = 0;
    }
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::ScanState
//==============================================================
ECode SupplicantStateTracker::ScanState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::HandshakeState
//==============================================================
const Int32 SupplicantStateTracker::HandshakeState::MAX_SUPPLICANT_LOOP_ITERATIONS;

ECode SupplicantStateTracker::HandshakeState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    mLoopDetectIndex = 0;
    mLoopDetectCount = 0;
    return NOERROR;
}

ECode SupplicantStateTracker::HandshakeState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    if (SupplicantStateTracker::DBG)
        Logger::D(SupplicantStateTracker::TAG, "HandshakeState msg: %08x", what);
    switch (what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject::Probe(obj.Get()));
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            AutoPtr<ISupplicantStateHelper> ssHelper;
            CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&ssHelper);
            Boolean bTmp;
            if (ssHelper->IsHandshakeState(state, &bTmp), bTmp) {
                if (mLoopDetectIndex > state - Elastos::Droid::Wifi::SupplicantState_DISCONNECTED/*state.ordinal()*/) {
                    mLoopDetectCount++;
                }
                if (mLoopDetectCount > MAX_SUPPLICANT_LOOP_ITERATIONS) {
                    Int32 networkId;
                    stateChangeResult->GetNetworkId(&networkId);
                    Logger::D(TAG, "Supplicant loop detected, disabling network %d", networkId);
                    mOwner->HandleNetworkConnectionFailure(networkId, IWifiConfiguration::DISABLED_AUTH_FAILURE);
                }
                mLoopDetectIndex = state - Elastos::Droid::Wifi::SupplicantState_DISCONNECTED;//state.ordinal();
                mOwner->SendSupplicantStateChangedBroadcast(state,
                        mOwner->mAuthFailureInSupplicantBroadcast);
            }
            else {
                //Have the DefaultState handle the transition
                *result = NOT_HANDLED;
                return NOERROR;
            }
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::CompletedState
//==============================================================
ECode SupplicantStateTracker::CompletedState::Enter()
{
    if (DBG) Logger::D("SupplicantStateTracker::CompletedState", "Enter");
    /* Reset authentication failure count */
    mOwner->mAuthenticationFailuresCount = 0;
    mOwner->mNetworksDisabledDuringConnect = 0;
    if (mOwner->mNetworksDisabledDuringConnect) {
        mOwner->mWifiConfigStore->EnableAllNetworks();
        mOwner->mNetworksDisabledDuringConnect = FALSE;
    }
    return NOERROR;
}

ECode SupplicantStateTracker::CompletedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    if (SupplicantStateTracker::DBG)
        Logger::D(SupplicantStateTracker::TAG, "CompletedState msg: %08x", what);
    switch (what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            if (DBG) Logger::D("SupplicantStateTracker::CompletedState",
                "ProcessMessage SUPPLICANT_STATE_CHANGE_EVENT");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject::Probe(obj.Get()));
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            mOwner->SendSupplicantStateChangedBroadcast(state, mOwner->mAuthFailureInSupplicantBroadcast);
            /* Ignore any connecting state in completed state. Group re-keying
             * events and other auth events that do not affect connectivity are
             * ignored
             */
            AutoPtr<ISupplicantStateHelper> ssHelper;
            CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&ssHelper);
            Boolean bTmp;
            if (ssHelper->IsConnecting(state, &bTmp), bTmp) {
                break;
            }
            mOwner->TransitionOnSupplicantStateChange(stateChangeResult);
            break;
        }
        // case WifiStateMachine::CMD_RESET_SUPPLICANT_STATE: {
        //     if (DBG) Logger::D("SupplicantStateTracker::CompletedState",
        //        "ProcessMessage CMD_RESET_SUPPLICANT_STATE");
        //     mOwner->SendSupplicantStateChangedBroadcast(Elastos::Droid::Wifi::SupplicantState_DISCONNECTED, FALSE);
        //     mOwner->TransitionTo(mOwner->mUninitializedState);
        //     break;
        // }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker::DormantState
//==============================================================
ECode SupplicantStateTracker::DormantState::Enter()
{
    if (DBG) Logger::D(TAG, GetName() + "\n");
    return NOERROR;
}


//==============================================================
// SupplicantStateTracker
//==============================================================
SupplicantStateTracker::SupplicantStateTracker(
    /* [in] */ IContext* c,
    /* [in] */ WifiStateMachine* wsm,
    /* [in] */ WifiConfigStore* wcs,
    /* [in] */ IHandler* l)
    : mWifiStateMachine(wsm)
    , mWifiConfigStore(wcs)
    , mAuthenticationFailuresCount(0)
    , mAssociationRejectCount(0)
    , mAuthFailureInSupplicantBroadcast(FALSE)
    , mNetworksDisabledDuringConnect(FALSE)
    , mContext(c)
    , mUninitializedState(new UninitializedState(this))
    , mDefaultState(new DefaultState(this))
    , mInactiveState(new InactiveState(this))
    , mDisconnectState(new DisconnectedState(this))
    , mScanState(new ScanState(this))
    , mHandshakeState(new HandshakeState(this))
    , mCompletedState(new CompletedState(this))
    , mDormantState(new DormantState(this))
{
    StateMachine::constructor(TAG, l);
    AutoPtr<IInterface> service;
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    serviceManager->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&service);
    mBatteryStats = IIBatteryStats::Probe(service);
    AddState(mDefaultState);
    AddState(mUninitializedState, mDefaultState);
    AddState(mInactiveState, mDefaultState);
    AddState(mDisconnectState, mDefaultState);
    AddState(mScanState, mDefaultState);
    AddState(mHandshakeState, mDefaultState);
    AddState(mCompletedState, mDefaultState);
    AddState(mDormantState, mDefaultState);

    SetInitialState(mUninitializedState);
    SetLogRecSize(50);
    SetLogOnlyTransitions(TRUE);

    //start the state machine
    Start();
}

void SupplicantStateTracker::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        DBG = true;
    }
    else {
        DBG = false;
    }
}

String SupplicantStateTracker::GetSupplicantStateName()
{
    String name;
    GetCurrentState()->GetName(&name);
    return name;
}


void SupplicantStateTracker::HandleNetworkConnectionFailure(
    /* [in] */ Int32 netId,
    /* [in] */ Int32 disableReason)
{
    if (DBG) {
        Logger::D(TAG, "handleNetworkConnectionFailure netId=%d, reason: %d, mNetworksDisabledDuringConnect=", netId, disableReason, mNetworksDisabledDuringConnect);
    }
    /* If other networks disabled during connection, enable them */
    if (mNetworksDisabledDuringConnect) {
        mWifiConfigStore->EnableAllNetworks();
        mNetworksDisabledDuringConnect = FALSE;
    }
    /* Disable failed network */
    Boolean bTmp;
    mWifiConfigStore->DisableNetwork(netId, disableReason, &bTmp);
}

void SupplicantStateTracker::TransitionOnSupplicantStateChange(
    /* [in] */ CStateChangeResult* stateChangeResult)
{
    SupplicantState supState;
    stateChangeResult->GetSupplicantState(&supState);

    if (DBG) Logger::D(TAG, "Supplicant state: %d\n", supState);

    switch (supState) {
        case Elastos::Droid::Wifi::SupplicantState_DISCONNECTED:
            TransitionTo(mDisconnectState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_INTERFACE_DISABLED:
            //we should have received a disconnection already, do nothing
            break;
        case Elastos::Droid::Wifi::SupplicantState_SCANNING:
            TransitionTo(mScanState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_AUTHENTICATING:
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATING:
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATED:
        case Elastos::Droid::Wifi::SupplicantState_FOUR_WAY_HANDSHAKE:
        case Elastos::Droid::Wifi::SupplicantState_GROUP_HANDSHAKE:
            TransitionTo(mHandshakeState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_COMPLETED:
            TransitionTo(mCompletedState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_DORMANT:
            TransitionTo(mDormantState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_INACTIVE:
            TransitionTo(mInactiveState);
            break;
        case Elastos::Droid::Wifi::SupplicantState_UNINITIALIZED:
        case Elastos::Droid::Wifi::SupplicantState_INVALID:
            TransitionTo(mUninitializedState);
            break;
        default:
            Logger::E(TAG, "Unknown supplicant state %d", supState);
            break;
    }
}

void SupplicantStateTracker::SendSupplicantStateChangedBroadcast(
    /* [in] */ SupplicantState state,
    /* [in] */ Boolean failedAuth)
{
    Int32 supplState;
    switch (state) {
        case Elastos::Droid::Wifi::SupplicantState_DISCONNECTED:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_DISCONNECTED; break;
        case Elastos::Droid::Wifi::SupplicantState_INTERFACE_DISABLED:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_INTERFACE_DISABLED; break;
        case Elastos::Droid::Wifi::SupplicantState_INACTIVE:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_INACTIVE; break;
        case Elastos::Droid::Wifi::SupplicantState_SCANNING:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_SCANNING; break;
        case Elastos::Droid::Wifi::SupplicantState_AUTHENTICATING:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_AUTHENTICATING; break;
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATING:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_ASSOCIATING; break;
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATED:
            supplState = IBatteryStats::WIFI_SUPPL_STATE_ASSOCIATED; break;
        case Elastos::Droid::Wifi::SupplicantState_FOUR_WAY_HANDSHAKE:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_FOUR_WAY_HANDSHAKE; break;
        case Elastos::Droid::Wifi::SupplicantState_GROUP_HANDSHAKE:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_GROUP_HANDSHAKE; break;
        case Elastos::Droid::Wifi::SupplicantState_COMPLETED:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_COMPLETED; break;
        case Elastos::Droid::Wifi::SupplicantState_DORMANT:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_DORMANT; break;
        case Elastos::Droid::Wifi::SupplicantState_UNINITIALIZED:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_UNINITIALIZED; break;
        case Elastos::Droid::Wifi::SupplicantState_INVALID:
             supplState = IBatteryStats::WIFI_SUPPL_STATE_INVALID; break;
        default:
              Logger::W(TAG, "Unknown supplicant state %d", state);
              supplState = IBatteryStats::WIFI_SUPPL_STATE_INVALID;
              break;
    }
    //try {
    mBatteryStats->NoteWifiSupplicantStateChanged(supplState, failedAuth);
    //} catch (RemoteException e) {
        // Won't happen.
    //}

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);

    AutoPtr<ISupplicantState> iss;
    CSupplicantState::New(state, (ISupplicantState**)&iss);
    intent->PutExtra(IWifiManager::EXTRA_NEW_STATE, IParcelable::Probe(iss.Get()));

    if (failedAuth) {
        intent->PutExtra(
                IWifiManager::EXTRA_SUPPLICANT_ERROR,
                IWifiManager::ERROR_AUTHENTICATING);
    }

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mContext->SendStickyBroadcastAsUser(intent, ALL);
}

ECode SupplicantStateTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StateMachine::Dump(fd, pw, args);
    pw->Println(String("mAuthenticationFailuresCount "));
    pw->Println(mAuthenticationFailuresCount);
    pw->Println(String("mAuthFailureInSupplicantBroadcast "));
    pw->Println(mAuthFailureInSupplicantBroadcast);
    pw->Println(String("mNetworksDisabledDuringConnect "));
    pw->Println(mNetworksDisabledDuringConnect);
    pw->Println();

    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
