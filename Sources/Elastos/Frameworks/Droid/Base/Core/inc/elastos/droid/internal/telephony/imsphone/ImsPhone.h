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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneBase.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICallSrvccState;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Droid::Internal::Telephony::ISubscription;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Ims::IImsCallForwardInfo;
using Elastos::Droid::Ims::IImsReasonInfo;
using Elastos::Droid::Ims::IImsSsInfo;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Core::IThrowable;
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
class ImsPhone
    : public ImsPhoneBase
    , public IImsPhone
{
private:
    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ ImsPhone* owner);

        // @Override
        CARAPI Run();

    private:
        ImsPhone* mOwner;
    };

    // Create Cf (Call forward) so that dialling number &
    // mIsCfu (true if reason is call forward unconditional)
    // mOnComplete (Message object passed by client) can be packed &
    // given as a single Cf object as user data to UtInterface.
    class Cf
        : public Object
    {
    public:
        Cf(
            /* [in] */ const String& cfNumber,
            /* [in] */ Boolean isCfu,
            /* [in] */ IMessage* onComplete);

    public:
        String mSetCfNumber;
        AutoPtr<IMessage> mOnComplete;
        Boolean mIsCfu;
    };

    class InnerImsEcbmStateListener
        : public Object
        //TODO , public IImsEcbmStateListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerImsEcbmStateListener(
            /* [in] */ ImsPhone* owner);

        // @Override
        CARAPI OnECBMEntered();

        // @Override
        CARAPI OnECBMExited();

    private:
        ImsPhone* mOwner;
    };

