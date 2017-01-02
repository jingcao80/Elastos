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

#include "R.h"
#include "elastos/droid/server/telecom/CallReceiver.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/NewOutgoingCallIntentBroadcaster.h"
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/net/Uri.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(CallReceiver, BroadcastReceiver, ICallReceiver)

const String CallReceiver::TAG("CallReceiver");
const String CallReceiver::KEY_IS_UNKNOWN_CALL("is_unknown_call");
const String CallReceiver::KEY_IS_INCOMING_CALL("is_incoming_call");
const String CallReceiver::KEY_IS_DEFAULT_DIALER("is_default_dialer");

ECode CallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean isUnknownCall;
    intent->GetBooleanExtra(KEY_IS_UNKNOWN_CALL, FALSE, &isUnknownCall);
    Boolean isIncomingCall;
    intent->GetBooleanExtra(KEY_IS_INCOMING_CALL, FALSE, &isIncomingCall);
    Log::I(TAG, "onReceive - isIncomingCall: %d isUnknownCall: %d", isIncomingCall,
            isUnknownCall);
    if (isUnknownCall) {
        ProcessUnknownCallIntent(intent);
    } else if (isIncomingCall) {
        ProcessIncomingCallIntent(intent);
    } else {
        ProcessOutgoingCallIntent(context, intent);
    }
    return NOERROR;
}

ECode CallReceiver::ProcessOutgoingCallIntent(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUri> handle;
    intent->GetData((IUri**)&handle);
    String scheme;
    handle->GetScheme(&scheme);
    String uriString;
    handle->GetSchemeSpecificPart(&uriString);
    if (!IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme)) {
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        Boolean isUriNumber;
        phoneNumberUtilsHelper->IsUriNumber(uriString, &isUriNumber);
        Uri::FromParts(isUriNumber ?
                IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL, uriString, String(NULL), (IUri**)&handle);
    }
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, (IParcelable**)&parcelable);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle = IPhoneAccountHandle::Probe(parcelable);
    AutoPtr<IBundle> clientExtras;
    Boolean hasExtra;
    intent->HasExtra(ITelecomManager::EXTRA_OUTGOING_CALL_EXTRAS, &hasExtra);
    if (hasExtra) {
        intent->GetBundleExtra(ITelecomManager::EXTRA_OUTGOING_CALL_EXTRAS, (IBundle**)&clientExtras);
    }
    Boolean isConferenceUri;
    intent->GetBooleanExtra(
            ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, FALSE, &isConferenceUri);
    Log::D(TAG, "isConferenceUri = %d", isConferenceUri);
    if (isConferenceUri) {
        if (clientExtras == NULL) {
            CBundle::New((IBundle**)&clientExtras);
        }
        clientExtras->PutBoolean(ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, isConferenceUri);
    }
    Boolean isAddParticipant;
    intent->GetBooleanExtra(
            ITelephonyProperties::ADD_PARTICIPANT_KEY, FALSE, &isAddParticipant);
    Log::D(TAG, "isAddparticipant = %d", isAddParticipant);
    if (isAddParticipant) {
        if (clientExtras == NULL) {
            CBundle::New((IBundle**)&clientExtras);
        }
        clientExtras->PutBoolean(ITelephonyProperties::ADD_PARTICIPANT_KEY, isAddParticipant);
    }
    Boolean isSkipSchemaParsing;
    intent->GetBooleanExtra(
            ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING, FALSE, &isSkipSchemaParsing);
    Log::D(TAG, "isSkipSchemaParsing = %d", isSkipSchemaParsing);
    if (isSkipSchemaParsing) {
        if (clientExtras == NULL) {
            CBundle::New((IBundle**)&clientExtras);
        }
        clientExtras->PutBoolean(ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING,
                isSkipSchemaParsing);
    }
    if (clientExtras == NULL) {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        helper->GetEMPTY((IBundle**)&clientExtras);
    }
    Boolean isDefaultDialer;
    intent->GetBooleanExtra(KEY_IS_DEFAULT_DIALER, FALSE, &isDefaultDialer);
    // Send to CallsManager to ensure the InCallUI gets kicked off before the broadcast returns
    AutoPtr<CallsManager> callsManager;
    GetCallsManager((CallsManager**)&callsManager);
    AutoPtr<ICall> call;
    callsManager->StartOutgoingCall(handle, phoneAccountHandle, clientExtras, (ICall**)&call);
    if (call != NULL) {
        // Asynchronous calls should not usually be made inside a BroadcastReceiver because once
        // onReceive is complete, the BroadcastReceiver's process runs the risk of getting
        // killed if memory is scarce. However, this is OK here because the entire Telecom
        // process will be running throughout the duration of the phone call and should never
        // be killed.
        AutoPtr<NewOutgoingCallIntentBroadcaster> broadcaster = new NewOutgoingCallIntentBroadcaster();
        broadcaster->constructor(
                context, callsManager, call, intent, isDefaultDialer);
        Int32 result;
        broadcaster->ProcessIntent(&result);
        const Boolean success = result == Elastos::Droid::Telephony::IDisconnectCause::NOT_DISCONNECTED;
        if (!success && call != NULL) {
            DisconnectCallAndShowErrorDialog(context, call, result);
        }
    }
    return NOERROR;
}

