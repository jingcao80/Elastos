
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/conn/SingleClientConnManager.h"
#include "org/apache/http/impl/conn/DefaultClientConnectionOperator.h"
#include "elastos/core/CSystem.h"
#include "elastos/core/Math.h"
#include "elastos/utility/concurrent/CTimeUnitHelper.h"
#include "elastos/utility/logging/Logger.h"
#include "libcore/io/CSocketTaggerHelper.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Math;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::ISocketTagger;
using Libcore::IO::ISocketTaggerHelper;
using Libcore::IO::CSocketTaggerHelper;
using Org::Apache::Http::Conn::EIID_IClientConnectionRequest;
using Org::Apache::Http::Conn::EIID_IClientConnectionManager;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

extern "C" const InterfaceID EIID_ConnAdapter =
        { 0xce93b6d, 0x382f, 0x4b88, { 0x87, 0x4a, 0xce, 0x49, 0x49, 0xe7, 0x96, 0xdf } };

//==============================================================================
// SingleClientConnManager::PoolEntry
//==============================================================================
SingleClientConnManager::PoolEntry::PoolEntry(
    /* [in] */ SingleClientConnManager* host)
    : AbstractPoolEntry(host->mConnOperator, NULL)
{}

ECode SingleClientConnManager::PoolEntry::Close()
{
    ShutdownEntry();
    AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(mConnection);
    Boolean isOpen;
    if (conn->IsOpen(&isOpen), isOpen)
        conn->Close();

    return NOERROR;
}

ECode SingleClientConnManager::PoolEntry::Shutdown()
{
    ShutdownEntry();
    AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(mConnection);
    Boolean isOpen;
    if (conn->IsOpen(&isOpen), isOpen)
        conn->Shutdown();

    return NOERROR;
}


//==============================================================================
// SingleClientConnManager::ConnAdapter
//==============================================================================
SingleClientConnManager::ConnAdapter::ConnAdapter(
    /* [in] */ PoolEntry* entry,
    /* [in] */ IHttpRoute* route,
    /* [in] */ SingleClientConnManager* host)
    : AbstractPooledConnAdapter(host, (AbstractPoolEntry*)entry)
{
    MarkReusable();
    entry->mRoute = route;
}

PInterface SingleClientConnManager::ConnAdapter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_ConnAdapter) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return AbstractPooledConnAdapter::Probe(riid);
    }
}


//==============================================================================
// SingleClientConnManager::MyClientConnectionRequest
//==============================================================================
SingleClientConnManager::MyClientConnectionRequest::MyClientConnectionRequest(
    /* [in] */ SingleClientConnManager* host,
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state)
    : mHost(host)
    , mRoute(route)
    , mState(state)
{}

CAR_INTERFACE_IMPL(SingleClientConnManager::MyClientConnectionRequest, Object, IClientConnectionRequest)

ECode SingleClientConnManager::MyClientConnectionRequest::AbortRequest()
{
    // Nothing to abort, since requests are immediate.
    return NOERROR;
}

ECode SingleClientConnManager::MyClientConnectionRequest::GetConnection(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* tunit,
    /* [out] */ IManagedClientConnection** connection)
{
    VALIDATE_NOT_NULL(connection)
    return mHost->GetConnection(mRoute, mState, connection);
}


//==============================================================================
// SingleClientConnManager
//==============================================================================
const String SingleClientConnManager::MISUSE_MESSAGE =
        String("Invalid use of SingleClientConnManager: connection still allocated.\n") +
        String("Make sure to release the connection before allocating another one.");

SingleClientConnManager::SingleClientConnManager(
    /* [in] */ IHttpParams* params,
    /* [in] */ ISchemeRegistry* schreg)
    : mLastReleaseTime(0)
    , mConnectionExpiresTime(0)
    , mAlwaysShutDown(FALSE)
    , mIsShutDown(FALSE)
{
    if (schreg == NULL) {
        Logger::E("SingleClientConnManager", "Scheme registry must not be null.");
        assert(0);
        // throw new IllegalArgumentException
        //     ("Scheme registry must not be null.");
    }
    mSchemeRegistry  = schreg;
    mConnOperator    = CreateConnectionOperator(schreg);
    mUniquePoolEntry = new PoolEntry(this);
    mLastReleaseTime = -1L;
    mAlwaysShutDown  = FALSE; //@@@ from params? as argument?
    mIsShutDown      = FALSE;
}

SingleClientConnManager::~SingleClientConnManager()
{
    Shutdown();
}

CAR_INTERFACE_IMPL(SingleClientConnManager, Object, IClientConnectionManager)

