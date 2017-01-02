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

#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::InCallUI::EIID_IInCallPhoneListener;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Telecom::ICallDetails;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Core::Thread;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const String TelecomAdapter::ADD_CALL_MODE_KEY("add_call_mode");
AutoPtr<TelecomAdapter> TelecomAdapter::sInstance;

CAR_INTERFACE_IMPL(TelecomAdapter, Object, IInCallPhoneListener)

AutoPtr<TelecomAdapter> TelecomAdapter::GetInstance()
{
    AutoPtr<IThread> thread;
    Looper::GetMainLooper()->GetThread((IThread**)&thread);
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if(FAILED(preconditions->CheckState(thread == Thread::GetCurrentThread())))
        return NULL;

    if (sInstance == NULL) {
        sInstance = new TelecomAdapter();
    }
    return sInstance;
}

TelecomAdapter::TelecomAdapter()
{}

void TelecomAdapter::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
}

ECode TelecomAdapter::SetPhone(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    return NOERROR;
}

ECode TelecomAdapter::ClearPhone()
{
    mPhone = NULL;
    return NOERROR;
}

AutoPtr<Elastos::Droid::Telecom::ICall> TelecomAdapter::GetTelecommCallById(
    /* [in] */ const String& callId)
{
    AutoPtr<Call> call = CallList::GetInstance()->GetCallById(callId);
    AutoPtr<Elastos::Droid::Telecom::ICall> icall = call == NULL ? NULL : call->GetTelecommCall();
    return icall;
}

void TelecomAdapter::AnswerCall(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    if (mPhone != NULL) {
        AutoPtr<Elastos::Droid::Telecom::ICall> call = GetTelecommCallById(callId);
        if (call != NULL) {
            call->Answer(videoState);
        }
        else {
            Logger::E("TelecomAdapter", "error answerCall, call not in call list: %s", callId.string());
        }
    }
    else {
        Logger::E("TelecomAdapter", "error answerCall, mPhone is NULL");
    }
}

void TelecomAdapter::RejectCall(
    /* [in] */ const String& callId,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& message)
{
    if (mPhone != NULL) {
        AutoPtr<Elastos::Droid::Telecom::ICall> call = GetTelecommCallById(callId);
        if (call != NULL) {
            call->Reject(rejectWithMessage, message);
        }
        else {
            Logger::E("TelecomAdapter", "error rejectCall, call not in call list: %s", callId.string());
        }
    }
    else {
        Logger::E("TelecomAdapter", "error rejectCall, mPhone is NULL");
    }
}

void TelecomAdapter::DisconnectCall(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->Disconnect();
    }
    else {
        Logger::E("TelecomAdapter", "error disconnectCall, mPhone is NULL");
    }
}

void TelecomAdapter::HoldCall(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->Hold();
    }
    else {
        Logger::E("TelecomAdapter", "error holdCall, mPhone is NULL");
    }
}

void TelecomAdapter::UnholdCall(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->Unhold();
    }
    else {
        Logger::E("TelecomAdapter", "error unholdCall, mPhone is NULL");
    }
}

void TelecomAdapter::Mute(
    /* [in] */ Boolean shouldMute)
{
    if (mPhone != NULL) {
        mPhone->SetMuted(shouldMute);
    }
    else {
        Logger::E("TelecomAdapter", "error mute, mPhone is NULL");
    }
}

void TelecomAdapter::SetAudioRoute(
    /* [in] */ Int32 route)
{
    if (mPhone != NULL) {
        mPhone->SetAudioRoute(route);
    }
    else {
        Logger::E("TelecomAdapter", "error setAudioRoute, mPhone is NULL");
    }
}

void TelecomAdapter::TurnOnProximitySensor()
{
    if (mPhone != NULL) {
        mPhone->SetProximitySensorOn();
    }
    else {
        Logger::E("TelecomAdapter", "error setProximitySensorOn, mPhone is NULL");
    }
}

