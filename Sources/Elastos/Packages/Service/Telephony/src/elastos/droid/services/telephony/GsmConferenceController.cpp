
#include "elastos/droid/services/telephony/GsmConferenceController.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

ECode GsmConferenceController::MyConnectionListener::OnStateChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 state)
{
    mHost->Recalculate();
    return NOERROR;
}

ECode GsmConferenceController::MyConnectionListener::OnDisconnected(
    /* [in] */ IConnection* c,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mHost->Recalculate();
    return NOERROR;
}

ECode GsmConferenceController::MyConnectionListener::OnDestroyed(
    /* [in] */ IConnection* connection)
{
    mHost->Remove(IGsmConnection::Probe(connection));
    return NOERROR;
}

const Int32 GsmConferenceController::GSM_CONFERENCE_MAX_SIZE = 5;

GsmConferenceController::GsmConferenceController(
    /* [in] */ ITelephonyConnectionService* connectionService)
    : mConnectionService(connectionService)
{
    mConnectionListener = new MyConnectionListener(this);

    CArrayList::New((IArrayList**)&mGsmConnections);
}

ECode GsmConferenceController::Add(
    /* [in] */ IGsmConnection* connection)
{
    mGsmConnections->Add(connection);
    connection->AddConnectionListener(mConnectionListener);
    Recalculate();
    return NOERROR;
}

ECode GsmConferenceController::Remove(
    /* [in] */ IGsmConnection* connection)
{
    connection->RemoveConnectionListener(mConnectionListener);
    mGsmConnections->Remove(connection);
    Recalculate();
    return NOERROR;
}

void GsmConferenceController::Recalculate()
{
    RecalculateConferenceable();
    RecalculateConference();
}

Boolean GsmConferenceController::IsFullConference(
    /* [in] */ IConference* conference)
{
    AutoPtr<IList> list;
    conference->GetConnections((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    return size >= GSM_CONFERENCE_MAX_SIZE;
}

Boolean GsmConferenceController::ParticipatesInFullConference(
    /* [in] */ IConnection* connection)
{
    AutoPtr<IConference> conference;
    connection->GetConference((IConference**)&conference);
    return  conference != NULL && IsFullConference(conference);
}

void GsmConferenceController::RecalculateConferenceable()
{
    Int32 size;
    mGsmConnections->GetSize(&size);
    Logger::V("GsmConferenceController", "recalculateConferenceable : %d", size);

    AutoPtr<IList> activeConnections;
    CArrayList::New(size, (IList**)&activeConnections);
    AutoPtr<IList> backgroundConnections;
    CArrayList::New(size, (IList**)&backgroundConnections);

    // Loop through and collect all calls which are active or holding
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mGsmConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> connection = IGsmConnection::Probe(obj);

        Logger::D("GsmConferenceController", "recalc - %s %s", connection.getState(), TO_CSTR(connection));

        if (!ParticipatesInFullConference(connection)) {

            switch (connection.getState()) {
                case IConnection::STATE_ACTIVE:
                    activeConnections->Add(TO_IINTERFACE(connection));
                    continue;
                case IConnection::STATE_HOLDING:
                    backgroundConnections->Add(TO_IINTERFACE(connection));
                    continue;
                default:
                    break;
            }
        }

        connection->SetConferenceableConnections(Collections.<Connection>emptyList());
    }

    Int32 tmp, tmp2;
    Logger::V("GsmConferenceController", "active: %d, holding: %d",
            (activeConnections->GetSize(&tmp), tmp),
            (backgroundConnections->GetSize(&tmp2), tmp2));

    // Go through all the active connections and set the background connections as
    // conferenceable.
    for (Int32 i = 0; i < tmp; i++) {
        AutoPtr<IInterface> obj;
        activeConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnectionn> connection = IConnectionn::Probe(obj);

        connection->SetConferenceableConnections(backgroundConnections);


    }

    // Go through all the background connections and set the active connections as
    // conferenceable.
    for (Int32 i = 0; i < tmp2; i++) {
        AutoPtr<IInterface> obj;
        backgroundConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnectionn> connection = IConnectionn::Probe(obj);

        connection->SetConferenceableConnections(activeConnections);
    }

    // Set the conference as conferenceable with all the connections
    if (mGsmConference != NULL && !IsFullConference(mGsmConference)) {
        AutoPtr<IList> nonConferencedConnections;
        Int32 size;
        mGsmConnections->GetSize(&size);
        CArrayList::New(size, (IList**)&nonConferencedConnections);

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mGsmConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IGsmConnection> c = IGsmConnection::Probe(obj);

            AutoPtr<IConference> conference;
            c->GetConference((IConference**)&conference);
            if (conference == NULL) {
                nonConferencedConnections->Add(TO_IINTERFACE(c));
            }

        }
        mGsmConference->SetConferenceableConnections(nonConferencedConnections);
    }

    // TODO: Do not allow conferencing of already conferenced connections.
}

