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

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/conn/params/CConnRouteParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL(CConnRouteParamBean, Object, IConnRouteParamBean)

CAR_OBJECT_IMPL(CConnRouteParamBean)

ECode CConnRouteParamBean::SetDefaultProxy(
    /* [in] */ IHttpHost* defaultProxy)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::DEFAULT_PROXY, defaultProxy,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::SetLocalAddress(
    /* [in] */ IInetAddress* address)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::LOCAL_ADDRESS, address,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::SetForcedRoute(
    /* [in] */ IHttpRoute* route)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::FORCED_ROUTE, route,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
