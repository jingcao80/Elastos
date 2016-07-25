
#include "elastos/droid/teleservice/services/telephony/GsmConferenceController.h"
#include "elastos/droid/teleservice/services/telephony/GsmConference.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecom::IConnectionService;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallState_ACTIVE;
using Elastos::Droid::Internal::Telephony::ICallState_HOLDING;
using Elastos::Droid::Internal::Telephony::EIID_IConnectionListener;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace TeleService {
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

    CArrayList::New((IList**)&mGsmConnections);
}

ECode GsmConferenceController::Add(
    /* [in] */ IGsmConnection* connection)
{
    mGsmConnections->Add(connection);
    IConnection::Probe(connection)->AddConnectionListener(mConnectionListener);
    Recalculate();
    return NOERROR;
}

ECode GsmConferenceController::Remove(
    /* [in] */ IGsmConnection* connection)
{
    IConnection::Probe(connection)->RemoveConnectionListener(mConnectionListener);
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
    IConnection::Probe(connection)->GetConference((IConference**)&conference);
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

        Int32 state;
        IConnection::Probe(connection)->GetState(&state);
        Logger::D("GsmConferenceController", "recalc - %d %s", state, TO_CSTR(connection));

        if (!ParticipatesInFullConference(IConnection::Probe(connection))) {
            switch (state) {
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

        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        AutoPtr<IList> list;
        helper->GetEmptyList((IList**)&list);
        IConnection::Probe(connection)->SetConferenceableConnections(list);
    }

    Int32  aSize, bSize;
    Logger::V("GsmConferenceController", "active: %d, holding: %d",
            (activeConnections->GetSize(&aSize), aSize),
            (backgroundConnections->GetSize(&bSize), bSize));

    // Go through all the active connections and set the background connections as
    // conferenceable.
    for (Int32 i = 0; i < aSize; i++) {
        AutoPtr<IInterface> obj;
        activeConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> connection = IConnection::Probe(obj);

        connection->SetConferenceableConnections(backgroundConnections);
    }

    // Go through all the background connections and set the active connections as
    // conferenceable.
    for (Int32 i = 0; i < bSize; i++) {
        AutoPtr<IInterface> obj;
        backgroundConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> connection = IConnection::Probe(obj);

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
            IConnection::Probe(c)->GetConference((IConference**)&conference);
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

        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> radioConnection;
        ITelephonyConnection::Probe(connection)->GetOriginalConnection(
                (Elastos::Droid::Internal::Telephony::IConnection**)&radioConnection);

        if (radioConnection != NULL) {
            ICallState state;
            radioConnection->GetState(&state);
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
        if (mGsmConference != NULL) {
            Logger::D("GsmConferenceController", "with a conference to destroy!");
            mGsmConference->Destroy();
            mGsmConference = NULL;
        }
    }
    else {
        if (mGsmConference != NULL) {
            AutoPtr<IList> existingConnections;
            mGsmConference->GetConnections((IList**)&existingConnections);
            // Remove any that no longer exist

            Int32 size;
            existingConnections->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                existingConnections->Get(i, (IInterface**)&obj);
                AutoPtr<IConnection> connection = IConnection::Probe(obj);

                Boolean res;
                if (conferencedConnections->Contains(TO_IINTERFACE(connection), &res), !res) {
                    mGsmConference->RemoveConnection(connection);
                }

            }

            // Add any new ones
            AutoPtr<ArrayOf<IInterface*> > array;
            conferencedConnections->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IConnection> connection = IConnection::Probe((*array)[i]);

                Boolean res;
                if (existingConnections->Contains(TO_IINTERFACE(connection), &res), !res) {
                    mGsmConference->AddConnection(connection, &res);
                }
            }
        }
        else {
            mGsmConference = new GsmConference(NULL);

            AutoPtr<ArrayOf<IInterface*> > array;
            conferencedConnections->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IConnection> connection = IConnection::Probe((*array)[i]);

                Logger::D("GsmConferenceController", "Adding a connection to a conference call: %s %s",
                        TO_CSTR(mGsmConference), TO_CSTR(connection));
                Boolean res;
                mGsmConference->AddConnection(connection, &res);

            }
            IConnectionService::Probe(mConnectionService)->AddConference(mGsmConference);
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
