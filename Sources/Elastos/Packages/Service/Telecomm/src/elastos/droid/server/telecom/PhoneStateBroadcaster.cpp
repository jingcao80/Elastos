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

#include "elastos/droid/server/telecom/PhoneStateBroadcaster.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/os/ServiceManager.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telephony::ITelephonyManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

PhoneStateBroadcaster::PhoneStateBroadcaster()
    : mCurrentState(ITelephonyManager::CALL_STATE_IDLE)
{}

ECode PhoneStateBroadcaster::constructor()
{
    mRegistry = IITelephonyRegistry::Probe(ServiceManager::GetService(
            String("telephony.registry")));
    if (mRegistry == NULL) {
        Log::W("PhoneStateBroadcaster", "TelephonyRegistry is null");
    }
    return NOERROR;
}

ECode PhoneStateBroadcaster::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    Boolean hasRingingCall;
    CallsManager::GetInstance()->HasRingingCall(&hasRingingCall);
    if ((newState == ICallState::DIALING || newState == ICallState::ACTIVE
            || newState == ICallState::ON_HOLD) && !hasRingingCall) {
        /*
         * EXTRA_STATE_RINGING takes precedence over EXTRA_STATE_OFFHOOK, so if there is
         * already a ringing call, don't broadcast EXTRA_STATE_OFFHOOK.
         */
        SendPhoneStateChangedBroadcast(call, ITelephonyManager::CALL_STATE_OFFHOOK);
    }
    return NOERROR;
}

ECode PhoneStateBroadcaster::OnCallAdded(
    /* [in] */ ICall* call)
{
    Int32 callState;
    ((Call*) call)->GetState(&callState);
    if (callState == ICallState::RINGING) {
        SendPhoneStateChangedBroadcast(call, ITelephonyManager::CALL_STATE_RINGING);
    }
    return NOERROR;
}

ECode PhoneStateBroadcaster::OnCallRemoved(
    /* [in] */ ICall* call)
{
    // Recalculate the current phone state based on the consolidated state of the remaining
    // calls in the call list.
    AutoPtr<CallsManager> callsManager = CallsManager::GetInstance();
    Int32 callState = ITelephonyManager::CALL_STATE_IDLE;
    Boolean hasRingingCall;
    callsManager->HasRingingCall(&hasRingingCall);
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);
    (*array)[0] = ICallState::DIALING;
    (*array)[1] = ICallState::ACTIVE;
    (*array)[2] = ICallState::ON_HOLD;
    AutoPtr<ICall> firstCall;
    callsManager->GetFirstCallWithState(array, (ICall**)&firstCall);
    if (hasRingingCall) {
        callState = ITelephonyManager::CALL_STATE_RINGING;
    } else if (firstCall != NULL) {
        callState = ITelephonyManager::CALL_STATE_OFFHOOK;
    }
    SendPhoneStateChangedBroadcast(call, callState);
    return NOERROR;
}

ECode PhoneStateBroadcaster::GetCallState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCurrentState;
    return NOERROR;
}

ECode PhoneStateBroadcaster::SendPhoneStateChangedBroadcast(
    /* [in] */ ICall* call,
    /* [in] */ Int32 phoneState)
{
    if (phoneState == mCurrentState) {
        return NOERROR;
    }
    mCurrentState = phoneState;
    String callHandle(NULL);
    AutoPtr<IUri> handle;
    ((Call*) call)->GetHandle((IUri**)&handle);
    if (handle != NULL) {
        handle->GetSchemeSpecificPart(&callHandle);
    }
    // try {
    ECode ec = NOERROR;
    do {
        if (mRegistry != NULL) {
            ec = mRegistry->NotifyCallState(phoneState, callHandle);
            if (FAILED(ec)) break;
            Log::I("PhoneStateBroadcaster", "Broadcasted state change: %d", mCurrentState);
        }
    } while(FALSE);
    // } catch (RemoteException e) {
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        Log::W("PhoneStateBroadcaster", "RemoteException when notifying TelephonyRegistry of call state change.");
    }
    // }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
