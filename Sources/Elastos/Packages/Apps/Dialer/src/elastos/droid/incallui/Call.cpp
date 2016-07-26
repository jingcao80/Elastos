
#include <elastos/droid/contacts/common/CallUtil.h>
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CallList.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::InCallUI::EIID_ICall;
using Elastos::Droid::Telecom::EIID_ICallListener;
using Elastos::Droid::Telecom::ICallDetails;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::CPhoneCapabilities;
using Elastos::Droid::Telecom::ICallProperties;
using Elastos::Droid::Telecom::IVideoProfileVideoStateHelper;
using Elastos::Droid::Telecom::CVideoProfileVideoStateHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// Call::State
//================================================================
const Int32 Call::State::INVALID;
const Int32 Call::State::IDLE;
const Int32 Call::State::ACTIVE;
const Int32 Call::State::INCOMING;
const Int32 Call::State::CALL_WAITING;
const Int32 Call::State::DIALING;
const Int32 Call::State::REDIALING;
const Int32 Call::State::ONHOLD;
const Int32 Call::State::DISCONNECTING;
const Int32 Call::State::DISCONNECTED;
const Int32 Call::State::CONFERENCED;
const Int32 Call::State::PRE_DIAL_WAIT;
const Int32 Call::State::CONNECTING;

Boolean Call::State::IsConnectingOrConnected(
    /* [in] */ Int32 state)
{
    switch (state) {
        case ACTIVE:
        case INCOMING:
        case CALL_WAITING:
        case CONNECTING:
        case DIALING:
        case REDIALING:
        case ONHOLD:
        case CONFERENCED:
            return TRUE;
        default:
            return FALSE;
    }
}

Boolean Call::State::IsDialing(
    /* [in] */ Int32 state)
{
    return state == DIALING || state == REDIALING;
}

String Call::State::ToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case INVALID:
            return String("INVALID");
        case IDLE:
            return String("IDLE");
        case ACTIVE:
            return String("ACTIVE");
        case INCOMING:
            return String("INCOMING");
        case CALL_WAITING:
            return String("CALL_WAITING");
        case DIALING:
            return String("DIALING");
        case REDIALING:
            return String("REDIALING");
        case ONHOLD:
            return String("ONHOLD");
        case DISCONNECTING:
            return String("DISCONNECTING");
        case DISCONNECTED:
            return String("DISCONNECTED");
        case CONFERENCED:
            return String("CONFERENCED");
        case PRE_DIAL_WAIT:
            return String("PRE_DIAL_WAIT");
        case CONNECTING:
            return String("CONNECTING");
        default:
            return String("UNKNOWN");
    }
}


//================================================================
// Call::SessionModificationState
//================================================================
const Int32 Call::SessionModificationState::NO_REQUEST;
const Int32 Call::SessionModificationState::WAITING_FOR_RESPONSE;
const Int32 Call::SessionModificationState::REQUEST_FAILED;
const Int32 Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST;


//================================================================
// Call::TelecommCallListener
//================================================================
CAR_INTERFACE_IMPL(Call::TelecommCallListener, Object, ICallListener)

ECode Call::TelecommCallListener::OnStateChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ Int32 state)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnParentChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ Elastos::Droid::Telecom::ICall* parent)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnChildrenChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ IList* children)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnDetailsChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ ICallDetails* details)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnCannedTextResponsesLoaded(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ IList* cannedTextResponses)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnPostDialWait(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ const String& remainingPostDialSequence)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnVideoCallChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ IInCallServiceVideoCall* videoCall)
{
    mHost->Update();
    return NOERROR;
}

ECode Call::TelecommCallListener::OnCallDestroyed(
    /* [in] */ Elastos::Droid::Telecom::ICall* call)
{
    return call->RemoveListener(mHost->mTelecommCallListener);
}

ECode Call::TelecommCallListener::OnConferenceableCallsChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ IList* conferenceableCalls)
{
    mHost->Update();
    return NOERROR;
}


//================================================================
// Call
//================================================================
const String Call::ID_PREFIX("Call_");
Int32 Call::sIdCounter = 0;

CAR_INTERFACE_IMPL(Call, Object, ICall)

Call::Call(
    /* [in] */ Elastos::Droid::Telecom::ICall* telecommCall)
    : mIsActiveSub(FALSE)
    , mTelecommCall(telecommCall)
    , mState(State::INVALID)
    , mSessionModificationState(0)
    , mIsOutgoing(FALSE)
{
    mTelecommCallListener = (ICallListener*)new TelecommCallListener(this);

    mId = ID_PREFIX + StringUtils::ToString(sIdCounter++);
    UpdateFromTelecommCall();
    mTelecommCall->AddListener(mTelecommCallListener);
}

AutoPtr<Elastos::Droid::Telecom::ICall> Call::GetTelecommCall()
{
    return mTelecommCall;
}

void Call::Update()
{
    Int32 oldState = GetState();
    UpdateFromTelecommCall();
    if (oldState != GetState() && GetState() == State::DISCONNECTED) {
        CallList::GetInstance()->OnDisconnect(this);
    }
    else {
        CallList::GetInstance()->OnUpdate(this);
    }
}

void Call::UpdateFromTelecommCall()
{
    Logger::D("Call", "updateFromTelecommCall: %s", TO_CSTR(mTelecommCall));
    Int32 state;
    mTelecommCall->GetState(&state);
    SetState(TranslateState(state));
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IDisconnectCause> cause;
    details->GetDisconnectCause((IDisconnectCause**)&cause);
    SetDisconnectCause(cause);

    AutoPtr<IInCallServiceVideoCall> videoCall;
    if (mTelecommCall->GetVideoCall((IInCallServiceVideoCall**)&videoCall), videoCall != NULL) {
        assert(0);
        // if (mVideoCallListener == NULL) {
        //     mVideoCallListener = new InCallVideoCallListener(this);
        // }
        // videoCall->SetVideoCallListener((IVideoCallListener*)mVideoCallListener.Get());
    }

    mChildCallIds.Clear();
    AutoPtr<IList> children;
    mTelecommCall->GetChildren((IList**)&children);
    Int32 size;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        mChildCallIds.PushBack(
                CallList::GetInstance()->GetCallByTelecommCall(
                        Elastos::Droid::Telecom::ICall::Probe(value))->GetId());
    }
}

Int32 Call::TranslateState(
    /* [in] */ Int32 state)
{
    switch (state) {
        case Elastos::Droid::Telecom::ICall::STATE_CONNECTING:
            return Call::State::CONNECTING;
        case Elastos::Droid::Telecom::ICall::STATE_PRE_DIAL_WAIT:
            return Call::State::PRE_DIAL_WAIT;
        case Elastos::Droid::Telecom::ICall::STATE_DIALING:
        case Elastos::Droid::Telecom::ICall::STATE_NEW:
            return Call::State::DIALING;
        case Elastos::Droid::Telecom::ICall::STATE_RINGING:
            return Call::State::INCOMING;
        case Elastos::Droid::Telecom::ICall::STATE_ACTIVE:
            return Call::State::ACTIVE;
        case Elastos::Droid::Telecom::ICall::STATE_HOLDING:
            return Call::State::ONHOLD;
        case Elastos::Droid::Telecom::ICall::STATE_DISCONNECTED:
            return Call::State::DISCONNECTED;
        case Elastos::Droid::Telecom::ICall::STATE_DISCONNECTING:
            return Call::State::DISCONNECTING;
        default:
            return Call::State::INVALID;
    }
}

String Call::GetId()
{
    return mId;
}

String Call::GetNumber()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IGatewayInfo> info;
    if (details->GetGatewayInfo((IGatewayInfo**)&info), info != NULL) {
        AutoPtr<IUri> address;
        info->GetOriginalAddress((IUri**)&address);
        String part;
        address->GetSchemeSpecificPart(&part);
        return part;
    }
    String part;
    AutoPtr<IUri> handle = GetHandle();
    if (handle != NULL) {
        handle->GetSchemeSpecificPart(&part);
    }
    return part;
}

AutoPtr<IUri> Call::GetHandle()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IUri> handle;
    details->GetHandle((IUri**)&handle);
    return handle;
}

Int32 Call::GetState()
{
    AutoPtr<Elastos::Droid::Telecom::ICall> parent;
    if (mTelecommCall->GetParent((Elastos::Droid::Telecom::ICall**)&parent), parent != NULL) {
        return State::CONFERENCED;
    }
    else {
        return mState;
    }
}

void Call::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
}

Int32 Call::GetNumberPresentation()
{
    AutoPtr<ICallDetails> details;
    GetTelecommCall()->GetDetails((ICallDetails**)&details);
    Int32 result;
    details->GetHandlePresentation(&result);
    return result;
}

Int32 Call::GetCnapNamePresentation()
{
    AutoPtr<ICallDetails> details;
    GetTelecommCall()->GetDetails((ICallDetails**)&details);
    Int32 result;
    details->GetCallerDisplayNamePresentation(&result);
    return result;
}

String Call::GetCnapName()
{
    AutoPtr<ICallDetails> details;
    GetTelecommCall()->GetDetails((ICallDetails**)&details);
    String name;
    details->GetCallerDisplayName(&name);
    return name;
}

AutoPtr<IDisconnectCause> Call::GetDisconnectCause()
{
    if (mState == State::DISCONNECTED || mState == State::IDLE) {
        return mDisconnectCause;
    }

    AutoPtr<IDisconnectCause> cause;
    CDisconnectCause::New(IDisconnectCause::UNKNOWN, (IDisconnectCause**)&cause);
    return cause;
}

