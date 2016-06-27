
#include "R.h"
#include "elastos/droid/server/telecom/CallAudioManager.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CreateConnectionProcessor.h"
#include "elastos/droid/server/telecom/InCallToneMonitor.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/NewOutgoingCallIntentBroadcaster.h"
#include "elastos/droid/server/telecom/RespondViaSmsManager.h"
#include "elastos/droid/server/telecom/RingbackPlayer.h"
#include "elastos/droid/server/telecom/StatusBarNotifier.h"
#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/Manifest.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Telecomm::Telecom::CCallState;
using Elastos::Droid::Telecomm::Telecom::CDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::IConnectionService;
using Elastos::Droid::Telecomm::Telecom::IGatewayInfo;
using Elastos::Droid::Telecomm::Telecom::IParcelableConnection;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecomm::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::MultiSimVariants;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Objects;
using Org::Apache::Http::Conn::Scheme::IScheme;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// CallsManager::LchHandler
//=============================================================================
CallsManager::LchHandler::LchHandler(
    /* [in] */ CallsManager* host)
    : mHost(host)
{}

ECode CallsManager::LchHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    if (msgWhat == PHONE_START_MSIM_INCALL_TONE) {
        Log::D(TAG, "PHONE_START_MSIM_INCALL_TONE...");
        mHost->StartMSimInCallTones();
    }
    else if (msgWhat == LCH_PLAY_DTMF) {
        mHost->PlayLchDtmf();
    }
    else if (msgWhat == LCH_STOP_DTMF) {
        mHost->StopLchDtmf();
    }
    return NOERROR;
}

//=============================================================================
// CallsManager
//=============================================================================
AutoPtr<CallsManager> CallsManager::INSTANCE;
const String CallsManager::TAG("CallsManager");
const Int32 CallsManager::MAXIMUM_LIVE_CALLS = 1;
const Int32 CallsManager::MAXIMUM_HOLD_CALLS = 1;
const Int32 CallsManager::MAXIMUM_RINGING_CALLS = 1;
const Int32 CallsManager::MAXIMUM_OUTGOING_CALLS = 1;
const Int32 CallsManager::MAXIMUM_DSDA_LIVE_CALLS = 2;
const Int32 CallsManager::MAXIMUM_DSDA_HOLD_CALLS = 2;
const Int32 CallsManager::MAXIMUM_TOP_LEVEL_CALLS = 10;
AutoPtr<ArrayOf<Int32> > CallsManager::OUTGOING_CALL_STATES = InitOUTGOING_CALL_STATES();
AutoPtr<ArrayOf<Int32> > CallsManager::LIVE_CALL_STATES = InitLIVE_CALL_STATES();
const Int32 CallsManager::LCH_PLAY_DTMF = 56;
const Int32 CallsManager::LCH_STOP_DTMF = 57;
const Int32 CallsManager::LCH_DTMF_PERIODICITY = 3000;
const Int32 CallsManager::LCH_DTMF_PERIOD = 500;
const Int32 CallsManager::PHONE_START_MSIM_INCALL_TONE = 55;
const String CallsManager::sSupervisoryCallHoldToneConfig = InitSupervisoryCallHoldToneConfig();

CallsManager::CallsManager()
    : mCanAddCall(TRUE)
    , mLchSub(NULL)
    , mLocalCallReminderTonePlayer(NULL)
    , mSupervisoryCallHoldTonePlayer(NULL)
    , mSubInConversation(NULL)
{
    AutoPtr<IConcurrentHashMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IConcurrentHashMap**)&map);
    AutoPtr<ICollections> collectionsHelper;
    CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
    collectionsHelper->SetFromMap(IMap::Probe(map), (ISet**)&mCalls);
    map = NULL;
    CConcurrentHashMap::New(16, 0.9f, 1, (IConcurrentHashMap**)&map);
    collectionsHelper->SetFromMap(IMap::Probe(map), (ISet**)&mListeners);
    CHashSet::New((ISet**)&mLocallyDisconnectingCalls);
    mHandler = new LchHandler(this);
}

AutoPtr<CallsManager> CallsManager::GetInstance()
{
    return INSTANCE;
}

ECode CallsManager::Initialize(
    /* [in] */ CallsManager* instance)
{
    INSTANCE = instance;
    return NOERROR;
}

ECode CallsManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ MissedCallNotifier* missedCallNotifier,
    /* [in] */ BlacklistCallNotifier* blacklistCallNotifier,
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar)
{
    mContext = context;
    mPhoneAccountRegistrar = phoneAccountRegistrar;
    mMissedCallNotifier = missedCallNotifier;
    mBlacklistCallNotifier = blacklistCallNotifier;
    AutoPtr<StatusBarNotifier> statusBarNotifier = new StatusBarNotifier();
    statusBarNotifier->constructor(context, this);
    mWiredHeadsetManager = new WiredHeadsetManager();
    mWiredHeadsetManager->constructor(context);
    mCallAudioManager = new CallAudioManager();
    mCallAudioManager->constructor(context, statusBarNotifier, mWiredHeadsetManager);
    AutoPtr<InCallTonePlayer::Factory> playerFactory = new InCallTonePlayer::Factory();
    playerFactory->constructor(mCallAudioManager);
    mPlayerFactory = playerFactory;
    mRinger = new Ringer();
    mRinger->constructor(mCallAudioManager, this, playerFactory, context);
    mHeadsetMediaButton = new HeadsetMediaButton();
    mHeadsetMediaButton->constructor(context, this);
    mTtyManager = new TtyManager();
    mTtyManager->constructor(context, mWiredHeadsetManager);
    mProximitySensorManager = new ProximitySensorManager();
    mProximitySensorManager->constructor(context);
    mPhoneStateBroadcaster = new PhoneStateBroadcaster();
    mPhoneStateBroadcaster->constructor();
    mCallLogManager = new CallLogManager();
    mCallLogManager->constructor(context);
    mInCallController = new InCallController();
    mInCallController->constructor(context);
    mDtmfLocalTonePlayer = new DtmfLocalTonePlayer();
    mDtmfLocalTonePlayer->constructor(context);
    mConnectionServiceRepository = new ConnectionServiceRepository();
    mConnectionServiceRepository->constructor(mPhoneAccountRegistrar,
            context);
    mCallerInfoBlocker = new SpamBlocker();
    mCallerInfoBlocker->constructor(context);
    mListeners->Add(TO_IINTERFACE(statusBarNotifier));
    mListeners->Add(TO_IINTERFACE(mCallLogManager));
    mListeners->Add(TO_IINTERFACE(mPhoneStateBroadcaster));
    mListeners->Add(TO_IINTERFACE(mInCallController));
    mListeners->Add(TO_IINTERFACE(mRinger));
    AutoPtr<RingbackPlayer> ringbackPlayer = new RingbackPlayer();
    ringbackPlayer->constructor(this, playerFactory);
    mListeners->Add(TO_IINTERFACE(ringbackPlayer));
    AutoPtr<InCallToneMonitor> inCallToneMonitor = new InCallToneMonitor();
    inCallToneMonitor->constructor(playerFactory, this);
    mListeners->Add(TO_IINTERFACE(inCallToneMonitor));
    mListeners->Add(TO_IINTERFACE(mCallAudioManager));
    mListeners->Add(TO_IINTERFACE(missedCallNotifier));
    mListeners->Add(TO_IINTERFACE(mDtmfLocalTonePlayer));
    mListeners->Add(TO_IINTERFACE(mHeadsetMediaButton));
    AutoPtr<RespondViaSmsManager> respondViaSmsManager;
    RespondViaSmsManager::GetInstance((RespondViaSmsManager**)&respondViaSmsManager);
    mListeners->Add(TO_IINTERFACE(respondViaSmsManager));
    mListeners->Add(TO_IINTERFACE(mProximitySensorManager));
    mListeners->Add(TO_IINTERFACE(mCallerInfoBlocker));
    return NOERROR;
}

ECode CallsManager::OnSuccessfulOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 callState)
{
    Log::V(TAG, "onSuccessfulOutgoingCall, %s", TO_CSTR(call));
    SetCallState(call, callState);
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        // Call was not added previously in startOutgoingCall due to it being a potential MMI
        // code, so add it now.
        AddCall(call);
    }
    // The call's ConnectionService has been updated.
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        AutoPtr<IConnectionServiceWrapper> connectionService;
        ((Call*) call)->GetConnectionService((IConnectionServiceWrapper**)&connectionService);
        listener->OnConnectionServiceChanged(call, NULL, connectionService);
    }
    MarkCallAsDialing(call);
    return NOERROR;
}

ECode CallsManager::OnFailedOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    Log::V(TAG, "onFailedOutgoingCall, call: %s", TO_CSTR(call));
    MarkCallAsRemoved(call);
    return NOERROR;
}

ECode CallsManager::OnSuccessfulIncomingCall(
    /* [in] */ ICall* incomingCall)
{
    Log::D(TAG, "onSuccessfulIncomingCall");
    Boolean isCallBlacklisted;
    IsCallBlacklisted(incomingCall, &isCallBlacklisted);
    String number;
    ((Call*) incomingCall)->GetNumber(&number);
    Boolean shouldBlock;
    mCallerInfoBlocker->ShouldBlock(number, &shouldBlock);
    if (isCallBlacklisted) {
        mCallLogManager->LogCall(incomingCall, ICalls::BLACKLIST_TYPE);
        AutoPtr<IDisconnectCause> disconnectCause;
        CDisconnectCause::New(Elastos::Droid::Telephony::IDisconnectCause::CALL_BLACKLISTED, (IDisconnectCause**)&disconnectCause);
        ((Call*) incomingCall)->SetDisconnectCause(disconnectCause);
    } else if (shouldBlock) {
        // TODO: show notification for blocked spam calls
        // TODO: add unique call type for spam
        mCallLogManager->LogCall(incomingCall, ICalls::BLACKLIST_TYPE);
        AutoPtr<IDisconnectCause> disconnectCause;
        CDisconnectCause::New(Droid::Telephony::IDisconnectCause::CALL_BLACKLISTED, (IDisconnectCause**)&disconnectCause);
        ((Call*) incomingCall)->SetDisconnectCause(disconnectCause);
    } else {
        SetCallState(incomingCall, ICallState::RINGING);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) incomingCall)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        String id;
        phoneAccountHandle->GetId(&id);
        Boolean hasMaximumRingingCalls;
        HasMaximumRingingCalls(id, &hasMaximumRingingCalls);
        if (hasMaximumRingingCalls) {
            ((Call*) incomingCall)->Reject(FALSE, String(NULL));
            // since the call was not added to the list of calls, we have to call the missed
            // call notifier and the call logger manually.
            mMissedCallNotifier->ShowMissedCallNotification(incomingCall);
            mCallLogManager->LogCall(incomingCall, ICalls::MISSED_TYPE);
        } else {
            ((Call*) incomingCall)->mIsActiveSub = TRUE;
            AddCall(incomingCall);
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) incomingCall)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            SetActiveSubscription(id);
        }
    }
    return NOERROR;
}

