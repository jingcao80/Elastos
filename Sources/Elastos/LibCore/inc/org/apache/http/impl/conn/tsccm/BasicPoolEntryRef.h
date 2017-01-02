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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRYREF_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRYREF_H__

#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"

using Org::Apache::Http::Conn::Routing::IHttpRoute;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPoolEntryRef;

/**
 * A weak reference to a {@link BasicPoolEntry BasicPoolEntry}.
 * This reference explicitly keeps the planned route, so the connection
 * can be reclaimed if it is lost to garbage collection.
 */
class BasicPoolEntryRef : public Object
{
public:
    /**
     * Creates a new reference to a pool entry.
     *
     * @param entry   the pool entry, must not be <code>null</code>
     * @param queue   the reference queue, or <code>null</code>
     */
    BasicPoolEntryRef(
        /* [in] */ BasicPoolEntry* entry,
        /* [in] */ IQueue* queue);

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(AutoPtr<IHttpRoute>) GetRoute();

private:
    /** The planned route of the entry. */
    AutoPtr<IHttpRoute> mRoute;

};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_BASICPOOLENTRYREF_H__
