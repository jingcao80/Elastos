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

#ifndef __ELASTOS_DROID_INCALLUI_CALL_H__
#define __ELASTOS_DROID_INCALLUI_CALL_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/InCallVideoCallListener.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecom::ICallListener;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::ICallDetails;
using Elastos::Droid::Telecom::IInCallServiceVideoCall;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IGatewayInfo;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call
    : public Object
    , public ICall
{
public:
    /* Defines different states of this call */
    class State
    {
    public:
        static CARAPI_(Boolean) IsConnectingOrConnected(
            /* [in] */ Int32 state);

        static CARAPI_(Boolean) IsDialing(
            /* [in] */ Int32 state);

        static CARAPI_(String) ToString(
            /* [in] */ Int32 state);

    public:
        static const Int32 INVALID = 0;
        static const Int32 IDLE = 1;           /* The call is idle.  Nothing active */
        static const Int32 ACTIVE = 2;         /* There is an active call */
        static const Int32 INCOMING = 3;       /* A normal incoming phone call */
        static const Int32 CALL_WAITING = 4;   /* Incoming call while another is active */
        static const Int32 DIALING = 5;        /* An outgoing call during dial phase */
        static const Int32 REDIALING = 6;      /* Subsequent dialing attempt after a failure */
        static const Int32 ONHOLD = 7;         /* An active phone call placed on hold */
        static const Int32 DISCONNECTING = 8;  /* A call is being ended. */
        static const Int32 DISCONNECTED = 9;   /* State after a call disconnects */
        static const Int32 CONFERENCED = 10;   /* Call part of a conference call */
        static const Int32 PRE_DIAL_WAIT = 11; /* Waiting for user before outgoing call */
        static const Int32 CONNECTING = 12;    /* Waiting for Telecomm broadcast to finish */
    };

    /**
     * Defines different states of session modify requests, which are used to upgrade to video, or
     * downgrade to audio.
     */
    class SessionModificationState
    {
    public:
        static const Int32 NO_REQUEST = 0;
        static const Int32 WAITING_FOR_RESPONSE = 1;
        static const Int32 REQUEST_FAILED = 2;
        static const Int32 RECEIVED_UPGRADE_TO_VIDEO_REQUEST = 3;
    };

private:
    class TelecommCallListener
        : public Object
        , public ICallListener
    {
    public:
        TelecommCallListener(
            /* [in] */ Call* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnStateChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ Int32 state);

        CARAPI OnParentChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ Elastos::Droid::Telecom::ICall* parent);

        CARAPI OnChildrenChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ IList* children);

        CARAPI OnDetailsChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ ICallDetails* details);

        CARAPI OnCannedTextResponsesLoaded(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ IList* cannedTextResponses);

        CARAPI OnPostDialWait(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ const String& remainingPostDialSequence);

        CARAPI OnVideoCallChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ IInCallServiceVideoCall* videoCall);

        CARAPI OnCallDestroyed(
            /* [in] */ Elastos::Droid::Telecom::ICall* call);

        CARAPI OnConferenceableCallsChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ IList* conferenceableCalls);

    private:
        Call* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Call(
        /* [in] */ Elastos::Droid::Telecom::ICall* telecommCall);

    CARAPI_(AutoPtr<Elastos::Droid::Telecom::ICall>) GetTelecommCall();

    CARAPI_(String) GetId();

    CARAPI_(String) GetNumber();

    CARAPI_(AutoPtr<IUri>) GetHandle();

    CARAPI_(Int32) GetState();

    CARAPI_(void) SetState(
        /* [in] */ Int32 state);

    CARAPI_(Int32) GetNumberPresentation();

    CARAPI_(Int32) GetCnapNamePresentation();

    CARAPI_(String) GetCnapName();

    /** Returns call disconnect cause, defined by {@link DisconnectCause}. */
    CARAPI_(AutoPtr<IDisconnectCause>) GetDisconnectCause();

    CARAPI_(void) SetDisconnectCause(
        /* [in] */ IDisconnectCause* disconnectCause);

    /** Returns the possible text message responses. */
    CARAPI_(AutoPtr<IList>) GetCannedSmsResponses();

    /** Checks if the call supports the given set of capabilities supplied as a bit mask. */
    CARAPI_(Boolean) Can(
        /* [in] */ Int32 capabilities);

    /** Gets the time when the call first became active. */
    CARAPI_(Int64) GetConnectTimeMillis();

    CARAPI_(Boolean) IsConferenceCall();

    CARAPI_(AutoPtr<IGatewayInfo>) GetGatewayInfo();

    CARAPI_(AutoPtr<IPhoneAccountHandle>) GetAccountHandle();

    CARAPI_(Int64) GetSubId();

    CARAPI_(AutoPtr<IInCallServiceVideoCall>) GetVideoCall();

    CARAPI_(List<String>&) GetChildCallIds();

    CARAPI_(String) GetParentId();

    CARAPI_(Int32) GetVideoState();

    CARAPI_(Boolean) IsVideoCall(
        /* [in] */ IContext* context);

    CARAPI_(void) SetSessionModificationState(
        /* [in] */ Int32 state);

    static CARAPI_(Boolean) AreSame(
        /* [in] */ Call* call1,
        /* [in] */ Call* call2);

    CARAPI_(Int32) GetSessionModificationState();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) Update();

    CARAPI_(void) UpdateFromTelecommCall();

    static CARAPI_(Int32) TranslateState(
        /* [in] */ Int32 state);

    CARAPI_(Boolean) HasProperty(
        /* [in] */ Int32 property);

public:
    Boolean mIsActiveSub;

private:
    static const String ID_PREFIX;
    static Int32 sIdCounter;

    AutoPtr<ICallListener> mTelecommCallListener;

    AutoPtr<Elastos::Droid::Telecom::ICall> mTelecommCall;
    String mId;
    Int32 mState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    Int32 mSessionModificationState;
    List<String> mChildCallIds;
    Boolean mIsOutgoing;

    AutoPtr<InCallVideoCallListener> mVideoCallListener;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALL_H__
