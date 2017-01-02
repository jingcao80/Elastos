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

#ifndef __ORG_APACHE_HTTP_CONN_CBASICMANAGEDENTITY_H_
#define __ORG_APACHE_HTTP_CONN_CBASICMANAGEDENTITY_H_

#include "_Org_Apache_Http_Conn_CBasicManagedEntity.h"
#include "org/apache/http/entity/HttpEntityWrapper.h"

using Elastos::IO::IInputStream;
using Org::Apache::Http::Entity::HttpEntityWrapper;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

/**
 * An entity that releases a {@link ManagedClientConnection connection}.
 * A {@link ManagedClientConnection} will
 * typically <i>not</i> return a managed entity, but you can replace
 * the unmanaged entity in the response with a managed one.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 672367 $
 *
 * @since 4.0
 */
CarClass(CBasicManagedEntity)
    , public HttpEntityWrapper
    , public IConnectionReleaseTrigger
    , public IEofSensorWatcher
{
public:
    CBasicManagedEntity()
        : mAttemptReuse(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI ConsumeContent();

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI ReleaseConnection();

    CARAPI AbortConnection();

    CARAPI EofDetected(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    CARAPI StreamClosed(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    CARAPI StreamAbort(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IHttpEntity* entity,
        /* [in] */ IManagedClientConnection* conn,
        /* [in] */ Boolean reuse);

protected:
    /**
     * Releases the connection gracefully.
     * The connection attribute will be nullified.
     * Subsequent invocations are no-ops.
     *
     * @throws IOException      in case of an IO problem.
     *         The connection attribute will be nullified anyway.
     */
    CARAPI ReleaseManagedConnection();

protected:
    /** The connection to auto-release. */
    AutoPtr<IManagedClientConnection> mManagedConn;

    /** Whether to keep the connection alive. */
    Boolean mAttemptReuse;
};

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_CBASICMANAGEDENTITY_H_
