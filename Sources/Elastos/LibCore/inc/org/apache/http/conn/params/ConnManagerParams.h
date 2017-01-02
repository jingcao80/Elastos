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

#ifndef __ORG_APACHE_HTTP_CONN_PARAMS_CONNMANAGERPARAMS_H__
#define __ORG_APACHE_HTTP_CONN_PARAMS_CONNMANAGERPARAMS_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

/**
 * This class represents a collection of HTTP protocol parameters applicable
 * to client-side
 * {@link org.apache.http.conn.ClientConnectionManager connection managers}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author Michael Becke
 *
 * @version $Revision: 658785 $
 *
 * @since 4.0
 *
 * @see ConnManagerPNames
 */
class ConnManagerParams
    : public Object
    , public IConnManagerPNames
{
private:
    class DefaultConnPerRoute
        : public Object
        , public IConnPerRoute
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetMaxForRoute(
            /* [in] */ IHttpRoute* route,
            /* [out] */ Int32* max);
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the timeout in milliseconds used when retrieving a
     * {@link org.apache.http.conn.ManagedClientConnection} from the
     * {@link org.apache.http.conn.ClientConnectionManager}.
     *
     * @return timeout in milliseconds.
     */
    static CARAPI GetTimeout(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int64* timeout);

    /**
     * Sets the timeout in milliseconds used when retrieving a
     * {@link org.apache.http.conn.ManagedClientConnection} from the
     * {@link org.apache.http.conn.ClientConnectionManager}.
     *
     * @param timeout the timeout in milliseconds
     */
    static CARAPI SetTimeout(
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
    static CARAPI SetMaxConnectionsPerRoute(
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
    static CARAPI GetMaxConnectionsPerRoute(
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
    static CARAPI SetMaxTotalConnections(
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
    static CARAPI GetMaxTotalConnections(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* number);

private:
    static CARAPI_(AutoPtr<IConnPerRoute>) InitDefaultRoute();

private:
    /** The default maximum number of connections allowed per host */
    static const AutoPtr<IConnPerRoute> DEFAULT_CONN_PER_ROUTE;

};

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_PARAMS_CONNMANAGERPARAMS_H__