public:
    TO_STRING_IMPL("Imsphone")

    CAR_INTERFACE_DECL();

    ImsPhone();
    // Constructors
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ IPhone* defaultPhone);

    virtual CARAPI UpdateParentPhone(
        /* [in] */ IPhoneBase* parentPhone);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI RemoveReferences();

    // @Override
    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    /* package */
    virtual CARAPI SetServiceState(
        /* [in] */ Int32 state);

    // @Override
    CARAPI GetCallTracker(
        /* [out] */ ICallTracker** result);

    // @Override
    CARAPI GetPendingMmiCodes(
        /* [out] */ IList/*<? extends ImsPhoneMmiCode>*/** result);

    // @Override
    CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    // @Override
    CARAPI DeflectCall(
        /* [in] */ const String& number);

    // @Override
    CARAPI RejectCall();

    // @Override
    CARAPI SwitchHoldingAndActive();

    // @Override
    CARAPI CanConference(
        /* [out] */ Boolean* result);

    virtual CARAPI CanDial(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Conference();

    // @Override
    CARAPI ClearDisconnected();

    // @Override
    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ExplicitCallTransfer();

    // @Override
    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetBackgroundCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    virtual CARAPI NotifySuppSvcNotification(
        /* [in] */ ISuppServiceNotification* suppSvc);

    // @Override
    CARAPI HandleInCallMmiCommands(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    virtual CARAPI IsInCall(
        /* [out] */ Boolean* result);

    virtual CARAPI NotifyNewRingingConnection(
        /* [in] */ IConnection* c);

    virtual CARAPI NotifyForVideoCapabilityChanged(
        /* [in] */ Boolean isVideoCapable);

    using ImsPhoneBase::Dial;

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI AddParticipant(
        /* [in] */ const String& dialString);

    // @Override
    CARAPI SendDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StartDtmf(
        /* [in] */ Char32 c);

    // @Override
    CARAPI StopDtmf();

    // @Override
    CARAPI SetOnPostDialCharacter(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /*package*/
    virtual CARAPI NotifyIncomingRing();

    // @Override
    CARAPI SetMute(
        /* [in] */ Boolean muted);

    // @Override
    CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetState(
        /* [out] */ PhoneConstantsDataState* result);

    // @Override
    CARAPI GetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ Int32 timerSeconds,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    /* package */
    virtual CARAPI GetCallBarring(
        /* [in] */ const String& facility,
        /* [in] */ IMessage* onComplete);

    /* package */
    virtual CARAPI SetCallBarring(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SendUssdResponse(
        /* [in] */ const String& ussdMessge);

    /* package */
    virtual CARAPI SendUSSD(
        /* [in] */ const String& ussdString,
        /* [in] */ IMessage* response);

    /* package */
    virtual CARAPI CancelUSSD();

    /* package */
    virtual CARAPI SendErrorResponse(
        /* [in] */ IMessage* onComplete);

    /* package */
    virtual CARAPI SendErrorResponse(
       /* [in] */ IMessage* onComplete,
       /* [in] */ IThrowable* e);

    /* package */
    virtual CARAPI SendErrorResponse(
        /* [in] */ IMessage* onComplete,
        /* [in] */ IImsReasonInfo* reasonInfo);

    /* package */
    virtual CARAPI GetCommandException(
        /* [in] */ Int32 code,
        /* [out] */ ICommandException** result);

    /* package */
    virtual CARAPI GetCommandException(
        /* [in] */ IThrowable* e,
        /* [out] */ ICommandException** result);

    /* package */
    virtual CARAPI OnIncomingUSSD(
        /* [in] */ Int32 ussdMode,
        /* [in] */ const String& ussdMessage);

    /**
      * Removes the given MMI from the pending list and notifies
      * registrants that it is complete.
      * @param mmi MMI that is done
      */
    /*package*/
    virtual CARAPI OnMMIDone(
        /* [in] */ IImsPhoneMmiCode* mmi);

    virtual CARAPI GetHandoverConnection(
        /* [out] */ IArrayList/*<IConnection>*/** result);

    virtual CARAPI NotifySrvccState(
        /* [in] */  ICallSrvccState state);

    /* package */
    virtual CARAPI InitiateSilentRedial();

    virtual CARAPI RegisterForSilentRedial(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForSilentRedial(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI GetSubId(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetPhoneId(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetSubscriptionInfo(
        /* [out] */ ISubscription** result);

    virtual CARAPI GetIccRecords(
        /* [out] */ IIccRecords** result);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    virtual CARAPI IsInEmergencyCall(
        /* [out] */ Boolean* result);

    virtual CARAPI IsInEcm(
        /* [out] */ Boolean* result);

    virtual CARAPI SendEmergencyCallbackModeChange();

    // @Override
    CARAPI ExitEmergencyCallbackMode();

    /**
      * Handle to cancel or restart Ecm timer in emergency call back mode if action is
      * CANCEL_ECM_TIMER, cancel Ecm timer and notify apps the timer is canceled; otherwise, restart
      * Ecm timer and notify apps the timer is restarted.
      */
    virtual CARAPI HandleTimerInEmergencyCallbackMode(
        /* [in] */ Int32 action);

    virtual CARAPI SetOnEcbModeExitResponse(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnsetOnEcbModeExitResponse(
        /* [in] */ IHandler* h);

    virtual CARAPI IsVolteEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI IsVtEnabled(
        /* [out] */ Boolean* result);

protected:
    using ImsPhoneBase::DialInternal;

    virtual CARAPI DialInternal(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

private:
    CARAPI_(Boolean) HandleCallDeflectionIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCallWaitingIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCallHoldIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleMultipartyIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleEctIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCcbsIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) IsValidCommandInterfaceCFReason(
        /* [in] */ Int32 commandInterfaceCFReason);

    CARAPI_(Boolean) IsValidCommandInterfaceCFAction(
        /* [in] */ Int32 commandInterfaceCFAction);

    CARAPI_(Boolean) IsCfEnable(
        /* [in] */ Int32 action);

    CARAPI_(Int32) GetConditionFromCFReason(
        /* [in] */ Int32 reason);

    CARAPI_(Int32) GetCFReasonFromCondition(
        /* [in] */ Int32 condition);

    CARAPI_(Int32) GetActionFromCFAction(
        /* [in] */ Int32 action);

    CARAPI_(Int32) GetCBTypeFromFacility(
        /* [in] */ const String& facility);

    CARAPI_(void) OnNetworkInitiatedUssd(
        /* [in] */ IImsPhoneMmiCode* mmi);

    CARAPI_(AutoPtr<ICallForwardInfo>) GetCallForwardInfo(
        /* [in] */ IImsCallForwardInfo* info);

    CARAPI_(AutoPtr<ArrayOf<ICallForwardInfo*> >) HandleCfQueryResult(
        /* [in] */ ArrayOf<IImsCallForwardInfo*>* infos);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) HandleCbQueryResult(
        /* [in] */ ArrayOf<IImsSsInfo*>* infos);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) HandleCwQueryResult(
        /* [in] */ ArrayOf<IImsSsInfo*>* infos);

    CARAPI_(void) SendResponse(
        /* [in] */ IMessage* onComplete,
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* e);

    CARAPI_(void) HandleEnterEmergencyCallbackMode();

    CARAPI_(void) HandleExitEmergencyCallbackMode();

public:
    static const Int32 RESTART_ECM_TIMER = 0;
    // restart Ecm timer
    static const Int32 CANCEL_ECM_TIMER = 1;
    // Instance Variables
    AutoPtr<IPhoneBase> mDefaultPhone;
    AutoPtr<IImsPhoneCallTracker> mCT;
    AutoPtr<IArrayList> mPendingMMIs;/*<ImsPhoneMmiCode>*/
    AutoPtr<IRegistrant> mPostDialHandler;
    AutoPtr<IServiceState> mSS;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    // List of Registrants to send supplementary service notifications to.
    AutoPtr<RegistrantList> mSsnRegistrants;
    /**
      * Listen to the IMS ECBM state change
      */
    AutoPtr</*TODO IImsEcbmStateListener*/IInterface> mImsEcbmStateListener;

protected:
    // STOPSHIP if true
    static const Int32 EVENT_SET_CALL_BARRING_DONE = /*TODO EVENT_LAST + */1;
    static const Int32 EVENT_GET_CALL_BARRING_DONE = /*TODO EVENT_LAST + */2;
    static const Int32 EVENT_SET_CALL_WAITING_DONE = /*TODO EVENT_LAST + */3;
    static const Int32 EVENT_GET_CALL_WAITING_DONE = /*TODO EVENT_LAST + */4;
    Boolean mIsPhoneInEcmState;

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    // cancel Ecm timer
    // Default Emergency Callback Mode exit timer
    static const Int32 DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;
    // To redial silently through GSM or CDMA when dialing through IMS fails
    String mLastDialString;
    // mEcmExitRespRegistrant is informed after the phone has been exited the emergency
    // callback mode keep track of if phone is in emergency callback mode
    AutoPtr<IRegistrant> mEcmExitRespRegistrant;
    AutoPtr<RegistrantList> mSilentRedialRegistrants;
    // Variable to cache the video capabilitity. In cases where we delete/re-create the phone
    // this information is getting lost.
    Boolean mIsVideoCapable;
    // A runnable which is used to automatically exit from Ecm after a period of time.
    AutoPtr<InnerRunnable> mExitEcmRunnable;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONE_H__