ECode CallsManager::OnFailedIncomingCall(
    /* [in] */ ICall* call)
{
    SetCallState(call, ICallState::DISCONNECTED);
    ((Call*) call)->RemoveListener(this);
    return NOERROR;
}

ECode CallsManager::OnSuccessfulUnknownCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 callState)
{
    SetCallState(call, callState);
    Log::I(TAG, "onSuccessfulUnknownCall for call %s", TO_CSTR(call));
    AddCall(call);
    return NOERROR;
}

ECode CallsManager::OnFailedUnknownCall(
    /* [in] */ ICall* call)
{
    Log::I(TAG, "onFailedUnknownCall for call %s", TO_CSTR(call));
    SetCallState(call, ICallState::DISCONNECTED);
    ((Call*) call)->RemoveListener(this);
    return NOERROR;
}

ECode CallsManager::OnRingbackRequested(
    /* [in] */ ICall* call,
    /* [in] */ Boolean ringback)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnRingbackRequested(call, ringback);
    }
    return NOERROR;
}

ECode CallsManager::OnPostDialWait(
    /* [in] */ ICall* call,
    /* [in] */ const String& remaining)
{
    mInCallController->OnPostDialWait(call, remaining);
    return NOERROR;
}

ECode CallsManager::OnParentChanged(
    /* [in] */ ICall* call)
{
    // parent-child relationship affects which call should be foreground, so do an update.
    UpdateCallsManagerState();
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnIsConferencedChanged(call);
    }
    return NOERROR;
}

ECode CallsManager::OnChildrenChanged(
    /* [in] */ ICall* call)
{
    // parent-child relationship affects which call should be foreground, so do an update.
    UpdateCallsManagerState();
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnIsConferencedChanged(call);
    }
    return NOERROR;
}

ECode CallsManager::OnIsVoipAudioModeChanged(
    /* [in] */ ICall* call)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnIsVoipAudioModeChanged(call);
    }
    return NOERROR;
}

ECode CallsManager::OnVideoStateChanged(
    /* [in] */ ICall* call)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnVideoStateChanged(call);
    }
    return NOERROR;
}

ECode CallsManager::OnCallSubstateChanged(
    /* [in] */ ICall* call)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnCallSubstateChanged(call);
    }
    return NOERROR;
}

ECode CallsManager::GetCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // return ImmutableList::CopyOf(mCalls);
    AutoPtr<IList> rev;
    CArrayList::New((IList**)&rev);
    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        rev->Add(obj);
    }
    *result = rev;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mForegroundCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetRinger(
    /* [out] */ Ringer** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRinger;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetInCallController(
    /* [out] */ InCallController** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInCallController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::HasEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        Boolean isEmergencyCall;
        ((Call*) call.Get())->IsEmergencyCall(&isEmergencyCall);
        if (isEmergencyCall) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CallsManager::GetAudioState(
    /* [out] */ IAudioState** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IAudioState> audioState;
    mCallAudioManager->GetAudioState((IAudioState**)&audioState);
    *result = audioState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::IsTtySupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isTtySupported;
    mTtyManager->IsTtySupported(&isTtySupported);
    *result = isTtySupported;
    return NOERROR;
}

ECode CallsManager::GetCurrentTtyMode(
    /* [out] */ Int32* result)
{
    return mTtyManager->GetCurrentTtyMode(result);
}

ECode CallsManager::AddListener(
    /* [in] */ ICallsManagerListener* listener)
{
    return mListeners->Add(listener);
}

ECode CallsManager::RemoveListener(
    /* [in] */ ICallsManagerListener* listener)
{
    return mListeners->Remove(listener);
}

ECode CallsManager::ProcessIncomingCallIntent(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [in] */ IBundle* extras)
{
    Log::D(TAG, "processIncomingCallIntent");
    AutoPtr<IParcelable> parcelable;
    extras->GetParcelable(ITelephonyManager::EXTRA_INCOMING_NUMBER, (IParcelable**)&parcelable);
    AutoPtr<IUri> handle = IUri::Probe(parcelable);
    AutoPtr<ICall> call = new Call();
    ((Call*) call.Get())->constructor(
            mContext,
            mConnectionServiceRepository,
            handle,
            NULL /* gatewayInfo */,
            NULL /* connectionManagerPhoneAccount */,
            phoneAccountHandle,
            TRUE /* isIncoming */,
            FALSE /* isConference */);
    ((Call*) call.Get())->SetExtras(extras);
    // TODO: Move this to be a part of addCall()
    ((Call*) call.Get())->AddListener(this);
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnProcessIncomingCall(call);
    }
    ((Call*) call.Get())->StartCreateConnection(mPhoneAccountRegistrar);
    return NOERROR;
}

ECode CallsManager::AddNewUnknownCall(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IParcelable> parcelable;
    extras->GetParcelable(ITelecomManager::EXTRA_UNKNOWN_CALL_HANDLE, (IParcelable**)&parcelable);
    AutoPtr<IUri> handle = IUri::Probe(parcelable);
    Log::I(TAG, "addNewUnknownCall with handle: %s", Log::Pii(handle).string());
    AutoPtr<ICall> call = new Call();
    ((Call*) call.Get())->constructor(
            mContext,
            mConnectionServiceRepository,
            handle,
            NULL /* gatewayInfo */,
            NULL /* connectionManagerPhoneAccount */,
            phoneAccountHandle,
            // Use onCreateIncomingConnection in TelephonyConnectionService, so that we attach
            // to the existing connection instead of trying to create a new one.
            TRUE /* isIncoming */,
            FALSE /* isConference */);
    AutoPtr<ISystem> systemHelper;
    CSystem::AcquireSingleton((ISystem**)&systemHelper);
    Int64 currentTimeMillis;
    systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
    ((Call*) call.Get())->SetConnectTimeMillis(currentTimeMillis);
    call->SetIsUnknown(TRUE);
    ((Call*) call.Get())->SetExtras(extras);
    ((Call*) call.Get())->AddListener(this);
    ((Call*) call.Get())->StartCreateConnection(mPhoneAccountRegistrar);
    return NOERROR;
}

ECode CallsManager::StartOutgoingCall(
    /* [in] */ IUri* handle,
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [in] */ IBundle* extras,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    // Create a call with original handle. The handle may be changed when the call is attached
    // to a connection service, but in most cases will remain the same.
    AutoPtr<ICall> call = new Call();
    ((Call*) call.Get())->constructor(
            mContext,
            mConnectionServiceRepository,
            handle,
            NULL /* gatewayInfo */,
            NULL /* connectionManagerPhoneAccount */,
            NULL /* phoneAccountHandle */,
            FALSE /* isIncoming */,
            FALSE /* isConference */);
    Boolean isExtraSkipSchemaParsing;
    extras->GetBoolean(ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING, FALSE, &isExtraSkipSchemaParsing);
    Boolean isExtraDialConferenceUri;
    extras->GetBoolean(ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI, FALSE, &isExtraDialConferenceUri);
    Boolean isSkipSchemaOrConfUri = ((extras != NULL) && (isExtraSkipSchemaParsing || isExtraDialConferenceUri));
    // Force tel scheme for ims conf uri/skip schema calls to avoid selection of sip accounts
    String handleScheme;
    handle->GetScheme(&handleScheme);
    String scheme = (isSkipSchemaOrConfUri? IPhoneAccount::SCHEME_TEL: handleScheme);
    AutoPtr<IList> accounts;
    mPhoneAccountRegistrar->GetCallCapablePhoneAccounts(scheme, (IList**)&accounts);
    // Only dial with the requested phoneAccount if it is still valid. Otherwise treat this call
    // as if a phoneAccount was not specified (does the default behavior instead).
    // Note: We will not attempt to dial with a requested phoneAccount if it is disabled.
    if (phoneAccountHandle != NULL) {
        Boolean isContains;
        accounts->Contains(phoneAccountHandle, &isContains);
        if (!isContains) {
            phoneAccountHandle = NULL;
        }
    }
    if (phoneAccountHandle == NULL) {
        // No preset account, check if default exists that supports the URI scheme for the
        // handle.
        AutoPtr<IPhoneAccountHandle> defaultAccountHandle;
        mPhoneAccountRegistrar->GetDefaultOutgoingPhoneAccount(scheme, (IPhoneAccountHandle**)&defaultAccountHandle);
        AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
        /* TelephonyManager */MultiSimVariants msimConfig;
        AutoPtr<ITelephonyManager> telephonyManager;
        telephonyManagerHelper->GetDefault((ITelephonyManager**)&telephonyManager);
        telephonyManager->GetMultiSimConfiguration(&msimConfig);
        Boolean isAlive;
        if (((msimConfig == Elastos::Droid::Telephony::MultiSimVariants_DSDS) ||
                (msimConfig == Elastos::Droid::Telephony::MultiSimVariants_TSTS)) &&
                (mForegroundCall != NULL) && (((Call*) mForegroundCall.Get())->IsAlive(&isAlive), isAlive)) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) mForegroundCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            defaultAccountHandle = phoneAccountHandle;
        }
        if (defaultAccountHandle != NULL) {
            phoneAccountHandle = defaultAccountHandle;
        }
    }
    ((Call*) call.Get())->SetTargetPhoneAccount(phoneAccountHandle);
    Boolean isEmergencyCall;
    AutoPtr<IUri> callHandle;
    ((Call*) call.Get())->GetHandle((IUri**)&callHandle);
    TelephonyUtil::ShouldProcessAsEmergency(mContext, callHandle, &isEmergencyCall);
    Boolean isPotentialInCallMMICode;
    IsPotentialInCallMMICode(handle, &isPotentialInCallMMICode);
    // Do not support any more live calls.  Our options are to move a call to hold, disconnect
    // a call, or cancel this call altogether.
    Boolean isMakeRoomForOutgoingCallOk;
    MakeRoomForOutgoingCall(call, isEmergencyCall, &isMakeRoomForOutgoingCallOk);
    if (!isPotentialInCallMMICode && !isMakeRoomForOutgoingCallOk) {
        // just cancel at this point.
        *result = NULL;
        return NOERROR;
    }
    Int32 accountsSize;
    accounts->GetSize(&accountsSize);
    if (phoneAccountHandle == NULL && accountsSize > 1 && !isEmergencyCall) {
        // This is the state where the user is expected to select an account
        ((Call*) call.Get())->SetState(ICallState::PRE_DIAL_WAIT);
        extras->PutParcelableList(Elastos::Droid::Telecomm::Telecom::ICall::AVAILABLE_PHONE_ACCOUNTS, accounts);
    } else {
        ((Call*) call.Get())->SetState(ICallState::CONNECTING);
    }
    ((Call*) call.Get())->SetExtras(extras);
    // Do not add the call if it is a potential MMI code.
    Boolean isPotentialMMICode;
    IsPotentialMMICode(handle, &isPotentialMMICode);
    if (phoneAccountHandle == NULL && isPotentialMMICode &&
            accountsSize > 1) {
        mCalls->Add(call);
        ((Call*) call.Get())->AddListener(this);
        extras->PutString(String("Handle"), TO_STR(handle));
        AutoPtr<IIntent> intent;
        CIntent::New(mContext, ECLSID_CAccountSelection, (IIntent**)&intent);
        intent->PutExtras(extras);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mContext->StartActivity(intent);
    } else if (isPotentialMMICode || isPotentialInCallMMICode) {
        ((Call*) call.Get())->AddListener(this);
    } else {
        AddCall(call);
    }
    *result = call;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::PlaceOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ IUri* handle,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ Boolean speakerphoneOn,
    /* [in] */ Int32 videoState)
{
    if (call == NULL) {
        // don't do anything if the call no longer exists
        Log::I(TAG, "Canceling unknown call.");
        return NOERROR;
    }
    AutoPtr<IUri> uriHandle;
    if (gatewayInfo == NULL) {
        uriHandle = handle;
    }
    else {
        gatewayInfo->GetGatewayAddress((IUri**)&uriHandle);
    }
    if (gatewayInfo == NULL) {
        Log::I(TAG, "Creating a new outgoing call with handle: %s", Log::PiiHandle(uriHandle).string());
    } else {
        Log::I(TAG, "Creating a new outgoing call with gateway handle: %s, original handle: %s",
                Log::Pii(uriHandle).string(), Log::Pii(handle).string());
    }
    ((Call*) call)->SetHandle(uriHandle);
    ((Call*) call)->SetGatewayInfo(gatewayInfo);
    call->SetStartWithSpeakerphoneOn(speakerphoneOn);
    call->SetVideoState(videoState);
    Boolean isEmergencyCall;
    AutoPtr<IUri> callHandle;
    ((Call*) call)->GetHandle((IUri**)&callHandle);
    TelephonyUtil::ShouldProcessAsEmergency(mContext, callHandle, &isEmergencyCall);
    if (isEmergencyCall) {
        // Emergency -- CreateConnectionProcessor will choose accounts automatically
        ((Call*) call)->SetTargetPhoneAccount(NULL);
    }
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    if (phoneAccountHandle != NULL || isEmergencyCall) {
        if (!isEmergencyCall) {
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            String id;
            phoneAccountHandle->GetId(&id);
            UpdateLchStatus(id);
        }
        // If the account has been set, proceed to place the outgoing call.
        // Otherwise the connection will be initiated when the account is set by the user.
        ((Call*) call)->StartCreateConnection(mPhoneAccountRegistrar);
    }
    return NOERROR;
}