ECode SingleClientConnManager::GetSchemeRegistry(
    /* [out] */ ISchemeRegistry** schemeRegistry)
{
    VALIDATE_NOT_NULL(schemeRegistry)
    *schemeRegistry = mSchemeRegistry;
    REFCOUNT_ADD(*schemeRegistry)
    return NOERROR;
}

AutoPtr<IClientConnectionOperator> SingleClientConnManager::CreateConnectionOperator(
    /* [in] */ ISchemeRegistry* schreg)
{
    AutoPtr<IClientConnectionOperator> oper = (IClientConnectionOperator*)new DefaultClientConnectionOperator(schreg);
    return oper;
}

ECode SingleClientConnManager::AssertStillUp()
{
    if (mIsShutDown) {
        Logger::E("SingleClientConnManager", "Manager is shut down.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SingleClientConnManager::RequestConnection(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [out] */ IClientConnectionRequest** request)
{
    VALIDATE_NOT_NULL(request)
    *request = (IClientConnectionRequest*)new MyClientConnectionRequest(this, route, state);
    REFCOUNT_ADD(*request)
    return NOERROR;
}

ECode SingleClientConnManager::GetConnection(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [out] */ IManagedClientConnection** connection)
{
    VALIDATE_NOT_NULL(connection)
    *connection = NULL;

    if (route == NULL) {
        Logger::E("SingleClientConnManager", "Route may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertStillUp())

    // if (log.isDebugEnabled()) {
    //     log.debug("Get connection for route " + route);
    // }

    if (mManagedConn != NULL) RevokeConnection();

    // check re-usability of the connection
    Boolean recreate = FALSE;
    Boolean shutdown = FALSE;

    // Kill the connection if it expired.
    CloseExpiredConnections();

    Boolean isOpen;
    if (IHttpConnection::Probe(mUniquePoolEntry->mConnection)->IsOpen(&isOpen), isOpen) {
        AutoPtr<IRouteTracker> tracker = mUniquePoolEntry->mTracker;
        if (tracker == NULL) {
            shutdown = TRUE;
        }
        else {
            AutoPtr<IHttpRoute> r;
            tracker->ToRoute((IHttpRoute**)&r);// can happen if method is aborted
            IObject::Probe(r)->Equals(route, &shutdown);
        }
    }
    else {
        // If the connection is not open, create a new PoolEntry,
        // as the connection may have been marked not reusable,
        // due to aborts -- and the PoolEntry should not be reused
        // either.  There's no harm in recreating an entry if
        // the connection is closed.
        recreate = TRUE;
    }

    if (shutdown) {
        recreate = TRUE;
        // try {
        mUniquePoolEntry->Shutdown();
        // } catch (IOException iox) {
        //     log.debug("Problem shutting down connection.", iox);
        // }
    }

    if (recreate)
        mUniquePoolEntry = new PoolEntry(this);

    // BEGIN android-changed
    // When using a recycled Socket, we need to re-tag it with any
    // updated statistics options.
    // try {
    AutoPtr<ISocket> socket;
    mUniquePoolEntry->mConnection->GetSocket((ISocket**)&socket);
    if (socket != NULL) {
        AutoPtr<ISocketTaggerHelper> helper;
        CSocketTaggerHelper::AcquireSingleton((ISocketTaggerHelper**)&helper);
        AutoPtr<ISocketTagger> tagger;
        helper->Get((ISocketTagger**)&tagger);
        tagger->Tag(socket);
    }
    // } catch (IOException iox) {
    //     log.debug("Problem tagging socket.", iox);
    // }
    // END android-changed

    mManagedConn = new ConnAdapter(mUniquePoolEntry, route, this);

    *connection = (IManagedClientConnection*)mManagedConn;
    REFCOUNT_ADD(*connection)
    return NOERROR;
}

ECode SingleClientConnManager::ReleaseConnection(
    /* [in] */ IManagedClientConnection* conn,
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* timeUnit)
{
    FAIL_RETURN(AssertStillUp())

    AutoPtr<ConnAdapter> sca = reinterpret_cast<ConnAdapter*>(conn->Probe(EIID_ConnAdapter));
    if (sca == NULL) {
        Logger::E("SingleClientConnManager", "Connection class mismatch, connection not obtained from this manager.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // if (log.isDebugEnabled()) {
    //     log.debug("Releasing connection " + conn);
    // }

    if (sca->mPoolEntry == NULL) {
        return NOERROR; // already released
    }

    AutoPtr<IClientConnectionManager> manager = sca->GetManager();
    if (manager != NULL && manager.Get() != this) {
        Logger::E("SingleClientConnManager", "Connection not obtained from this manager.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    // BEGIN android-changed
    // When recycling a Socket, we un-tag it to avoid collecting
    // statistics from future users.
    AutoPtr<ISocket> socket;
    mUniquePoolEntry->mConnection->GetSocket((ISocket**)&socket);
    if (socket != NULL) {
        AutoPtr<ISocketTaggerHelper> helper;
        CSocketTaggerHelper::AcquireSingleton((ISocketTaggerHelper**)&helper);
        AutoPtr<ISocketTagger> tagger;
        helper->Get((ISocketTagger**)&tagger);
        tagger->Untag(socket);
    }
    // END android-changed

    // make sure that the response has been read completely
    Boolean isOpen, isMarkedReusable;
    if ((sca->IsOpen(&isOpen), isOpen) && (mAlwaysShutDown ||
            (sca->IsMarkedReusable(&isMarkedReusable), !isMarkedReusable)))
    {
        // if (log.isDebugEnabled()) {
        //     log.debug
        //         ("Released connection open but not reusable.");
        // }

        // make sure this connection will not be re-used
        // we might have gotten here because of a shutdown trigger
        // shutdown of the adapter also clears the tracked route
        sca->Shutdown();
    }
    // } catch (IOException iox) {
    //     //@@@ log as warning? let pass?
    //     if (log.isDebugEnabled())
    //         log.debug("Exception shutting down released connection.",
    //                   iox);
    // } finally {
    //     sca.detach();
    //     managedConn = null;
    //     lastReleaseTime = System.currentTimeMillis();
    //     if(validDuration > 0)
    //         connectionExpiresTime = timeUnit.toMillis(validDuration) + lastReleaseTime;
    //     else
    //         connectionExpiresTime = Long.MAX_VALUE;
    // }
    sca->Detach();
    mManagedConn = NULL;
    AutoPtr<CSystem> cs;
    CSystem::AcquireSingletonByFriend((CSystem**)&cs);
    cs->GetCurrentTimeMillis(&mLastReleaseTime);
    if(validDuration > 0) {
        Int64 millis;
        timeUnit->ToMillis(validDuration, &millis);
        mConnectionExpiresTime = millis + mLastReleaseTime;
    }
    else {
        mConnectionExpiresTime = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    return NOERROR;
}

ECode SingleClientConnManager::CloseExpiredConnections()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 current;
    if(system->GetCurrentTimeMillis(&current), current >= mConnectionExpiresTime) {
        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> milliseconds;
        helper->GetMILLISECONDS((ITimeUnit**)&milliseconds);
        CloseIdleConnections(0, milliseconds);
    }
    return NOERROR;
}

ECode SingleClientConnManager::CloseIdleConnections(
    /* [in] */ Int64 idletime,
    /* [in] */ ITimeUnit* tunit)
{
    FAIL_RETURN(AssertStillUp())

    // idletime can be 0 or negative, no problem there
    if (tunit == NULL) {
        Logger::E("SingleClientConnManager", "Time unit must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isOpen;
    if ((mManagedConn == NULL)
            && (IHttpConnection::Probe(mUniquePoolEntry->mConnection)->IsOpen(&isOpen), isOpen)) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 current;
        system->GetCurrentTimeMillis(&current);
        Int64 millis;
        tunit->ToMillis(idletime, &millis);
        Int64 cutoff = current - millis;
        if (mLastReleaseTime <= cutoff) {
            // try {
            mUniquePoolEntry->Close();
            // } catch (IOException iox) {
            //     // ignore
            //     log.debug("Problem closing idle connection.", iox);
            // }
        }
    }
    return NOERROR;
}

ECode SingleClientConnManager::Shutdown()
{
    mIsShutDown = TRUE;

    if (mManagedConn != NULL)
        mManagedConn->Detach();

    // try {
    if (mUniquePoolEntry != NULL) // and connection open?
        mUniquePoolEntry->Shutdown();
    // } catch (IOException iox) {
    //     // ignore
    //     log.debug("Problem while shutting down manager.", iox);
    // } finally {
    //     uniquePoolEntry = null;
    // }
    mUniquePoolEntry = NULL;
    return NOERROR;
}

ECode SingleClientConnManager::RevokeConnection()
{
    if (mManagedConn == NULL)
        return NOERROR;

    Logger::D("SingleClientConnManager", "%s", MISUSE_MESSAGE.string());

    mManagedConn->Detach();

    // try {
    mUniquePoolEntry->Shutdown();
    // } catch (IOException iox) {
    //     // ignore
    //     log.debug("Problem while shutting down connection.", iox);
    // }
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org