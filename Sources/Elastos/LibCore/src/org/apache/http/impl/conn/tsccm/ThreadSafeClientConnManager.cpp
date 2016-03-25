
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/conn/tsccm/ThreadSafeClientConnManager.h"
#include "org/apache/http/impl/conn/tsccm/ConnPoolByRoute.h"
#include "org/apache/http/impl/conn/tsccm/BasicPooledConnAdapter.h"
#include "org/apache/http/impl/conn/DefaultClientConnectionOperator.h"
#include "libcore/io/CSocketTaggerHelper.h"
#include "elastos/core/AutoLock.h"
#include "elastos/utility/logging/Logger.h"

using Libcore::IO::ISocketTagger;
using Libcore::IO::ISocketTaggerHelper;
using Libcore::IO::CSocketTaggerHelper;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::EIID_IManagedClientConnection;
using Org::Apache::Http::Conn::EIID_IClientConnectionRequest;
using Org::Apache::Http::Conn::EIID_IClientConnectionManager;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Impl::Conn::DefaultClientConnectionOperator;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

//==============================================================================
// ThreadSafeClientConnManager::MyClientConnectionRequest
//==============================================================================
ThreadSafeClientConnManager::MyClientConnectionRequest::MyClientConnectionRequest(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IPoolEntryRequest* request,
    /* [in] */ ThreadSafeClientConnManager* host)
    : mRoute(route)
    , mRequest(request)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(ThreadSafeClientConnManager::MyClientConnectionRequest, Object, IClientConnectionRequest)

ECode ThreadSafeClientConnManager::MyClientConnectionRequest::AbortRequest()
{
    return mRequest->AbortRequest();
}

ECode ThreadSafeClientConnManager::MyClientConnectionRequest::GetConnection(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* tunit,
    /* [out] */ IManagedClientConnection** connection)
{
    VALIDATE_NOT_NULL(connection)
    *connection = NULL;

    if (mRoute == NULL) {
        Logger::E("ThreadSafeClientConnManager::MyClientConnectionRequest", "Route may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // if (log.isDebugEnabled()) {
    //     log.debug("ThreadSafeClientConnManager.getConnection: "
    //         + route + ", timeout = " + timeout);
    // }

    AutoPtr<IInterface> value;
    mRequest->GetPoolEntry(timeout, tunit, (IInterface**)&value);
    AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
    // BEGIN android-changed
    // When using a recycled Socket, we need to re-tag it with any
    // updated statistics options.
    // try {
    AutoPtr<ISocket> socket;
    entry->GetConnection()->GetSocket((ISocket**)&socket);
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
    AutoPtr<BasicPooledConnAdapter> adapter = new BasicPooledConnAdapter(mHost, entry);
    *connection = (IManagedClientConnection*)adapter->Probe(EIID_IManagedClientConnection);
    REFCOUNT_ADD(*connection)
    return NOERROR;
}


//==============================================================================
// ThreadSafeClientConnManager
//==============================================================================
ThreadSafeClientConnManager::ThreadSafeClientConnManager(
    /* [in] */ IHttpParams* params,
    /* [in] */ ISchemeRegistry* schreg)
{
    if (params == NULL) {
        Logger::E("ThreadSafeClientConnManager", "HTTP parameters may not be null");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSchemeRegistry = schreg;
    mConnOperator   = CreateConnectionOperator(schreg);
    mConnectionPool = CreateConnectionPool(params);
}

CAR_INTERFACE_IMPL(ThreadSafeClientConnManager, Object, IClientConnectionManager)

ThreadSafeClientConnManager::~ThreadSafeClientConnManager()
{
    Shutdown();
}

AutoPtr<AbstractConnPool> ThreadSafeClientConnManager::CreateConnectionPool(
    /* [in] */ IHttpParams* params)
{
    AutoPtr<AbstractConnPool> acp = (AbstractConnPool*)new ConnPoolByRoute(mConnOperator, params);
    Boolean conngc = TRUE; //@@@ check parameters to decide
    if (conngc) {
        acp->EnableConnectionGC();
    }
    return acp;
}

AutoPtr<IClientConnectionOperator> ThreadSafeClientConnManager::CreateConnectionOperator(
    /* [in] */ ISchemeRegistry* schreg)
{
    return (IClientConnectionOperator*)new DefaultClientConnectionOperator(schreg);
}

ECode ThreadSafeClientConnManager::GetSchemeRegistry(
    /* [out] */ ISchemeRegistry** schemeRegistry)
{
    VALIDATE_NOT_NULL(schemeRegistry)
    *schemeRegistry = mSchemeRegistry;
    REFCOUNT_ADD(*schemeRegistry)
    return NOERROR;
}

ECode ThreadSafeClientConnManager::RequestConnection(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [out] */ IClientConnectionRequest** request)
{
    VALIDATE_NOT_NULL(request)

    AutoPtr<IPoolEntryRequest> poolRequest = mConnectionPool->RequestPoolEntry(route, state);

    *request = (IClientConnectionRequest*)new MyClientConnectionRequest(route, poolRequest, this);
    REFCOUNT_ADD(*request)
    return NOERROR;
}

ECode ThreadSafeClientConnManager::ReleaseConnection(
    /* [in] */ IManagedClientConnection* conn,
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* timeUnit)
{
    AutoPtr<BasicPooledConnAdapter> hca
            = reinterpret_cast<BasicPooledConnAdapter*>(conn->Probe(EIID_BasicPooledConnAdapter));
    if (hca == NULL) {
        Logger::E("ThreadSafeClientConnManager", "Connection class mismatch, connection not obtained from this manager.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((hca->GetPoolEntry() != NULL) && (hca->GetManager().Get() != this)) {
        Logger::E("ThreadSafeClientConnManager", "Connection not obtained from this manager.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    // BEGIN android-changed
    // When recycling a Socket, we un-tag it to avoid collecting
    // statistics from future users.
    AutoPtr<BasicPoolEntry> entry = (BasicPoolEntry*)hca->GetPoolEntry().Get();
    AutoPtr<ISocket> socket;
    entry->GetConnection()->GetSocket((ISocket**)&socket);
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
    if ((hca->IsOpen(&isOpen), isOpen) && (hca->IsMarkedReusable(&isMarkedReusable), !isMarkedReusable)) {
        // if (log.isDebugEnabled()) {
        //     log.debug
        //         ("Released connection open but not marked reusable.");
        // }
        // In MTHCM, there would be a call to
        // SimpleHttpConnectionManager.finishLastResponse(conn);
        // Consuming the response is handled outside in 4.0.

        // make sure this connection will not be re-used
        // Shut down rather than close, we might have gotten here
        // because of a shutdown trigger.
        // Shutdown of the adapter also clears the tracked route.
        hca->Shutdown();
    }
    // } catch (IOException iox) {
    //     //@@@ log as warning? let pass?
    //     if (log.isDebugEnabled())
    //         log.debug("Exception shutting down released connection.",
    //                   iox);
    // } finally {
    //     BasicPoolEntry entry = (BasicPoolEntry) hca.getPoolEntry();
    //     boolean reusable = hca.isMarkedReusable();
    //     hca.detach();
    //     if (entry != null) {
    //         connectionPool.freeEntry(entry, reusable, validDuration, timeUnit);
    //     }
    // }
    entry = (BasicPoolEntry*)hca->GetPoolEntry().Get();
    Boolean reusable;
    hca->IsMarkedReusable(&reusable);
    hca->Detach();
    if (entry != NULL) {
        mConnectionPool->FreeEntry(entry, reusable, validDuration, timeUnit);
    }

    return NOERROR;
}

ECode ThreadSafeClientConnManager::Shutdown()
{
    mConnectionPool->Shutdown();
    return NOERROR;
}

Int32 ThreadSafeClientConnManager::GetConnectionsInPool(
    /* [in] */ IHttpRoute* route)
{
    return ((ConnPoolByRoute*)mConnectionPool.Get())->GetConnectionsInPool(route);
}

Int32 ThreadSafeClientConnManager::GetConnectionsInPool()
{
    Int32 value;
    synchronized(mConnectionPool) {
        value = mConnectionPool->mNumConnections;
    }
    return value;
}

ECode ThreadSafeClientConnManager::CloseIdleConnections(
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* timeUnit)
{
    // combine these two in a single call?
    mConnectionPool->CloseIdleConnections(validDuration, timeUnit);
    mConnectionPool->DeleteClosedConnections();
    return NOERROR;
}

ECode ThreadSafeClientConnManager::CloseExpiredConnections()
{
    mConnectionPool->CloseExpiredConnections();
    mConnectionPool->DeleteClosedConnections();
    return NOERROR;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org