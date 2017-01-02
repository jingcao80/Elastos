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

#include "org/apache/http/conn/params/CConnRouteParams.h"
#include "org/apache/http/conn/params/ConnRouteParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL_2(CConnRouteParams, Singleton, IConnRouteParams, IConnRoutePNames)

CAR_SINGLETON_IMPL(CConnRouteParams)

ECode CConnRouteParams::GetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpHost** proxy)
{
    return ConnRouteParams::GetDefaultProxy(params, proxy);
}

ECode CConnRouteParams::SetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpHost* proxy)
{
    return ConnRouteParams::SetDefaultProxy(params, proxy);
}

ECode CConnRouteParams::GetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpRoute** route)
{
    return ConnRouteParams::GetForcedRoute(params, route);
}

ECode CConnRouteParams::SetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpRoute* route)
{
    return ConnRouteParams::SetForcedRoute(params, route);
}

ECode CConnRouteParams::GetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [out] */ IInetAddress** local)
{
    return ConnRouteParams::GetLocalAddress(params, local);
}

ECode CConnRouteParams::SetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [in] */ IInetAddress* local)
{
    return ConnRouteParams::SetLocalAddress(params, local);
}

ECode CConnRouteParams::GetNoHost(
    /* [out] */ IHttpHost** host)
{
    VALIDATE_NOT_NULL(host)
    *host = ConnRouteParams::NO_HOST;
    REFCOUNT_ADD(*host)
    return NOERROR;
}

ECode CConnRouteParams::GetNoRoute(
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)
    *route = ConnRouteParams::NO_ROUTE;
    REFCOUNT_ADD(*route)
    return NOERROR;
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org