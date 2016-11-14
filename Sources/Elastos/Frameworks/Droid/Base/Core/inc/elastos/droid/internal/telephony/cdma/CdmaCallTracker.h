#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLTRACKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/CallTracker.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/RegistrantList.h"
#include <elastos/core/Runnable.h>

// import android.telephony.DisconnectCause;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.ServiceState;
// import android.telephony.Rlog;
// import android.os.SystemProperties;
// import android.text.TextUtils;
// import com.android.internal.telephony.CallStateException;
// import com.android.internal.telephony.CallTracker;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.Phone;
// import com.android.internal.telephony.PhoneBase;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.TelephonyProperties;
// import java.util.List;

using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Core::Runnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * {@hide}
  */
class CdmaCallTracker
    : public CallTracker
    , public ICdmaCallTracker
{
private:
    class CdmaCallTrackerRunnable
        : public Runnable
    {
    public:
        CdmaCallTrackerRunnable(
            /* [in] */ CdmaCallTracker* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CdmaCallTracker* mHost;
    };

public:
    TO_STRING_IMPL("CdmaCallTracker")

    CAR_INTERFACE_DECL();

    //    boolean needsPoll;
    //***** Events
    //***** Constructors
    CdmaCallTracker();

    ~CdmaCallTracker();

    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone);

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

    virtual CARAPI RegisterForCallWaiting(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForCallWaiting(
        /* [in] */ IHandler* h);

    /**
      * clirMode is one of the CLIR_ constants
      */
    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 clirMode,
        /* [out] */ IConnection** result);

    virtual CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [out] */ IConnection** result);

    virtual CARAPI AcceptCall();

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

    //***** Called from CdmaConnection
    /*package*/
    virtual CARAPI Hangup(
        /* [in] */ ICdmaConnection* conn);

    /*package*/
    virtual CARAPI Separate(
        /* [in] */ ICdmaConnection* conn);

    //***** Called from CDMAPhone
    /*package*/
    virtual CARAPI SetMute(
        /* [in] */ Boolean mute);

    /*package*/
    virtual CARAPI GetMute(
        /* [out] */ Boolean* result);

    //***** Called from CdmaCall
    /* package */
    virtual CARAPI Hangup(
        /* [in] */ ICdmaCall* call);

    /* package */
    virtual CARAPI HangupWaitingOrBackground();

    /* package */
    virtual CARAPI HangupForegroundResumeBackground();

    virtual CARAPI HangupConnectionByIndex(
        /* [in] */ ICdmaCall* call,
        /* [in] */ Int32 index);

    virtual CARAPI HangupAllConnections(
        /* [in] */ ICdmaCall* call);

    /* package */
    virtual CARAPI GetConnectionByIndex(
        /* [in] */ ICdmaCall* call,
        /* [in] */ Int32 index,
        /* [out] */ ICdmaConnection** result);

    //****** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Check if current call is in emergency call
      *
      * @return true if it is in emergency call
      *         false if it is not in emergency call
      */
    virtual CARAPI IsInEmergencyCall(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // ***** Overwritten from CallTracker
    // @Override
    CARAPI_(void) HandlePollCalls(
        /* [in] */ AsyncResult* ar);

    // @Override
    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    //CARAPI_(AutoPtr<ICdmaConnection>) MiddleInitMconnections();

    CARAPI_(AutoPtr<IConnection>) DialThreeWay(
        /* [in] */ const String& dialString);

    //***** Private Instance Methods
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

    CARAPI_(void) FlashAndSetGenericTrue();

    CARAPI_(void) HandleRadioNotAvailable();

    CARAPI_(void) NotifyCallWaitingInfo(
        /* [in] */ ICdmaCallWaitingNotification* obj);

    CARAPI_(void) HandleCallWaitingInfo(
        /* [in] */ ICdmaCallWaitingNotification* cw);

    /**
      * Handle Ecm timer to be canceled or re-started
      */
    CARAPI_(void) HandleEcmTimer(
        /* [in] */ Int32 action);

    /**
      * Disable data call when emergency call is connected
      */
    CARAPI_(void) DisableDataCallInEmergencyCall(
        /* [in] */ const String& dialString);

    /**
      * Check and enable data call after an emergency call is dropped if it's
      * not in ECM
      */
    CARAPI_(void) CheckAndEnableDataCallAfterEmergencyCallDropped();

    /**
      * Check the MT call to see if it's a new ring or
      * a unknown connection.
      */
    CARAPI_(AutoPtr<IConnection>) CheckMtFindNewRinging(
        /* [in] */ IDriverCall* dc,
        /* [in] */ Int32 i);

public:
    static const String LOGTAG;
    //***** Constants
    static const Int32 MAX_CONNECTIONS = 8;
    static const Int32 MAX_CONNECTIONS_PER_CALL = 1;
    // only 1 connection allowed per call
    //***** Instance Variables
    AutoPtr<ArrayOf<ICdmaConnection*> > mConnections;
    AutoPtr<RegistrantList> mVoiceCallEndedRegistrants;
    AutoPtr<RegistrantList> mVoiceCallStartedRegistrants;
    AutoPtr<RegistrantList> mCallWaitingRegistrants;
    // connections dropped during last poll
    AutoPtr<IArrayList> mDroppedDuringPoll; // ICdmaConnection
    AutoPtr<ICdmaCall> mRingingCall;
    // A call that is ringing or (call) waiting
    AutoPtr<ICdmaCall> mForegroundCall;
    AutoPtr<ICdmaCall> mBackgroundCall;
    AutoPtr<ICdmaConnection> mPendingMO;
    Boolean mHangupPendingMO;
    Boolean mPendingCallInEcm;
    Boolean mIsInEmergencyCall;
    AutoPtr<ICDMAPhone> mPhone;
    Boolean mDesiredMute;
    // false = mute off
    Int32 mPendingCallClirMode;
    PhoneConstantsState mState;

private:
    static const Boolean REPEAT_POLLING;
    static const Boolean DBG_POLL;
    Boolean mIsEcmTimerCanceled;
    Int32 m3WayCallFlashDelay;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLTRACKER_H__