void Call::SetDisconnectCause(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mDisconnectCause = disconnectCause;
}

AutoPtr<IList> Call::GetCannedSmsResponses()
{
    AutoPtr<IList> responses;
    mTelecommCall->GetCannedTextResponses((IList**)&responses);
    return responses;
}

Boolean Call::Can(
    /* [in] */ Int32 capabilities)
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    Int32 supportedCapabilities;
    details->GetCallCapabilities(&supportedCapabilities);

    if ((capabilities & IPhoneCapabilities::MERGE_CONFERENCE) != 0) {
        // We allow you to merge if the capabilities allow it or if it is a call with
        // conferenceable calls.
        AutoPtr<IList> calls;
        mTelecommCall->GetConferenceableCalls((IList**)&calls);
        Boolean isEmpty;
        if ((calls->IsEmpty(&isEmpty), isEmpty) &&
                ((IPhoneCapabilities::MERGE_CONFERENCE & supportedCapabilities) == 0)) {
            // Cannot merge calls if there are no calls to merge with.
            return FALSE;
        }
        capabilities &= ~IPhoneCapabilities::MERGE_CONFERENCE;
    }
    Int32 callCapabilities;
    return (capabilities == (capabilities & (details->GetCallCapabilities(&callCapabilities), callCapabilities)));
}

Boolean Call::HasProperty(
    /* [in] */ Int32 property)
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    Int32 callCapabilities;
    return property == (property & (details->GetCallCapabilities(&callCapabilities), callCapabilities));
}

Int64 Call::GetConnectTimeMillis()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    Int64 timeMillis;
    details->GetConnectTimeMillis(&timeMillis);
    return timeMillis;
}

Boolean Call::IsConferenceCall()
{
    return HasProperty(ICallProperties::CONFERENCE);
}

AutoPtr<IGatewayInfo> Call::GetGatewayInfo()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IGatewayInfo> info;
    details->GetGatewayInfo((IGatewayInfo**)&info);
    return info;
}

AutoPtr<IPhoneAccountHandle> Call::GetAccountHandle()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IPhoneAccountHandle> handle;
    details->GetAccountHandle((IPhoneAccountHandle**)&handle);
    return handle;
}

AutoPtr<IInCallServiceVideoCall> Call::GetVideoCall()
{
    AutoPtr<IInCallServiceVideoCall> call;
    mTelecommCall->GetVideoCall((IInCallServiceVideoCall**)&call);
    return call;
}

List<String>& Call::GetChildCallIds()
{
    return mChildCallIds;
}

String Call::GetParentId()
{
    AutoPtr<Elastos::Droid::Telecom::ICall> parentCall;
    mTelecommCall->GetParent((Elastos::Droid::Telecom::ICall**)&parentCall);
    if (parentCall != NULL) {
        return CallList::GetInstance()->GetCallByTelecommCall(parentCall)->GetId();
    }
    return String(NULL);
}

Int32 Call::GetVideoState()
{
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    Int32 state;
    details->GetVideoState(&state);
    return state;
}

Boolean Call::IsVideoCall(
    /* [in] */ IContext* context)
{
    if (!CallUtil::IsVideoEnabled(context)) {
        return FALSE;
    }
    else {
        AutoPtr<IVideoProfileVideoStateHelper> helper;
        CVideoProfileVideoStateHelper::AcquireSingleton((IVideoProfileVideoStateHelper**)&helper);
        Boolean result;
        helper->IsBidirectional(GetVideoState(), &result);
        return result;
    }
}

void Call::SetSessionModificationState(
    /* [in] */ Int32 state)
{
    Boolean hasChanged = mSessionModificationState != state;
    mSessionModificationState = state;

    if (hasChanged) {
        Update();
    }
}

Boolean Call::AreSame(
    /* [in] */ Call* call1,
    /* [in] */ Call* call2)
{
    if (call1 == NULL && call2 == NULL) {
        return TRUE;
    }
    else if (call1 == NULL || call2 == NULL) {
        return FALSE;
    }

    // otherwise compare call Ids
    return call1->GetId().Equals(call2->GetId());
}

Int32 Call::GetSessionModificationState()
{
    return mSessionModificationState;
}

ECode Call::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ICallDetails> details;
    mTelecommCall->GetDetails((ICallDetails**)&details);
    Int32 caps;
    details->GetCallCapabilities(&caps);
    AutoPtr<IPhoneCapabilities> capabilities;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&capabilities);
    String s;
    capabilities->ToString(caps, &s);
    Int32 state;
    details->GetVideoState(&state);
    *str = String("[") + mId + "," + State::ToString(GetState()) + "," + s
            + ", children:" + ", parent:" + GetParentId() + ", videoState:" + StringUtils::ToString(state);
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
