
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCONNECTION_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmConnection.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/Connection.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Connection;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::IDriverCallState;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerWakeLock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGsmConnection)
    , public Connection
    , public IGsmConnection
{
public:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CGsmConnection* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<CGsmConnection> mHost;
    };

public:
    CGsmConnection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /** This is probably an MT call that we first saw in a CLCC response */
    /*package*/
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDriverCall* dc,
        /* [in] */ IGsmCallTracker* ct,
        /* [in] */ Int32 index);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& dialString,
        /* [in] */ IGsmCallTracker* ct,
        /* [in] */ IGsmCall* parent);

    CARAPI Dispose();

    static CARAPI EqualsHandlesNulls(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI CompareTo(
        /* [in] */ IDriverCall* c,
        /* [out] */ Boolean* result);

    CARAPI GetCall(
        /* [out] */ ICall** result);

    CARAPI GetDisconnectTime(
        /* [out] */ Int64* result);

    CARAPI GetHoldDurationMillis(
        /* [out] */ Int64* result);

    CARAPI GetDisconnectCause(
        /* [out] */ Int32* result);

    CARAPI GetState(
        /* [out] */ ICallState* result);

    CARAPI Hangup();

    CARAPI Separate();

    CARAPI GetPostDialState(
        /* [out] */ IConnectionPostDialState* result);

    CARAPI ProceedAfterWaitChar();

    CARAPI ProceedAfterWildChar(
        /* [in] */ const String& str);

    CARAPI CancelPostDial();

    /**
     * Called when this Connection is being hung up locally (eg, user pressed "end")
     * Note that at this point, the hangup request has been dispatched to the radio
     * but no response has yet been received so update() has not yet been called
     */
    CARAPI OnHangupLocal();

    /**
     * Maps RIL call disconnect code to {@link DisconnectCause}.
     * @param causeCode RIL disconnect code
     * @return the corresponding value from {@link DisconnectCause}
     */
    CARAPI DisconnectCauseFromCode(
        /* [in] */ Int32 causeCode,
        /* [out] */ Int32* result);

    /*package*/
    CARAPI OnRemoteDisconnect(
    /* [in] */ Int32 causeCode);

    /**
     * Called when the radio indicates the connection has been disconnected.
     * @param cause call disconnect cause; values are defined in {@link DisconnectCause}
     */
    /*package*/
    CARAPI OnDisconnect(
        /* [in] */ Int32 cause,
        /* [out] */ Boolean* result);

    // Returns true if state has changed, false if nothing changed
    /*package*/
    CARAPI Update (
        /* [in] */ IDriverCall* dc,
        /* [out] */ Boolean* result);

    /**
     * Called when this Connection is in the foregroundCall
     * when a dial is initiated.
     * We know we're ACTIVE, and we know we're going to end up
     * HOLDING in the backgroundCall
     */
    CARAPI FakeHoldBeforeDial();

    /*package*/
    CARAPI GetGSMIndex(
        /* [out] */ Int32* result);

    /**
     * An incoming or outgoing call has connected
     */
    CARAPI OnConnectedInOrOut();

    /*package*/
    CARAPI OnStartedHolding();

    CARAPI GetRemainingPostDialString(
        /* [out] */ String* result);

    CARAPI GetNumberPresentation(
        /* [out] */ Int32* result);

    CARAPI GetUUSInfo(
        /* [out] */ IUUSInfo** result);

    CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* result);

    CARAPI MigrateFrom(
        /* [in] */ IConnection* c);

    CARAPI GetOrigConnection(
        /* [out] */ IConnection** result);

    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

protected:
    CARAPI_(void) Finalize();

private:
    /**
     * Performs the appropriate action for a post-dial char, but does not
     * notify application. returns false if the character is invalid and
     * should be ignored
     */
    CARAPI_(Boolean) ProcessPostDialChar(
        /* [in] */ Char32 c);

    CARAPI_(void) ProcessNextPostDialChar();

    /** "connecting" means "has never been ACTIVE" for both incoming
     *  and outgoing calls
     */
    CARAPI_(Boolean) IsConnectingInOrOut();

    CARAPI_(AutoPtr<IGsmCall>) ParentFromDCState (
        /* [in] */ IDriverCallState state);

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

    CARAPI_(void) ReleaseWakeLock();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    AutoPtr<IGsmCallTracker> mOwner;
    AutoPtr<IGsmCall> mParent;

    String mPostDialString;      // outgoing calls only
    Boolean mDisconnected;

    Int32 mIndex;          // index in GsmCallTracker.connections[], -1 if unassigned
                        // The GSM index is 1 + this

    /*
     * These time/timespan values are based on System.currentTimeMillis(),
     * i.e., "wall clock" time.
     */
    Int64 mDisconnectTime;

    Int32 mNextPostDialChar;       // index into postDialString

    Int32 mCause; // = DisconnectCause.NOT_DISCONNECTED;
    IConnectionPostDialState mPostDialState; // = PostDialState.NOT_STARTED;
    AutoPtr<IUUSInfo> mUusInfo;
    Int32 mPreciseCause; // = 0;

    AutoPtr<IConnection> mOrigConnection;

    AutoPtr<IHandler> mHandler;

private:
    static const String TAG; // = "GsmConnection";
    static const Boolean DBG = TRUE;
    AutoPtr<IPowerManagerWakeLock> mPartialWakeLock;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCONNECTION_H__
