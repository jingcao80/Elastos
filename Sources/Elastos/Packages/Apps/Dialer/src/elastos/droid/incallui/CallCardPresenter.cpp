
#include "elastos/droid/incallui/CallCardPresenter.h"
#include "elastos/droid/incallui/CallTimer.h"
#include "elastos/droid/incallui/CallerInfoUtils.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::InCallUI::EIID_IContactInfoCacheCallback;
using Elastos::Droid::InCallUI::EIID_ICallCardPresenter;
using Elastos::Droid::InCallUI::EIID_IIncomingCallListener;
using Elastos::Droid::InCallUI::EIID_IIncomingCallListener;
using Elastos::Droid::InCallUI::EIID_IInCallDetailsListener;
using Elastos::Droid::InCallUI::EIID_IInCallEventListener;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Telephony::Utility::CBlacklistUtils;
using Elastos::Droid::Internal::Telephony::Utility::IBlacklistUtils;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::CPhoneCapabilities;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IStatusHints;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CallCardPresenter::ContactLookupCallback
//================================================================

CallCardPresenter::ContactLookupCallback(
    /* [in] */ CallCardPresenter* callCardPresenter,
    /* [in] */ Boolean isPrimary)
    : mIsPrimary(isPrimary)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(callCardPresenter);
    wrs->GetWeakReference((IWeakReference**)&mCallCardPresenter);
}

CAR_INTERFACE_IMPL(CallCardPresenter::ContactLookupCallback, Object, IContactInfoCacheCallback);

ECode CallCardPresenter::ContactLookupCallback::OnContactInfoComplete(
    /* [in] */ const String& callId,
    /* [in] */ IContactCacheEntry* entry)
{
    AutoPtr<IInterface> obj;
    mCallCardPresenter->Resolve(EIID_ICallCardPresenter, (IInterface**)&obj);
    AutoPtr<ICallCardPresenter> p = ICallCardPresenter::Probe(obj);
    if (p != NULL) {
        AutoPtr<CallCardPresenter> presenter = (CallCardPresenter*)p.Get();
        presenter->OnContactInfoComplete(callId, entry, mIsPrimary);
    }
    return NOERROR;
}

ECode CallCardPresenter::ContactLookupCallback::OnImageLoadComplete(
    /* [in] */ const String& callId,
    /* [in] */ IContactCacheEntry* entry)
{
    AutoPtr<IInterface> obj;
    mCallCardPresenter->Resolve(EIID_ICallCardPresenter, (IInterface**)&obj);
    AutoPtr<ICallCardPresenter> p = ICallCardPresenter::Probe(obj);
    if (p != NULL) {
        AutoPtr<CallCardPresenter> presenter = (CallCardPresenter*)p.Get();
        presenter->OnImageLoadComplete(callId, entry);
    }
    return NOERROR;
}


//================================================================
// CallCardPresenter::UpdateCallTimeRunnable
//================================================================

ECode CallCardPresenter::UpdateCallTimeRunnable::Run()
{
    mHost->UpdateCallTime();
    return NOERROR;
}


//==========================================================================
// CallCardPresenter::DialogOnClickListener
//==========================================================================

CAR_INTERFACE_IMPL(CallCardPresenter::DialogOnClickListener, Object, IDialogInterfaceOnClickListener);

ECode CallCardPresenter::DialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::D(TAG, "hanging up due to blacklist: %s", mHost->mPrimary->GetId().string());
    TelecomAdapter::GetInstance()->DisconnectCall(mHost->mPrimary->GetId());
    AutoPtr<IBlacklistUtils> utils;
    CBlacklistUtils::AcquireSingleton((IBlacklistUtils**)&utils);
    utils->AddOrUpdate(mContext, mHost->mPrimary->GetNumber(),
            IBlacklistUtils::BLOCK_CALLS, IBlacklistUtils::BLOCK_CALLS);
    return NOERROR;
}


//================================================================
// CallCardPresenter
//================================================================

const String CallCardPresenter::TAG("CallCardPresenter");
const Int64 CALL_TIME_UPDATE_INTERVAL_MS;

