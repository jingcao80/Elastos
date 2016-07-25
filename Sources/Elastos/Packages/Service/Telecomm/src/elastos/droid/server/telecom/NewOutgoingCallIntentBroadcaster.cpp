
#include "elastos/droid/server/telecom/NewOutgoingCallIntentBroadcaster.h"
#include "R.h"
#include "elastos/droid/server/telecom/CNewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Telecom::CGatewayInfo;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver
//=============================================================================
CAR_INTERFACE_IMPL(NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver, BroadcastReceiver, INewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver)

ECode NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (NewOutgoingCallIntentBroadcaster*) IObject::Probe(host);
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Log::V("NewOutgoingCallIntentBroadcaster", "onReceive: %s", TO_CSTR(intent));
    // Once the NEW_OUTGOING_CALL broadcast is finished, the resultData is used as the
    // actual number to call. (If null, no call will be placed.)
    String resultNumber;
    GetResultData(&resultNumber);
    Log::V("NewOutgoingCallIntentBroadcaster", "- got number from resultData: %s", Log::Pii(StringUtils::ParseCharSequence(resultNumber)).string());
    Boolean endEarly = FALSE;
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isPotentialLocalEmergencyNumber;
    phoneNumberUtilsHelper->IsPotentialLocalEmergencyNumber(mHost->mContext, resultNumber, &isPotentialLocalEmergencyNumber);
    if (resultNumber == NULL) {
        Log::V("NewOutgoingCallIntentBroadcaster", "Call cancelled (null number), returning...");
        endEarly = TRUE;
    } else if (isPotentialLocalEmergencyNumber) {
        Log::W("NewOutgoingCallIntentBroadcaster", "Cannot modify outgoing call to emergency number %s.", resultNumber.string());
        endEarly = TRUE;
    }
    if (endEarly) {
        if (mHost->mCall != NULL) {
            ((Call*) mHost->mCall.Get())->Disconnect();
        }
        return NOERROR;
    }
    Boolean isUriNumber;
    phoneNumberUtilsHelper->IsUriNumber(resultNumber, &isUriNumber);
    AutoPtr<IUri> resultHandleUri;
    Uri::FromParts(isUriNumber ?
            IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL, resultNumber, String(NULL), (IUri**)&resultHandleUri);
    AutoPtr<IUri> originalUri;
    mHost->mIntent->GetData((IUri**)&originalUri);
    String schemeSpecificPart;
    originalUri->GetSchemeSpecificPart(&schemeSpecificPart);
    if (schemeSpecificPart.Equals(resultNumber)) {
        Log::V("NewOutgoingCallIntentBroadcaster", "Call number unmodified after new outgoing call intent broadcast.");
    } else {
        Log::V("NewOutgoingCallIntentBroadcaster", "Retrieved modified handle after outgoing call intent broadcast: Original: %s, Modified: %s",
                Log::Pii(originalUri).string(),
                Log::Pii(resultHandleUri).string());
    }
    AutoPtr<IGatewayInfo> gatewayInfo;
    GetGateWayInfoFromIntent(intent, resultHandleUri, (IGatewayInfo**)&gatewayInfo);
    Boolean isEXTRA_START_CALL_WITH_SPEAKERPHONE;
    mHost->mIntent->GetBooleanExtra(ITelecomManager::EXTRA_START_CALL_WITH_SPEAKERPHONE, FALSE, &isEXTRA_START_CALL_WITH_SPEAKERPHONE);
    Int32 extraStartCallWithVideoState;
    mHost->mIntent->GetInt32Extra(ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE, IVideoProfileVideoState::AUDIO_ONLY, &extraStartCallWithVideoState);
    mHost->mCallsManager->PlaceOutgoingCall(mHost->mCall, resultHandleUri, gatewayInfo,
            isEXTRA_START_CALL_WITH_SPEAKERPHONE,
            extraStartCallWithVideoState);
    return NOERROR;
}

//=============================================================================
// NewOutgoingCallIntentBroadcaster
//=============================================================================
const String NewOutgoingCallIntentBroadcaster::EXTRA_GATEWAY_PROVIDER_PACKAGE("com.android.phone.extra.GATEWAY_PROVIDER_PACKAGE");
const String NewOutgoingCallIntentBroadcaster::EXTRA_GATEWAY_URI("com.android.phone.extra.GATEWAY_URI");
const String NewOutgoingCallIntentBroadcaster::EXTRA_GATEWAY_ORIGINAL_URI("com.android.phone.extra.GATEWAY_ORIGINAL_URI");
const String NewOutgoingCallIntentBroadcaster::PERMISSION = Elastos::Droid::Manifest::permission::PROCESS_OUTGOING_CALLS;
const String NewOutgoingCallIntentBroadcaster::EXTRA_ACTUAL_NUMBER_TO_DIAL("android.telecom.extra.ACTUAL_NUMBER_TO_DIAL");

NewOutgoingCallIntentBroadcaster::NewOutgoingCallIntentBroadcaster()
    : mIsDefaultOrSystemPhoneApp(FALSE)
{}

ECode NewOutgoingCallIntentBroadcaster::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CallsManager* callsManager,
    /* [in] */ ICall* call,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean isDefaultPhoneApp)
{
    mContext = context;
    mCallsManager = callsManager;
    mCall = call;
    mIntent = intent;
    mIsDefaultOrSystemPhoneApp = isDefaultPhoneApp;
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::ProcessIntent(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Log::V("NewOutgoingCallIntentBroadcaster", "Processing call intent in OutgoingCallIntentBroadcaster.");
    AutoPtr<IIntent> intent = mIntent;
    String action;
    intent->GetAction(&action);
    AutoPtr<IUri> handle;
    intent->GetData((IUri**)&handle);
    if (handle == NULL) {
        Log::W("NewOutgoingCallIntentBroadcaster", "Empty handle obtained from the call intent.");
        *result = Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER;
        return NOERROR;
    }
    String scheme;
    handle->GetScheme(&scheme);
    Boolean isVoicemailNumber = IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme);
    if (isVoicemailNumber) {
        if (IIntent::ACTION_CALL.Equals(action)
                || IIntent::ACTION_CALL_PRIVILEGED.Equals(action)) {
            // Voicemail calls will be handled directly by the telephony connection manager
            Log::I("NewOutgoingCallIntentBroadcaster", "Placing call immediately instead of waiting for OutgoingCallBroadcastReceiver: %s", TO_CSTR(intent));
            Boolean speakerphoneOn;
            mIntent->GetBooleanExtra(ITelecomManager::EXTRA_START_CALL_WITH_SPEAKERPHONE, FALSE, &speakerphoneOn);
            mCallsManager->PlaceOutgoingCall(mCall, handle, NULL, speakerphoneOn,
                    IVideoProfileVideoState::AUDIO_ONLY);
            *result = Elastos::Droid::Telephony::IDisconnectCause::NOT_DISCONNECTED;
            return NOERROR;
        } else {
            Log::I("NewOutgoingCallIntentBroadcaster", "Unhandled intent %s. Ignoring and not placing call.", TO_CSTR(intent));
            *result = Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_CANCELED;
            return NOERROR;
        }
    }
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    String number;
    phoneNumberUtilsHelper->GetNumberFromIntent(intent, mContext, &number);
    Boolean isConferenceUri;
    intent->GetBooleanExtra(
            ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, FALSE, &isConferenceUri);
    if (!isConferenceUri && TextUtils::IsEmpty(number)) {
        Log::W("NewOutgoingCallIntentBroadcaster", "Empty number obtained from the call intent.");
        *result = Elastos::Droid::Telephony::IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED;
        return NOERROR;
    }
    Boolean isUriNumber;
    phoneNumberUtilsHelper->IsUriNumber(number, &isUriNumber);
    Boolean isSkipSchemaParsing;
    intent->GetBooleanExtra(
            ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING, FALSE, &isSkipSchemaParsing);
    Log::V("NewOutgoingCallIntentBroadcaster","processIntent isConferenceUri: %d isSkipSchemaParsing = %d",
            isConferenceUri, isSkipSchemaParsing);
    if (!isUriNumber && !isConferenceUri && !isSkipSchemaParsing) {
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        phoneNumberUtilsHelper->ConvertKeypadLettersToDigits(number, &number);
        phoneNumberUtilsHelper->StripSeparators(number, &number);
    }
    Boolean isPotentialEmergencyNumber;
    IsPotentialEmergencyNumber(number, &isPotentialEmergencyNumber);
    Log::V("NewOutgoingCallIntentBroadcaster", "isPotentialEmergencyNumber = %d", isPotentialEmergencyNumber);
    RewriteCallIntentAction(intent, isPotentialEmergencyNumber);
    intent->GetAction(&action);
    // True for certain types of numbers that are not intended to be intercepted or modified
    // by third parties (e.g. emergency numbers).
    Boolean callImmediately = FALSE;
    if (IIntent::ACTION_CALL.Equals(action)) {
        if (isPotentialEmergencyNumber) {
            if (!mIsDefaultOrSystemPhoneApp) {
                Log::W("NewOutgoingCallIntentBroadcaster", "Cannot call potential emergency number %s with CALL Intent %s unless caller is system or default dialer.", number.string(), TO_CSTR(intent));
                AutoPtr<IUri> data;
                intent->GetData((IUri**)&data);
                LaunchSystemDialer(data);
                *result = Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_CANCELED;
                return NOERROR;
            } else {
                callImmediately = TRUE;
            }
        }
    } else if (IIntent::ACTION_CALL_EMERGENCY.Equals(action)) {
        if (!isPotentialEmergencyNumber) {
            Log::W("NewOutgoingCallIntentBroadcaster", "Cannot call non-potential-emergency number %s with EMERGENCY_CALL Intent %s.", number.string(), TO_CSTR(intent));
            *result = Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_CANCELED;
            return NOERROR;
        }
        callImmediately = TRUE;
    } else {
        Log::W("NewOutgoingCallIntentBroadcaster", "Unhandled Intent %s. Ignoring and not placing call.", TO_CSTR(intent));
        *result = Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER;
        return NOERROR;
    }
    Boolean isProcessAddParticipantOk;
    ProcessAddParticipant(intent, number, &isProcessAddParticipantOk);
    if (!isProcessAddParticipantOk && callImmediately) {
        Log::I("NewOutgoingCallIntentBroadcaster", "Placing call immediately instead of waiting for OutgoingCallBroadcastReceiver: %s", TO_CSTR(intent));
        String scheme = isUriNumber ? IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL;
        Boolean speakerphoneOn;
        mIntent->GetBooleanExtra(ITelecomManager::EXTRA_START_CALL_WITH_SPEAKERPHONE, FALSE, &speakerphoneOn);
        Int32 videoState;
        mIntent->GetInt32Extra(
                ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE,
                IVideoProfileVideoState::AUDIO_ONLY, &videoState);
        AutoPtr<IUri> uri;
        Uri::FromParts(scheme, number, String(NULL), (IUri**)&uri);
        mCallsManager->PlaceOutgoingCall(mCall, uri, NULL,
                speakerphoneOn, videoState);
        // Don't return but instead continue and send the ACTION_NEW_OUTGOING_CALL broadcast
        // so that third parties can still inspect (but not intercept) the outgoing call. When
        // the broadcast finally reaches the OutgoingCallBroadcastReceiver, we'll know not to
        // initiate the call again because of the presence of the EXTRA_ALREADY_CALLED extra.
    }
    BroadcastIntent(intent, number, !callImmediately);
    *result = Elastos::Droid::Telephony::IDisconnectCause::NOT_DISCONNECTED;
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::ProcessAddParticipant(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = FALSE;
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isUriNumber;
    phoneNumberUtilsHelper->IsUriNumber(handle, &isUriNumber);
    Boolean isADD_PARTICIPANT_KEY;
    intent->GetBooleanExtra(ITelephonyProperties::ADD_PARTICIPANT_KEY, FALSE, &isADD_PARTICIPANT_KEY);
    if (isADD_PARTICIPANT_KEY) {
        String scheme = isUriNumber ? IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL;
        Boolean speakerphoneOn;
        mIntent->GetBooleanExtra(ITelecomManager::EXTRA_START_CALL_WITH_SPEAKERPHONE, FALSE, &speakerphoneOn);
        Int32 videoState;
        mIntent->GetInt32Extra(
                ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE,
                IVideoProfileVideoState::AUDIO_ONLY, &videoState);
        AutoPtr<IUri> uri;
        Uri::FromParts(scheme, handle, String(NULL), (IUri**)&uri);
        mCallsManager->AddParticipant(mCall, uri, NULL,
                speakerphoneOn, videoState);
        ret = TRUE;
    }
    Log::D("NewOutgoingCallIntentBroadcaster", "processAddParticipant return = %d", ret);
    *result = ret;
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::BroadcastIntent(
    /* [in] */ IIntent* originalCallIntent,
    /* [in] */ const String& number,
    /* [in] */ Boolean receiverRequired)
{
    AutoPtr<IIntent> broadcastIntent;
    CIntent::New(IIntent::ACTION_NEW_OUTGOING_CALL, (IIntent**)&broadcastIntent);
    if (number != NULL) {
        broadcastIntent->PutExtra(IIntent::EXTRA_PHONE_NUMBER, number);
    }
    // Force receivers of this broadcast intent to run at foreground priority because we
    // want to finish processing the broadcast intent as soon as possible.
    broadcastIntent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    Log::V("NewOutgoingCallIntentBroadcaster", "Broadcasting intent: %s.", TO_CSTR(broadcastIntent));
    CheckAndCopyProviderExtras(originalCallIntent, broadcastIntent);
    AutoPtr<INewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver> receiver;
    if (receiverRequired) {
        CNewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver::New(TO_IINTERFACE(this), (INewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver**)&receiver);
    }
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> current;
    userHandleHelper->GetCURRENT((IUserHandle**)&current);
    mContext->SendOrderedBroadcastAsUser(
            broadcastIntent,
            current,
            PERMISSION,
            IBroadcastReceiver::Probe(receiver),
            NULL,  // scheduler
            IActivity::RESULT_OK,  // initialCode
            number,  // initialData: initial value for the result data (number to be modified)
            NULL);  // initialExtras
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::CheckAndCopyProviderExtras(
    /* [in] */ IIntent* src,
    /* [in] */ IIntent* dst)
{
    if (src == NULL) {
        return NOERROR;
    }
    Boolean hasGatewayProviderExtras;
    HasGatewayProviderExtras(src, &hasGatewayProviderExtras);
    if (hasGatewayProviderExtras) {
        String s;
        src->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &s);
        dst->PutExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, s);
        src->GetStringExtra(EXTRA_GATEWAY_URI, &s);
        dst->PutExtra(EXTRA_GATEWAY_URI, s);
        Log::D("NewOutgoingCallIntentBroadcaster", "Found and copied gateway provider extras to broadcast intent.");
        return NOERROR;
    }
    Log::D("NewOutgoingCallIntentBroadcaster", "No provider extras found in call intent.");
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::HasGatewayProviderExtras(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String name;
    intent->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &name);
    String uriString;
    intent->GetStringExtra(EXTRA_GATEWAY_URI, &uriString);
    *result = !TextUtils::IsEmpty(name) && !TextUtils::IsEmpty(uriString);
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::GetGatewayUriFromString(
    /* [in] */ const String& gatewayUriString,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)


    if (TextUtils::IsEmpty(gatewayUriString)) {
        *result = NULL;
    }
    else {
        Uri::Parse(gatewayUriString, result);
    }
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::GetGateWayInfoFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IUri* trueHandle,
    /* [out] */ IGatewayInfo** result)
{
    VALIDATE_NOT_NULL(result)

    if (intent == NULL) {
        *result = NULL;
        return NOERROR;
    }
    // Check if gateway extras are present.
    String gatewayPackageName;
    intent->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &gatewayPackageName);
    String s;
    intent->GetStringExtra(EXTRA_GATEWAY_URI, &s);
    AutoPtr<IUri> gatewayUri;
    GetGatewayUriFromString(s, (IUri**)&gatewayUri);
    if (!TextUtils::IsEmpty(gatewayPackageName) && gatewayUri != NULL) {
        return CGatewayInfo::New(gatewayPackageName, gatewayUri, trueHandle, result);
    }
    *result = NULL;
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::LaunchSystemDialer(
    /* [in] */ IUri* handle)
{
    AutoPtr<IIntent> systemDialerIntent;
    CIntent::New((IIntent**)&systemDialerIntent);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String resStr;
    resources->GetString(R::string::ui_default_package, &resStr);
    String resStr2;
    resources->GetString(R::string::dialer_default_class, &resStr2);
    systemDialerIntent->SetClassName(resStr, resStr);
    systemDialerIntent->SetAction(IIntent::ACTION_DIAL);
    systemDialerIntent->SetData(handle);
    systemDialerIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    Log::V("NewOutgoingCallIntentBroadcaster", "calling startActivity for default dialer: %s", TO_CSTR(systemDialerIntent));
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> current;
    userHandleHelper->GetCURRENT((IUserHandle**)&current);
    mContext->StartActivityAsUser(systemDialerIntent, current);
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log::V("NewOutgoingCallIntentBroadcaster", "Checking restrictions for number : %s", Log::Pii(StringUtils::ParseCharSequence(number)).string());
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isPotentialLocalEmergencyNumber;
    phoneNumberUtilsHelper->IsPotentialLocalEmergencyNumber(mContext, number, &isPotentialLocalEmergencyNumber);
    *result = (number != NULL) && isPotentialLocalEmergencyNumber;
    return NOERROR;
}

ECode NewOutgoingCallIntentBroadcaster::RewriteCallIntentAction(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean isPotentialEmergencyNumber)
{
    String action;
    intent->GetAction(&action);
    /* Change CALL_PRIVILEGED into CALL or CALL_EMERGENCY as needed. */
    if (IIntent::ACTION_CALL_PRIVILEGED.Equals(action)) {
        if (isPotentialEmergencyNumber) {
            Log::I("NewOutgoingCallIntentBroadcaster", "ACTION_CALL_PRIVILEGED is used while the number is a potential emergency number. Using ACTION_CALL_EMERGENCY as an action instead.");
            action = IIntent::ACTION_CALL_EMERGENCY;
        } else {
            action = IIntent::ACTION_CALL;
        }
        Log::V("NewOutgoingCallIntentBroadcaster", " - updating action from CALL_PRIVILEGED to %s", action.string());
        intent->SetAction(action);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
