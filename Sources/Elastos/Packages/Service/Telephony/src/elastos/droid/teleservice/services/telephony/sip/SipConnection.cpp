
#include "elastos/droid/teleservice/services/telephony/sip/SipConnection.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/DisconnectCauseUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/utility/Objects.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICallState_IDLE;
using Elastos::Droid::Internal::Telephony::ICallState_ACTIVE;
using Elastos::Droid::Internal::Telephony::ICallState_HOLDING;
using Elastos::Droid::Internal::Telephony::ICallState_DIALING;
using Elastos::Droid::Internal::Telephony::ICallState_ALERTING;
using Elastos::Droid::Internal::Telephony::ICallState_INCOMING;
using Elastos::Droid::Internal::Telephony::ICallState_WAITING;
using Elastos::Droid::Internal::Telephony::ICallState_DISCONNECTED;
using Elastos::Droid::Internal::Telephony::ICallState_DISCONNECTING;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

SipConnection::MyHandler::MyHandler(
    /* [in] */ SipConnection* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode SipConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_PRECISE_CALL_STATE_CHANGED:
            mHost->UpdateState(FALSE);
            break;
    }
    return NOERROR;
}

const String SipConnection::PREFIX("[SipConnection] ");
const Boolean SipConnection::VERBOSE = FALSE; /* STOP SHIP if true */

SipConnection::SipConnection()
{
    mHandler = new MyHandler(this);

    mOriginalConnectionState = ICallState_IDLE;

    if (VERBOSE) Log(String("new SipConnection"));
    SetInitializing();
}

ECode SipConnection::Initialize(
    /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection)
{
    if (VERBOSE) Log(String("init SipConnection, connection: ") + TO_CSTR(connection));
    mOriginalConnection = connection;

    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        IPhone::Probe(phone)->RegisterForPreciseCallStateChanged(mHandler, MSG_PRECISE_CALL_STATE_CHANGED, NULL);
    }
    UpdateAddress();
    SetInitialized();
    return NOERROR;
}

ECode SipConnection::OnAudioStateChanged(
    /* [in] */ IAudioState* state)
{
    if (VERBOSE) Log(String("onAudioStateChanged: ") + TO_CSTR(state));
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        IPhone::Probe(phone)->SetEchoSuppressionEnabled();
    }
    return NOERROR;
}

ECode SipConnection::OnStateChanged(
    /* [in] */ Int32 state)
{
    if (VERBOSE) {
        String str;
        Connection::StateToString(state, &str);
        Log(String("onStateChanged, state: ") + str);
    }
    return NOERROR;
}

ECode SipConnection::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    if (VERBOSE) Log(String("onPlayDtmfTone"));
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        IPhone::Probe(phone)->StartDtmf(c);
    }
    return NOERROR;
}

ECode SipConnection::OnStopDtmfTone()
{
    if (VERBOSE) Log(String("onStopDtmfTone"));
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        IPhone::Probe(phone)->StopDtmf();
    }
    return NOERROR;
}

