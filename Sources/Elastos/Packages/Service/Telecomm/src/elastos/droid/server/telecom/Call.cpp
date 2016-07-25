
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallIdMapper.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"
#include "elastos/droid/server/telecom/ContactsAsyncHelper.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/RespondViaSmsManager.h"
#include "elastos/droid/server/telecom/Timeouts.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Telecom.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Internal::Telephony::CCallerInfoAsyncQueryHelper;
using Elastos::Droid::Internal::Telephony::EIID_ICallerInfoAsyncQueryOnQueryCompleteListener;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQuery;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQueryHelper;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Telecom::CCallState;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Droid::Telecom::CPhoneCapabilities;
using Elastos::Droid::Telecom::EIID_IResponse;
using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telecom::IConnection;
using Elastos::Droid::Telecom::IParcelableConnection;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Objects;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// Call::ListenerBase
//=============================================================================
CAR_INTERFACE_IMPL(Call::ListenerBase, Object, ICallListener)

ECode Call::ListenerBase::OnSuccessfulOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 callState)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnFailedOutgoingCall(
    /* [in] */ ICall* call,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnSuccessfulIncomingCall(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnFailedIncomingCall(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnSuccessfulUnknownCall(
    /* [in] */ ICall* call,
    /* [in] */ Int32 callState)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnFailedUnknownCall(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnRingbackRequested(
    /* [in] */ ICall* call,
    /* [in] */ Boolean ringbackRequested)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnPostDialWait(
    /* [in] */ ICall* call,
    /* [in] */ const String& remaining)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCallCapabilitiesChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCallPropertiesChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnParentChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnChildrenChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCannedSmsResponsesLoaded(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnVideoCallProviderChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCallerInfoChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnIsVoipAudioModeChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnStatusHintsChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnHandleChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCallerDisplayNameChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnVideoStateChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnTargetPhoneAccountChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnConnectionManagerPhoneAccountChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnPhoneAccountChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnConferenceableCallsChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Call::ListenerBase::OnCallSubstateChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

//=============================================================================
// Call::SubCallerInfoAsyncQueryOnQueryCompleteListener
//=============================================================================
CAR_INTERFACE_IMPL(Call::SubCallerInfoAsyncQueryOnQueryCompleteListener, Object, ICallerInfoAsyncQueryOnQueryCompleteListener)

ECode Call::SubCallerInfoAsyncQueryOnQueryCompleteListener::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICallerInfo* callerInfo)
{
    if (cookie != NULL) {
        ((Call*) IObject::Probe(cookie))->SetCallerInfo(callerInfo, token);
    }
    return NOERROR;
}

//=============================================================================
// Call::SubContactsAsyncHelperOnImageLoadCompleteListener
//=============================================================================
CAR_INTERFACE_IMPL(Call::SubContactsAsyncHelperOnImageLoadCompleteListener, Object, IContactsAsyncHelperOnImageLoadCompleteListener)

ECode Call::SubContactsAsyncHelperOnImageLoadCompleteListener::OnImageLoadComplete(
    /* [in] */ Int32 token,
    /* [in] */ IDrawable* photo,
    /* [in] */ IBitmap* photoIcon,
    /* [in] */ IObject* cookie)
{
    if (cookie != NULL) {
        ((Call*) cookie)->SetPhoto(photo, photoIcon, token);
    }
    return NOERROR;
}

//=============================================================================
// Call::SubRunnable
//=============================================================================
CAR_INTERFACE_IMPL(Call::SubRunnable, Object, IRunnable)

Call::SubRunnable::SubRunnable(
    /* [in] */ Call* host)
    : mHost(host)
{}

ECode Call::SubRunnable::Run()
{
    return mHost->ProcessDirectToVoicemail();
}

//=============================================================================
// Call::SubResponse
//=============================================================================
CAR_INTERFACE_IMPL(Call::SubResponse, Object, IResponse)

Call::SubResponse::SubResponse(
    /* [in] */ Call* host)
    : mHost(host)
{}


ECode Call::SubResponse::OnResult(
    /* [in] */ IInterface* request,
    /* [in] */ ArrayOf<IInterface*>* result)
{
    if (result->GetLength() > 0) {
        Log::D("Call", "maybeLoadCannedSmsResponses: got %s", TO_CSTR((*result)[0]));
        mHost->mCannedSmsResponses = IList::Probe((*result)[0]);
        AutoPtr<IIterator> it;
        mHost->mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnCannedSmsResponsesLoaded(mHost);
        }
    }
    return NOERROR;
}

ECode Call::SubResponse::OnError(
    /* [in] */ IInterface* request,
    /* [in] */ Int32 code,
    /* [in] */ const String& msg)
{
    Log::W("Call", "Error obtaining canned SMS responses: %d %s", code,
            msg.string());
    return NOERROR;
}

//=============================================================================
// Call
//=============================================================================
CAR_INTERFACE_IMPL_2(Call, Object, ICreateConnectionResponse, ICall)

const String Call::KEY_OEM_EXTRAS("OEMExtras");
AutoPtr<ICallerInfoAsyncQueryOnQueryCompleteListener> Call::sCallerInfoQueryListener = new SubCallerInfoAsyncQueryOnQueryCompleteListener();
AutoPtr<IContactsAsyncHelperOnImageLoadCompleteListener> Call::sPhotoLoadListener = new SubContactsAsyncHelperOnImageLoadCompleteListener();

Call::Call()
    : mIsActiveSub(FALSE)
    , mIsIncoming(FALSE)
    , mIsUnknown(FALSE)
    , mCreationTimeMillis(0)
    , mConnectTimeMillis(0)
    , mState(0)
    , mHandlePresentation(0)
    , mCallerDisplayNamePresentation(0)
    , mIsEmergencyCall(FALSE)
    , mSpeakerphoneOn(FALSE)
    , mVideoStateHistory(0)
    , mVideoState(0)
    , mQueryToken(0)
    , mRingbackRequested(FALSE)
    , mDirectToVoicemailQueryPending(FALSE)
    , mCallCapabilities(0)
    , mCallProperties(0)
    , mIsConference(FALSE)
    , mCannedSmsResponsesLoadingStarted(FALSE)
    , mIsVoipAudioMode(FALSE)
    , mCallSubstate(0)
    , mWasConferencePreviouslyMerged(FALSE)
    , mIsLocallyDisconnecting(FALSE)
{
    CDisconnectCause::New(IDisconnectCause::UNKNOWN, (IDisconnectCause**)&mDisconnectCause);
    AutoPtr<IBundleHelper> helper;
    CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
    helper->GetEMPTY((IBundle**)&mExtras);
    CLinkedList::New((IList**)&mChildCalls);
    AutoPtr<ICollections> collectionsHelper;
    CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
    collectionsHelper->GetEmptyList((IList**)&mCannedSmsResponses);
    AutoPtr<IConcurrentHashMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IConcurrentHashMap**)&map);
    collectionsHelper->SetFromMap(IMap::Probe(map), (ISet**)&mListeners);
    mDirectToVoicemailRunnable = new SubRunnable(this);
    CHandler::New((IHandler**)&mHandler);
    CArrayList::New((IList**)&mConferenceableCalls);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreationTimeMillis);
}

