
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALLTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALLTRACKER_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmCallTracker.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/CallTracker.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::CallTracker;
using Elastos::Droid::Internal::Telephony::ICallSrvccState;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhoneSuppService;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGsmCallTracker)
    , public CallTracker
    , public IGsmCallTracker
{
public:
    CGsmCallTracker();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGSMPhone* phone);

    CARAPI Dispose();

    CARAPI RegisterForVoiceCallStarted(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForVoiceCallStarted(
        /* [in] */ IHandler* h);

    CARAPI RegisterForVoiceCallEnded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForVoiceCallEnded(
        /* [in] */ IHandler* h);

    /**
     * clirMode is one of the CLIR_ constants
     */
    /*synchronized*/
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 clirMode,
        /* [in] */ IUUSInfo* uusInfo,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 clirMode,
        /* [out] */ IConnection** result);

    CARAPI AcceptCall();

    CARAPI RejectCall();

    CARAPI SwitchWaitingOrHoldingAndActive();

    CARAPI Conference();

    CARAPI ExplicitCallTransfer();

    CARAPI ClearDisconnected();

    CARAPI CanConference(
        /* [out] */ Boolean* result);

    CARAPI CanDial(
        /* [out] */ Boolean* result);

    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    /* package */
    CARAPI Hangup(
        /* [in] */ IGsmConnection* conn);

    /* package */
    CARAPI Separate(
        /* [in] */ IGsmConnection* conn);

    /* package */
    CARAPI SetMute(
        /* [in] */ Boolean mute);

    /* package */
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    /* package */
    CARAPI Hangup(
        /* [in] */ IGsmCall* call);

    /* package */
    CARAPI HangupWaitingOrBackground();

    /* package */
    CARAPI HangupForegroundResumeBackground();

    CARAPI HangupConnectionByIndex(
        /* [in] */ IGsmCall* call,
        /* [in] */ Int32 index);

    CARAPI HangupAllConnections(
        /* [in] */ IGsmCall* call);

    /* package */
    CARAPI GetConnectionByIndex(
        /* [in] */ IGsmCall* call,
        /* [in] */ Int32 index,
        /* [out] */ IGsmConnection** result);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    CARAPI_(void) HandlePollCalls(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    CARAPI_(void) FakeHoldForegroundBeforeDial();

    CARAPI_(void) InternalClearDisconnected();

    /**
     * Obtain a message to use for signalling "invoke getCurrentCalls() when
     * this operation and all other pending operations are complete
     */
    CARAPI_(AutoPtr<IMessage>) ObtainCompleteMessage();

    /**
     * Obtain a message to use for signalling "invoke getCurrentCalls() when
     * this operation and all other pending operations are complete
     */
    CARAPI_(AutoPtr<IMessage>) ObtainCompleteMessage(
        /* [in] */ Int32 what);

    CARAPI_(void) OperationComplete();

    CARAPI_(void) UpdatePhoneState();

    CARAPI_(void) HandleRadioNotAvailable();

    CARAPI_(void) DumpState();

    CARAPI_(IPhoneSuppService) GetFailedService(
        /* [in] */ Int32 what);

public:
    static const Int32 MAX_CONNECTIONS = 7;   // only 7 connections allowed in GSM
    static const Int32 MAX_CONNECTIONS_PER_CALL = 5; // only 5 connections allowed per call

    //***** Instance Variables
    AutoPtr<ArrayOf<IGsmConnection*> > mConnections; // = new GsmConnection[MAX_CONNECTIONS];
    AutoPtr<IRegistrantList> mVoiceCallEndedRegistrants; // = new RegistrantList();
    AutoPtr<IRegistrantList> mVoiceCallStartedRegistrants; // = new RegistrantList();

    // connections dropped during last poll
    AutoPtr<IArrayList> mDroppedDuringPoll; // = new ArrayList<GsmConnection>(MAX_CONNECTIONS);

    AutoPtr<IGsmCall> mRingingCall; // = new GsmCall(this);
            // A call that is ringing or (call) waiting
    AutoPtr<IGsmCall> mForegroundCall; // = new GsmCall(this);
    AutoPtr<IGsmCall> mBackgroundCall; // = new GsmCall(this);

    AutoPtr<IGsmConnection> mPendingMO;
    Boolean mHangupPendingMO;

    AutoPtr<IGSMPhone> mPhone;

    Boolean mDesiredMute; // = false;    // false = mute off

    PhoneConstantsState mState; // = PhoneConstants.State.IDLE;

    ICallSrvccState mSrvccState; // = Call.SrvccState.NONE;

private:
    static const String TAG; // = "GsmCallTracker";

    static const Boolean REPEAT_POLLING = FALSE;

    static const Boolean DBG_POLL;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALLTRACKER_H__