void TelecomAdapter::TurnOffProximitySensor(
    /* [in] */ Boolean screenOnImmediately)
{
    if (mPhone != NULL) {
        mPhone->SetProximitySensorOff(screenOnImmediately);
    }
    else {
        Logger::E("TelecomAdapter", "error setProximitySensorOff, mPhone is NULL");
    }
}

void TelecomAdapter::SeparateCall(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->SplitFromConference();
    }
    else {
        Logger::E("TelecomAdapter", "error separateCall, mPhone is NULL.");
    }
}

void TelecomAdapter::Merge(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        AutoPtr<Elastos::Droid::Telecom::ICall> call = GetTelecommCallById(callId);
        AutoPtr<IList> conferenceable;
        call->GetConferenceableCalls((IList**)&conferenceable);
        Boolean isEmpty;
        if (conferenceable->IsEmpty(&isEmpty), !isEmpty) {
            AutoPtr<IInterface> value;
            conferenceable->Get(0, (IInterface**)&value);
            call->Conference(Elastos::Droid::Telecom::ICall::Probe(value));
        }
        else {
            AutoPtr<ICallDetails> details;
            call->GetDetails((ICallDetails**)&details);
            Int32 capabilities;
            details->GetCallCapabilities(&capabilities);
            if (0 != (capabilities & IPhoneCapabilities::MERGE_CONFERENCE)) {
                call->MergeConference();
            }
        }
    }
    else {
        Logger::E("TelecomAdapter", "error merge, mPhone is NULL.");
    }
}

void TelecomAdapter::Swap(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        AutoPtr<Elastos::Droid::Telecom::ICall> call = GetTelecommCallById(callId);
        AutoPtr<ICallDetails> details;
        call->GetDetails((ICallDetails**)&details);
        Int32 capabilities;
        details->GetCallCapabilities(&capabilities);
        if (0 != (capabilities & IPhoneCapabilities::SWAP_CONFERENCE)) {
            call->SwapConference();
        }
    }
    else {
        Logger::E("TelecomAdapter", "Error swap, mPhone is NULL.");
    }
}

void TelecomAdapter::AddCall()
{
    if (mContext != NULL) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_DIAL, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        // when we request the dialer come up, we also want to inform
        // it that we're going through the "add call" option from the
        // InCallScreen / PhoneUtils.
        intent->PutExtra(ADD_CALL_MODE_KEY, TRUE);
        // try {
        Logger::D("TelecomAdapter", "Sending the add Call intent");
        if (FAILED(mContext->StartActivity(intent))) {
            Logger::E("TelecomAdapter", "Activity for adding calls isn't found.");
        }
        // } catch (ActivityNotFoundException e) {
        //     // This is rather rare but possible.
        //     // Note: this method is used even when the phone is encrypted. At that moment
        //     // the system may not find any Activity which can accept this Intent.
        //     Logger::E("TelecomAdapter", "Activity for adding calls isn't found.", e);
        // }
    }
}

void TelecomAdapter::PlayDtmfTone(
    /* [in] */ const String& callId,
    /* [in] */ Char32 digit)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->PlayDtmfTone(digit);
    }
    else {
        Logger::E("TelecomAdapter", "error playDtmfTone, mPhone is NULL");
    }
}

void TelecomAdapter::StopDtmfTone(
    /* [in] */ const String& callId)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->StopDtmfTone();
    }
    else {
        Logger::E("TelecomAdapter", "error stopDtmfTone, mPhone is NULL");
    }
}

void TelecomAdapter::PostDialContinue(
    /* [in] */ const String& callId,
    /* [in] */ Boolean proceed)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->PostDialContinue(proceed);
    }
    else {
        Logger::E("TelecomAdapter", "error postDialContinue, mPhone is NULL");
    }
}

void TelecomAdapter::PhoneAccountSelected(
    /* [in] */ const String& callId,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (mPhone != NULL) {
        GetTelecommCallById(callId)->PhoneAccountSelected(accountHandle);
    }
    else {
        Logger::E("TelecomAdapter", "error phoneAccountSelected, mAdapter is NULL");
    }

    if (accountHandle == NULL) {
        Logger::E("TelecomAdapter", "error phoneAccountSelected, accountHandle is NULL");
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