ECode Call::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ConnectionServiceRepository* repository,
    /* [in] */ IUri* handle,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccountHandle,
    /* [in] */ IPhoneAccountHandle* targetPhoneAccountHandle,
    /* [in] */ Boolean isIncoming,
    /* [in] */ Boolean isConference)
{
    mState = isConference ? ICallState::ACTIVE : ICallState::NEW;
    mContext = context;
    mRepository = repository;
    SetHandle(handle);
    SetHandle(handle, ITelecomManager::PRESENTATION_ALLOWED);
    mGatewayInfo = gatewayInfo;
    SetConnectionManagerPhoneAccount(connectionManagerPhoneAccountHandle);
    SetTargetPhoneAccount(targetPhoneAccountHandle);
    mIsIncoming = isIncoming;
    mIsConference = isConference;
    MaybeLoadCannedSmsResponses();
    return NOERROR;
}

ECode Call::AddListener(
    /* [in] */ ICallListener* listener)
{
    return mListeners->Add(listener);
}

ECode Call::RemoveListener(
    /* [in] */ ICallListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
    return NOERROR;
}

ECode Call::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)


    String component(NULL);
    if (mConnectionService != NULL) {
        AutoPtr<IComponentName> componentName;
        ((ConnectionServiceWrapper*) mConnectionService.Get())->GetComponentName((IComponentName**)&componentName);
        if (componentName != NULL) {
            componentName->FlattenToShortString(&component);
        }
    }
    String rev;
    Int32 callCapabilities;
    GetCallCapabilities(&callCapabilities);
    Int32 callSubstate;
    GetCallSubstate(&callSubstate);
    AutoPtr<IPhoneCapabilities> phoneCapabilitiesHelper;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&phoneCapabilitiesHelper);
    String s;
    phoneCapabilitiesHelper->ToString(callCapabilities, &s);
    AutoPtr<ISystem> systemHelper;
    CSystem::AcquireSingleton((ISystem**)&systemHelper);
    Int32 identityHashCode;
    systemHelper->IdentityHashCode(TO_IINTERFACE(this), &identityHashCode);
    Int32 videoState;
    GetVideoState(&videoState);
    AutoPtr<IList> childCalls;
    GetChildCalls((IList**)&childCalls);
    Int32 size;
    childCalls->GetSize(&size);
    AutoPtr<ICallState> callStateHelper;
    CCallState::AcquireSingleton((ICallState**)&callStateHelper);
    String sCallState;
    callStateHelper->ToString(mState, &sCallState);
    AutoPtr<ICall> parentCall;
    GetParentCall((ICall**)&parentCall);
    rev.AppendFormat("[%d, %s, %s, %s, %d, childs(%d), has_parent(%d), [%s], %d, %s %d]",
            identityHashCode,
            sCallState.string(),
            component.string(),
            Log::PiiHandle(mHandle).string(),
            videoState,
            size,
            parentCall != NULL,
            s.string(),
            mIsActiveSub,
            TO_CSTR(mTargetPhoneAccountHandle),
            callSubstate);
    *result = rev;
    return NOERROR;
}

ECode Call::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode Call::SetState(
    /* [in] */ Int32 newState)
{
    if (mState != newState) {
        Log::V("Call", "setState %d -> %d", mState, newState);
        mState = newState;
        MaybeLoadCannedSmsResponses();
        if (mState == ICallState::DISCONNECTED) {
            SetLocallyDisconnecting(FALSE);
            FixParentAfterDisconnect();
        }
    }
    return NOERROR;
}

ECode Call::SetRingbackRequested(
    /* [in] */ Boolean ringbackRequested)
{
    mRingbackRequested = ringbackRequested;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnRingbackRequested(this, mRingbackRequested);
    }
    return NOERROR;
}

ECode Call::IsRingbackRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRingbackRequested;
    return NOERROR;
}

ECode Call::IsConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsConference;
    return NOERROR;
}

ECode Call::GetHandle(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetHandlePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandlePresentation;
    return NOERROR;
}

ECode Call::SetHandle(
    /* [in] */ IUri* handle)
{
    return SetHandle(handle, ITelecomManager::PRESENTATION_ALLOWED);
}