void GsmConferenceController::RecalculateConference()
{
    AutoPtr<ISet> conferencedConnections;
    CHashSet::New((ISet**)&conferencedConnections);

    Int32 size;
    mGsmConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mGsmConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> connection = IGsmConnection::Probe(obj);

        com.android.internal.telephony.Connection radioConnection =
            connection.getOriginalConnection();

        if (radioConnection != NULL) {
            ICallState state = radioConnection.getState();
            AutoPtr<ICall> call;
            radioConnection->GetCall((ICall**)&call);
            Boolean res;
            if ((state == ICallState_ACTIVE || state == ICallState_HOLDING) &&
                    (call != NULL && (call->IsMultiparty(&res), res))) {
                conferencedConnections->Add(TO_IINTERFACE(connection));
            }
        }

    }

    Logger::D("GsmConferenceController", "Recalculate conference calls %s %s.",
            TO_CSTR(mGsmConference), TO_CSTR(conferencedConnections));


    if ((conferencedConnections->GetSize(&size), size) < 2) {
        Logger::D("GsmConferenceController", "less than two conference calls!");
        // No more connections are conferenced, destroy any existing conference.
        if (mGsmConference != null) {
            Logger::D("GsmConferenceController", "with a conference to destroy!");
            mGsmConference->Destroy();
            mGsmConference = NULL;
        }
    } else {
        if (mGsmConference != NULL) {
            AutoPtr<IList> existingConnections;
            mGsmConference->GetConnections((IList**)&existingConnections);
            // Remove any that no longer exist

            Int32 tmp;
            existingConnections->GetSize(&tmp);
            for (Int32 i = 0; i < tmp; i++) {
                AutoPtr<IInterface> obj;
                existingConnections->Get(i, (IInterface**)&obj);
                AutoPtr<IConnectionn> connection = IConnectionn::Probe(obj);

                Boolean res;
                if (conferencedConnections->Contains(TO_IINTERFACE(connection), &res), !res) {
                    mGsmConference->RemoveConnection(TO_IINTERFACE(connection));
                }

            }

            // Add any new ones
            Int32 tmp2;
            conferencedConnections->GetSize(&tmp2);
            for (Int32 i = 0; i < tmp2; i++) {
                AutoPtr<IInterface> obj;
                conferencedConnections->Get(i, (IInterface**)&obj);
                AutoPtr<IConnectionn> connection = IConnectionn::Probe(obj);

                Boolean res;
                if (existingConnections->Contains(TO_IINTERFACE(connection), &res), !res) {
                    mGsmConference->AddConnection(TO_IINTERFACE(connection));
                }


            }
        }
        else {
            mGsmConference = new GsmConference(NULL);
            Int32 size;
            conferencedConnections->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                conferencedConnections->Get(i, (IInterface**)&obj);
                AutoPtr<IConnectionn> connection = IConnectionn::Probe(obj);

                Logger::D("GsmConferenceController", "Adding a connection to a conference call: %s %s",
                        TO_CSTR(mGsmConference), TO_CSTR(connection));
                mGsmConference->AddConnection(TO_IINTERFACE(connection));

            }
            mConnectionService->AddConference(TO_IINTERFACE(mGsmConference));
        }

        // Set the conference state to the same state as its child connections.
        AutoPtr<IList> connections;
        mGsmConference->GetConnections((IList**)&connections);
        AutoPtr<IInterface> obj;
        connections->Get(0, (IInterface**)&obj);
        AutoPtr<IConnection> conferencedConnection = IConnection::Probe(obj);

        Int32 state;
        conferencedConnection->GetState(&state);
        switch (state) {
            case IConnection::STATE_ACTIVE:
                mGsmConference->SetActive();
                break;
            case IConnection::STATE_HOLDING:
                mGsmConference->SetOnHold();
                break;
        }
    }
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos