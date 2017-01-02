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

#ifndef __ORG_APACHE_HTTP_CONN_PARAMS_CCONNMANAGERPARAMS_H__
#define __ORG_APACHE_HTTP_CONN_PARAMS_CCONNMANAGERPARAMS_H__

#include "_Org_Apache_Http_Conn_Params_CConnManagerParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CarClass(CConnManagerParams)
    , public Singleton
    , public IConnManagerParams
    , public IConnManagerPNames
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the timeout in milliseconds used when retrieving a
     * {@link org.apache.http.conn.ManagedClientConnection} from the
     * {@link org.apache.http.conn.ClientConnectionManager}.
     *
     * @return timeout in milliseconds.
     */
    CARAPI GetTimeout(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int64* timeout);

    /**
     * Sets the timeout in milliseconds used when retrieving a
     * {@link org.apache.http.conn.ManagedClientConnection} from the
     * {@link org.apache.http.conn.ClientConnectionManager}.
     *
     * @param timeout the timeout in milliseconds
     */
    CARAPI SetTimeout(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int64 timeout);

    /**
     * Sets lookup interface for maximum number of connections allowed per route.
     *
     * @param params HTTP parameters
     * @param connPerRoute lookup interface for maximum number of connections allowed
     *        per route
     *
     * @see ConnManagerPNames#MAX_CONNECTIONS_PER_ROUTE
     */
    CARAPI SetMaxConnectionsPerRoute(
        /* [in] */ IHttpParams* params,
        /* [in] */ IConnPerRoute* connPerRoute);

    /**
     * Returns lookup interface for maximum number of connections allowed per route.
     *
     * @param params HTTP parameters
     *
     * @return lookup interface for maximum number of connections allowed per route.
     *
     * @see ConnManagerPNames#MAX_CONNECTIONS_PER_ROUTE
     */
    CARAPI GetMaxConnectionsPerRoute(
        /* [in] */ IHttpParams* params,
        /* [out] */ IConnPerRoute** route);

    /**
     * Sets the maximum number of connections allowed.
     *
     * @param params HTTP parameters
     * @param maxTotalConnections The maximum number of connections allowed.
     *
     * @see ConnManagerPNames#MAX_TOTAL_CONNECTIONS
     */
    CARAPI SetMaxTotalConnections(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int32 maxTotalConnections);

    /**
     * Gets the maximum number of connections allowed.
     *
     * @param params HTTP parameters
     *
     * @return The maximum number of connections allowed.
     *
     * @see ConnManagerPNames#MAX_TOTAL_CONNECTIONS
     */
    CARAPI GetMaxTotalConnections(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* number);
};

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_PARAMS_CCONNMANAGERPARAMS_H__