ECode Call::SetHandle(
    /* [in] */ IUri* handle,
    /* [in] */ Int32 presentation)
{
    if (!Objects::Equals(handle, mHandle.Get()) || presentation != mHandlePresentation) {
        mHandle = handle;
        mHandlePresentation = presentation;
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        Boolean isLocalEmergencyNumber;
        String schemeSpecificPart;
        mHandle->GetSchemeSpecificPart(&schemeSpecificPart);
        phoneNumberUtilsHelper->IsLocalEmergencyNumber(mContext, schemeSpecificPart, &isLocalEmergencyNumber);
        mIsEmergencyCall = mHandle != NULL && isLocalEmergencyNumber;
        Logger::W("Call", "TODO Call::SetHandle ignore the function StartCallerInfoLookup!!");
        //TODO leliang StartCallerInfoLookup();
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnHandleChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::GetCallerDisplayName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayName;
    return NOERROR;
}

ECode Call::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode Call::SetCallerDisplayName(
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    if (!TextUtils::Equals(callerDisplayName, mCallerDisplayName) ||
            presentation != mCallerDisplayNamePresentation) {
        mCallerDisplayName = callerDisplayName;
        mCallerDisplayNamePresentation = presentation;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnCallerDisplayNameChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String name;
    mCallerInfo->GetName(&name);
    *result = mCallerInfo == NULL ? String(NULL) : name;
    return NOERROR;
}

ECode Call::GetNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallerInfo != NULL) {
        String phoneNumber;
        mCallerInfo->GetPhoneNumber(&phoneNumber);
        *result = phoneNumber;
        return NOERROR;
    }
    if (mHandle == NULL) {
        *result = String(NULL);
        return NOERROR;
    }
    else {
        return mHandle->GetSchemeSpecificPart(result);
    }
    return NOERROR;
}

ECode Call::GetPhotoIcon(
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallerInfo == NULL) {
        *result = NULL;
        return NOERROR;
    }
    else {
        AutoPtr<IBitmap> cachedPhotoIcon;
        mCallerInfo->GetCachedPhotoIcon((IBitmap**)&cachedPhotoIcon);
        *result = cachedPhotoIcon;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode Call::GetPhoto(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallerInfo == NULL) {
        *result = NULL;
        return NOERROR;
    }
    else {
        AutoPtr<IDrawable> cachedPhoto;
        mCallerInfo->GetCachedPhoto((IDrawable**)&cachedPhoto);
        *result = cachedPhoto;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode Call::SetDisconnectCause(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    // TODO: Consider combining this method with a setDisconnected() method that is totally
    // separate from setState.
    mDisconnectCause = disconnectCause;
    return NOERROR;
}

ECode Call::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::IsEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsEmergencyCall;
    return NOERROR;
}

ECode Call::GetOriginalHandle(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isEmpty;
    if (mGatewayInfo != NULL)
        mGatewayInfo->IsEmpty(&isEmpty);
    if (mGatewayInfo != NULL && !isEmpty) {
        return mGatewayInfo->GetOriginalAddress(result);
    }
    return GetHandle(result);
}

ECode Call::GetGatewayInfo(
    /* [out] */ IGatewayInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetGatewayInfo(
    /* [in] */ IGatewayInfo* gatewayInfo)
{
    mGatewayInfo = gatewayInfo;
    return NOERROR;
}

ECode Call::GetConnectionManagerPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionManagerPhoneAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetConnectionManagerPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (!Objects::Equals(mConnectionManagerPhoneAccountHandle.Get(), accountHandle)) {
        mConnectionManagerPhoneAccountHandle = accountHandle;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnConnectionManagerPhoneAccountChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::GetTargetPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTargetPhoneAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetTargetPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (!Objects::Equals(mTargetPhoneAccountHandle.Get(), accountHandle)) {
        Log::I("Call", "setTargetPhoneAccount  %s ", TO_CSTR(accountHandle));
        mTargetPhoneAccountHandle = accountHandle;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnTargetPhoneAccountChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::IsIncoming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsIncoming;
    return NOERROR;
}

ECode Call::GetAgeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISystem> systemHelper;
    CSystem::AcquireSingleton((ISystem**)&systemHelper);
    Int64 currentTimeMillis;
    systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
    *result = currentTimeMillis - mCreationTimeMillis;
    return NOERROR;
}

ECode Call::GetDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    if (mConnectTimeMillis != 0) {
        AutoPtr<ISystem> systemHelper;
        CSystem::AcquireSingleton((ISystem**)&systemHelper);
        Int64 currentTimeMillis;
        systemHelper->GetCurrentTimeMillis(&currentTimeMillis);
        *result = currentTimeMillis - mConnectTimeMillis;
        return NOERROR;
    } else {
        *result = 0;
        return NOERROR;
    }
    return NOERROR;
}

ECode Call::GetCreationTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCreationTimeMillis;
    return NOERROR;
}

ECode Call::SetCreationTimeMillis(
    /* [in] */ Int64 time)
{
    mCreationTimeMillis = time;
    return NOERROR;
}

ECode Call::GetConnectTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectTimeMillis;
    return NOERROR;
}

ECode Call::SetConnectTimeMillis(
    /* [in] */ Int64 connectTimeMillis)
{
    mConnectTimeMillis = connectTimeMillis;
    return NOERROR;
}

ECode Call::GetCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallCapabilities;
    return NOERROR;
}

ECode Call::SetCallCapabilities(
    /* [in] */ Int32 callCapabilities)
{
    return SetCallCapabilities(callCapabilities, FALSE /* forceUpdate */);
}