ECode CallReceiver::ProcessIncomingCallIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, (IParcelable**)&parcelable);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle = IPhoneAccountHandle::Probe(parcelable);
    if (phoneAccountHandle == NULL) {
        Log::W(TAG, "Rejecting incoming call due to null phone account");
        return NOERROR;
    }
    AutoPtr<IComponentName> componentName;
    phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
    if (componentName == NULL) {
        Log::W(TAG, "Rejecting incoming call due to null component name");
        return NOERROR;
    }
    AutoPtr<IBundle> clientExtras;
    Boolean hasExtra;
    intent->HasExtra(ITelecomManager::EXTRA_INCOMING_CALL_EXTRAS, &hasExtra);
    if (hasExtra) {
        intent->GetBundleExtra(ITelecomManager::EXTRA_INCOMING_CALL_EXTRAS, (IBundle**)&clientExtras);
    }
    if (clientExtras == NULL) {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        helper->GetEMPTY((IBundle**)&clientExtras);
    }
    Log::D(TAG, "Processing incoming call from connection service [%s]",
            TO_CSTR(componentName));
    AutoPtr<CallsManager> callsManager;
    GetCallsManager((CallsManager**)&callsManager);
    callsManager->ProcessIncomingCallIntent(phoneAccountHandle, clientExtras);
    return NOERROR;
}

ECode CallReceiver::ProcessUnknownCallIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, (IParcelable**)&parcelable);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle = IPhoneAccountHandle::Probe(parcelable);
    if (phoneAccountHandle == NULL) {
        Log::W(TAG, "Rejecting unknown call due to null phone account");
        return NOERROR;
    }
    AutoPtr<IComponentName> componentName;
    phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
    if (componentName == NULL) {
        Log::W(TAG, "Rejecting unknown call due to null component name");
        return NOERROR;
    }
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    AutoPtr<CallsManager> callsManager;
    GetCallsManager((CallsManager**)&callsManager);
    callsManager->AddNewUnknownCall(phoneAccountHandle, extras);
    return NOERROR;
}

ECode CallReceiver::GetCallsManager(
    /* [out] */ CallsManager** result)
{
    VALIDATE_NOT_NULL(result)

    *result = CallsManager::GetInstance();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallReceiver::DisconnectCallAndShowErrorDialog(
    /* [in] */ IContext* context,
    /* [in] */ ICall* call,
    /* [in] */ Int32 errorCode)
{
    ((Call*) call)->Disconnect();
    AutoPtr<IIntent> errorIntent;
    CIntent::New(context, ECLSID_CErrorDialogActivity, (IIntent**)&errorIntent);
    Int32 errorMessageId = -1;
    switch (errorCode) {
        case Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER:
        case Elastos::Droid::Telephony::IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED:
            errorMessageId = R::string::outgoing_call_error_no_phone_number_supplied;
            break;
    }
    if (errorMessageId != -1) {
        errorIntent->PutExtra(IErrorDialogActivity::ERROR_MESSAGE_ID_EXTRA, errorMessageId);
    }
    errorIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> current;
    userHandleHelper->GetCURRENT((IUserHandle**)&current);
    context->StartActivityAsUser(errorIntent, current);
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