ECode CallsManager::AddParticipant(
    /* [in] */ ICall* call,
    /* [in] */ IUri* handle,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ Boolean speakerphoneOn,
    /* [in] */ Int32 videoState)
{
    if (call == NULL) {
        // don't do anything if the call no longer exists
        Log::I(TAG, "Canceling unknown call.");
        return NOERROR;
    }
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    if (phoneAccountHandle != NULL) {
        // If the account has been set, proceed to place the add participant.
        // Otherwise the connection will be initiated when the account is set by the user.
        ((Call*) call)->StartCreateConnection(mPhoneAccountRegistrar);
    }
    return NOERROR;
}

ECode CallsManager::Conference(
    /* [in] */ ICall* call,
    /* [in] */ ICall* otherCall)
{
    return ((Call*) call)->ConferenceWith(otherCall);
}

ECode CallsManager::AnswerCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 videoState)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to answer a non-existent call %s", TO_CSTR(call));
    } else {
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        String id;
        phoneAccountHandle->GetId(&id);
        AutoPtr<ICall> activeCall;
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        (*array)[0] = ICallState::ACTIVE;
        (*array)[1] = ICallState::DIALING;
        GetFirstCallWithStateUsingSubId(id, array, (ICall**)&activeCall);
        //Check for existence of active call & held
        AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(1);
        (*array2)[0] = ICallState::ON_HOLD;
        AutoPtr<ICall> firstCallWithStateUsingSubId;
        GetFirstCallWithStateUsingSubId(id, array2, (ICall**)&firstCallWithStateUsingSubId);
        if ((activeCall != NULL) && (firstCallWithStateUsingSubId != NULL)) {
            Log::I(TAG, "Disconnect active call");
            // Both active call & Held call are present, hence disconnect active call
            // before sending answer request on waiting call.
            ((Call*) activeCall.Get())->Disconnect();
            activeCall = NULL;
        }
        // If the foreground call is not the ringing call and it is currently isActive() or
        // STATE_DIALING, put it on hold before answering the call.
        if (activeCall != NULL) {
            Boolean isActive;
            ((Call*) activeCall.Get())->IsActive(&isActive);
            Int32 activeCallState;
            ((Call*) activeCall.Get())->GetState(&activeCallState);
            if (activeCall.Get() != call &&
                        (isActive || activeCallState == ICallState::DIALING)) {
                Int32 callCapabilities;
                ((Call*) activeCall.Get())->GetCallCapabilities(&callCapabilities);
                if (0 == (callCapabilities & IPhoneCapabilities::HOLD)) {
                    // This call does not support hold.  If it is from a different connection
                    // service, then disconnect it, otherwise allow the connection service to
                    // figure out the right states.
                    AutoPtr<IConnectionServiceWrapper> activeConnectionService;
                    ((Call*) activeCall.Get())->GetConnectionService((IConnectionServiceWrapper**)&activeConnectionService);
                    AutoPtr<IConnectionServiceWrapper> connectionService;
                    ((Call*) call)->GetConnectionService((IConnectionServiceWrapper**)&connectionService);
                    if (activeConnectionService != connectionService) {
                        ((Call*) activeCall.Get())->Disconnect();
                    }
                } else {
                    Log::I(TAG, "Holding active/dialing call %s before answering incoming call %s.",
                            TO_CSTR(mForegroundCall), TO_CSTR(call));
                    ((Call*) activeCall.Get())->Hold();
                }
                // TODO: Wait until we get confirmation of the active call being
                // on-hold before answering the new call.
                // TODO: Import logic from CallManager.acceptCall()
            }
        }
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
            listener->OnIncomingCallAnswered(call);
        }
        UpdateLchStatus(id);
        // We do not update the UI until we get confirmation of the answer() through
        // {@link #markCallAsActive}.
        ((Call*) call)->Answer(videoState);
        Boolean isPluggedIn;
        mWiredHeadsetManager->IsPluggedIn(&isPluggedIn);
        Boolean isBluetoothDeviceAvailable;
        mCallAudioManager->IsBluetoothDeviceAvailable(&isBluetoothDeviceAvailable);
        assert(0 && "IVideoProfileVideoStateHelper");
        // if (VideoProfile::VideoState::IsVideo(videoState) &&
        //     !isPluggedIn &&
        //     !isBluetoothDeviceAvailable) {
        //     call->SetStartWithSpeakerphoneOn(TRUE);
        // }
    }
    return NOERROR;
}

ECode CallsManager::DeflectCall(
    /* [in] */ ICall* call,
    /* [in] */ const String& number)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to deflect a non-existent call %s", TO_CSTR(call));
    } else {
        ((Call*) call)->Deflect(number);
    }
    return NOERROR;
}

ECode CallsManager::RejectCall(
    /* [in] */ ICall* call,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to reject a non-existent call %s", TO_CSTR(call));
    } else {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
            listener->OnIncomingCallRejected(call, rejectWithMessage, textMessage);
        }
        String conversationSub;
        GetConversationSub(&conversationSub);
        SetActiveSubscription(conversationSub);
        ((Call*) call)->Reject(rejectWithMessage, textMessage);
    }
    return NOERROR;
}

ECode CallsManager::PlayDtmfTone(
    /* [in] */ ICall* call,
    /* [in] */ Char32 digit)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to play DTMF in a non-existent call %s", TO_CSTR(call));
    } else {
        ((Call*) call)->PlayDtmfTone(digit);
        mDtmfLocalTonePlayer->PlayTone(call, digit);
    }
    return NOERROR;
}

ECode CallsManager::StopDtmfTone(
    /* [in] */ ICall* call)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to stop DTMF in a non-existent call %s", TO_CSTR(call));
    } else {
        ((Call*) call)->StopDtmfTone();
        mDtmfLocalTonePlayer->StopTone(call);
    }
    return NOERROR;
}

ECode CallsManager::PostDialContinue(
    /* [in] */ ICall* call,
    /* [in] */ Boolean proceed)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Request to continue post-dial string in a non-existent call %s", TO_CSTR(call));
    } else {
        ((Call*) call)->PostDialContinue(proceed);
    }
    return NOERROR;
}

ECode CallsManager::DisconnectCall(
    /* [in] */ ICall* call)
{
    Log::V(TAG, "disconnectCall %s", call);
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::W(TAG, "Unknown call (%s) asked to disconnect", TO_CSTR(call));
    } else {
        mLocallyDisconnectingCalls->Add(call);
        ((Call*) call)->Disconnect();
    }
    return NOERROR;
}

ECode CallsManager::DisconnectAllCalls()
{
    Log::V(TAG, "disconnectAllCalls");
    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        DisconnectCall(call);
    }
    return NOERROR;
}

ECode CallsManager::HoldCall(
    /* [in] */ ICall* call)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::W(TAG, "Unknown call (%s) asked to be put on hold", TO_CSTR(call));
    } else {
        Log::D(TAG, "Putting call on hold: (%s)", TO_CSTR(call));
        ((Call*) call)->Hold();
    }
    return NOERROR;
}

ECode CallsManager::UnholdCall(
    /* [in] */ ICall* call)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::W(TAG, "Unknown call (%s) asked to be removed from hold", TO_CSTR(call));
    } else {
        Log::D(TAG, "unholding call: (%s)", TO_CSTR(call));
        AutoPtr<IPhoneAccountHandle> ph;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph);
        AutoPtr<IList> childCalls;
        ((Call*) call)->GetChildCalls((IList**)&childCalls);
        Int32 size;
        childCalls->GetSize(&size);
        if ((ph == NULL) && (size > 1)) {
            AutoPtr<IInterface> obj;
            childCalls->Get(0, (IInterface**)&obj);
            AutoPtr<ICall> child = ICall::Probe(obj);
            ((Call*) child.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph);
        }
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> c = ICall::Probe(obj);
            // Only operate on top-level calls
            AutoPtr<ICall> parentCall;
            ((Call*) c.Get())->GetParentCall((ICall**)&parentCall);
            if (parentCall != NULL) {
                continue;
            }
            AutoPtr<IPhoneAccountHandle> ph1;
            ((Call*) c.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph1);
            Int32 size;
            childCalls->GetSize(&size);
            if ((ph1 == NULL) && (size > 1)) {
                AutoPtr<IInterface> obj;
                childCalls->Get(0, (IInterface**)&obj);
                AutoPtr<ICall> child = ICall::Probe(obj);
                ((Call*) child.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph1);
            }
            // if 'c' is not for same subscription as call, then don't disturb 'c'
            Boolean isAlive;
            ((Call*) c.Get())->IsAlive(&isAlive);
            String id;
            ph1->GetId(&id);
            String phId;
            ph->GetId(&phId);
            Boolean isSameIdOrSipId;
            IsSameIdOrSipId(phId, id, &isSameIdOrSipId);
            if (c != NULL && isAlive && c.Get() != call && (ph != NULL
                    && ph1 != NULL && isSameIdOrSipId)) {
                ((Call*) c.Get())->Hold();
            }
        }
        ((Call*) call)->Unhold();
    }
    return NOERROR;
}

