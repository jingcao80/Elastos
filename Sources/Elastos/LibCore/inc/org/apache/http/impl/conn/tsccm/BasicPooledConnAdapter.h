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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLEDCONNADAPTER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLEDCONNADAPTER_H__

#include "org/apache/http/impl/conn/AbstractPooledConnAdapter.h"
#include "org/apache/http/impl/conn/AbstractPoolEntry.h"
#include "org/apache/http/impl/conn/tsccm/ThreadSafeClientConnManager.h"

using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Impl::Conn::AbstractPooledConnAdapter;
using Org::Apache::Http::Impl::Conn::AbstractPoolEntry;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPooledConnAdapter;

/**
 * A connection wrapper and callback handler.
 * All connections given out by the manager are wrappers which
 * can be {@link #detach detach}ed to prevent further use on release.
 */
class BasicPooledConnAdapter : public AbstractPooledConnAdapter
{
protected:
    /**
     * Creates a new adapter.
     *
     * @param tsccm   the connection manager
     * @param entry   the pool entry for the connection being wrapped
     */
    BasicPooledConnAdapter(
        /* [in] */ ThreadSafeClientConnManager* tsccm,
        /* [in] */ AbstractPoolEntry* entry);

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(AutoPtr<IClientConnectionManager>) GetManager();

    /**
     * Obtains the pool entry.
     *
     * @return  the pool entry, or <code>null</code> if detached
     */
    CARAPI_(AutoPtr<AbstractPoolEntry>) GetPoolEntry();

    CARAPI_(void) Detach();

private:
    friend class ThreadSafeClientConnManager;
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLEDCONNADAPTER_H__