ECode Call::SetCallCapabilities(
    /* [in] */ Int32 callCapabilities,
    /* [in] */ Boolean forceUpdate)
{
    AutoPtr<IPhoneCapabilities> phoneCapabilitiesHelper;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&phoneCapabilitiesHelper);
    String s;
    phoneCapabilitiesHelper->ToString(callCapabilities, &s);
    Log::V("Call", "setCallCapabilities: %s", s.string());
    if (forceUpdate || mCallCapabilities != callCapabilities) {
       mCallCapabilities = callCapabilities;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnCallCapabilitiesChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::GetCallProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallProperties;
    return NOERROR;
}

ECode Call::SetCallProperties(
    /* [in] */ Int32 callProperties)
{
    Log::V("Call", "setCallProperties: 0x%x", callProperties);
    if (mCallProperties != callProperties) {
        mCallProperties = callProperties;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnCallPropertiesChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::GetParentCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetChildCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChildCalls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::WasConferencePreviouslyMerged(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWasConferencePreviouslyMerged;
    return NOERROR;
}

ECode Call::GetConferenceLevelActiveCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConferenceLevelActiveCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetConnectionService(
    /* [out] */ IConnectionServiceWrapper** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionService;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetConnectionService(
    /* [in] */ IConnectionServiceWrapper* service)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(service);
    ClearConnectionService();
    ((ConnectionServiceWrapper*) service)->IncrementAssociatedCallCount();
    mConnectionService = service;
    ((ConnectionServiceWrapper*) mConnectionService.Get())->AddCall(this);
    return NOERROR;
}

ECode Call::ClearConnectionService()
{
    if (mConnectionService != NULL) {
        AutoPtr<IConnectionServiceWrapper> serviceTemp = mConnectionService;
        mConnectionService = NULL;
        ((ConnectionServiceWrapper*) serviceTemp.Get())->RemoveCall(this);
        // Decrementing the count can cause the service to unbind, which itself can trigger the
        // service-death code.  Since the service death code tries to clean up any associated
        // calls, we need to make sure to remove that information (e.g., removeCall()) before
        // we decrement. Technically, invoking removeCall() prior to decrementing is all that is
        // necessary, but cleaning up mConnectionService prior to triggering an unbind is good
        // to do.
        DecrementAssociatedCallCount(IServiceBinder::Probe(serviceTemp));
    }
    return NOERROR;
}

ECode Call::ProcessDirectToVoicemail()
{
    if (mDirectToVoicemailQueryPending) {
        Boolean shouldSendToVoicemail;
        mCallerInfo->GetShouldSendToVoicemail(&shouldSendToVoicemail);
        if (mCallerInfo != NULL && shouldSendToVoicemail) {
            Log::I("Call", "Directing call to voicemail: %s.", TO_CSTR(this));
            // TODO: Once we move State handling from CallsManager to Call, we
            // will not need to set STATE_RINGING state prior to calling reject.
            SetState(ICallState::RINGING);
            Reject(FALSE, String(NULL));
        } else {
            // TODO: Make this class (not CallsManager) responsible for changing
            // the call state to STATE_RINGING.
            // TODO: Replace this with state transition to STATE_RINGING.
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<ICallListener> l = ICallListener::Probe(obj);
                l->OnSuccessfulIncomingCall(this);
            }
        }
        mDirectToVoicemailQueryPending = FALSE;
    }
    return NOERROR;
}

ECode Call::StartCreateConnection(
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckState(mCreateConnectionProcessor == NULL);
    //note keep the variable here, because in the CreateConnectionProcessor::Process(),
    //mCreateConnectionProcessor will be set NULL,
    //also see  HandleCreateConnectionFailure() and HandleCreateConnectionSuccess();
    AutoPtr<CreateConnectionProcessor> tmp = new CreateConnectionProcessor();
    mCreateConnectionProcessor = tmp;
    mCreateConnectionProcessor->constructor(this, mRepository, this,
            phoneAccountRegistrar, mContext);
    mCreateConnectionProcessor->Process();
    return NOERROR;
}

ECode Call::HandleCreateConnectionSuccess(
    /* [in] */ ICallIdMapper* idMapper,
    /* [in] */ IParcelableConnection* connection)
{
    Log::V("Call", "handleCreateConnectionSuccessful %s", TO_CSTR(connection));
    mCreateConnectionProcessor = NULL;
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    connection->GetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    SetTargetPhoneAccount(phoneAccountHandle);
    AutoPtr<IUri> handle;
    connection->GetHandle((IUri**)&handle);
    Int32 handlePresentation;
    connection->GetHandlePresentation(&handlePresentation);
    SetHandle(handle, handlePresentation);
    Int32 callerDisplayNamePresentation;
    connection->GetCallerDisplayNamePresentation(&callerDisplayNamePresentation);
    String callerDisplayName;
    connection->GetCallerDisplayName(&callerDisplayName);
    SetCallerDisplayName(
            callerDisplayName, callerDisplayNamePresentation);
    Int32 capabilities;
    connection->GetCapabilities(&capabilities);
    SetCallCapabilities(capabilities);
    Int32 properties;
    connection->GetProperties(&properties);
    SetCallProperties(properties);
    AutoPtr<IIVideoProvider> videoProvider;
    connection->GetVideoProvider((IIVideoProvider**)&videoProvider);
    SetVideoProvider(videoProvider);
    Int32 videoState;
    connection->GetVideoState(&videoState);
    SetVideoState(videoState);
    Boolean isRingbackRequested;
    connection->IsRingbackRequested(&isRingbackRequested);
    SetRingbackRequested(isRingbackRequested);
    Boolean isVoipAudioMode;
    connection->GetIsVoipAudioMode(&isVoipAudioMode);
    SetIsVoipAudioMode(isVoipAudioMode);
    AutoPtr<IStatusHints> statusHints;
    connection->GetStatusHints((IStatusHints**)&statusHints);
    SetStatusHints(statusHints);
    Int32 callSubstate;
    connection->GetCallSubstate(&callSubstate);
    SetCallSubstate(callSubstate);
    mConferenceableCalls->Clear();
    AutoPtr<IList> conferenceableConnectionIds;
    connection->GetConferenceableConnectionIds((IList**)&conferenceableConnectionIds);
    AutoPtr<IIterator> it;
    conferenceableConnectionIds->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String id;
        IObject::Probe(obj)->ToString(&id);
        AutoPtr<ICall> call;
        ((CallIdMapper*) idMapper)->GetCall(StringUtils::ParseCharSequence(id), (ICall**)&call);
        mConferenceableCalls->Add(call);
    }
    if (mIsUnknown) {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            Int32 state;
            connection->GetState(&state);
            Int32 stateFromConnectionState;
            GetStateFromConnectionState(state, &stateFromConnectionState);
            l->OnSuccessfulUnknownCall(this, stateFromConnectionState);
        }
    } else if (mIsIncoming) {
        // We do not handle incoming calls immediately when they are verified by the connection
        // service. We allow the caller-info-query code to execute first so that we can read the
        // direct-to-voicemail property before deciding if we want to show the incoming call to
        // the user or if we want to reject the call.
        mDirectToVoicemailQueryPending = TRUE;
        // Timeout the direct-to-voicemail lookup execution so that we dont wait too long before
        // showing the user the incoming call screen.
        AutoPtr<IContentResolver> contentResolver;
        mContext->GetContentResolver((IContentResolver**)&contentResolver);
        Int64 directToVoicemailMillis;
        Timeouts::GetDirectToVoicemailMillis(contentResolver, &directToVoicemailMillis);
        Boolean bNoUse;
        mHandler->PostDelayed(mDirectToVoicemailRunnable, directToVoicemailMillis, &bNoUse);
    } else {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            Int32 state;
            connection->GetState(&state);
            Int32 stateFromConnectionState;
            GetStateFromConnectionState(state, &stateFromConnectionState);
            l->OnSuccessfulOutgoingCall(this, stateFromConnectionState);
        }
    }
    return NOERROR;
}