ECode CallsManager::IsSameIdOrSipId(
    /* [in] */ const String& id1,
    /* [in] */ const String& id2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = ((id1 != NULL && id2 != NULL) &&
            (id1.Equals(id2) || id1.Contains("sip") || id2.Contains("sip")));
    Log::D(TAG, "isSameIdOrSipId: id1 = %s id2 = %s ret = %d", id1.string(), id2.string(), ret);
    *result = ret;
    return NOERROR;
}

ECode CallsManager::Mute(
    /* [in] */ Boolean shouldMute)
{
    return mCallAudioManager->Mute(shouldMute);
}

ECode CallsManager::SetAudioRoute(
    /* [in] */ Int32 route)
{
    return mCallAudioManager->SetAudioRoute(route);
}

ECode CallsManager::TurnOnProximitySensor()
{
    return mProximitySensorManager->TurnOn();
}

ECode CallsManager::TurnOffProximitySensor(
    /* [in] */ Boolean screenOnImmediately)
{
    return mProximitySensorManager->TurnOff(screenOnImmediately);
}

ECode CallsManager::PhoneAccountSelected(
    /* [in] */ ICall* call,
    /* [in] */ IPhoneAccountHandle* account)
{
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (!isContains) {
        Log::I(TAG, "Attempted to add account to unknown call %s", TO_CSTR(call));
    } else {
        // TODO: There is an odd race condition here. Since NewOutgoingCallIntentBroadcaster and
        // the PRE_DIAL_WAIT sequence run in parallel, if the user selects an account before the
        // NEW_OUTGOING_CALL sequence finishes, we'll start the call immediately without
        // respecting a rewritten number or a canceled number. This is unlikely since
        // NEW_OUTGOING_CALL sequence, in practice, runs a lot faster than the user selecting
        // a phone account from the in-call UI.
        String id;
        account->GetId(&id);
        Log::I(TAG, "phoneAccountSelected , id = %s", id.string());
        UpdateLchStatus(id);
        ((Call*) call)->SetTargetPhoneAccount(account);
        // Note: emergency calls never go through account selection dialog so they never
        // arrive here.
        Boolean isMakeRoomForOutgoingCallOk;
        MakeRoomForOutgoingCall(call, FALSE /* isEmergencyCall */, &isMakeRoomForOutgoingCallOk);
        if (isMakeRoomForOutgoingCallOk) {
            ((Call*) call)->StartCreateConnection(mPhoneAccountRegistrar);
        } else {
            ((Call*) call)->Disconnect();
        }
    }
    return NOERROR;
}

ECode CallsManager::PhoneAccountSelectedForMMI(
    /* [in] */ IUri* handle,
    /* [in] */ IPhoneAccountHandle* account)
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = ICallState::PRE_DIAL_WAIT;
    AutoPtr<ICall> call;
    GetFirstCallWithHandle(handle, array, (ICall**)&call);
    Log::D(TAG,"call: %s", TO_CSTR(call));
    if (account != NULL) {
        PhoneAccountSelected(call, account);
    } else {
        ((Call*) call.Get())->Disconnect();
    }
    return NOERROR;
}

ECode CallsManager::OnAudioStateChanged(
    /* [in] */ IAudioState* oldAudioState,
    /* [in] */ IAudioState* newAudioState)
{
    Log::V(TAG, "onAudioStateChanged, audioState: %s -> %s", TO_CSTR(oldAudioState), TO_CSTR(newAudioState));
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnAudioStateChanged(oldAudioState, newAudioState);
    }
    return NOERROR;
}

ECode CallsManager::MarkCallAsRinging(
    /* [in] */ ICall* call)
{
    return SetCallState(call, ICallState::RINGING);
}

ECode CallsManager::MarkCallAsDialing(
    /* [in] */ ICall* call)
{
    SetCallState(call, ICallState::DIALING);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    String id;
    phoneAccountHandle->GetId(&id);
    SetActiveSubscription(id);
    return NOERROR;
}

ECode CallsManager::MarkCallAsActive(
    /* [in] */ ICall* call)
{
    Int64 connectTimeMillis;
    ((Call*) call)->GetConnectTimeMillis(&connectTimeMillis);
    if (connectTimeMillis == 0) {
        AutoPtr<ISystem> systemHelper;
        CSystem::AcquireSingleton((ISystem**)&systemHelper);
        Int64 currentTimeMillis;
        systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
        ((Call*) call)->SetConnectTimeMillis(currentTimeMillis);
    }
    SetCallState(call, ICallState::ACTIVE);
    Boolean startWithSpeakerphone;
    call->GetStartWithSpeakerphoneOn(&startWithSpeakerphone);
    if (startWithSpeakerphone) {
        SetAudioRoute(IAudioState::ROUTE_SPEAKER);
    }
    return NOERROR;
}

ECode CallsManager::MarkCallAsOnHold(
    /* [in] */ ICall* call)
{
    return SetCallState(call, ICallState::ON_HOLD);
}

ECode CallsManager::MarkCallAsDisconnected(
    /* [in] */ ICall* call,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    ((Call*) call)->SetDisconnectCause(disconnectCause);
    SetCallState(call, ICallState::DISCONNECTED);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IPhoneAccount> phAcc;
    phoneAccountRegistrar->GetPhoneAccount(phoneAccountHandle, (IPhoneAccount**)&phAcc);
    String id;
    phoneAccountHandle->GetId(&id);
    String conversationSub;
    GetConversationSub(&conversationSub);
    String activeSubscription;
    GetActiveSubscription(&activeSubscription);
    Boolean isSetLCH;
    phAcc->IsSet(IPhoneAccount::LCH, &isSetLCH);
    if ((phoneAccountHandle != NULL &&
                id.Equals(activeSubscription)) &&
                (phAcc != NULL) && isSetLCH &&
                (!conversationSub.IsNull()) &&
                (!conversationSub.Equals(activeSubscription))) {
        Log::D(TAG,"Set active sub to conversation sub");
        SetActiveSubscription(conversationSub);
    }
    if ((phoneAccountHandle != NULL) && (phAcc != NULL) && isSetLCH) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
        (*array)[0] = ICallState::RINGING;
        (*array)[1] = ICallState::DIALING;
        (*array)[2] = ICallState::ACTIVE;
        (*array)[3] = ICallState::ON_HOLD;
        AutoPtr<ICall> activecall;
        GetFirstCallWithStateUsingSubId(id, array, (ICall**)&activecall);
        Log::D(TAG,"activecall: %s", TO_CSTR(activecall));
        if (activecall == NULL) {
            phAcc->UnSetBit(IPhoneAccount::LCH);
            ManageMSimInCallTones(FALSE);
        }
    }
    RemoveCall(call);
    Boolean hasAnyCalls;
    HasAnyCalls(&hasAnyCalls);
    if (!hasAnyCalls) {
        UpdateLchStatus(String(NULL));
        SetActiveSubscription(String(NULL));
        ManageMSimInCallTones(FALSE);
    }
    return NOERROR;
}

ECode CallsManager::MarkCallAsRemoved(
    /* [in] */ ICall* call)
{
    RemoveCall(call);
    Boolean isContains;
    mLocallyDisconnectingCalls->Contains(call, &isContains);
    if (isContains) {
        mLocallyDisconnectingCalls->Remove(call);
        Int32 state;
        ((Call*) mForegroundCall.Get())->GetState(&state);
        if (mForegroundCall != NULL && state == ICallState::ON_HOLD) {
            ((Call*) mForegroundCall.Get())->Unhold();
        }
    }
    return NOERROR;
}

ECode CallsManager::HandleConnectionServiceDeath(
    /* [in] */ IConnectionServiceWrapper* service)
{
    if (service != NULL) {
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<IConnectionServiceWrapper> connectionServiceWrapper;
            ((Call*) call.Get())->GetConnectionService((IConnectionServiceWrapper**)&connectionServiceWrapper);
            if (connectionServiceWrapper.Get() == service) {
                AutoPtr<IDisconnectCause> disconnectCause;
                CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&disconnectCause);
                MarkCallAsDisconnected(call, disconnectCause);
            }
        }
    }
    return NOERROR;
}

ECode CallsManager::HasAnyCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isEmpty;
    mCalls->IsEmpty(&isEmpty);
    *result = !isEmpty;
    return NOERROR;
}

ECode CallsManager::HasActiveOrHoldingCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(2);
    (*states)[0] = ICallState::ACTIVE;
    (*states)[1] = ICallState::ON_HOLD;
    AutoPtr<ICall> call;
    GetFirstCallWithState(states, (ICall**)&call);
    return call != NULL;
}

ECode CallsManager::HasActiveOrHoldingCall(
    /* [in] */ const String& sub,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = ICallState::ACTIVE;
    (*array)[1] = ICallState::ON_HOLD;
    AutoPtr<ICall> firstCallWithStateUsingSubId;
    GetFirstCallWithStateUsingSubId(sub, array, (ICall**)&firstCallWithStateUsingSubId);
    *result = (firstCallWithStateUsingSubId != NULL);
    return NOERROR;
}

ECode CallsManager::HasRingingCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::RINGING;
    AutoPtr<ICall> call;
    GetFirstCallWithState(states, (ICall**)&call);
    *result = call != NULL;
    return NOERROR;
}

