#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
//TODO #include "elastos/droid/internal/telephony/Connection.h"

// import com.android.internal.telephony.*;
// import android.os.AsyncResult;
// import android.os.PowerManager;
// import android.os.Registrant;
// import android.os.SystemClock;
// import android.telephony.DisconnectCause;
// import android.telephony.Rlog;
// import android.text.TextUtils;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.ServiceState;
// import com.android.internal.telephony.uicc.UiccCardApplication;
// import com.android.internal.telephony.uicc.UiccController;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;

using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IDriverCallState;
using Elastos::Droid::Internal::Telephony::IConnectionPostDialState;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * {@hide}
  */
class CdmaConnection
    : public Object //TODO Connection
    , public ICdmaConnection
{
public:
    //***** Inner Classes
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* l);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    CAR_INTERFACE_DECL();

    //***** Constructors
    /** This is probably an MT call that we first saw in a CLCC response */
    /*package*/
    CdmaConnection(
        /* [in] */ IContext* context,
        /* [in] */ IDriverCall* dc,
        /* [in] */ ICdmaCallTracker* ct,
        /* [in] */ Int32 index);

    /** This is an MO call/three way call, created when dialing */
    /*package*/
    CdmaConnection(
        /* [in] */ IContext* context,
        /* [in] */ const String& dialString,
        /* [in] */ ICdmaCallTracker* ct,
        /* [in] */ ICdmaCall* parent);

    /** This is a Call waiting call*/
    CdmaConnection(
        /* [in] */ IContext* context,
        /* [in] */ ICdmaCallWaitingNotification* cw,
        /* [in] */ ICdmaCallTracker* ct,
        /* [in] */ ICdmaCall* parent);

    virtual CARAPI Dispose();

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b);

    /*package*/
    virtual CARAPI CompareTo(
        /* [in] */ IDriverCall* c,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetOrigDialString(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCall(
        /* [out] */ ICdmaCall** result);

    // @Override
    CARAPI GetDisconnectTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetHoldDurationMillis(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetDisconnectCause(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetState(
        /* [out] */ ICallState* result);

    // @Override
    CARAPI Hangup();

    // @Override
    CARAPI Separate();

    // @Override
    CARAPI GetPostDialState(
        /* [out] */ IConnectionPostDialState* result);

    // @Override
    CARAPI ProceedAfterWaitChar();

    // @Override
    CARAPI ProceedAfterWildChar(
        /* [in] */ const String& str);

    // @Override
    CARAPI CancelPostDial();

    /**
      * Called when this Connection is being hung up locally (eg, user pressed "end")
      * Note that at this point, the hangup request has been dispatched to the radio
      * but no response has yet been received so update() has not yet been called
      */
    virtual CARAPI OnHangupLocal();

    /**
      * Maps RIL call disconnect code to {@link DisconnectCause}.
      * @param causeCode RIL disconnect code
      * @return the corresponding value from {@link DisconnectCause}
      */
    virtual CARAPI DisconnectCauseFromCode(
        /* [in] */ Int32 causeCode,
        /* [out] */ Int32* result);

    /*package*/
    virtual CARAPI OnRemoteDisconnect(
        /* [in] */ Int32 causeCode);

    /**
      * Called when the radio indicates the connection has been disconnected.
      * @param cause call disconnect cause; values are defined in {@link DisconnectCause}
      */
    /*package*/
    virtual CARAPI OnDisconnect(
        /* [in] */ Int32 cause,
        /* [out] */ Boolean* result);

    /** Called when the call waiting connection has been hung up */
    /*package*/
    virtual CARAPI OnLocalDisconnect();

    // Returns true if state has changed, false if nothing changed
    /*package*/
    virtual CARAPI Update(
        /* [in] */ IDriverCall* dc,
        /* [out] */ Boolean* result);

    /**
      * Called when this Connection is in the foregroundCall
      * when a dial is initiated.
      * We know we're ACTIVE, and we know we're going to end up
      * HOLDING in the backgroundCall
      */
    virtual CARAPI FakeHoldBeforeDial();

    /*package*/
    virtual CARAPI GetCDMAIndex(
        /* [out] */ Int32* result);

    /**
      * An incoming or outgoing call has connected
      */
    virtual CARAPI OnConnectedInOrOut();

    /*package*/
    virtual CARAPI OnStartedHolding();

    // @Override
    CARAPI GetRemainingPostDialString(
        /* [out] */ String* result);

    virtual CARAPI UpdateParent(
        /* [in] */ ICdmaCall* oldParent,
        /* [in] */ ICdmaCall* newParent);

    virtual CARAPI ProcessNextPostDialChar();

    /**
      * format original dial string
      * 1) convert international dialing prefix "+" to
      *    string specified per region
      *
      * 2) handle corner cases for PAUSE/WAIT dialing:
      *
      *    If PAUSE/WAIT sequence at the end, ignore them.
      *
      *    If consecutive PAUSE/WAIT sequence in the middle of the string,
      *    and if there is any WAIT in PAUSE/WAIT sequence, treat them like WAIT.
      */
    static CARAPI_(String) FormatDialString(
        /* [in] */ const String& phoneNumber);

    // @Override
    CARAPI GetNumberPresentation(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetUUSInfo(
        /* [out] */ IUUSInfo** result);

    virtual CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetOrigConnection(
        /* [out] */ IConnection** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* resutl);

protected:
    // @Override
    CARAPI_(void) Finalize();

private:
    CARAPI_(void) DoDisconnect();

    /**
      * Performs the appropriate action for a post-dial char, but does not
      * notify application. returns false if the character is invalid and
      * should be ignored
      */
    CARAPI_(Boolean) ProcessPostDialChar(
        /* [in] */ Byte c);

    /** "connecting" means "has never been ACTIVE" for both incoming
      *  and outgoing calls
      */
    CARAPI_(Boolean) IsConnectingInOrOut();

    CARAPI_(AutoPtr<ICdmaCall>) ParentFromDCState(
        /* [in] */ IDriverCallState state);

    /**
      * Set post dial state and acquire wake lock while switching to "started" or "wait"
      * state, the wake lock will be released if state switches out of "started" or "wait"
      * state or after WAKE_LOCK_TIMEOUT_MILLIS.
      * @param s new PostDialState
      */
    CARAPI_(void) SetPostDialState(
        /* [in] */ IConnectionPostDialState s);

    CARAPI_(void) CreateWakeLock(
        /* [in] */ IContext* context);

    CARAPI_(void) AcquireWakeLock();

    CARAPI_(void) ReleaseWakeLock();

    static CARAPI_(Boolean) IsPause(
        /* [in] */ Byte c);

    static CARAPI_(Boolean) IsWait(
        /* [in] */ Byte c);

    // This function is to find the next PAUSE character index if
    // multiple pauses in a row. Otherwise it finds the next non PAUSE or
    // non WAIT character index.
    static CARAPI_(Int32) FindNextPCharOrNonPOrNonWCharIndex(
        /* [in] */ const String& phoneNumber,
        /* [in] */ Int32 currIndex);

    // This function returns either PAUSE or WAIT character to append.
    // It is based on the next non PAUSE/WAIT character in the phoneNumber and the
    // index for the current PAUSE/WAIT character
    static CARAPI_(Byte) FindPOrWCharToAppend(
        /* [in] */ const String& phoneNumber,
        /* [in] */ Int32 currPwIndex,
        /* [in] */ Int32 nextNonPwCharIndex);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    static const String LOGTAG;
    //***** Instance Variables
    AutoPtr<ICdmaCallTracker> mOwner;
    AutoPtr<ICdmaCall> mParent;
    String mPostDialString;
    // outgoing calls only
    Boolean mDisconnected;
    Int32 mIndex;
    // index in CdmaCallTracker.connections[], -1 if unassigned
    /*
      * These time/timespan values are based on System.currentTimeMillis(),
      * i.e., "wall clock" time.
      */
    Int64 mDisconnectTime;
    Int32 mNextPostDialChar;
    // index into postDialString
    Int32 mCause;
    IConnectionPostDialState mPostDialState;
    Int32 mPreciseCause;
    AutoPtr<IHandler> mHandler;
    //***** Event Constants
    static const Int32 EVENT_DTMF_DONE = 1;
    static const Int32 EVENT_PAUSE_DONE = 2;
    static const Int32 EVENT_NEXT_POST_DIAL = 3;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT = 4;
    //***** Constants
    static const Int32 WAKE_LOCK_TIMEOUT_MILLIS = 60*1000;
    static const Int32 PAUSE_DELAY_MILLIS = 2 * 1000;

private:
    static const Boolean VDBG;
    AutoPtr<IPowerManagerWakeLock> mPartialWakeLock;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACONNECTION_H__