ECode Call::HandleCreateConnectionFailure(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mCreateConnectionProcessor = NULL;
    ClearConnectionService();
    SetDisconnectCause(disconnectCause);
    CallsManager::GetInstance()->MarkCallAsDisconnected(this, disconnectCause);
    if (mIsUnknown) {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> listener = ICallListener::Probe(obj);
            listener->OnFailedUnknownCall(this);
        }
    } else if (mIsIncoming) {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> listener = ICallListener::Probe(obj);
            listener->OnFailedIncomingCall(this);
        }
    } else {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> listener = ICallListener::Probe(obj);
            listener->OnFailedOutgoingCall(this, disconnectCause);
        }
    }
    return NOERROR;
}

ECode Call::PlayDtmfTone(
    /* [in] */ Char32 digit)
{
    if (mConnectionService == NULL) {
        Log::W("Call", "playDtmfTone() request on a call without a connection service.");
    } else {
        Log::I("Call", "Send playDtmfTone to connection service for call %s", TO_CSTR(this));
        ((ConnectionServiceWrapper*) mConnectionService.Get())->PlayDtmfTone(this, digit);
    }
    return NOERROR;
}

ECode Call::StopDtmfTone()
{
    if (mConnectionService == NULL) {
        Log::W("Call", "stopDtmfTone() request on a call without a connection service.");
    } else {
        Log::I("Call", "Send stopDtmfTone to connection service for call %s", TO_CSTR(this));
        ((ConnectionServiceWrapper*) mConnectionService.Get())->StopDtmfTone(this);
    }
    return NOERROR;
}

ECode Call::Disconnect()
{
    // Track that the call is now locally disconnecting.
    SetLocallyDisconnecting(TRUE);
    if (mState == ICallState::NEW || mState == ICallState::PRE_DIAL_WAIT ||
            mState == ICallState::CONNECTING) {
        Log::V("Call", "Aborting call %s", TO_CSTR(this));
        Abort();
    } else if (mState != ICallState::ABORTED && mState != ICallState::DISCONNECTED) {
        if (mConnectionService == NULL) {
            Log::E("Call", "disconnect() request on a call without a connection service.");
        } else {
            Log::I("Call", "Send disconnect to connection service for call: %s", TO_CSTR(this));
            // The call isn't officially disconnected until the connection service
            // confirms that the call was actually disconnected. Only then is the
            // association between call and connection service severed, see
            // {@link CallsManager#markCallAsDisconnected}.
            ((ConnectionServiceWrapper*) mConnectionService.Get())->Disconnect(this);
        }
    }
    return NOERROR;
}

ECode Call::Abort()
{
    if (mCreateConnectionProcessor != NULL) {
        mCreateConnectionProcessor->Abort();
    } else if (mState == ICallState::NEW || mState == ICallState::PRE_DIAL_WAIT
            || mState == ICallState::CONNECTING) {
        AutoPtr<IDisconnectCause> disconnectCause;
        CDisconnectCause::New(IDisconnectCause::CANCELED, (IDisconnectCause**)&disconnectCause);
        HandleCreateConnectionFailure(disconnectCause);
    } else {
        Log::V("Call", "Cannot abort a call which isn't either PRE_DIAL_WAIT or CONNECTING");
    }
    return NOERROR;
}