ECode CallsManager::OnMediaButton(
    /* [in] */ Int32 type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean hasAnyCalls;
    HasAnyCalls(&hasAnyCalls);
    if (hasAnyCalls) {
        if (HeadsetMediaButton::SHORT_PRESS == type) {
            AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
            (*states)[0] = ICallState::RINGING;
            AutoPtr<ICall> ringingCall;
            GetFirstCallWithState(states, (ICall**)&ringingCall);
            if (ringingCall == NULL) {
                mCallAudioManager->ToggleMute();
                *result = TRUE;
                return NOERROR;
            } else {
                Int32 videoState;
                ringingCall->GetVideoState(&videoState);
                ((Call*) ringingCall.Get())->Answer(videoState);
                *result = TRUE;
                return NOERROR;
            }
        } else if (HeadsetMediaButton::LONG_PRESS == type) {
            Log::D(TAG, "handleHeadsetHook: longpress -> hangup");
            AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(4);
            (*states)[0] = ICallState::RINGING;
            (*states)[1] = ICallState::DIALING;
            (*states)[2] = ICallState::ACTIVE;
            (*states)[3] = ICallState::ON_HOLD;
            AutoPtr<ICall> callToHangup;
            GetFirstCallWithState(states, (ICall**)&callToHangup);
            if (callToHangup != NULL) {
                ((Call*) callToHangup.Get())->Disconnect();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CallsManager::CanAddCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        Boolean isEmergencyCall;
        ((Call*) call.Get())->IsEmergencyCall(&isEmergencyCall);
        AutoPtr<ICall> parentCall;
        ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
        if (isEmergencyCall) {
            // We never support add call if one of the calls is an emergency call.
            *result = FALSE;
            return NOERROR;
        } else if (parentCall == NULL) {
            count++;
        }
        // We do not check states for canAddCall. We treat disconnected calls the same
        // and wait until they are removed instead. If we didn't count disconnected calls,
        // we could put InCallServices into a state where they are showing two calls but
        // also support add-call. Technically it's right, but overall looks better (UI-wise)
        // and acts better if we wait until the call is removed.
        if (count >= MAXIMUM_TOP_LEVEL_CALLS) {
            *result = FALSE;
            return NOERROR;
        }
        // Commented below block as 'Add Call' button was still missing in single
        // sim VoLTE conference cases even after increasing MAXIMUM_TOP_LEVEL_CALLS
        // TODO: Check if the below block needs changes to cover MSIM cases
        /*
        PhoneAccountHandle ph = call.getTargetPhoneAccount();
        // Loop through all the other calls and there exists a top level (has no parent) call
        // that is not the specified call, return false.
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> otherCall = ICall::Probe(obj);
            PhoneAccountHandle otherCallPh = otherCall.getTargetPhoneAccount();
            // if 'otherCall' is not for same subscription as 'call', then don't consider it
            if (call != otherCall && otherCall.getParentCall() == null && ph != null
                    && otherCallPh != null && isSameIdOrSipId(ph.getId(), otherCallPh.getId())) {
                return FALSE;
            }
        }
        if ((call.getState() != CallState.ACTIVE) && (call.getState() != CallState.ON_HOLD)) {
            return FALSE;
        } */
    }
    *result = TRUE;
    return NOERROR;
}

ECode CallsManager::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::RINGING;
    return GetFirstCallWithState(states, result);
}

ECode CallsManager::GetActiveCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::ACTIVE;
    return GetFirstCallWithState(states, result);
}

ECode CallsManager::GetDialingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::DIALING;
    return GetFirstCallWithState(states, result);;
}

ECode CallsManager::GetHeldCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::ON_HOLD;
    return GetFirstCallWithState(states, result);;
}

ECode CallsManager::GetNumHeldCalls(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        AutoPtr<ICall> parentCall;
        ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
        Int32 callState;
        ((Call*) call.Get())->GetState(&callState);
        if (parentCall == NULL && callState == ICallState::ON_HOLD) {
            count++;
        }
    }
    *result = count;
    return NOERROR;
}

ECode CallsManager::GetFirstCallWithState(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ ICall** result)
{
    return GetFirstCallWithState(NULL, states, result);
}

