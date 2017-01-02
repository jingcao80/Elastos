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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTHTTPROUTEPLANNER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTHTTPROUTEPLANNER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Routing::IHttpRoutePlanner;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Default implementation of an {@link HttpRoutePlanner}.
 * This implementation is based on
 * {@link org.apache.http.conn.params.ConnRoutePNames parameters}.
 * It will not make use of any Java system properties,
 * nor of system or browser proxy settings.
 */
class DefaultHttpRoutePlanner
    : public Object
    , public IHttpRoutePlanner
{
public:
    /**
     * Creates a new client connection operator for the given scheme registry.
     *
     * @param schemes   the scheme registry
     */
    DefaultHttpRoutePlanner(
        /* [in] */ ISchemeRegistry* schreg);

    CAR_INTERFACE_DECL()

    CARAPI DetermineRoute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpRoute** httpRoute);

protected:
    /** The scheme registry. */
    AutoPtr<ISchemeRegistry> mSchemeRegistry;

};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTHTTPROUTEPLANNER_H__
