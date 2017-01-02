//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRY_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/conn/AbstractPoolEntry.h"

using Elastos::Utility::IQueue;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Impl::Conn::AbstractPoolEntry;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

class BasicPoolEntryRef;
class AbstractConnPool;
class ConnPoolByRoute;
class RouteSpecificPool;
class ThreadSafeClientConnManager;

extern "C" const InterfaceID EIID_BasicPoolEntry;

/**
 * Basic implementation of a connection pool entry.
 */
class BasicPoolEntry : public AbstractPoolEntry
{
public:
    /**
     * Creates a new pool entry.
     *
     * @param op      the connection operator
     * @param route   the planned route for the connection
     * @param queue   the reference queue for tracking GC of this entry,
     *                or <code>null</code>
     */
    BasicPoolEntry(
        /* [in] */ IClientConnectionOperator* op,
        /* [in] */ IHttpRoute* route,
        /* [in] */ IQueue* queue);/* ReferenceQueue<Object> */

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

protected:
    CARAPI_(AutoPtr<IOperatedClientConnection>) GetConnection();

    CARAPI_(AutoPtr<IHttpRoute>) GetPlannedRoute();

    CARAPI_(AutoPtr<BasicPoolEntryRef>) GetWeakRef();

private:
    /**
     * A weak reference to <code>this</code> used to detect GC of entries.
     * Pool entries can only be GCed when they are allocated by an application
     * and therefore not referenced with a hard link in the manager.
     */
    AutoPtr<BasicPoolEntryRef> mReference;

    friend class AbstractConnPool;
    friend class BasicPoolEntryRef;
    friend class ConnPoolByRoute;
    friend class RouteSpecificPool;
    friend class ThreadSafeClientConnManager;
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRY_H__