CAR_INTERFACE_IMPL_5(CallCardPresenter, Presenter
        , ICallCardPresenter
        , IInCallStateListener
        , IIncomingCallListener
        , IInCallDetailsListener
        , IInCallEventListener);

CallCardPresenter::CallCardPresenter()
{
    // create the call timer
    AutoPtr<IRunnable> runnable = (IRunnable*)new UpdateCallTimeRunnable(this);
    mCallTimer = new CallTimer(runnable);
}

void CallCardPresenter::Init(
    /* [in] */ IContext* context,
    /* [in] */ Call* call)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    mContext = NULL;
    preconditions->CheckNotNull(context, (IContext**)&mContext);

    // Call may be null if disconnect happened already.
    if (call != NULL) {
        mPrimary = call;

        // start processing lookups right away.
        if (!call->IsConferenceCall()) {
            StartContactInfoSearch(call, TRUE, call->GetState() == Call::State::INCOMING);
        }
        else {
            UpdateContactEntry(NULL, TRUE);
        }
    }
}

ECode CallCardPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    Presenter::OnUiReady(ui);

    // Contact search may have completed before ui is ready.
    if (mPrimaryContactInfo != NULL) {
        UpdatePrimaryDisplayInfo();
    }

    // Register for call state changes last
    AutoPtr<InCallPresenter> presenter = InCallPresenter::GetInstance();
    presenter->AddListener(this);
    presenter->AddIncomingCallListener(this);
    presenter->AddDetailsListener(this);
    presenter->AddInCallEventListener(this);
    return NOERROR;
}

ECode CallCardPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    Presenter::OnUiUnready(ui);

    // stop getting call state changes
    AutoPtr<InCallPresenter> presenter = InCallPresenter::GetInstance();
    presenter->RemoveListener(this);
    presenter->RemoveIncomingCallListener(this);
    presenter->RemoveDetailsListener(this);
    presenter->RemoveInCallEventListener(this);

    mPrimary = NULL;
    mPrimaryContactInfo = NULL;
    mSecondaryContactInfo = NULL;
    return NOERROR;
}

ECode CallCardPresenter::OnIncomingCall(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICall* call)
{
    // same logic should happen as with onStateChange()
    AutoPtr<CallList> callList = CallList::GetInstance()
    OnStateChange(oldState, newState, callList);
    return NOERROR;
}