ECode CallsManager::GetFirstCallWithHandle(
    /* [in] */ IUri* handle,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    for (int i = 0; i < states->GetLength(); ++i) {
        Int32 currentState = (*states)[i];
        AutoPtr<IIterator> iter;
        mCalls->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<IUri> callHandle;
            ((Call*) call.Get())->GetHandle((IUri**)&callHandle);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if ((currentState == callState) && TO_STR(callHandle).Equals(TO_STR(handle))) {
                *result = call;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CallsManager::GetFirstCallWithState(
    /* [in] */ ICall* callToSkip,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < states->GetLength(); ++i) {
        Int32 currentState = (*states)[i];
        // check the foreground first
        Int32 foregroundCallState;
        ((Call*) mForegroundCall.Get())->GetState(&foregroundCallState);
        if (mForegroundCall != NULL && foregroundCallState == currentState) {
            *result = mForegroundCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            if (Objects::Equals(callToSkip, call.Get())) {
                continue;
            }
            // Only operate on top-level calls
            AutoPtr<ICall> parentCall;
            ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
            if (parentCall != NULL) {
                continue;
            }
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (currentState == callState) {
                *result = call;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CallsManager::GetFirstCallWithStateUsingSubId(
    /* [in] */ const String& subId,
    /* [in] */ ICall* callToSkip,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < states->GetLength(); ++i) {
        Int32 currentState = (*states)[i];
        // check the foreground first
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) mForegroundCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        Boolean isSameIdOrSipId;
        String id;
        phoneAccountHandle->GetId(&id);
        IsSameIdOrSipId(id, subId, &isSameIdOrSipId);
        Int32 foregroundCallState;
        ((Call*) mForegroundCall.Get())->GetState(&foregroundCallState);
        if (mForegroundCall != NULL && foregroundCallState == currentState
                && phoneAccountHandle != NULL
                && isSameIdOrSipId) {
            *result = mForegroundCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            if (Objects::Equals(callToSkip, call.Get())) {
                continue;
            }
            // Only operate on top-level calls
            AutoPtr<ICall> parentCall;
            ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
            if (parentCall != NULL) {
                continue;
            }
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            AutoPtr<IList> childCalls;
            ((Call*) call.Get())->GetChildCalls((IList**)&childCalls);
            Int32 size;
            childCalls->GetSize(&size);
            if ((phoneAccountHandle == NULL) && (size > 1)) {
                AutoPtr<IInterface> obj;
                childCalls->Get(0, (IInterface**)&obj);
                AutoPtr<ICall> child = ICall::Probe(obj);
                AutoPtr<IPhoneAccountHandle> childph;
                ((Call*) child.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&childph);
                String id;
                childph->GetId(&id);
                Boolean isSameIdOrSipId;
                IsSameIdOrSipId(id, subId, &isSameIdOrSipId);
                if (childph != NULL && isSameIdOrSipId) {
                    *result = call;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
            Boolean isSameIdOrSipId;
            String id;
            phoneAccountHandle->GetId(&id);
            IsSameIdOrSipId(id, subId, &isSameIdOrSipId);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (currentState == callState && phoneAccountHandle != NULL
                    && isSameIdOrSipId) {
                *result = call;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CallsManager::CreateConferenceCall(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ IParcelableConference* parcelableConference,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICall> call = new Call();
    ((Call*) call.Get())->constructor(
            mContext,
            mConnectionServiceRepository,
            NULL /* handle */,
            NULL /* gatewayInfo */,
            NULL /* connectionManagerPhoneAccount */,
            phoneAccount,
            FALSE /* isIncoming */,
            TRUE /* isConference */);
    Int32 parcelableConferenceState;
    parcelableConference->GetState(&parcelableConferenceState);
    Int32 stateFromConnectionState;
    Call::GetStateFromConnectionState(parcelableConferenceState, &stateFromConnectionState);
    SetCallState(call, stateFromConnectionState);
    Int32 callState;
    ((Call*) call.Get())->GetState(&callState);
    if (callState == ICallState::ACTIVE) {
        AutoPtr<ISystem> systemHelper;
        CSystem::AcquireSingleton((ISystem**)&systemHelper);
        Int64 currentTimeMillis;
        systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
        ((Call*) call.Get())->SetConnectTimeMillis(currentTimeMillis);
    }
    Int32 capabilities;
    parcelableConference->GetCapabilities(&capabilities);
    ((Call*) call.Get())->SetCallCapabilities(capabilities);
    // TODO: Move this to be a part of addCall()
    ((Call*) call.Get())->AddListener(this);
    AddCall(call);
    *result = call;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetCallState(
    /* [out] */ Int32* result)
{
    return mPhoneStateBroadcaster->GetCallState(result);
}

ECode CallsManager::GetPhoneAccountRegistrar(
    /* [out] */ PhoneAccountRegistrar** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneAccountRegistrar;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetMissedCallNotifier(
    /* [out] */ MissedCallNotifier** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMissedCallNotifier;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::GetBlacklistCallNotifier(
    /* [out] */ BlacklistCallNotifier** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlacklistCallNotifier;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::AddCall(
    /* [in] */ ICall* call)
{
    Log::V(TAG, "addCall(%s)", TO_CSTR(call));
    ((Call*) call)->AddListener(this);
    mCalls->Add(call);
    // TODO: Update mForegroundCall prior to invoking
    // onCallAdded for calls which immediately take the foreground (like the first call).
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnCallAdded(call);
    }
    UpdateCallsManagerState();
    return NOERROR;
}

ECode CallsManager::RemoveCall(
    /* [in] */ ICall* call)
{
    Log::V(TAG, "removeCall(%s)", TO_CSTR(call));
    ((Call*) call)->SetParentCall(NULL);  // need to clean up parent relationship before destroying.
    ((Call*) call)->RemoveListener(this);
    ((Call*) call)->ClearConnectionService();
    Boolean shouldNotify = FALSE;
    Boolean isContains;
    mCalls->Contains(call, &isContains);
    if (isContains) {
        mCalls->Remove(call);
        shouldNotify = TRUE;
    }
    // Only broadcast changes for calls that are being tracked.
    if (shouldNotify) {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
            listener->OnCallRemoved(call);
        }
        UpdateCallsManagerState();
    }
    return NOERROR;
}

ECode CallsManager::SetCallState(
    /* [in] */ ICall* call,
    /* [in] */ Int32 newState)
{
    if (call == NULL) {
        return NOERROR;
    }
    Int32 oldState;
    ((Call*) call)->GetState(&oldState);
    AutoPtr<ICallState> helper;
    CCallState::AcquireSingleton((ICallState**)&helper);
    String oldStateStr;
    helper->ToString(oldState, &oldStateStr);
    String newStateStr;
    helper->ToString(newState, &newStateStr);
    Log::I(TAG, "setCallState %s -> %s, call: %s", oldStateStr.string(), newStateStr.string(), TO_CSTR(call));
    if (newState != oldState) {
        // Unfortunately, in the telephony world the radio is king. So if the call notifies
        // us that the call is in a particular state, we allow it even if it doesn't make
        // sense (e.g., STATE_ACTIVE -> STATE_RINGING).
        // TODO: Consider putting a stop to the above and turning CallState
        // into a well-defined state machine.
        // TODO: Define expected state transitions here, and log when an
        // unexpected transition occurs.
        ((Call*) call)->SetState(newState);
        // Only broadcast state change for calls that are being tracked.
        Boolean isContains;
        mCalls->Contains(call, &isContains);
        if (isContains) {
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
                listener->OnCallStateChanged(call, oldState, newState);
            }
            UpdateCallsManagerState();
        }
    }
    ManageMSimInCallTones(FALSE);
    return NOERROR;
}

ECode CallsManager::SetCallExtras(
    /* [in] */ ICall* call,
    /* [in] */ IBundle* extras)
{
    if (extras == NULL) {
        Log::D(TAG, "setCallExtras Null extras Bundle");
        return NOERROR;
    }
    AutoPtr<IBundle> callExtras;
    ((Call*) call)->GetExtras((IBundle**)&callExtras);
    // NOTE: OEM extras are packed "as is" within the Call
    // object's mExtras Bundle so as to preserve the
    // original contents of the mExtras Bundle. We don't
    // want to overwrite mExtras with the OEM extras.
    callExtras->PutBundle(ICall::KEY_OEM_EXTRAS, extras);
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
        listener->OnCallExtrasUpdated(call);
    }
    return NOERROR;
}

ECode CallsManager::UpdateForegroundCall()
{
    AutoPtr<ICall> newForegroundCall;
        // TODO: Foreground-ness needs to be explicitly set. No call, regardless
        // of its state will be foreground by default and instead the connection service should
        // be notified when its calls enter and exit foreground state. Foreground will mean that
        // the call should play audio and listen to microphone if it wants.
    AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
    AutoPtr<ITelephonyManager> tm;
    telephonyManagerHelper->GetDefault((ITelephonyManager**)&tm);
    Elastos::Droid::Telephony::MultiSimVariants conf;
    tm->GetMultiSimConfiguration(&conf);
    if (conf == Elastos::Droid::Telephony::MultiSimVariants_DSDA) {
        String lchSub;
        GetLchSub(&lchSub);
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            // Only top-level calls can be in foreground
            AutoPtr<ICall> parentCall;
            ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
            if (parentCall != NULL) {
                continue;
            }
            AutoPtr<IPhoneAccountHandle> ph;
            ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph);
            String id;
            ph->GetId(&id);
            if (ph != NULL && id.Equals(lchSub)) continue;
            // Active calls have priority.
            Boolean isActive;
            ((Call*) call.Get())->IsActive(&isActive);
            if (isActive) {
                newForegroundCall = call;
                break;
            }
            Boolean isAlive;
            ((Call*) call.Get())->IsAlive(&isAlive);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (isAlive || callState == ICallState::RINGING) {
                newForegroundCall = call;
                // Don't break in case there's an active call that has priority.
            }
        }
        // if active sub doesn't have any calls, then consider calls on all subs,
        // which ever call is active set that as foreground call. give more priority
        // to ringing call on LCH sub over active call.
        if (newForegroundCall == NULL) {
            AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
            (*states)[0] = ICallState::RINGING;
            GetFirstCallWithState(states, (ICall**)&newForegroundCall);
            if (newForegroundCall == NULL) {
                AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
                (*states)[0] = ICallState::ACTIVE;
                GetFirstCallWithState(states, (ICall**)&newForegroundCall);
            }
        }
    } else {
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            // Only top-level calls can be in foreground
            AutoPtr<ICall> parentCall;
            ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
            if (parentCall != NULL) {
                continue;
            }
            // Active calls have priority.
            Boolean isActive;
            ((Call*) call.Get())->IsActive(&isActive);
            if (isActive) {
                newForegroundCall = call;
                break;
            }
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (isActive || callState == ICallState::RINGING) {
                newForegroundCall = call;
                // Don't break in case there's an active call that has priority.
            }
        }
    }
    if (newForegroundCall != mForegroundCall) {
        Log::V(TAG, "Updating foreground call, %s -> %s.", TO_CSTR(mForegroundCall), TO_CSTR(newForegroundCall));
        AutoPtr<ICall> oldForegroundCall = mForegroundCall;
        mForegroundCall = newForegroundCall;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
            listener->OnForegroundCallChanged(oldForegroundCall, mForegroundCall);
        }
    }
    return NOERROR;
}

ECode CallsManager::UpdateCanAddCall()
{
    Boolean newCanAddCall;
    CanAddCall(&newCanAddCall);
    if (newCanAddCall != mCanAddCall) {
        mCanAddCall = newCanAddCall;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
            listener->OnCanAddCallChanged(mCanAddCall);
        }
    }
    return NOERROR;
}

ECode CallsManager::UpdateCallsManagerState()
{
    UpdateForegroundCall();
    UpdateCanAddCall();
    return NOERROR;
}

ECode CallsManager::IsPotentialMMICode(
    /* [in] */ IUri* handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String schemeSpecificPart;
    handle->GetSchemeSpecificPart(&schemeSpecificPart);
    *result = (handle != NULL && !schemeSpecificPart.IsNull()
            && schemeSpecificPart.Contains("#"));
    return NOERROR;
}

ECode CallsManager::IsPotentialInCallMMICode(
    /* [in] */ IUri* handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String schemeSpecificPart;
    handle->GetSchemeSpecificPart(&schemeSpecificPart);
    String scheme;
    handle->GetScheme(&scheme);
    if (handle != NULL && !schemeSpecificPart.IsNull() &&
            scheme.Equals(IPhoneAccount::SCHEME_TEL)) {
        String dialedNumber;
        handle->GetSchemeSpecificPart(&dialedNumber);
        *result = (dialedNumber.Equals("0") ||
                (dialedNumber.StartWith("1") && dialedNumber.GetLength() <= 2) ||
                (dialedNumber.StartWith("2") && dialedNumber.GetLength() <= 2) ||
                dialedNumber.Equals("3") ||
                dialedNumber.Equals("4") ||
                dialedNumber.Equals("5"));
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CallsManager::GetNumCallsWithState(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    for (Int32 i = 0; i < states->GetLength(); ++i) {
        Int32 state = (*states)[i];
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<ICall> parentCall;
            ((Call*) call.Get())->GetParentCall((ICall**)&parentCall);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (parentCall == NULL && callState == state) {
                count++;
            }
        }
    }
    *result = count;
    return NOERROR;
}

ECode CallsManager::GetNumCallsWithState(
    /* [in] */ const String& subId,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    for (Int32 i = 0; i < states->GetLength(); ++i) {
        Int32 state = (*states)[i];
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            Boolean isSameIdOrSipId;
            String id;
            phoneAccountHandle->GetId(&id);
            IsSameIdOrSipId(id, subId, &isSameIdOrSipId);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if (callState == state && phoneAccountHandle != NULL
                    && isSameIdOrSipId) {
                count++;
            }
        }
    }
    *result = count;
    return NOERROR;
}

ECode CallsManager::HasMaximumLiveCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 numCallsWithState;
    GetNumCallsWithState(LIVE_CALL_STATES, &numCallsWithState);
    *result = MAXIMUM_LIVE_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumLiveCalls(
    /* [in] */ const String& subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 numCallsWithState;
    GetNumCallsWithState(subId, LIVE_CALL_STATES, &numCallsWithState);
    *result = MAXIMUM_LIVE_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumHoldingCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::ON_HOLD;
    Int32 numCallsWithState;
    GetNumCallsWithState(states, &numCallsWithState);
    *result = MAXIMUM_HOLD_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumHoldingCalls(
    /* [in] */ const String& subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::ON_HOLD;
    Int32 numCallsWithState;
    GetNumCallsWithState(subId, states, &numCallsWithState);
    *result = MAXIMUM_HOLD_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumRingingCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::RINGING;
    Int32 numCallsWithState;
    GetNumCallsWithState(states, &numCallsWithState);
    *result = MAXIMUM_RINGING_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumRingingCalls(
    /* [in] */ const String& subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    (*states)[0] = ICallState::RINGING;
    Int32 numCallsWithState;
    GetNumCallsWithState(subId, states, &numCallsWithState);
    *result = MAXIMUM_RINGING_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::HasMaximumOutgoingCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 numCallsWithState;
    GetNumCallsWithState(OUTGOING_CALL_STATES, &numCallsWithState);
    *result = MAXIMUM_OUTGOING_CALLS <= numCallsWithState;
    return NOERROR;
}

ECode CallsManager::MakeRoomForOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ Boolean isEmergency,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
    AutoPtr<ITelephonyManager> tm;
    telephonyManagerHelper->GetDefault((ITelephonyManager**)&tm);
    Elastos::Droid::Telephony::MultiSimVariants msc;
    tm->GetMultiSimConfiguration(&msc);
    if (msc == Elastos::Droid::Telephony::MultiSimVariants_DSDA) {
        return MakeRoomForOutgoingCallForDsda(call, isEmergency, result);
    }
    Boolean hasMaximumLiveCalls;
    HasMaximumLiveCalls(&hasMaximumLiveCalls);
    if (hasMaximumLiveCalls) {
        // NOTE: If the amount of live calls changes beyond 1, this logic will probably
        // have to change.
        AutoPtr<ICall> liveCall;
        GetFirstCallWithState(call, LIVE_CALL_STATES, (ICall**)&liveCall);
        if (call == liveCall) {
            // If the call is already the foreground call, then we are golden.
            // This can happen after the user selects an account in the PRE_DIAL_WAIT
            // state since the call was already populated into the list.
            *result = TRUE;
            return NOERROR;
        }
        Boolean hasMaximumOutgoingCalls;
        HasMaximumOutgoingCalls(&hasMaximumOutgoingCalls);
        if (hasMaximumOutgoingCalls) {
            // Disconnect the current outgoing call if it's not an emergency call. If the user
            // tries to make two outgoing calls to different emergency call numbers, we will try
            // to connect the first outgoing call.
            if (isEmergency) {
                AutoPtr<ICall> outgoingCall;
                GetFirstCallWithState(OUTGOING_CALL_STATES, (ICall**)&outgoingCall);
                Boolean isEmergencyCall;
                ((Call*) outgoingCall.Get())->IsEmergencyCall(&isEmergencyCall);
                if (!isEmergencyCall) {
                    ((Call*) outgoingCall.Get())->Disconnect();
                    *result = TRUE;
                    return NOERROR;
                }
            }
            *result = FALSE;
            return NOERROR;
        }
        Boolean hasMaximumHoldingCalls;
        HasMaximumHoldingCalls(&hasMaximumHoldingCalls);
        if (hasMaximumHoldingCalls) {
            // There is no more room for any more calls, unless it's an emergency.
            if (isEmergency) {
                // Kill the current active call, this is easier then trying to disconnect a
                // holding call and hold an active call.
                ((Call*) liveCall.Get())->Disconnect();
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;  // No more room!
        }
        // We have room for at least one more holding call at this point.
        // First thing, if we are trying to make a call with the same phone account as the live
        // call, then allow it so that the connection service can make its own decision about
        // how to handle the new call relative to the current one.
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        AutoPtr<IPhoneAccountHandle> livePhoneAccountHandle;
        ((Call*) liveCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&livePhoneAccountHandle);
        if (Objects::Equals(livePhoneAccountHandle, phoneAccountHandle)) {
            *result = TRUE;
            return NOERROR;
        } else if (phoneAccountHandle == NULL) {
            // Without a phone account, we can't say reliably that the call will fail.
            // If the user chooses the same phone account as the live call, then it's
            // still possible that the call can be made (like with CDMA calls not supporting
            // hold but they still support adding a call by going immediately into conference
            // mode). Return true here and we'll run this code again after user chooses an
            // account.
            *result = TRUE;
            return NOERROR;
        }
        // Try to hold the live call before attempting the new outgoing call.
        Boolean canHold;
        ((Call*) liveCall.Get())->Can(IPhoneCapabilities::HOLD, &canHold);
        if (canHold) {
            ((Call*) liveCall.Get())->Hold();
            *result = TRUE;
            return NOERROR;
        }
        // The live call cannot be held so we're out of luck here.  There's no room.
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CallsManager::MakeRoomForOutgoingCallForDsda(
    /* [in] */ ICall* call,
    /* [in] */ Boolean isEmergency,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (isEmergency) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IPhoneAccountHandle> phAcc;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phAcc);
    if (phAcc == NULL) {
        Int32 numCallsWithState;
        GetNumCallsWithState(LIVE_CALL_STATES, &numCallsWithState);
        AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
        (*states)[0] = ICallState::ON_HOLD;
        Int32 numCallsWithStateOnHold;
        GetNumCallsWithState(states, &numCallsWithStateOnHold);
        if (numCallsWithState == MAXIMUM_DSDA_LIVE_CALLS
                && numCallsWithStateOnHold == MAXIMUM_DSDA_HOLD_CALLS) {
            *result = FALSE;
            return NOERROR;
        } else {
            *result = TRUE;
            return NOERROR;
        }
    }
    String id;
    phAcc->GetId(&id);
    Boolean hasMaximumLiveCalls;
    HasMaximumLiveCalls(id, &hasMaximumLiveCalls);
    if (hasMaximumLiveCalls) {
        // NOTE: If the amount of live calls changes beyond 1, this logic will probably
        // have to change.
        AutoPtr<ICall> liveCall;
        GetFirstCallWithStateUsingSubId(id, call, LIVE_CALL_STATES, (ICall**)&liveCall);
        Boolean hasMaximumHoldingCalls;
        HasMaximumHoldingCalls(id, &hasMaximumHoldingCalls);
        if (hasMaximumHoldingCalls) {
            // There is no more room for any more calls, unless it's an emergency.
            *result = FALSE;
            return NOERROR;  // No more room!
        }
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        AutoPtr<IPhoneAccountHandle> livePhoneAccountHandle;
        ((Call*) liveCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&livePhoneAccountHandle);
        if (Objects::Equals(livePhoneAccountHandle, phoneAccountHandle)) {
            *result = TRUE;
            return NOERROR;
        }
        // Try to hold the live call before attempting the new outgoing call.
        Boolean canHold;
        ((Call*) liveCall.Get())->Can(IPhoneCapabilities::HOLD, &canHold);
        if (canHold) {
            ((Call*) liveCall.Get())->Hold();
            *result = TRUE;
            return NOERROR;
        }
        // The live call cannot be held so we're out of luck here.  There's no room.
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CallsManager::CreateCallForExistingConnection(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConnection* connection,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUri> connectionHandle;
    connection->GetHandle((IUri**)&connectionHandle);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    connection->GetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    AutoPtr<ICall> call = new Call();
    ((Call*) call.Get())->constructor(
            mContext,
            mConnectionServiceRepository,
            connectionHandle /* handle */,
            NULL /* gatewayInfo */,
            NULL /* connectionManagerPhoneAccount */,
            phoneAccountHandle, /* targetPhoneAccountHandle */
            FALSE /* isIncoming */,
            FALSE /* isConference */);
    Int32 connectionState;
    connection->GetState(&connectionState);
    Int32 stateFromConnectionState;
    Call::GetStateFromConnectionState(connectionState, &stateFromConnectionState);
    SetCallState(call, stateFromConnectionState);
    AutoPtr<ISystem> systemHelper;
    CSystem::AcquireSingleton((ISystem**)&systemHelper);
    Int64 currentTimeMillis;
    systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
    ((Call*) call.Get())->SetConnectTimeMillis(currentTimeMillis);
    Int32 capabilities;
    connection->GetCapabilities(&capabilities);
    ((Call*) call.Get())->SetCallCapabilities(capabilities);
    String callerDisplayName;
    connection->GetCallerDisplayName(&callerDisplayName);
    Int32 callerDisplayNamePresentation;
    connection->GetCallerDisplayNamePresentation(&callerDisplayNamePresentation);
    ((Call*) call.Get())->SetCallerDisplayName(callerDisplayName, callerDisplayNamePresentation);
    ((Call*) call.Get())->AddListener(this);
    AddCall(call);
    *result = call;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG);
    pw->IncreaseIndent();
    if (mCalls != NULL) {
        IPrintWriter::Probe(pw)->Println(String("mCalls: "));
        pw->IncreaseIndent();
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            IPrintWriter::Probe(pw)->Println(call);
        }
        pw->DecreaseIndent();
    }
    pw->DecreaseIndent();
    return NOERROR;
}

ECode CallsManager::SwitchToOtherActiveSub(
    /* [in] */ const String& subId,
    /* [in] */ Boolean retainLch)
{
    AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
    AutoPtr<ITelephonyManager> telephonyManager;
    telephonyManagerHelper->GetDefault((ITelephonyManager**)&telephonyManager);
    Elastos::Droid::Telephony::MultiSimVariants msc;
    telephonyManager->GetMultiSimConfiguration(&msc);
    if (msc != Elastos::Droid::Telephony::MultiSimVariants_DSDA) {
        return NOERROR;
    }
    Log::I(TAG, "switchToOtherActiveSub sub:%s retainLch:%d", subId.string(), retainLch);
    SetActiveSubscription(subId);
    UpdateLchStatus(subId);
    ManageMSimInCallTones(TRUE);
    UpdateForegroundCall();
    return NOERROR;
}

ECode CallsManager::SetActiveSubscription(
    /* [in] */ const String& subId)
{
    Log::I(TAG, "setActiveSubscription = %s", subId.string());
    AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
    AutoPtr<ITelephonyManager> telephonyManager;
    telephonyManagerHelper->GetDefault((ITelephonyManager**)&telephonyManager);
    Elastos::Droid::Telephony::MultiSimVariants msc;
    telephonyManager->GetMultiSimConfiguration(&msc);
    if (msc != Elastos::Droid::Telephony::MultiSimVariants_DSDA) {
        return NOERROR;
    }
    Boolean changed = FALSE;
    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IList> callCapablePhoneAccounts;
    phoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> ph = IPhoneAccountHandle::Probe(obj);
        AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
        GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
        AutoPtr<IPhoneAccount> phAcc;
        phoneAccountRegistrar->GetPhoneAccount(ph, (IPhoneAccount**)&phAcc);
        String id;
        ph->GetId(&id);
        Boolean isSet;
        phAcc->IsSet(IPhoneAccount::ACTIVE, &isSet);
        if (subId != NULL && subId.Equals(id) && !isSet) {
            changed = TRUE;
            phAcc->SetBit(IPhoneAccount::ACTIVE);
        } else if (subId != NULL && !subId.Equals(id) && isSet) {
            changed = TRUE;
            phAcc->UnSetBit(IPhoneAccount::ACTIVE);
        } else if (subId == NULL && isSet) {
            phAcc->UnSetBit(IPhoneAccount::ACTIVE);
        }
    }
    if (!changed) {
        Log::I(TAG, "setActiveSubscription not changed ");
        return NOERROR;
    } else {
        Log::I(TAG, "setActiveSubscription changed " );
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<IPhoneAccountHandle> ph;
            ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph);
            if (ph != NULL) {
                String id;
                ph->GetId(&id);
                ((Call*) call.Get())->mIsActiveSub = id.Equals(subId) ? TRUE : FALSE;
            }
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<ICallsManagerListener> listener = ICallsManagerListener::Probe(obj);
                Int32 callState;
                ((Call*) call.Get())->GetState(&callState);
                listener->OnCallStateChanged(call, callState, callState);
            }
        }
        AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(4);
        (*states)[0] = ICallState::RINGING;
        (*states)[1] = ICallState::DIALING;
        (*states)[2] = ICallState::ACTIVE;
        (*states)[3] = ICallState::ON_HOLD;
        AutoPtr<ICall> call;
        GetFirstCallWithStateUsingSubId(subId, states, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetActiveSubscription();
        }
    }
    return NOERROR;
}

ECode CallsManager::GetActiveSubscription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IList> callCapablePhoneAccounts;
    phoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> ph = IPhoneAccountHandle::Probe(obj);
        AutoPtr<IPhoneAccount> phoneAccount;
        phoneAccountRegistrar->GetPhoneAccount(ph, (IPhoneAccount**)&phoneAccount);
        Boolean isSet;
        phoneAccount->IsSet(IPhoneAccount::ACTIVE, &isSet);
        if (isSet) {
            String id;
            ph->GetId(&id);
            Log::D(TAG, "getActiveSubscription: %s", id.string());
            *result = id;
            return NOERROR;
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CallsManager::GetConversationSub(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IList> callCapablePhoneAccounts;
    phoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> ph = IPhoneAccountHandle::Probe(obj);
        Boolean isSet;
        AutoPtr<IPhoneAccount> phoneAccount;
        phoneAccountRegistrar->GetPhoneAccount(ph, (IPhoneAccount**)&phoneAccount);
        phoneAccount->IsSet(IPhoneAccount::LCH, &isSet);
        String id;
        ph->GetId(&id);
        AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(3);
        (*states)[0] = ICallState::ACTIVE;
        (*states)[1] = ICallState::DIALING;
        (*states)[2] = ICallState::ON_HOLD;
        AutoPtr<ICall> firstCallWithStateUsingSubId;
        GetFirstCallWithStateUsingSubId(id, states, (ICall**)&firstCallWithStateUsingSubId);
        if (!isSet && firstCallWithStateUsingSubId != NULL) {
            Log::D(TAG, "getConversationSub: %s", id.string());
            *result = id;
            return NOERROR;
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CallsManager::ManageMSimInCallTones(
    /* [in] */ Boolean isSubSwitch)
{
    Log::I(TAG, " entered manageMSimInCallTones ");
    // If there is no background active subscription available, stop playing the tones.
    // Do not start/stop LCH/SCH tones when phone is in RINGING state.
    Boolean hasRingingCall;
    HasRingingCall(&hasRingingCall);
    String lchSub;
    GetLchSub(&lchSub);
    if (lchSub != NULL && !hasRingingCall) {
        //If sub switch happens re-start the tones with a delay of 100msec.
        if (isSubSwitch) {
            Log::I(TAG, " manageMSimInCallTones: re-start playing tones, lch sub = %s", lchSub.string());
            ReStartMSimInCallTones();
        } else {
            Log::I(TAG, " entered manageMSimInCallTones ");
            StartMSimInCallTones();
        }
    } else if (lchSub.IsNull()) {
        // if there is no sub in Lch state, then stop playing the tones
        StopMSimInCallTones();
    }
    return NOERROR;
}

ECode CallsManager::ReStartMSimInCallTones()
{
    Log::I(TAG, " reStartMSimInCallTones");
    StopMSimInCallTones();
    /* Remove any pending PHONE_START_MSIM_INCALL_TONE messages from queue */
    mHandler->RemoveMessages(PHONE_START_MSIM_INCALL_TONE);
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> message;
    helper->Obtain(mHandler, PHONE_START_MSIM_INCALL_TONE, (IMessage**)&message);
    Boolean bNoUse;
    mHandler->SendMessageDelayed(message, 100, &bNoUse);
    return NOERROR;
}

ECode CallsManager::GetFirstCallWithStateUsingSubId(
    /* [in] */ const String& sub,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < states->GetLength(); ++i) {
        Int32 currentState = (*states)[i];
        // check the foreground first
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) mForegroundCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        String phoneAccountHandleId;
        phoneAccountHandle->GetId(&phoneAccountHandleId);
        Boolean isSameIdOrSipId;
        IsSameIdOrSipId(phoneAccountHandleId, sub, &isSameIdOrSipId);
        Int32 foregroundCallState;
        ((Call*) mForegroundCall.Get())->GetState(&foregroundCallState);
        if (mForegroundCall != NULL && foregroundCallState == currentState
                && (phoneAccountHandle != NULL)
                && isSameIdOrSipId) {
            *result = mForegroundCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        AutoPtr<IIterator> it;
        mCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
            ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
            Boolean isSameIdOrSipId;
            String phoneAccountHandleid;
            phoneAccountHandle->GetId(&phoneAccountHandleId);
            IsSameIdOrSipId(phoneAccountHandleId, sub, &isSameIdOrSipId);
            Int32 callState;
            ((Call*) call.Get())->GetState(&callState);
            if ((currentState == callState) &&
                    (phoneAccountHandle != NULL) &&
                    (isSameIdOrSipId)) {
                *result = call;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CallsManager::GetLchSub(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IList> callCapablePhoneAccounts;
    phoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> ph = IPhoneAccountHandle::Probe(obj);
        AutoPtr<IPhoneAccount> phoneAccount;
        phoneAccountRegistrar->GetPhoneAccount(ph, (IPhoneAccount**)&phoneAccount);
        Boolean isSet;
        phoneAccount->IsSet(IPhoneAccount::LCH, &isSet);
        if (isSet) {
            String phId;
            ph->GetId(&phId);
            *result = phId;
            return NOERROR;
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CallsManager::PlayLchDtmf()
{
    Boolean hasMessages;
    mHandler->HasMessages(LCH_PLAY_DTMF, &hasMessages);
    if (mLchSub != NULL || hasMessages) {
        // Ignore any redundant requests to start playing tones
        return NOERROR;
    }
    GetLchSub(&mLchSub);
    Log::I(TAG, " playLchDtmf... lch sub %s", mLchSub.string());
    if (mLchSub == NULL) return NOERROR;
    RemoveAnyPendingDtmfMsgs();
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String str;
    res->GetString(R::string::Lch_dtmf_key, &str);
    Char32 c = str.GetChar(0);
    AutoPtr<ICall> call;
    GetNonRingingLiveCall(mLchSub, (ICall**)&call);
    if (call == NULL) {
        mLchSub = NULL;
        return NOERROR;
    }
    ((Call*) call.Get())->PlayDtmfTone(c);
    // Keep playing LCH DTMF tone to remote party on LCH call, with periodicity
    // "LCH_DTMF_PERIODICITY" until call moves out of LCH.
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(mHandler, LCH_PLAY_DTMF, (IMessage**)&msg);
    Boolean bNoUse;
    mHandler->SendMessageDelayed(msg, LCH_DTMF_PERIODICITY, &bNoUse);
    msg = NULL;
    helper->Obtain(mHandler, LCH_STOP_DTMF, (IMessage**)&msg);
    mHandler->SendMessageDelayed(msg, LCH_DTMF_PERIOD, &bNoUse);
    return NOERROR;
}

ECode CallsManager::GetNonRingingLiveCall(
    /* [in] */ const String& subId,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(3);
    (*states)[0] = ICallState::DIALING;
    (*states)[1] = ICallState::ACTIVE;
    (*states)[2] = ICallState::ON_HOLD;
    AutoPtr<ICall> firstCallWithStateUsingSubId;
    GetFirstCallWithStateUsingSubId(subId, states, (ICall**)&firstCallWithStateUsingSubId);
    *result = firstCallWithStateUsingSubId;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallsManager::StopLchDtmf()
{
    if (!mLchSub.IsNull()) {
        // Ignore any redundant requests to stop playing tones
        AutoPtr<ICall> call;
        GetNonRingingLiveCall(mLchSub, (ICall**)&call);
        Log::I(TAG, " stopLchDtmf... call: %s mLchSub: %s", TO_CSTR(call), mLchSub.string());
        if (call == NULL) {
            mLchSub = String(NULL);
            return NOERROR;
        }
        ((Call*) call.Get())->StopDtmfTone();
    }
    mLchSub = String(NULL);
    return NOERROR;
}

ECode CallsManager::StartMSimInCallTones()
{
    if (mLocalCallReminderTonePlayer == NULL) {
        Log::I(TAG, " Play local call hold reminder tone ");
        mPlayerFactory->CreatePlayer(InCallTonePlayer::TONE_HOLD_RECALL, (InCallTonePlayer**)&mLocalCallReminderTonePlayer);
        mLocalCallReminderTonePlayer->Start();
    }
    if (sSupervisoryCallHoldToneConfig.Equals("inband")) {
        // if "persist.radio.sch_tone" is set to "inband", play inband supervisory
        // call hold tone. if set to "dtmf", play the SCH tones
        // over DTMF, don't play SCH tones for anyother value.
        if (mSupervisoryCallHoldTonePlayer == NULL) {
            Log::I(TAG, " startMSimInCallTones: Supervisory call hold tone ");
            mPlayerFactory->CreatePlayer(InCallTonePlayer::TONE_SUPERVISORY_CH, (InCallTonePlayer**)&mSupervisoryCallHoldTonePlayer);
            mSupervisoryCallHoldTonePlayer->Start();
        }
    } else if (sSupervisoryCallHoldToneConfig.Equals("dtmf")) {
        Log::I(TAG, " startMSimInCallTones: Supervisory call hold tone over dtmf ");
        PlayLchDtmf();
    }
    return NOERROR;
}

ECode CallsManager::RemoveAnyPendingDtmfMsgs()
{
    mHandler->RemoveMessages(LCH_PLAY_DTMF);
    mHandler->RemoveMessages(LCH_STOP_DTMF);
    return NOERROR;
}

ECode CallsManager::StopMSimInCallTones()
{
    if (mLocalCallReminderTonePlayer != NULL) {
        Log::I(TAG, " stopMSimInCallTones: local call hold reminder tone ");
        mLocalCallReminderTonePlayer->StopTone();
        mLocalCallReminderTonePlayer = NULL;
    }
    if (mSupervisoryCallHoldTonePlayer != NULL) {
        Log::I(TAG, " stopMSimInCallTones: Supervisory call hold tone ");
        mSupervisoryCallHoldTonePlayer->StopTone();
        mSupervisoryCallHoldTonePlayer = NULL;
    }
    if (sSupervisoryCallHoldToneConfig.Equals("dtmf")) {
        Log::I(TAG, " stopMSimInCallTones: stop SCH Dtmf call hold tone ");
        StopLchDtmf();
        /* Remove any previous dtmf nssages from queue */
        RemoveAnyPendingDtmfMsgs();
    }
    return NOERROR;
}

ECode CallsManager::UpdateLchStatus(
    /* [in] */ const String& subInConversation)
{
    Log::I(TAG, "updateLchStatus subInConversation: %s", subInConversation.string());
    if (subInConversation != NULL && subInConversation.Contains("sip")) {
        return NOERROR;
    }
    AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
    GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
    AutoPtr<IList> callCapablePhoneAccounts;
    phoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> ph = IPhoneAccountHandle::Probe(obj);
        String sub;
        ph->GetId(&sub);
        if (sub != NULL && sub.Contains("sip")) {
            Log::D(TAG, "update lch. Skipping account: %s", sub.string());
            continue;
        }
        AutoPtr<IPhoneAccount> phAcc;
        phoneAccountRegistrar->GetPhoneAccount(ph, (IPhoneAccount**)&phAcc);
        Boolean lchState = FALSE;
        Boolean hasActiveOrHoldingCall;
        HasActiveOrHoldingCall(sub, &hasActiveOrHoldingCall);
        if (subInConversation != NULL && hasActiveOrHoldingCall &&
                !sub.Equals(subInConversation)) {
            // if sub is not conversation  sub and if it has an active
            // voice call then update lchStatus as Active
            lchState = TRUE;
        }
        // Update state only if the new state is different
        Boolean isSetLCH;
        phAcc->IsSet(IPhoneAccount::LCH, &isSetLCH);
        if (lchState != isSetLCH) {
            AutoPtr<ICall> call;
            GetNonRingingLiveCall(sub, (ICall**)&call);
            Log::I(TAG, " setLocal Call Hold to  = %d sub:%s", lchState, sub.string());
            if (call != NULL) {
                ((Call*) call.Get())->SetLocalCallHold(lchState ? 1 : 0);
            }
            if (lchState) {
                phAcc->SetBit(IPhoneAccount::LCH);
            } else {
                phAcc->UnSetBit(IPhoneAccount::LCH);
            }
        }
    }
    return NOERROR;
}

ECode CallsManager::IsCallBlacklisted(
    /* [in] */ ICall* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String number;
    ((Call*) c)->GetNumber(&number);
    if (number == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // See if the number is in the blacklist
    // Result is one of: MATCH_NONE, MATCH_LIST or MATCH_REGEX
    assert(0 && "TODO: BlacklistUtils");
    // Int32 listType = BlacklistUtils::IsListed(mContext, number, IBlacklistUtils::BLOCK_CALLS);
    // if (listType != IBlacklistUtils::MATCH_NONE) {
    //     // We have a match, set the user and hang up the call and notify
    //     Log::D(TAG, "Incoming call from " + number + " blocked.");
    //     mBlacklistCallNotifier->NotifyBlacklistedCall(number,
    //             c->GetCreationTimeMillis(), listType);
    //     *result = TRUE;
    //     return NOERROR;
    // }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CallsManager::InitOUTGOING_CALL_STATES()
{
    AutoPtr<ArrayOf<Int32> > rev = ArrayOf<Int32>::Alloc(2);
    (*rev)[0] = ICallState::CONNECTING;
    (*rev)[1] = ICallState::DIALING;
    return rev;
}

AutoPtr<ArrayOf<Int32> > CallsManager::InitLIVE_CALL_STATES()
{
    AutoPtr<ArrayOf<Int32> > rev = ArrayOf<Int32>::Alloc(3);
    (*rev)[0] = ICallState::CONNECTING;
    (*rev)[1] = ICallState::DIALING;
    (*rev)[2] = ICallState::ACTIVE;
    return rev;
}

String CallsManager::InitSupervisoryCallHoldToneConfig()
{
    String rev;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Get(String("persist.radio.sch_tone"), String("none"), &rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
