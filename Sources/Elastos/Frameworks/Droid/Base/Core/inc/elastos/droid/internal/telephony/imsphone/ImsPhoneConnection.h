#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECONNECTION_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/telephony/Connection.h"

// import android.os.AsyncResult;
// import android.os.Bundle;
// import android.os.PowerManager;
// import android.os.Registrant;
// import android.os.SystemClock;
// import android.telecom.Log;
// import android.telephony.DisconnectCause;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.Rlog;
// import com.android.ims.ImsException;
// import com.android.internal.telephony.CallStateException;
// import com.android.internal.telephony.Connection;
// import com.android.internal.telephony.Phone;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.UUSInfo;
// import com.android.ims.ImsCallProfile;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
//TODO using Elastos::Droid::Ims::IImsCall;
//TODO using Elastos::Droid::Ims::IImsStreamMediaProfile;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::Connection;
using Elastos::Droid::Internal::Telephony::IConnectionPostDialState;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * {@hide}
  */
class ImsPhoneConnection
    : public Connection
    , public IImsPhoneConnection
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
    /** This is probably an MT call */
    /*package*/
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
        /* [in] */ IImsPhoneCallTracker* ct,
        /* [in] */ IImsPhoneCall* parent);

    /** This is an MO call, created when dialing */
    /*package*/
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& dialString,
        /* [in] */ IImsPhoneCallTracker* ct,
        /* [in] */ IImsPhoneCall* parent,
        /* [in] */ IBundle* extras);

    virtual CARAPI Dispose();

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b);

    // @Override
    CARAPI GetOrigDialString(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCall(
        /* [out] */ ICall** result);

    // @Override
    CARAPI GetDisconnectTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetHoldingStartTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetHoldDurationMillis(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetDisconnectCause(
        /* [out] */ Int32* result);

    virtual CARAPI SetDisconnectCause(
        /* [in] */ Int32 cause);

    virtual CARAPI GetOwner(
        /* [out] */ IImsPhoneCallTracker** result);

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
      */
    virtual CARAPI OnHangupLocal();

    /** Called when the connection has been disconnected */
    /*package*/
    virtual CARAPI OnDisconnect(
        /* [in] */ Int32 cause,
        /* [out] */ Boolean* result);

    /*package*/
    virtual CARAPI OnDisconnect(
        /* [out] */ Boolean* result);

    /**
      * An incoming or outgoing call has connected
      */
    virtual CARAPI OnConnectedInOrOut();

    /*package*/
    virtual CARAPI OnStartedHolding();

    // @Override
    CARAPI GetRemainingPostDialString(
        /* [out] */ String* result);

    virtual CARAPI ReleaseWakeLock();

    // @Override
    CARAPI GetNumberPresentation(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetUUSInfo(
        /* [out] */ IUUSInfo** result);

    // @Override
    CARAPI GetOrigConnection(
        /* [out] */ IConnection** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

    /*package*/
    virtual CARAPI GetImsCall(
        /* [out] */ /*TODO IImsCall*/IInterface** result);

    /*package*/
    virtual CARAPI SetImsCall(
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall);

    /*package*/
    virtual CARAPI ChangeParent(
        /* [in] */ IImsPhoneCall* parent);

    /*package*/
    virtual CARAPI Update(
        /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
        /* [in] */  ICallState state,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* result);

    virtual CARAPI GetCallExtras(
        /* [out] */ IBundle** result);

    /**
      * Notifies this Connection of a request to disconnect a participant of the conference managed
      * by the connection.
      *
      * @param endpoint the {@link android.net.Uri} of the participant to disconnect.
      */
    // @Override
    CARAPI OnDisconnectConferenceParticipant(
        /* [in] */ IUri* endpoint);

    /**
      * Provides a string representation of the {@link ImsPhoneConnection}.  Primarily intended for
      * use in log statements.
      *
      * @return String representation of call.
      */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    // @Override
    CARAPI Finalize();

private:
    /**
      * Determines the {@link ImsPhoneConnection} audio quality based on an
      * {@link ImsStreamMediaProfile}.
      *
      * @param mediaProfile The media profile.
      * @return The audio quality.
      */
    CARAPI_(Int32) GetAudioQualityFromMediaProfile(
        /* [in] */ /*TODO IImsStreamMediaProfile*/IInterface* mediaProfile);

    /**
      * Performs the appropriate action for a post-dial char, but does not
      * notify application. returns false if the character is invalid and
      * should be ignored
      */
    CARAPI_(Boolean) ProcessPostDialChar(
        /* [in] */ Char16 c);

    CARAPI_(void) ProcessNextPostDialChar();

    /**
      * Set post dial state and acquire wake lock while switching to "started"
      * state, the wake lock will be released if state switches out of "started"
      * state or after WAKE_LOCK_TIMEOUT_MILLIS.
      * @param s new PostDialState
      */
    CARAPI_(void) SetPostDialState(
        /* [in] */ IConnectionPostDialState s);

    CARAPI_(void) CreateWakeLock(
        /* [in] */ IContext* context);

    CARAPI_(void) AcquireWakeLock();

private:
    static const String LOGTAG;
    static const Boolean DBG;
    //***** Instance Variables
    AutoPtr<IImsPhoneCallTracker> mOwner;
    AutoPtr<IImsPhoneCall> mParent;
    AutoPtr</*TODO IImsCall*/IInterface> mImsCall;
    String mPostDialString;
    // outgoing calls only
    Boolean mDisconnected;
    AutoPtr<IBundle> mCallExtras;
    /*
     int mIndex;          // index in ImsPhoneCallTracker.connections[], -1 if unassigned
                         // The GSM index is 1 + this
     */
    /*
      * These time/timespan values are based on System.currentTimeMillis(),
      * i.e., "wall clock" time.
      */
    Int64 mDisconnectTime;
    Int32 mNextPostDialChar;
    // index into postDialString
    Int32 mCause;
    IConnectionPostDialState mPostDialState;
    AutoPtr<IUUSInfo> mUusInfo;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IPowerManagerWakeLock> mPartialWakeLock;
    //***** Event Constants
    static const Int32 EVENT_DTMF_DONE = 1;
    static const Int32 EVENT_PAUSE_DONE = 2;
    static const Int32 EVENT_NEXT_POST_DIAL = 3;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT = 4;
    //***** Constants
    static const Int32 PAUSE_DELAY_MILLIS = 3 * 1000;
    static const Int32 WAKE_LOCK_TIMEOUT_MILLIS = 60*1000;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECONNECTION_H__
