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

#include "org/apache/http/conn/params/ConnManagerParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

/////////////////////////////////////////////////////
// ConnManagerParams::DefaultConnPerRoute
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ConnManagerParams::DefaultConnPerRoute, Object, IConnPerRoute)

ECode ConnManagerParams::DefaultConnPerRoute::GetMaxForRoute(
    /* [in] */ IHttpRoute* route,
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max)
    *max = IConnPerRouteBean::DEFAULT_MAX_CONNECTIONS_PER_ROUTE;
    return NOERROR;
}


/////////////////////////////////////////////////////
// ConnManagerParams
/////////////////////////////////////////////////////

AutoPtr<IConnPerRoute> ConnManagerParams::InitDefaultRoute()
{
    AutoPtr<IConnPerRoute> route = (IConnPerRoute*)new ConnManagerParams::DefaultConnPerRoute();
    return route;
}
const AutoPtr<IConnPerRoute> ConnManagerParams::DEFAULT_CONN_PER_ROUTE = ConnManagerParams::InitDefaultRoute();

CAR_INTERFACE_IMPL(ConnManagerParams, Object, IConnManagerPNames)

ECode ConnManagerParams::GetTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    *timeout = 0;
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt64Parameter(TIMEOUT, 0, timeout);
}

ECode ConnManagerParams::SetTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int64 timeout)
{
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetInt64Parameter(TIMEOUT, timeout,
            (IHttpParams**)&p);
}

ECode ConnManagerParams::SetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IConnPerRoute* connPerRoute)
{
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    params->SetParameter(MAX_CONNECTIONS_PER_ROUTE, connPerRoute,
            (IHttpParams**)&p);
    return NOERROR;
}

ECode ConnManagerParams::GetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IConnPerRoute** route)
{
    VALIDATE_NOT_NULL(route)
    *route = NULL;
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(MAX_CONNECTIONS_PER_ROUTE, (IInterface**)&o);
    AutoPtr<IConnPerRoute> connPerRoute = IConnPerRoute::Probe(o);
    if (connPerRoute == NULL) {
        connPerRoute = DEFAULT_CONN_PER_ROUTE;
    }
    *route = connPerRoute;
    REFCOUNT_ADD(*route)
    return NOERROR;
}

ECode ConnManagerParams::SetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 maxTotalConnections)
{
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    params->SetInt32Parameter(MAX_TOTAL_CONNECTIONS, maxTotalConnections,
            (IHttpParams**)&p);
    return NOERROR;
}

ECode ConnManagerParams::GetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    if (params == NULL) {
        Logger::E("ConnManagerParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt32Parameter(MAX_TOTAL_CONNECTIONS, IConnManagerParams::DEFAULT_MAX_TOTAL_CONNECTIONS, number);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org