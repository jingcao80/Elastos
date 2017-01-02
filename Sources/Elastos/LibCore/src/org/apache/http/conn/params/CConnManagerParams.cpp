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

#include "org/apache/http/conn/params/CConnManagerParams.h"
#include "org/apache/http/conn/params/ConnManagerParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL_2(CConnManagerParams, Object, IConnManagerParams, IConnManagerPNames)

CAR_SINGLETON_IMPL(CConnManagerParams)

ECode CConnManagerParams::GetTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int64* timeout)
{
    return ConnManagerParams::GetTimeout(params, timeout);
}

ECode CConnManagerParams::SetTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int64 timeout)
{
    return ConnManagerParams::SetTimeout(params, timeout);
}

ECode CConnManagerParams::SetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IConnPerRoute* connPerRoute)
{
    return ConnManagerParams::SetMaxConnectionsPerRoute(params, connPerRoute);
}

ECode CConnManagerParams::GetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IConnPerRoute** route)
{
    return ConnManagerParams::GetMaxConnectionsPerRoute(params, route);
}

ECode CConnManagerParams::SetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 maxTotalConnections)
{
    return ConnManagerParams::SetMaxTotalConnections(params, maxTotalConnections);
}

ECode CConnManagerParams::GetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* number)
{
    return ConnManagerParams::GetMaxTotalConnections(params, number);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org