ECode SipConnection::OnDisconnect()
{
    if (VERBOSE) Log(String("onDisconnect"));
    //try {
    ECode ec = NOERROR;
    AutoPtr<ICall> call = GetCall();
    Boolean res;
    if (call != NULL && (call->IsMultiparty(&res), !res)) {
        call->Hangup();
    }
    else if (mOriginalConnection != NULL) {
        ec = mOriginalConnection->Hangup();
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onDisconnect, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnSeparate()
{
    if (VERBOSE) Log(String("onSeparate"));
    //try {
    ECode ec = NOERROR;
    if (mOriginalConnection != NULL) {
        ec = mOriginalConnection->Separate();
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onSeparate, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnAbort()
{
    if (VERBOSE) Log(String("onAbort"));
    OnDisconnect();
    return NOERROR;
}

ECode SipConnection::OnHold()
{
    if (VERBOSE) Log(String("onHold"));
    //try {
    ECode ec = NOERROR;
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        Int32 state;
        GetState(&state);
        if (state == STATE_ACTIVE) {
            ec = IPhone::Probe(phone)->SwitchHoldingAndActive();
        }
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onHold, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnUnhold()
{
    if (VERBOSE) Log(String("onUnhold"));
    //try {
    ECode ec = NOERROR;
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        Int32 state;
        GetState(&state);
        if (state == STATE_HOLDING) {
            ec = IPhone::Probe(phone)->SwitchHoldingAndActive();
        }
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onUnhold, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnAnswer(
    /* [in] */ Int32 videoState)
{
    if (VERBOSE) Log(String("onAnswer"));
    //try {
    ECode ec = NOERROR;
    if (IsValidRingingCall()) {
        AutoPtr<ISipPhone> phone = GetPhone();
        if (phone != NULL) {
            ec = IPhone::Probe(phone)->AcceptCall(videoState);
        }
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onAnswer, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnReject()
{
    if (VERBOSE) Log(String("onReject"));
    //try {
    ECode ec = NOERROR;
    if (IsValidRingingCall()) {
        AutoPtr<ISipPhone> phone = GetPhone();
        if (phone != NULL) {
            ec = IPhone::Probe(phone)->RejectCall();
        }
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("onReject, exception: ") + StringUtils::ToString(ec));
    }
    return NOERROR;
}

ECode SipConnection::OnPostDialContinue(
    /* [in] */ Boolean proceed)
{
    if (VERBOSE) Log(String("onPostDialContinue, proceed: ") + StringUtils::ToString(proceed));
    // SIP doesn't have post dial support.
    return NOERROR;
}

AutoPtr<ICall> SipConnection::GetCall()
{
    if (mOriginalConnection != NULL) {
        AutoPtr<ICall> call;
        mOriginalConnection->GetCall((ICall**)&call);
        return call;
    }
    return NULL;
}

AutoPtr<ISipPhone> SipConnection::GetPhone()
{
    AutoPtr<ICall> call = GetCall();
    if (call != NULL) {
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        return ISipPhone::Probe(phone);
    }
    return NULL;
}

Boolean SipConnection::IsValidRingingCall()
{
    AutoPtr<ICall> call = GetCall();
    assert(0);
    // return call != NULL && call.getState().isRinging() &&
    //         call.getEarliestConnection() == mOriginalConnection;
    return FALSE;
}

void SipConnection::UpdateState(
    /* [in] */ Boolean force)
{
    if (mOriginalConnection == NULL) {
        return;
    }

    ICallState newState;
    mOriginalConnection->GetState(&newState);
    if (VERBOSE) {
        StringBuilder sb;
        sb += "updateState, ";
        sb += mOriginalConnectionState;
        sb += " -> ";
        sb += newState;
        Log(sb.ToString());
    }
    if (force || mOriginalConnectionState != newState) {
        mOriginalConnectionState = newState;
        switch (newState) {
            case ICallState_IDLE:
                break;
            case ICallState_ACTIVE:
                SetActive();
                break;
            case ICallState_HOLDING:
                SetOnHold();
                break;
            case ICallState_DIALING:
            case ICallState_ALERTING:
                SetDialing();
                // For SIP calls, we need to ask the framework to play the ringback for us.
                SetRingbackRequested(true);
                break;
            case ICallState_INCOMING:
            case ICallState_WAITING:
                SetRinging();
                break;
            case ICallState_DISCONNECTED:
            {
                Int32 cause;
                mOriginalConnection->GetDisconnectCause(&cause);
                AutoPtr<IDisconnectCause> dcause = DisconnectCauseUtil::ToTelecomDisconnectCause(cause);
                SetDisconnected(dcause);
                Close();
                break;
            }
            case ICallState_DISCONNECTING:
                break;
        }
        UpdateCallCapabilities(force);
    }
}

Int32 SipConnection::BuildCallCapabilities()
{
    Int32 capabilities = IPhoneCapabilities::MUTE | IPhoneCapabilities::SUPPORT_HOLD;
    Int32 state;
    GetState(&state);
    if (state == STATE_ACTIVE || state == STATE_HOLDING) {
        capabilities |= IPhoneCapabilities::HOLD;
    }
    return capabilities;
}

ECode SipConnection::UpdateCallCapabilities(
    /* [in] */ Boolean force)
{
    Int32 newCallCapabilities = BuildCallCapabilities();
    if (force) {
        return SetCallCapabilities(newCallCapabilities);
    }
    Int32 capabilities;
    GetCallCapabilities(&capabilities);
    if (capabilities != newCallCapabilities) {
        return SetCallCapabilities(newCallCapabilities);
    }
    return NOERROR;
}

ECode SipConnection::OnAddedToCallService()
{
    if (VERBOSE) Log(String("onAddedToCallService"));
    UpdateState(TRUE);
    UpdateCallCapabilities(TRUE);
    SetAudioModeIsVoip(TRUE);
    if (mOriginalConnection != NULL) {
        String name;
        mOriginalConnection->GetCnapName(&name);
        Int32 presentation;
        mOriginalConnection->GetCnapNamePresentation(&presentation);
        SetCallerDisplayName(name, presentation);
    }
    return NOERROR;
}

void SipConnection::UpdateAddress()
{
    if (mOriginalConnection != NULL) {
        String str;
        mOriginalConnection->GetAddress(&str);
        AutoPtr<IUri> address = GetAddressFromNumber(str);

        Int32 presentation;
        mOriginalConnection->GetNumberPresentation(&presentation);
        AutoPtr<IUri> _address;
        GetAddress((IUri**)&_address);
        if (!Objects::Equals(address, _address)) {
            Logger::V("SipConnection", "updateAddress, address changed");
            SetAddress(address, presentation);
        }
        else {
            Int32 aPresentation;
            GetAddressPresentation(&aPresentation);
            if (presentation != aPresentation) {
                Logger::V("SipConnection", "updateAddress, address changed");
                SetAddress(address, presentation);
            }
        }

        String name;
        mOriginalConnection->GetCnapName(&name);
        Int32 namePresentation;
        mOriginalConnection->GetCnapNamePresentation(&namePresentation);
        String displayName;
        GetCallerDisplayName(&displayName);
        if (!name.Equals(displayName)) {
            Logger::V("SipConnection", "updateAddress, caller display name changed");
            SetCallerDisplayName(name, namePresentation);
        }
        else {
            Int32 cpresentation;
            GetCallerDisplayNamePresentation(&cpresentation);
            if (namePresentation != cpresentation) {
                Logger::V("SipConnection", "updateAddress, caller display name changed");
                SetCallerDisplayName(name, namePresentation);
            }
        }
    }
}

AutoPtr<IUri> SipConnection::GetAddressFromNumber(
    /* [in] */ const String& number)
{
    // Address can be null for blocked calls.
    assert(0);
    // if (number.IsNull()) {
    //     number = "";
    // }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->FromParts(IPhoneAccount::SCHEME_SIP, number, String(NULL), (IUri**)&uri);
    return uri;
}

void SipConnection::Close()
{
    AutoPtr<ISipPhone> phone = GetPhone();
    if (phone != NULL) {
        IPhone::Probe(phone)->UnregisterForPreciseCallStateChanged(mHandler);
    }
    mOriginalConnection = NULL;
    Destroy();
}

void SipConnection::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos