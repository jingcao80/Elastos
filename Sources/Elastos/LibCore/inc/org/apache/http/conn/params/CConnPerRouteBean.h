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

#ifndef __ORG_APACHE_HTTP_CONN_PARAMS_CCONNPERROUTEBEANS_H_
#define __ORG_APACHE_HTTP_CONN_PARAMS_CCONNPERROUTEBEANS_H_

#include "_Org_Apache_Http_Conn_Params_CConnPerRouteBean.h"
#include "elastos/core/Object.h"
#include "elastos/utility/etl/HashMap.h"

using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IMap;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Params::IHttpParams;

DEFINE_OBJECT_HASH_FUNC_FOR(Org::Apache::Http::Conn::Routing::IHttpRoute)

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

/**
 * This class maintains a map of HTTP routes to maximum number of connections allowed
 * for those routes. This class can be used by pooling
 * {@link org.apache.http.conn.ClientConnectionManager connection managers} for
 * a fine-grained control of connections on a per route basis.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 652947 $
 *
 * @since 4.0
 */
CarClass(CConnPerRouteBean)
    , public Object
    , public IConnPerRouteBean
    , public IConnPerRoute
{
public:
    CConnPerRouteBean();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetDefaultMax(
        /* [out] */ Int32* max);

    CARAPI SetDefaultMaxPerRoute(
        /* [in] */ Int32 max);

    CARAPI SetMaxForRoute(
        /* [in] */ IHttpRoute* route,
        /* [in] */ Int32 max);

    CARAPI GetMaxForRoute(
        /* [in] */ IHttpRoute* route,
        /* [out] */ Int32* number);

    CARAPI SetMaxForRoutes(
        /* [in] */ IMap* map);

    CARAPI constructor(
        /* [in] */ Int32 defaultMax);

    CARAPI constructor();

private:
    HashMap<AutoPtr<IHttpRoute>, AutoPtr<IInteger32> > mMaxPerHostMap;

    Int32 mDefaultMax;

};

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_PARAMS_CCONNPERROUTEBEANS_H_
