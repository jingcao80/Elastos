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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALLTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALLTRACKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/CallTracker.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
//TODO using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Ims::IImsReasonInfo;
//TODO using Elastos::Droid::Ims::IImsUtInterface;
//TODO using Elastos::Droid::Ims::IImsEcbm;
//TODO using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::CallTracker;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::ICallSrvccState;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::Thread;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * {@hide}
  */
class ImsPhoneCallTracker
    : public CallTracker
    , public IImsPhoneCallTracker
{
private:
    class InnerBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ImsPhoneCallTracker::InnerBroadcastReceiver")

        InnerBroadcastReceiver(
            /* [in] */ ImsPhoneCallTracker* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ImsPhoneCallTracker* mOwner;
    };

    class InnerImsCallListener
        : public Object//TODO ImsCallListener
    {
    public:
        InnerImsCallListener(
            /* [in] */ ImsPhoneCallTracker* owner);

        // @Override
        CARAPI OnCallProgressing(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallStarted(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallUpdated(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        /**
          * onCallStartFailed will be invoked when:
          * case 1) Dialing fails
          * case 2) Ringing call is disconnected by local or remote user
          */
        // @Override
        CARAPI OnCallStartFailed(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallTerminated(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallHeld(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallHoldFailed(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallResumed(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallResumeFailed(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallResumeReceived(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallHoldReceived(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallMerged(
            /* [in] */ /*TODO IImsCall*/IInterface* call);

        // @Override
        CARAPI OnCallMergeFailed(
            /* [in] */ /*TODO IImsCall*/IInterface* call,
            /* [in] */ IImsReasonInfo* reasonInfo);

        /**
          * Called when the state of IMS conference participant(s) has changed.
          *
          * @param call the call object that carries out the IMS call.
          * @param participants the participant(s) and their new state information.
          */
        // @Override
        CARAPI OnConferenceParticipantsStateChanged(
            /* [in] */ /*TODO IImsCall*/IInterface* call,
            /* [in] */ IList/*<IConferenceParticipant>*/* participants);

    private:
        ImsPhoneCallTracker* mOwner;
    };

    class InnerImsCallListener1
        : public Object//TODO  ImsCallListener
    {
    public:
        InnerImsCallListener1(
            /* [in] */ ImsPhoneCallTracker* owner);

        // @Override
        CARAPI OnCallStarted(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

        // @Override
        CARAPI OnCallStartFailed(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallTerminated(
            /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
            /* [in] */ IImsReasonInfo* reasonInfo);

        // @Override
        CARAPI OnCallUssdMessageReceived(
            /* [in] */ /*TODO IImsCall*/IInterface* call,
            /* [in] */ Int32 mode,
            /* [in] */ const String& ussdMessage);

    private:
        ImsPhoneCallTracker* mOwner;
    };

    class InnerImsConnectionStateListener
        : public Object
        //TODO , public IImsConnectionStateListener
    {
    public:
        InnerImsConnectionStateListener(
            /* [in] */ ImsPhoneCallTracker* owner);

        // @Override
        CARAPI OnImsConnected();

        // @Override
        CARAPI OnImsDisconnected();

        // @Override
        CARAPI OnImsResumed();

        // @Override
        CARAPI OnImsSuspended();

        // @Override
        CARAPI OnFeatureCapabilityChanged(
            /* [in] */ Int32 serviceClass,
            /* [in] */ ArrayOf<Int32>* enabledFeatures,
            /* [in] */ ArrayOf<Int32>* disabledFeatures);

        // @Override
        CARAPI OnVoiceMessageCountChanged(
            /* [in] */ Int32 count);

    private:
        ImsPhoneCallTracker* mOwner;
    };

    class InnerThread
         : public Thread
    {
    public:
        InnerThread(
            /* [in] */ ImsPhoneCallTracker* host)
            : mOwner(host)
        {
            constructor(String("InnerThread"));
        }

        CARAPI Run();

    private:
        ImsPhoneCallTracker* mOwner;
    };

public:
    TO_STRING_IMPL("ImsPhoneCallTracker")

    CAR_INTERFACE_DECL();

    ImsPhoneCallTracker();

    virtual ~ImsPhoneCallTracker();

    //***** Events
    //***** Constructors
    CARAPI constructor(
        /* [in] */ IImsPhone* phone);

    virtual CARAPI Dispose();

    //***** Instance Methods
    //***** Public Methods
    // @Override
    CARAPI RegisterForVoiceCallStarted(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForVoiceCallStarted(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForVoiceCallEnded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForVoiceCallEnded(
        /* [in] */ IHandler* h);

    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    /**
      * oirMode is one of the CLIR_ constants
      */
    // synchronized
    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 clirMode,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    virtual CARAPI AddParticipant(
        /* [in] */ const String& dialString);

    /**
      * Accepts a call with the specified video state.  The video state is the video state that the
      * user has agreed upon in the InCall UI.
      *
      * @param videoState The video State
      * @throws CallStateException
      */
    virtual CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    virtual CARAPI DeflectCall(
        /* [in] */ const String& number);

    virtual CARAPI RejectCall();

    virtual CARAPI SwitchWaitingOrHoldingAndActive();

    virtual CARAPI Conference();

    virtual CARAPI ExplicitCallTransfer();

    virtual CARAPI ClearDisconnected();

    virtual CARAPI CanConference(
        /* [out] */ Boolean* result);

    virtual CARAPI CanDial(
        /* [out] */ Boolean* result);

    virtual CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    //***** Called from ImsPhone
    virtual CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    /*package*/
    virtual CARAPI SetMute(
        /* [in] */ Boolean mute);

    /*package*/
    virtual CARAPI GetMute(
        /* [out] */ Boolean* result);

    /*package*/
    virtual CARAPI SendDtmf(
        /* [in] */ Char16 c);

    /*package*/
    virtual CARAPI StartDtmf(
        /* [in] */ Char16 c);

    /*package*/
    virtual CARAPI StopDtmf();

    //***** Called from ImsPhoneConnection
    /*package*/
    virtual CARAPI Hangup(
        /* [in] */ IImsPhoneConnection* conn);

    //***** Called from ImsPhoneCall
    /* package */
    virtual CARAPI Hangup(
        /* [in] */ IImsPhoneCall* call);

    /* package */
    virtual CARAPI ResumeWaitingOrHolding();

    /* package */
    virtual CARAPI SendUSSD(
        /* [in] */ const String& ussdString,
        /* [in] */ IMessage* response);

    /* package */
    virtual CARAPI CancelUSSD();

    /* package */
    virtual CARAPI GetUtInterface(
        /* [out] */ /*TODO IImsUtInterface*/IInterface** result);

    using CallTracker::NotifySrvccState;

    /* package */
    virtual CARAPI NotifySrvccState(
        /* [in] */ ICallSrvccState state);

    //****** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    /* package */
    virtual CARAPI GetEcbmInterface(
        /* [out] */ /*TODO IImsEcbm*/IInterface** result);

    virtual CARAPI IsInEmergencyCall(
        /* [out] */ Boolean* result);

    virtual CARAPI IsVolteEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI IsVtEnabled(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI Finalize();

    // @Override
    CARAPI_(void) Log(
        /* [in] */ const String& msg);

    virtual CARAPI_(void) Loge(
        /* [in] */ const String& msg);

    // @Override
    CARAPI_(void) HandlePollCalls(
        /* [in] */ AsyncResult* ar);

private:
    CARAPI_(AutoPtr<IPendingIntent>) CreateIncomingCallPendingIntent();

    CARAPI_(void) GetImsService();

    CARAPI_(void) HandleEcmTimer(
        /* [in] */ Int32 action);

    CARAPI_(void) DialInternal(
        /* [in] */ IImsPhoneConnection* conn,
        /* [in] */ Int32 clirMode,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras);

    //***** Private Instance Methods
    CARAPI_(void) InternalClearDisconnected();

    CARAPI_(void) UpdatePhoneState();

    CARAPI_(void) HandleRadioNotAvailable();

    CARAPI_(void) DumpState();

    // synchronized
    CARAPI_(AutoPtr<IImsPhoneConnection>) FindConnection(
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

    // synchronized
    CARAPI_(void) RemoveConnection(
        /* [in] */ IImsPhoneConnection* conn);

    // synchronized
    CARAPI_(void) AddConnection(
        /* [in] */ IImsPhoneConnection* conn);

    CARAPI_(void) ProcessCallStateChange(
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
        /* [in] */ ICallState state,
        /* [in] */ Int32 cause);

    CARAPI_(Int32) GetDisconnectCauseFromReasonInfo(
        /* [in] */ IImsReasonInfo* reasonInfo);

    CARAPI_(void) TransferHandoverConnections(
        /* [in] */ IImsPhoneCall* call);

public:
    static const String LOGTAG;
    //***** Constants
    static const Int32 MAX_CONNECTIONS = 7;
    static const Int32 MAX_CONNECTIONS_PER_CALL = 5;
    AutoPtr<IImsPhoneCall> mRingingCall;
    AutoPtr<IImsPhoneCall> mForegroundCall;
    AutoPtr<IImsPhoneCall> mBackgroundCall;
    AutoPtr<IImsPhoneCall> mHandoverCall;
    AutoPtr<IImsPhone> mPhone;
    PhoneConstantsState mState;

private:
    static const Boolean DBG;
    Boolean mIsVolteEnabled;
    Boolean mIsVtEnabled;
    AutoPtr<InnerBroadcastReceiver> mReceiver;
    static const Int32 EVENT_HANGUP_PENDINGMO = 18;
    static const Int32 EVENT_RESUME_BACKGROUND = 19;
    static const Int32 EVENT_DIAL_PENDINGMO = 20;
    static const Int32 TIMEOUT_HANGUP_PENDINGMO = 500;
    //***** Instance Variables
    AutoPtr<IArrayList> mConnections;/*<ImsPhoneConnection>*/
    AutoPtr<RegistrantList> mVoiceCallEndedRegistrants;
    AutoPtr<RegistrantList> mVoiceCallStartedRegistrants;
    AutoPtr<IImsPhoneConnection> mPendingMO;
    Int32 mClirMode;
    Object mSyncHold;
    AutoPtr</*TODO IImsCall*/IInterface> mUssdSession;
    AutoPtr<IMessage> mPendingUssd;
    Boolean mDesiredMute;
    // false = mute off
    Boolean mOnHoldToneStarted;
    AutoPtr</*TODO IImsManager*/IInterface> mImsManager;
    Int32 mServiceId;
    ICallSrvccState mSrvccState;
    Boolean mIsInEmergencyCall;
    Int32 mPendingCallClirMode;
    Int32 mPendingCallVideoState;
    Boolean mPendingCallInEcm;
    /**
      * Listen to the IMS call state change
      */
    AutoPtr</*TODO ImsCallListener*/IInterface> mImsCallListener;
    /**
      * Listen to the IMS call state change
      */
    AutoPtr</*TODO ImsCallListener*/IInterface> mImsUssdListener;
    /**
      * Listen to the IMS service state change
      *
      */
    AutoPtr</*TODO IImsConnectionStateListener*/IInterface> mImsConnectionStateListener;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECALLTRACKER_H__