ECode CallCardPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    Logger::D(TAG, "onStateChange() %s", TO_CSTR(newState));
    AutoPtr<CallCardUi> ui = GetUi();
    if (ui == NULL) {
        return NOERROR;
    }

    AutoPtr<Call> primary;
    AutoPtr<Call> secondary;

    if (newState == InCallState::INCOMING) {
        primary = callList->GetIncomingCall();
    }
    else if (newState == InCallState::PENDING_OUTGOING || newState == InCallState::OUTGOING) {
        primary = callList->GetOutgoingCall();
        if (primary == NULL) {
            primary = callList->GetPendingOutgoingCall();
        }
        // getCallToDisplay doesn't go through outgoing or incoming calls. It will return the
        // highest priority call to display as the secondary call.
        secondary = GetCallToDisplay(callList, NULL, TRUE);
    }
    else if (newState == InCallState::INCALL) {
        primary = GetCallToDisplay(callList, NULL, FALSE);
        secondary = GetCallToDisplay(callList, primary, TRUE);
    }

    Logger::D(TAG, "Primary call: %s", TO_CSTR(primary));
    Logger::D(TAG, "Secondary call: %s", TO_CSTR(secondary));

    Boolean primaryChanged = !Call::AreSame(mPrimary, primary);
    Boolean primaryForwardedChanged = IsForwarded(mPrimary) != IsForwarded(primary);
    Boolean secondaryChanged = !Call::AreSame(mSecondary, secondary);

    mSecondary = secondary;
    mPrimary = primary;

    // Refresh primary call information if either:
    // 1. Primary call changed.
    // 2. The call's ability to manage conference has changed.
    if (mPrimary != NULL && (primaryChanged ||
            ui->IsManageConferenceVisible() != ShouldShowManageConference())) {
        // primary call has changed
        mPrimaryContactInfo = ContactInfoCache::BuildCacheEntryFromCall(mContext, mPrimary,
                mPrimary->GetState() == Call::State::INCOMING);
        UpdatePrimaryDisplayInfo();
        MaybeStartSearch(mPrimary, TRUE);
        mPrimary->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    }
    else if (primaryForwardedChanged && mPrimary != NULL) {
        UpdatePrimaryDisplayInfo();
    }

    if (mSecondary == NULL) {
        // Secondary call may have ended.  Update the ui.
        mSecondaryContactInfo = NULL;
        UpdateSecondaryDisplayInfo();
    }
    else if (secondaryChanged) {
        // secondary call has changed
        mSecondaryContactInfo = ContactInfoCache::BuildCacheEntryFromCall(mContext, mSecondary,
                mSecondary->GetState() == Call::State::INCOMING);
        UpdateSecondaryDisplayInfo();
        MaybeStartSearch(mSecondary, FALSE);
        mSecondary->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    }

    // Start/stop timers.
    if (mPrimary != NULL && mPrimary->GetState() == Call::State::ACTIVE) {
        Logger::D(TAG, "Starting the calltime timer");
        mCallTimer->Start(CALL_TIME_UPDATE_INTERVAL_MS);
    }
    else {
        Logger::D(TAG, "Canceling the calltime timer");
        mCallTimer->Cancel();
        ui->SetPrimaryCallElapsedTime(FALSE, NULL);
    }

    // Set the call state
    Int32 callState = Call::State::IDLE;
    if (mPrimary != NULL) {
        callState = mPrimary->GetState();
        UpdatePrimaryCallState();
    }
    else {
        AutoPtr<IDisconnectCause> cause;
        CDisconnectCause::New(DisconnectCause::UNKNOWN, (IDisconnectCause**)&cause);
        AutoPtr<IUi> ui;
        GetUi((IUi**)&ui);
        ICallCardUi::Probe(ui)->SetCallState(callState, VideoProfile::VideoState::AUDIO_ONLY,
                Call::SessionModificationState::NO_REQUEST, cause,
                String(NULL), NULL, String(NULL));
    }

    // Hide/show the contact photo based on the video state.
    // If the primary call is a video call on hold, still show the contact photo.
    // If the primary call is an active video call, hide the contact photo.
    if (mPrimary != NULL) {
        AutoPtr<IUi> ui;
        GetUi((IUi**)&ui);
        ICallCardUi::Probe(ui)->SetPhotoVisible(!(mPrimary->IsVideoCall(mContext) &&
                callState != Call::State::ONHOLD));
    }

    MaybeShowManageConferenceCallButton();

    Boolean enableEndCallButton = Call::State::IsConnectingOrConnected(callState) &&
            callState != Call::State::INCOMING && mPrimary != NULL;
    // Hide the end call button instantly if we're receiving an incoming call.
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallCardUi::Probe(ui)->SetEndCallButtonEnabled(
            enableEndCallButton, callState != Call::State::INCOMING /* animate */);

    return NOERROR;
}

ECode CallCardPresenter::OnDetailsChanged(
    /* [in] */ ICall* call,
    /* [in] */ ICallDetails* details)
{
    UpdatePrimaryCallState();
    return NOERROR;
}

String CallCardPresenter::GetSubscriptionNumber()
{
    // If it's an emergency call, and they're not populating the callback number,
    // then try to fall back to the phone sub info (to hopefully get the SIM's
    // number directly from the telephony layer).
    AutoPtr<IPhoneAccountHandle> accountHandle = mPrimary->GetAccountHandle();
    if (accountHandle != NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
        AutoPtr<ITelecomManager> mgr = ITelecomManager::Probe(service);
        AutoPtr<IPhoneAccount> account;
        mgr->GetPhoneAccount(accountHandle, (IPhoneAccount**)&account);
        if (account != NULL) {
            AutoPtr<IUri> uri;
            account->GetSubscriptionAddress((IUri**)&uri);
            return GetNumberFromHandle(uri);
        }
    }
    return String(NULL);
}

void CallCardPresenter::UpdatePrimaryCallState()
{
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    if (ui != NULL && mPrimary != NULL) {
        ICallCardUi::Probe(ui)->SetCallState(
                mPrimary->GetState(),
                mPrimary->GetVideoState(),
                mPrimary->GetSessionModificationState(),
                mPrimary->GetDisconnectCause(),
                GetConnectionLabel(),
                GetConnectionIcon(),
                GetGatewayNumber());
        SetCallbackNumber();
    }
}

void CallCardPresenter::MaybeShowManageConferenceCallButton()
{
    if (mPrimary == NULL) {
        AutoPtr<IUi> ui;
        GetUi((IUi**)&ui);
        ICallCardUi::Probe(ui)->ShowManageConferenceCallButton(ShouldShowManageConference());
        return;
    }

    Boolean canManageConference = mPrimary->Can(IPhoneCapabilities::MANAGE_CONFERENCE);
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallCardUi::Probe(ui)->ShowManageConferenceCallButton(mPrimary->IsConferenceCall() && canManageConference);
}

Boolean CallCardPresenter::ShouldShowManageConference()
{
    if (mPrimary == NULL) {
        return FALSE;
    }

    return mPrimary->Can(IPhoneCapabilities::MANAGE_CONFERENCE);
}

void CallCardPresenter::SetCallbackNumber()
{
    String callbackNumber(NULL);

    AutoPtr<IUir> handle = mPrimary->GetHandle();
    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    Boolean isEmergencyCall;
    utils->IsEmergencyNumber(GetNumberFromHandle(handle), &isEmergencyCall);
    if (isEmergencyCall) {
        callbackNumber = GetSubscriptionNumber();
    }
    else {
        AutoPtr<ICallDetails> details;
        mPrimary->GetTelecommCall()->GetDetails((ICallDetails**)&details);
        AutoPtr<IStatusHints> statusHints;
        details->GetStatusHints((IStatusHints**)&statusHints);
        if (statusHints != NULL) {
            AutoPtr<IBundle> extras;
            statusHints->GetExtras((IBundle**)&extras);
            if (extras != NULL) {
                extras->GetString(ITelecomManager::EXTRA_CALL_BACK_NUMBER, &callbackNumber);
            }
        }
    }

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelecomManager> telephonyManager = ITelecomManager::Probe(service);
    String simNumber;
    telephonyManager->GetLine1Number(&simNumber);
    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    Boolean result;
    if (utils->Compare(callbackNumber, simNumber, &result), result) {
        Logger::D(TAG, "Numbers are the same; not showing the callback number");
        callbackNumber = String(NULL);
    }

    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    ICallCardUi::Probe(ui)->SetCallbackNumber(callbackNumber, isEmergencyCall);
}

void CallCardPresenter::UpdateCallTime()
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallCardUi> ui = ICallCardUi::Probe(temp);

    if (ui == NULL || mPrimary == NULL || mPrimary->GetState() != Call::State::ACTIVE) {
        if (ui != NULL) {
            ui->SetPrimaryCallElapsedTime(FALSE, NULL);
        }
        mCallTimer->Cancel();
    }
    else {
        Int64 callStart = mPrimary->GetConnectTimeMillis();
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 current;
        sys->GetCurrentTimeMillis(&current);
        Int64 duration = current - callStart;
        AutoPtr<IDateUtils> utils;
        CDateUtils::AcquireSingleton((IDateUtils**)&utils);
        String result;
        utils->FormatElapsedTime(duration / 1000, &result);
        ui->SetPrimaryCallElapsedTime(TRUE, result);
    }
}

void CallCardPresenter::OnCallStateButtonTouched()
{
    assert(0);
    // AutoPtr<IIntent> broadcastIntent = ObjectFactory::GetCallStateButtonBroadcastIntent(mContext);
    // if (broadcastIntent != NULL) {
    //     Logger::D(TAG, "Sending call state button broadcast: %s", TO_CSTR(broadcastIntent));
    //     mContext->SendBroadcast(broadcastIntent, Elastos::Droid::Manifest::permission::READ_PHONE_STATE);
    // }
}

void CallCardPresenter::MaybeStartSearch(
    /* [in] */ Call* call,
    /* [in] */ Boolean isPrimary)
{
    // no need to start search for conference calls which show generic info.
    if (call != NULL && !call->IsConferenceCall()) {
        StartContactInfoSearch(call, isPrimary, call->GetState() == Call::State::INCOMING);
    }
}

void CallCardPresenter::StartContactInfoSearch(
    /* [in] */ Call* call,
    /* [in] */ Boolean isPrimary,
    /* [in] */ Boolean isIncoming)
{
    AutoPtr<ContactInfoCache> cache = ContactInfoCache::GetInstance(mContext);

    AutoPtr<ContactLookupCallback> cb = new ContactLookupCallback(this, isPrimary);
    cache->FindInfo(call, isIncoming, cb);
}

void CallCardPresenter::OnContactInfoComplete(
    /* [in] */ const String& callId,
    /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
    /* [in] */ Boolean isPrimary)
{
    UpdateContactEntry(entry, isPrimary);
    if (!entry->mName.IsNull()) {
        Logger::D(TAG, "Contact found: %s", TO_CSTR(entry));
    }
    if (entry->mContactUri != NULL) {
        CallerInfoUtils::SendViewNotification(mContext, entry->mContactUri);
    }
}

void CallCardPresenter::OnImageLoadComplete(
    /* [in] */ const String& callId,
    /* [in] */ ContactInfoCache::ContactCacheEntry* entry)
{
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    if (ui == NULL) {
        return;
    }

    if (entry->mPhoto != NULL) {
        if (mPrimary != NULL && callId.Equals(mPrimary->GetId())) {
            ICallCardUi::Probe(ui)->SetPrimaryImage(entry->mPhoto);
        }
    }
}

Boolean CallCardPresenter::IsConference(
    /* [in] */ Call* call)
{
    return call != NULL && call->IsConferenceCall();
}

Boolean CallCardPresenter::CanManageConference(
    /* [in] */ Call* call)
{
    return call != NULL && call->Can(IPhoneCapabilities::MANAGE_CONFERENCE);
}

void CallCardPresenter::UpdateContactEntry(
    /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
    /* [in] */ Boolean isPrimary,
    /* [in] */ Boolean isConference)
{
    if (isPrimary) {
        mPrimaryContactInfo = entry;
        UpdatePrimaryDisplayInfo();
    }
    else {
        mSecondaryContactInfo = entry;
        UpdateSecondaryDisplayInfo();
    }
}

AutoPtr<Call> callCardPresenter::GetCallToDisplay(
    /* [in] */ CallList* callList,
    /* [in] */ Call* ignore,
    /* [in] */ Boolean skipDisconnected)
{
    // Active calls come second.  An active call always gets precedent.
    AutoPtr<Call> retval = callList->GetActiveCall();
    if (retval != NULL && retval != ignore) {
        return retval;
    }

    // Disconnected calls get primary position if there are no active calls
    // to let user know quickly what call has disconnected. Disconnected
    // calls are very short lived.
    if (!skipDisconnected) {
        retval = callList->GetDisconnectingCall();
        if (retval != NULL && retval != ignore) {
            return retval;
        }
        retval = callList->GetDisconnectedCall();
        if (retval != NULL && retval != ignore) {
            return retval;
        }
    }

    // Then we go to background call (calls on hold)
    retval = callList->GetBackgroundCall();
    if (retval != NULL && retval != ignore) {
        return retval;
    }

    // Lastly, we go to a second background call.
    retval = callList->GetSecondBackgroundCall();

    return retval;
}

void CallCardPresenter::UpdatePrimaryDisplayInfo(
    /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
    /* [in] */ Boolean isConference)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallCardUi> ui = ICallCardUi::Probe(temp);
    if (ui == NULL) {
        // TODO: May also occur if search result comes back after ui is destroyed. Look into
        // removing that case completely.
        Logger::d(TAG, "updatePrimaryDisplayInfo called but ui is null!");
        return;
    }

    Boolean canManageConference = CanManageConference(mPrimary);
    if (entry != NULL && mPrimary != NULL) {
        String name = GetNameForCall(entry);
        String number = GetNumberForCall(entry);
        Boolean nameIsNumber = !name.IsNull() && name.Equals(entry->mNumber);
        ui->SetPrimary(number, name, nameIsNumber, entry->mLabel,
                entry->mPhoto, isConference, canManageConference, entry->mIsSipCall);
    }
    else {
        ui->SetPrimary(String(NULL), String(NULL), FALSE, String(NULL), NULL,
                isConference, canManageConference, FALSE)
    }
}

