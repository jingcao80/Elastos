
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "org/apache/http/impl/conn/IdleConnectionHandler.h"
#include "elastos/core/CSystem.h"
#include "elastos/core/Math.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Math;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

//==============================================================================
// IdleConnectionHandler::TimeValues
//==============================================================================
IdleConnectionHandler::TimeValues::TimeValues(
    /* [in] */ Int64 now,
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* validUnit)
    : mTimeAdded(0)
    , mTimeExpires(0)
{
    mTimeAdded = now;
    if (validDuration > 0) {
        Int64 millis;
        validUnit->ToMillis(validDuration, &millis);
        mTimeExpires = now + millis;
    }
    else {
        mTimeExpires = Elastos::Core::Math::INT64_MAX_VALUE;
    }
}


//==============================================================================
// IdleConnectionHandler
//==============================================================================
IdleConnectionHandler::IdleConnectionHandler()
{
    AutoPtr<IHashMap> hm;
    CHashMap::New((IHashMap**)&hm);
    mConnectionToTimes = IMap::Probe(hm);
}

void IdleConnectionHandler::Add(
    /* [in] */ IHttpConnection* connection,
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* unit)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 timeAdded;
    system->GetCurrentTimeMillis(&timeAdded);

    // if (log.isDebugEnabled()) {
    //     log.debug("Adding connection at: " + timeAdded);
    // }

    AutoPtr<TimeValues> timeValues = new TimeValues(timeAdded, validDuration, unit);
    mConnectionToTimes->Put(connection, timeValues->Probe(EIID_IInterface));
}

Boolean IdleConnectionHandler::Remove(
    /* [in] */ IHttpConnection* connection)
{
    AutoPtr<IInterface> value;
    mConnectionToTimes->Remove(connection, (IInterface**)&value);
    if(value == NULL) {
        Logger::D("IdleConnectionHandler", "Removing a connection that never existed!");
        return TRUE;
    }
    else {
        AutoPtr<TimeValues> times = (TimeValues*)(Object*)(IObject*)value.Get();
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 current;
        system->GetCurrentTimeMillis(&current);
        return current <= times->mTimeExpires;
    }
}

void IdleConnectionHandler::RemoveAll()
{
    mConnectionToTimes->Clear();
}

void IdleConnectionHandler::CloseIdleConnections(
    /* [in] */ Int64 idleTime)
{
    // the latest time for which connections will be closed
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 current;
    system->GetCurrentTimeMillis(&current);
    Int64 idleTimeout = current - idleTime;

    // if (log.isDebugEnabled()) {
    //     log.debug("Checking for connections, idleTimeout: "  + idleTimeout);
    // }
    AutoPtr<ISet> keySet;
    mConnectionToTimes->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> key;
        it->GetNext((IInterface**)&key);
        AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(key);
        AutoPtr<IInterface> value;
        mConnectionToTimes->Get(conn, (IInterface**)&value);
        AutoPtr<TimeValues> times = (TimeValues*)(Object*)(IObject*)value.Get();
        Int64 connectionTime = times->mTimeAdded;
        if (connectionTime <= idleTimeout) {
            // if (log.isDebugEnabled()) {
            //     log.debug("Closing connection, connection time: "  + connectionTime);
            // }
            it->Remove();
            // try {
            conn->Close();
            // } catch (IOException ex) {
            //     log.debug("I/O error closing connection", ex);
            // }
        }
    }
}

void IdleConnectionHandler::CloseExpiredConnections()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    // if (log.isDebugEnabled()) {
    //     log.debug("Checking for expired connections, now: "  + now);
    // }

    AutoPtr<ISet> keySet;
    mConnectionToTimes->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> key;
        it->GetNext((IInterface**)&key);
        AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(key);
        AutoPtr<IInterface> value;
        mConnectionToTimes->Get(conn, (IInterface**)&value);
        AutoPtr<TimeValues> times = (TimeValues*)(Object*)(IObject*)value.Get();
        if(times->mTimeExpires <= now) {
            // if (log.isDebugEnabled()) {
            //     log.debug("Closing connection, expired @: "  + times.timeExpires);
            // }
            it->Remove();
            // try {
            conn->Close();
            // } catch (IOException ex) {
            //     log.debug("I/O error closing connection", ex);
            // }
        }
    }
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org