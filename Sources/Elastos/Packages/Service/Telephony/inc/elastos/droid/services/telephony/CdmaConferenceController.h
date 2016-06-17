#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCECONTROLLER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCECONTROLLER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/telecomm/telecom/Conference.h"
#include "elastos/droid/services/telephony/CdmaConnection.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Telecomm::Telecom::IDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::Connection;
using Elastos::Droid::Telecomm::Telecom::Conference;
using Elastos::Droid::Telecomm::Telecom::IConnection;
using Elastos::Droid::Telecomm::Telecom::IConnectionListener;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * Manages CDMA conference calls. CDMA conference calls are much more limited than GSM conference
 * calls. Two main points of difference:
 * 1) Users cannot manage individual calls within a conference
 * 2) Whether a conference call starts off as a conference or as two distinct calls is a matter of
 *    physical location (some antennas are different than others). Worst still, there's no
 *    indication given to us as to what state they are in.
 *
 * To make life easier on the user we do the following: Whenever there exist 2 or more calls, we
 * say that we are in a conference call with {@link Connection#CAPABILITY_GENERIC_CONFERENCE}.
 * Generic indicates that this is a simple conference that doesn't support conference management.
 * The conference call will also support "MERGE" to begin with and stop supporting it the first time
 * we are asked to actually execute a merge. I emphasize when "we are asked" because we get no
 * indication whether the merge succeeds from CDMA, we just assume it does. Thats the best we
 * can do. Also, we do not kill a conference call once it is created unless all underlying
 * connections also go away.
 *
 * Outgoing CDMA calls made while another call exists would normally trigger a conference to be
 * created. To avoid this and make it seem like there is a "dialing" state, we fake it and prevent
 * the conference from being created for 3 seconds. This is a more pleasant experience for the user.
 */
class CdmaConferenceController
    : public Conference
{
private:
    class MyConnectionListener
        : public Connection::Listener
    {
    public:
        TO_STRING_IMPL("CdmaConferenceController::MyConnectionListener")

        MyConnectionListener(
            /* [in] */ CdmaConferenceController* mHost)
            : mHost(host)
        {}

        //@Override
        CARAPI OnStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 state);

        //@Override
        CARAPI OnDisconnected(
            /* [in] */ IConnection* c,
            /* [in] */ IDisconnectCause* disconnectCause);

        //@Override
        CARAPI OnDestroyed(
            /* [in] */ IConnection* c);

    private:
        CdmaConferenceController* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CdmaConferenceController::MyRunnable")

        MyRunnable(
            /* [in] */ CdmaConferenceController* host,
            /* [in] */ CdmaConnection* connection,
            /* [in] */ IList* connectionsToReset)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CdmaConferenceController* mHost;
        AutOpTR<CdmaConnection> mConnection;
        AutOpTR<IList> mConnectionsToReset;
    };

public:
    TO_STRING_IMPL("CdmaConferenceController")

    CdmaConferenceController(
        /* [in] */ ITelephonyConnectionService* connectionService);

    CARAPI Add(
        /* [in] */ CdmaConnection* connection);

private:
    CARAPI_(void) AddInternal(
        /* [in] */ CdmaConnection* connection);

    CARAPI_(void) Remove(
        /* [in] */ CdmaConnection* connection);

    CARAPI_(void) RecalculateConference();

private:
    AutoPtr<IConnectionListener> mConnectionListener;

    static const Int32 ADD_OUTGOING_CONNECTION_DELAY_MILLIS;

    /** The known CDMA connections. */
    AutoPtr<IList> mCdmaConnections;

    /**
     * Newly added connections.  We keep track of newly added outgoing connections because we do not
     * create a conference until a second outgoing call has existing for
     * {@link #ADD_OUTGOING_CONNECTION_DELAY_MILLIS} milliseconds.  This allows the UI to show the
     * call as "Dialing" for a certain amount of seconds.
     */
    AutoPtr<IList> mPendingOutgoingConnections;

    AutoPtr<ITelephonyConnectionService> mConnectionService;

    AutoPtr<IHandler> mHandler;

    /** The CDMA conference connection object. */
    AutoPtr<ICdmaConference> mConference;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCECONTROLLER_H__