void CallCardPresenter::UpdateSecondaryDisplayInfo(
    /* [in] */ Boolean isConference)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallCardUi> ui = ICallCardUi::Probe(temp);
    if (ui == NULL) {
        return;
    }

    Boolean canManageConference = CanManageConference(mSecondary);
    if (mSecondaryContactInfo != NULL && mSecondary != NULL) {
        Logger::D(TAG, "updateSecondaryDisplayInfo() %s", TO_CSTR(mSecondaryContactInfo));
        String nameForCall = GetNameForCall(mSecondaryContactInfo);

        Boolean nameIsNumber = !nameForCall.IsNull() && nameForCall.Equals(
                mSecondaryContactInfo->mNumber);
        ui->SetSecondary(TRUE /* show */, nameForCall, nameIsNumber, mSecondaryContactInfo->mLabel,
                GetCallProviderLabel(mSecondary), GetCallProviderIcon(mSecondary),
                isConference, canManageConference);
    }
    else {
        // reset to nothing so that it starts off blank next time we use it.
        ui->SetSecondary(FALSE, String(NULL), FALSE, String(NULL), String(NULL), NULL, isConference, canManageConference);
    }
}

AutoPtr<IPhoneAccount> CallCardPresenter::GetAccountForCall(
    /* [in] */ Call* call)
{
    AutoPtr<IPhoneAccountHandle> accountHandle = call->GetAccountHandle();
    if (accountHandle == NULL) {
        return NULL;
    }
    AutoPtr<IPhoneAccount> account;
    GetTelecomManager()->GetPhoneAccount(accountHandle, (IPhoneAccount**)&account);
    return account;
}

String CallCardPresenter::GetGatewayNumber()
{
    if (HasOutgoingGatewayCall()) {
        AutoPtr<IUri> address;
        mPrimary->GetGatewayInfo()->GetGatewayAddress((IUri**)&address);
        return GetNumberFromHandle(address);
    }
    return String(NULL);
}

AutoPtr<IDrawable> CallCardPresenter::GetCallProviderIcon(
    /* [in] */ Call* call)
{
    AutoPtr<IPhoneAccount> account = GetAccountForCall(call);

    // on MSIM devices irrespective of number of enabled phone
    // accounts pick icon from phone account and display on UI
    Boolean result;
    if (account != NULL && (GetTelecomManager()->HasMultipleCallCapableAccounts(&result), result)
            || (CallList::PHONE_COUNT > 1))) {
        AutoPtr<IDrawable> d;
        account->GetIcon(mContext, (IDrawable**)&d);
        return d;
    }
    return NULL;
}

String CallCardPresenter::GetCallProviderLabel(
    /* [in] */ Call* call)
{
    AutoPtr<IPhoneAccount> account = GetAccountForCall(call);

    // on MSIM devices irrespective of number of
    // enabled phone accounts display label info on UI
    Boolean result;
    if (account != NULL && (GetTelecomManager()->HasMultipleCallCapableAccounts(&result), result)
            || (CallList::PHONE_COUNT > 1))) {
        AutoPtr<ICharSequence> cs;
        account->GetLabel((ICharSequence**)&cs);
        String str;
        cs->ToString(&str);
        return str;
    }
    return String(NULL);
}

String CallCardPresenter::GetConnectionLabel()
{
    AutoPtr<ICallDetails> details;
    mPrimary->GetTelecommCall()->GetDetails((ICallDetails**)&details);
    AutoPtr<IStatusHints> statusHints;
    details->GetStatusHints((IStatusHints**)&statusHints);
    if (statusHints != NULL) {
        AutoPtr<ICharSequence> label;
        statusHints->GetLabel((ICharSequence**)&label);
        if (!TextUtils::IsEmpty(label)) {
            String str;
            label->ToString(&str);
            return str;
        }
    }

    AutoPtr<IUi> ui;
    if (HasOutgoingGatewayCall() && (GetUi((IUi**)&ui), ui != NULL)) {
        // Return the label for the gateway app on outgoing calls.
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        // try {
        String packageName;
        mPrimary->GetGatewayInfo()->GetGatewayProviderPackageName(&packageName);
        AutoPtr<IApplicationInfo> info;
        if (FAILED(pm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&info))) {
            Logger::E(TAG, "Gateway Application Not Found.");
            return String(NULL);
        }
        AutoPtr<ICharSequence> label;
        pm->GetApplicationLabel((ICharSequence**)&label);
        String str;
        label->ToString(&str);
        return str;
        // } catch (PackageManager.NameNotFoundException e) {
        //     Log.e(this, "Gateway Application Not Found.", e);
        //     return null;
        // }
    }
    return GetCallProviderLabel(mPrimary);
}

AutoPtr<IDrawable> CallCardPresenter::GetConnectionIcon()
{
    AutoPtr<ICallDetails> details;
    mPrimary->GetTelecommCall()->GetDetails((ICallDetails**)&details);
    AutoPtr<IStatusHints> statusHints;
    details->GetStatusHints((IStatusHints**)&statusHints);
    Int32 id;
    if (statusHints != NULL && (statusHints->GetIconResId(&id), id != 0)) {
        AutoPtr<IDrawable> icon;
        statusHints->GetIcon(mContext, (IDrawable**)&icon);
        if (icon != NULL) {
            return icon;
        }
    }
    return GetCallProviderIcon(mPrimary);
}

Boolean CallCardPresenter::HasOutgoingGatewayCall()
{
    // We only display the gateway information while STATE_DIALING so return false for any othe
    // call state.
    // TODO: mPrimary can be null because this is called from updatePrimaryDisplayInfo which
    // is also called after a contact search completes (call is not present yet).  Split the
    // UI update so it can receive independent updates.
    if (mPrimary == NULL) {
        return FALSE;
    }
    Boolean isEmpty;
    return Call::State::IsDialing(mPrimary->GetState()) && mPrimary->GetGatewayInfo() != NULL &&
            (mPrimary->GetGatewayInfo()->IsEmpty(&isEmpty), !isEmpty);
}

String CallCardPresenter::GetNameForCall(
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo)
{
    if (TextUtils::IsEmpty(contactInfo->mName)) {
        return contactInfo.number;
    }
    return contactInfo->mName;
}

String CallCardPresenter::GetNumberForCall(
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo)
{
    // If the name is empty, we use the number for the name...so dont show a second
    // number in the number field
    if (TextUtils::IsEmpty(contactInfo->mName)) {
        return contactInfo->mLocation;
    }
    return contactInfo->mNumber;
}

void CallCardPresenter::SecondaryInfoClicked()
{
    if (mSecondary == NULL) {
        Logger::D(TAG, "Secondary info clicked but no secondary call.");
        return;
    }

    Logger::I(TAG, "Swapping call to foreground: %s", TO_CSTR(mSecondary));
    TelecomAdapter::GetInstance()->UnholdCall(mSecondary->GetId());
}

void CallCardPresenter::EndCallClicked()
{
    if (mPrimary == NULL) {
        return;
    }

    Logger::I(TAG, "Disconnecting call: %s", TO_CSTR(mPrimary));
    mPrimary->SetState(Call::State::DISCONNECTING);
    CallList::GetInstance()->OnUpdate(mPrimary);
    TelecomAdapter::GetInstance()->DisconnectCall(mPrimary->GetId());
}

String CallCardPresenter::GetNumberFromHandle(
    /* [in] */ IUri* handle)
{
    String str;
    return handle == NULL ? String("") : (handle->GetSchemeSpecificPart(&str), str);
}

ECode CallCardPresenter::OnFullScreenVideoStateChanged(
    /* [in] */ Boolean isFullScreenVideo)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<ICallCardUi> ui = ICallCardUi::Probe(temp);
    if (ui == NULL) {
        return NOERROR;
    }
    return ui->SetCallCardVisible(!isFullScreenVideo);
}

AutoPtr<ITelecomManager> CallCardPresenter::GetTelecomManager()
{
    if (mTelecomManager == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
        mTelecomManager = ITelecomManager::Probe(service);
    }
    return mTelecomManager;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
