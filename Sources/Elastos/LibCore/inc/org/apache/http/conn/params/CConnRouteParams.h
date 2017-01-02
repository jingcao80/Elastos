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

#ifndef __ORG_APACHE_HTTP_CONN_PARAMS_CCONNROUTEPARAMS_H__
#define __ORG_APACHE_HTTP_CONN_PARAMS_CCONNROUTEPARAMS_H__

#include "_Org_Apache_Http_Conn_Params_CConnRouteParams.h"
#include "elastos/core/Singleton.h"

using Elastos::Net::IInetAddress;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CarClass(CConnRouteParams)
    , public Singleton
    , public IConnRouteParams
    , public IConnRoutePNames
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Obtains the {@link ConnRoutePNames#DEFAULT_PROXY DEFAULT_PROXY}
     * parameter value.
     * {@link #NO_HOST} will be mapped to <code>null</code>,
     * to allow unsetting in a hierarchy.
     *
     * @param params    the parameters in which to look up
     *
     * @return  the default proxy set in the argument parameters, or
     *          <code>null</code> if not set
     */
    CARAPI GetDefaultProxy(
        /* [in] */ IHttpParams* params,
        /* [out] */ IHttpHost** proxy);

    /**
     * Sets the {@link ConnRoutePNames#DEFAULT_PROXY DEFAULT_PROXY}
     * parameter value.
     *
     * @param params    the parameters in which to set the value
     * @param proxy     the value to set, may be <code>null</code>.
     *                  Note that {@link #NO_HOST} will be mapped to
     *                  <code>null</code> by {@link #getDefaultProxy},
     *                  to allow for explicit unsetting in hierarchies.
     */
    CARAPI SetDefaultProxy(
        /* [in] */ IHttpParams* params,
        /* [in] */ IHttpHost* proxy);

    /**
     * Obtains the {@link ConnRoutePNames#FORCED_ROUTE FORCED_ROUTE}
     * parameter value.
     * {@link #NO_ROUTE} will be mapped to <code>null</code>,
     * to allow unsetting in a hierarchy.
     *
     * @param params    the parameters in which to look up
     *
     * @return  the forced route set in the argument parameters, or
     *          <code>null</code> if not set
     */
    CARAPI GetForcedRoute(
        /* [in] */ IHttpParams* params,
        /* [out] */ IHttpRoute** route);

    /**
     * Sets the {@link ConnRoutePNames#FORCED_ROUTE FORCED_ROUTE}
     * parameter value.
     *
     * @param params    the parameters in which to set the value
     * @param route     the value to set, may be <code>null</code>.
     *                  Note that {@link #NO_ROUTE} will be mapped to
     *                  <code>null</code> by {@link #getForcedRoute},
     *                  to allow for explicit unsetting in hierarchies.
     */
    CARAPI SetForcedRoute(
        /* [in] */ IHttpParams* params,
        /* [in] */ IHttpRoute* route);

    /**
     * Obtains the {@link ConnRoutePNames#LOCAL_ADDRESS LOCAL_ADDRESS}
     * parameter value.
     * There is no special value that would automatically be mapped to
     * <code>null</code>. You can use the wildcard address (0.0.0.0 for IPv4,
     * :: for IPv6) to override a specific local address in a hierarchy.
     *
     * @param params    the parameters in which to look up
     *
     * @return  the local address set in the argument parameters, or
     *          <code>null</code> if not set
     */
    CARAPI GetLocalAddress(
        /* [in] */ IHttpParams* params,
        /* [out] */ IInetAddress** local);

    /**
     * Sets the {@link ConnRoutePNames#LOCAL_ADDRESS LOCAL_ADDRESS}
     * parameter value.
     *
     * @param params    the parameters in which to set the value
     * @param local     the value to set, may be <code>null</code>
     */
    CARAPI SetLocalAddress(
        /* [in] */ IHttpParams* params,
        /* [in] */ IInetAddress* local);

    CARAPI GetNoHost(
        /* [out] */ IHttpHost** host);

    CARAPI GetNoRoute(
        /* [out] */ IHttpRoute** route);
};

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_PARAMS_CCONNROUTEPARAMS_H__
