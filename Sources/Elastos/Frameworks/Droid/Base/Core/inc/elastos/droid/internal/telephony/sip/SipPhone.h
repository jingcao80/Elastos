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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneBase.h"
#include "elastos/droid/internal/telephony/sip/SipCallBase.h"
#include "elastos/droid/internal/telephony/sip/SipConnectionBase.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IConnection;
//TODO using Elastos::Droid::Net::Sip::ISipAudioCall;
//TODO using Elastos::Droid::Net::Rtp::IAudioGroup;
//TODO using Elastos::Droid::Net::Sip::ISipProfile;
//TODO using Elastos::Droid::Net::Sip::ISipManager;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

/**
  * {@hide}
  */
class SipPhone
    : public SipPhoneBase
    , public ISipPhone
{
private:
    class SipConnection;
    class SipCall
        : public SipCallBase
    {
        friend class SipPhone;
    public:
        SipCall(
            /* [in] */ SipPhone* host)
            : mHost(host)
        {}

        // STOPSHIP if true
        virtual CARAPI Reset();

        virtual CARAPI SwitchWith(
            /* [in] */ SipCall* that);

        // @Override
        CARAPI GetPhone(
            /* [out] */ IPhone** result);

        // @Override
        CARAPI GetConnections(
            /* [out] */ IList/*<Connection>*/** result);

        virtual CARAPI Dial(
            /* [in] */ const String& originalNumber,
            /* [out] */ IConnection** result);

        // @Override
        CARAPI Hangup();

        virtual CARAPI InitIncomingCall(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
            /* [in] */ Boolean makeCallWait,
            /* [out] */ SipConnection** result);

        virtual CARAPI RejectCall();

        virtual CARAPI AcceptCall();

        virtual CARAPI SetAudioGroupMode();

        virtual CARAPI Hold();

        virtual CARAPI Unhold();

        virtual CARAPI SetMute(
            /* [in] */ Boolean muted);

        virtual CARAPI GetMute(
            /* [out] */ Boolean* result);

        virtual CARAPI Merge(
            /* [in] */ SipCall* that);

        virtual CARAPI SendDtmf(
            /* [in] */ Char16 c);

        virtual CARAPI OnConnectionStateChanged(
            /* [in] */ SipConnection* conn);

        virtual CARAPI OnConnectionEnded(
            /* [in] */ SipConnection* conn);

    protected:
        // @Override
        CARAPI_(void) SetState(
            /* [in] */ ICallState newState);

    private:
        CARAPI_(void) TakeOver(
            /* [in] */ SipCall* that);

        CARAPI_(Boolean) IsSpeakerOn();

        CARAPI_(void) Add(
            /* [in] */ SipConnection* conn);

        CARAPI ConvertDtmf(
            /* [in] */ Char16 c,
            /* [out] */ Int32* result);

        CARAPI_(AutoPtr</*TODO IAudioGroup*/IInterface>) GetAudioGroup();

        CARAPI_(void) Log(
            /* [in] */ const String& s);

    private:
        static const String SC_TAG;
        static const Boolean SC_DBG;
        static const Boolean SC_VDBG;

        SipPhone* mHost;
    };

    class SipAudioCallAdapter
        : public Object
        //TODO , public ISipAudioCallListener
    {
        friend class SipConnection;
    public:
        // @Override
        CARAPI OnCallEnded(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call);

        // @Override
        CARAPI OnCallBusy(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call);

        // @Override
        CARAPI OnError(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call,
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& errorMessage);

    protected:
        /** Call ended with cause defined in {@link DisconnectCause}. */
        virtual CARAPI OnCallEnded(
            /* [in] */ Int32 cause) = 0;

        /** Call failed with cause defined in {@link DisconnectCause}. */
        virtual CARAPI OnError(
            /* [in] */ Int32 cause) = 0;

    private:
        CARAPI_(void) Log(
            /* [in] */ const String& s);

    private:
        static const String SACA_TAG;
        static const Boolean SACA_DBG;
    };

    class InnerSipAudioCallAdapter;
    class SipConnection
        : public SipConnectionBase
    {
        friend class InnerSipAudioCallAdapter;
    public:
        SipConnection(
            /* [in] */ SipPhone* host,
            /* [in] */ SipCall* owner,
            /* [in] */ /*TODO ISipProfile*/IInterface* callee,
            /* [in] */ const String& originalNumber);

        SipConnection(
            /* [in] */ SipPhone* host,
            /* [in] */ SipCall* owner,
            /* [in] */ /*TODO ISipProfile*/IInterface* callee);

        // @Override
        CARAPI GetCnapName(
            /* [out] */ String* result);

        // @Override
        CARAPI GetNumberPresentation(
            /* [out] */ Int32* result);

        virtual CARAPI InitIncomingCall(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
            /* [in] */ ICallState newState);

        virtual CARAPI AcceptCall();

        virtual CARAPI ChangeOwner(
            /* [in] */ SipCall* owner);

        virtual CARAPI GetAudioGroup(
            /* [out] */ /*TODO IAudioGroup*/IInterface** result);

        virtual CARAPI Dial();

        virtual CARAPI Hold();

        virtual CARAPI Unhold(
            /* [in] */ /*TODO IAudioGroup*/IInterface* audioGroup);

        virtual CARAPI SetMute(
            /* [in] */ Boolean muted);

        virtual CARAPI GetMute(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetState(
            /* [out] */ ICallState* result);

        // @Override
        CARAPI IsIncoming(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetAddress(
            /* [out] */ String* result);

        // @Override
        CARAPI GetCall(
            /* [out] */ ICall** result);

        // @Override
        CARAPI Hangup();

        // @Override
        CARAPI Separate();

    protected:
        // @Override
        CARAPI_(void) SetState(
            /* [in] */ ICallState state);

        // @Override
        CARAPI_(AutoPtr<IPhone>) GetPhone();

    private:
        CARAPI_(void) Log(
            /* [in] */ const String& s);

    private:
        static const String SCN_TAG;
        static const Boolean SCN_DBG;
        AutoPtr<SipCall> mOwner;
        AutoPtr</*TODO ISipAudioCall*/IInterface> mSipAudioCall;
        ICallState mState;
        AutoPtr</*TODO ISipProfile*/IInterface> mPeer;
        Boolean mIncoming;
        String mOriginalNumber;
        // may be a PSTN number
        AutoPtr<SipAudioCallAdapter> mAdapter;
        SipPhone* mHost;
    };

    class InnerSipAudioCallAdapter
        : public SipAudioCallAdapter
    {
    public:
        InnerSipAudioCallAdapter(
            /* [in] */ SipConnection* owner);

        // @Override
        CARAPI OnCallEstablished(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call);

        // @Override
        CARAPI OnCallHeld(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call);

        // @Override
        CARAPI OnChanged(
            /* [in] */ /*TODO ISipAudioCall*/IInterface* call);

    protected:
        // @Override
        CARAPI OnCallEnded(
            /* [in] */ Int32 cause);

        // @Override
        CARAPI OnError(
            /* [in] */ Int32 cause);

    private:
        SipConnection* mOwner;
    };

public:
    TO_STRING_IMPL("SipPhone")

    CAR_INTERFACE_DECL();

    SipPhone();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ /*TODO ISipProfile*/IInterface* profile);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    virtual CARAPI GetSipUri(
        /* [out] */ String* result);

    virtual CARAPI Equals(
        /* [in] */ ISipPhone* phone,
        /* [out] */ Boolean* result);

    virtual CARAPI TakeIncomingCall(
        /* [in] */ IInterface* incomingCall,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    // @Override
    CARAPI RejectCall();

    using SipPhoneBase::Dial;

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI SwitchHoldingAndActive();

    // @Override
    CARAPI CanConference(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Conference();

    virtual CARAPI Conference(
        /* [in] */ ICall* that);

    // @Override
    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ExplicitCallTransfer();

    // @Override
    CARAPI ClearDisconnected();

    // @Override
    CARAPI SendDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StartDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StopDtmf();

    using SipPhoneBase::SendBurstDtmf;

    virtual CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString);

    // @Override
    CARAPI GetOutgoingCallerIdDisplay(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetOutgoingCallerIdDisplay(
        /* [in] */ Int32 commandInterfaceCLIRMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetEchoSuppressionEnabled();

    // @Override
    CARAPI SetMute(
        /* [in] */ Boolean muted);

    // @Override
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetBackgroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

private:
    using SipPhoneBase::DialInternal;

    CARAPI DialInternal(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    CARAPI_(String) GetUriString(
        /* [in] */ /*TODO ISipProfile*/IInterface* p);

    CARAPI_(String) GetSipDomain(
        /* [in] */ /*TODO ISipProfile*/IInterface* p);

    static CARAPI_(ICallState) GetCallStateFrom(
        /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    static CARAPI_(void) Slog(
        /* [in] */ const String& s);

    CARAPI_(void) Loge(
        /* [in] */ const String& s);

    //CARAPI_(void) Loge(
    //    /* [in] */ const String& s,
    //    /* [in] */ Exception* e);

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    // STOPSHIP if true
    static const Int32 TIMEOUT_MAKE_CALL = 15;
    // in seconds
    static const Int32 TIMEOUT_ANSWER_CALL = 8;
    // in seconds
    static const Int32 TIMEOUT_HOLD_CALL = 15;
    // in seconds
    // A call that is ringing or (call) waiting
    AutoPtr<SipCall> mRingingCall;
    AutoPtr<SipCall> mForegroundCall;
    AutoPtr<SipCall> mBackgroundCall;
    AutoPtr</*TODO ISipManager*/IInterface> mSipManager;
    AutoPtr</*TODO ISipProfile*/IInterface> mProfile;
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPPHONE_H__