ECode Call::Answer(
    /* [in] */ Int32 videoState)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    // Check to verify that the call is still in the ringing state. A call can change states
    // between the time the user hits 'answer' and Telecom receives the command.
    Boolean isRinging;
    IsRinging(String("answer"), &isRinging);
    if (isRinging) {
        // At this point, we are asking the connection service to answer but we don't assume
        // that it will work. Instead, we wait until confirmation from the connectino service
        // that the call is in a non-STATE_RINGING state before changing the UI. See
        // {@link ConnectionServiceAdapter#setActive} and other set* methods.
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Answer(this, videoState);
    }
    return NOERROR;
}

ECode Call::Deflect(
    /* [in] */ const String& number)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    // Check to verify that the call is still in the ringing state. A call can change states
    // between the time the user hits 'deflect' and Telecomm receives the command.
    Boolean isRinging;
    IsRinging(String("deflect"), &isRinging);
    if (isRinging) {
        // At this point, we are asking the call service to deflect but we don't assume that
        // it will work. Instead, we wait until confirmation from the call service that the
        // call is in a non-RINGING state before changing the UI. See
        // {@link CallServiceAdapter#setActive} and other set* methods.
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Deflect(this, number);
    }
    return NOERROR;
}

ECode Call::Reject(
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    // Check to verify that the call is still in the ringing state. A call can change states
    // between the time the user hits 'reject' and Telecomm receives the command.
    Boolean isRinging;
    IsRinging(String("reject"), &isRinging);
    if (isRinging) {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Reject(this);
    }
    return NOERROR;
}

ECode Call::Hold()
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    if (mState == ICallState::ACTIVE) {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Hold(this);
    }
    return NOERROR;
}

ECode Call::Unhold()
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    if (mState == ICallState::ON_HOLD) {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Unhold(this);
    }
    return NOERROR;
}

ECode Call::SetLocalCallHold(
    /* [in] */ Int32 lchState)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    ((ConnectionServiceWrapper*) mConnectionService.Get())->SetLocalCallHold(this, lchState);
    return NOERROR;
}

ECode Call::SetActiveSubscription()
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(mConnectionService);
    ((ConnectionServiceWrapper*) mConnectionService.Get())->SetActiveSubscription(this);
    return NOERROR;
}

ECode Call::IsAlive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (mState) {
        case ICallState::NEW:
        case ICallState::RINGING:
        case ICallState::DISCONNECTED:
        case ICallState::ABORTED:
            *result = FALSE;
            return NOERROR;
        default:
            *result = TRUE;
            return NOERROR;
    }
    return NOERROR;
}

ECode Call::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState == ICallState::ACTIVE;
    return NOERROR;
}

ECode Call::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode Call::GetContactUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

        Boolean contactExists;
        mCallerInfo->GetContactExists(&contactExists);
        if (mCallerInfo == NULL || !contactExists) {
            return GetHandle(result);
        }
        AutoPtr<IContactsContractContacts> helper;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&helper);
        String lookupKey;
        mCallerInfo->GetLookupKey(&lookupKey);
        Int64 contactIdOrZero;
        mCallerInfo->GetContactIdOrZero(&contactIdOrZero);
        return helper->GetLookupUri(contactIdOrZero, lookupKey, result);
}

ECode Call::GetRingtone(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUri> contactRingtoneUri;
    mCallerInfo->GetContactRingtoneUri((IUri**)&contactRingtoneUri);
    *result = mCallerInfo == NULL ? NULL : contactRingtoneUri;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::OnPostDialWait(
    /* [in] */ const String& remaining)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnPostDialWait(this, remaining);
    }
    return NOERROR;
}

ECode Call::PostDialContinue(
    /* [in] */ Boolean proceed)
{
    return ((ConnectionServiceWrapper*) mConnectionService.Get())->OnPostDialContinue(this, proceed);
}

ECode Call::ConferenceWith(
    /* [in] */ ICall* otherCall)
{
    if (mConnectionService == NULL) {
        Log::W("Call", "conference requested on a call without a connection service.");
    } else {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->Conference(this, otherCall);
    }
    return NOERROR;
}

ECode Call::SplitFromConference()
{
    if (mConnectionService == NULL) {
        Log::W("Call", "splitting from conference call without a connection service");
    } else {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->SplitFromConference(this);
    }
    return NOERROR;
}

ECode Call::MergeConference()
{
    Boolean isCan;
    Can(IPhoneCapabilities::MERGE_CONFERENCE, &isCan);
    if (mConnectionService == NULL) {
        Log::W("Call", "merging conference calls without a connection service.");
    } else if (isCan) {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->MergeConference(this);
        mWasConferencePreviouslyMerged = TRUE;
    }
    return NOERROR;
}

ECode Call::SwapConference()
{
    Boolean canSwap;
    Can(IPhoneCapabilities::SWAP_CONFERENCE, &canSwap);
    if (mConnectionService == NULL) {
        Log::W("Call", "swapping conference calls without a connection service.");
    } else if (canSwap) {
        ((ConnectionServiceWrapper*) mConnectionService.Get())->SwapConference(this);
        Int32 size;
        mChildCalls->GetSize(&size);
        switch (size) {
            case 1: {
                AutoPtr<IInterface> obj;
                mChildCalls->Get(0, (IInterface**)&obj);
                mConferenceLevelActiveCall = ICall::Probe(obj);
                break;
            }
            case 2: {
                // swap
                AutoPtr<IInterface> obj;
                mChildCalls->Get(0, (IInterface**)&obj);
                if (obj == mConferenceLevelActiveCall) {
                    obj = NULL;
                    mChildCalls->Get(1, (IInterface**)&obj);
                    mConferenceLevelActiveCall = ICall::Probe(obj);
                }
                else {
                    mConferenceLevelActiveCall = ICall::Probe(obj);
                }
                break;
            }
            default:
                // For anything else 0, or 3+, set it to null since it is impossible to tell.
                mConferenceLevelActiveCall = NULL;
                break;
        }
    }
    return NOERROR;
}

ECode Call::SetParentCall(
    /* [in] */ ICall* parentCall)
{
    if (parentCall == this) {
        Log::E("Call", "setting the parent to self");
        return NOERROR;
    }
    if (parentCall == mParentCall) {
        // nothing to do
        return NOERROR;
    }
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckState(parentCall == NULL || mParentCall == NULL);
    AutoPtr<ICall> oldParent = mParentCall;
    if (mParentCall != NULL) {
        ((Call*) mParentCall.Get())->RemoveChildCall(this);
    }
    mParentCall = parentCall;
    if (mParentCall != NULL) {
        ((Call*) mParentCall.Get())->AddChildCall(this);
    }
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnParentChanged(this);
    }
    return NOERROR;
}

ECode Call::SetConferenceableCalls(
    /* [in] */ IList* conferenceableCalls)
{
    mConferenceableCalls->Clear();
    Boolean bNoUse;
    mConferenceableCalls->AddAll(ICollection::Probe(conferenceableCalls), &bNoUse);
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnConferenceableCallsChanged(this);
    }
    return NOERROR;
}

ECode Call::GetConferenceableCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConferenceableCalls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Can(
    /* [in] */ Int32 capability,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mCallCapabilities & capability) == capability;
    return NOERROR;
}

ECode Call::AddChildCall(
    /* [in] */ ICall* call)
{
    Boolean isContains;
    mChildCalls->Contains(call, &isContains);
    if (!isContains) {
        // Set the pseudo-active call to the latest child added to the conference.
        // See definition of mConferenceLevelActiveCall for more detail.
        mConferenceLevelActiveCall = call;
        mChildCalls->Add(call);
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnChildrenChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::RemoveChildCall(
    /* [in] */ ICall* call)
{
    Boolean isRemoveOk;
    mChildCalls->Remove(call, &isRemoveOk);
    if (isRemoveOk) {
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnChildrenChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::IsRespondViaSmsCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != ICallState::RINGING) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IUri> uri;
    GetHandle((IUri**)&uri);
    if (uri == NULL) {
        // No incoming number known or call presentation is "PRESENTATION_RESTRICTED", in
        // other words, the user should not be able to see the incoming phone number.
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    Boolean isUriNumber;
    phoneNumberUtilsHelper->IsUriNumber(TO_STR(uri), &isUriNumber);
    if (isUriNumber) {
        // The incoming number is actually a URI (i.e. a SIP address),
        // not a regular PSTN phone number, and we can't send SMSes to
        // SIP addresses.
        // (TODO: That might still be possible eventually, though. Is
        // there some SIP-specific equivalent to sending a text message?)
        *result = FALSE;
        return NOERROR;
    }
    // Is there a valid SMS application on the phone?
    assert(0 && "TODO: SmsApplication");
    // if (SmsApplication::GetDefaultRespondViaMessageApplication(mContext,
            // TRUE /*updateIfNeeded*/) == NULL) {
        // *result = FALSE;
        // return NOERROR;
    // }
    // TODO: with some carriers (in certain countries) you *can* actually
    // tell whether a given number is a mobile phone or not. So in that
    // case we could potentially return false here if the incoming call is
    // from a land line.
    // If none of the above special cases apply, it's OK to enable the
    // "Respond via SMS" feature.
    *result = TRUE;
    return NOERROR;
}

ECode Call::GetCannedSmsResponses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCannedSmsResponses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::FixParentAfterDisconnect()
{
    return SetParentCall(NULL);
}

ECode Call::IsRinging(
    /* [in] */ const String& actionName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState == ICallState::RINGING) {
        *result = TRUE;
        return NOERROR;
    }
    Log::I("Call", "Request to %s a non-ringing call %s", actionName.string(), TO_CSTR(this));
    *result = FALSE;
    return NOERROR;
}

ECode Call::DecrementAssociatedCallCount(
    /* [in] */ IServiceBinder* binder)
{
    if (binder != NULL) {
        ((ServiceBinder*) binder)->DecrementAssociatedCallCount();
    }
    return NOERROR;
}

ECode Call::StartCallerInfoLookup()
{
    String schemeSpecificPart;
    mHandle->GetSchemeSpecificPart(&schemeSpecificPart);
    String number = mHandle == NULL ? String(NULL) : schemeSpecificPart;
    mQueryToken++;  // Updated so that previous queries can no longer set the information.
    mCallerInfo = NULL;
    if (!TextUtils::IsEmpty(number)) {
        Log::V("Call", "Looking up information for: %s.", Log::PiiHandle(StringUtils::ParseCharSequence(number)).string());
        AutoPtr<ICallerInfoAsyncQueryHelper> helper;
        CCallerInfoAsyncQueryHelper::AcquireSingleton((ICallerInfoAsyncQueryHelper**)&helper);
        AutoPtr<ICallerInfoAsyncQuery> iNoUse;
        helper->StartQuery(
                mQueryToken,
                mContext,
                number,
                sCallerInfoQueryListener,
                TO_IINTERFACE(this),
                (ICallerInfoAsyncQuery**)&iNoUse);
    }
    return NOERROR;
}

ECode Call::SetCallerInfo(
    /* [in] */ ICallerInfo* callerInfo,
    /* [in] */ Int32 token)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckNotNull(callerInfo);
    if (mQueryToken == token) {
        mCallerInfo = callerInfo;
        Log::I("Call", "CallerInfo received for %s: %s", Log::PiiHandle(mHandle).string(), TO_CSTR(callerInfo));
        AutoPtr<IUri> contactDisplayPhotoUri;
        mCallerInfo->GetContactDisplayPhotoUri((IUri**)&contactDisplayPhotoUri);
        if (contactDisplayPhotoUri != NULL) {
            Log::D("Call", "Searching person uri %s for call %s",
                    TO_CSTR(contactDisplayPhotoUri), TO_CSTR(this));
            ContactsAsyncHelper::StartObtainPhotoAsync(
                    token,
                    mContext,
                    contactDisplayPhotoUri,
                    sPhotoLoadListener,
                    this);
            // Do not call onCallerInfoChanged yet in this case.  We call it in setPhoto().
        } else {
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<ICallListener> l = ICallListener::Probe(obj);
                l->OnCallerInfoChanged(this);
            }
        }
        ProcessDirectToVoicemail();
    }
    return NOERROR;
}

ECode Call::GetCallerInfo(
    /* [out] */ ICallerInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetPhoto(
    /* [in] */ IDrawable* photo,
    /* [in] */ IBitmap* photoIcon,
    /* [in] */ Int32 token)
{
    if (mQueryToken == token) {
        mCallerInfo->SetCachedPhoto(photo);
        mCallerInfo->SetCachedPhotoIcon(photoIcon);
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICallListener> l = ICallListener::Probe(obj);
            l->OnCallerInfoChanged(this);
        }
    }
    return NOERROR;
}

ECode Call::MaybeLoadCannedSmsResponses()
{
    Boolean isRespondViaSmsCapable;
    IsRespondViaSmsCapable(&isRespondViaSmsCapable);
    if (mIsIncoming && isRespondViaSmsCapable && !mCannedSmsResponsesLoadingStarted) {
        Log::D("Call", "maybeLoadCannedSmsResponses: starting task to load messages");
        mCannedSmsResponsesLoadingStarted = TRUE;
        AutoPtr<RespondViaSmsManager> respondViaSmsManager;
        RespondViaSmsManager::GetInstance((RespondViaSmsManager**)&respondViaSmsManager);
        respondViaSmsManager->LoadCannedTextMessages(
                new SubResponse(this),
                mContext);
    } else {
        Log::D("Call", "maybeLoadCannedSmsResponses: doing nothing");
    }
    return NOERROR;
}

ECode Call::SetStartWithSpeakerphoneOn(
    /* [in] */ Boolean startWithSpeakerphone)
{
    mSpeakerphoneOn = startWithSpeakerphone;
    return NOERROR;
}

ECode Call::GetStartWithSpeakerphoneOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpeakerphoneOn;
    return NOERROR;
}

ECode Call::SetVideoProvider(
    /* [in] */ IIVideoProvider* videoProvider)
{
    mVideoProvider = videoProvider;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnVideoCallProviderChanged(this);
    }
    return NOERROR;
}

ECode Call::GetVideoProvider(
    /* [out] */ IIVideoProvider** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoProvider;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode Call::GetVideoStateHistory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoStateHistory;
    return NOERROR;
}

ECode Call::SetVideoState(
    /* [in] */ Int32 videoState)
{
    // Track which video states were applicable over the duration of the call.
    mVideoStateHistory = mVideoStateHistory | videoState;
    mVideoState = videoState;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnVideoStateChanged(this);
    }
    return NOERROR;
}

ECode Call::GetIsVoipAudioMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsVoipAudioMode;
    return NOERROR;
}

ECode Call::SetIsVoipAudioMode(
    /* [in] */ Boolean audioModeIsVoip)
{
    mIsVoipAudioMode = audioModeIsVoip;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnIsVoipAudioModeChanged(this);
    }
    return NOERROR;
}

ECode Call::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::SetStatusHints(
    /* [in] */ IStatusHints* statusHints)
{
    mStatusHints = statusHints;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnStatusHintsChanged(this);
    }
    return NOERROR;
}

ECode Call::IsUnknown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsUnknown;
    return NOERROR;
}

ECode Call::SetIsUnknown(
    /* [in] */ Boolean isUnknown)
{
    mIsUnknown = isUnknown;
    return NOERROR;
}

ECode Call::IsLocallyDisconnecting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsLocallyDisconnecting;
    return NOERROR;
}

ECode Call::SetLocallyDisconnecting(
    /* [in] */ Boolean isLocallyDisconnecting)
{
    mIsLocallyDisconnecting = isLocallyDisconnecting;
    return NOERROR;
}

ECode Call::GetStateFromConnectionState(
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    switch (state) {
        case IConnection::STATE_INITIALIZING:
            *result = ICallState::CONNECTING;
            return NOERROR;
        case IConnection::STATE_ACTIVE:
            *result = ICallState::ACTIVE;
            return NOERROR;
        case IConnection::STATE_DIALING:
            *result = ICallState::DIALING;
            return NOERROR;
        case IConnection::STATE_DISCONNECTED:
            *result = ICallState::DISCONNECTED;
            return NOERROR;
        case IConnection::STATE_HOLDING:
            *result = ICallState::ON_HOLD;
            return NOERROR;
        case IConnection::STATE_NEW:
            *result = ICallState::NEW;
            return NOERROR;
        case IConnection::STATE_RINGING:
            *result = ICallState::RINGING;
            return NOERROR;
    }
    *result = ICallState::DISCONNECTED;
    return NOERROR;
}

ECode Call::GetCallSubstate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallSubstate;
    return NOERROR;
}

ECode Call::SetCallSubstate(
    /* [in] */ Int32 callSubstate)
{
    mCallSubstate = callSubstate;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallListener> l = ICallListener::Probe(obj);
        l->OnCallSubstateChanged(this);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
