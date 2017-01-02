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

#ifndef __ORG_APACHE_HTTP_IMPL_DEFAULTCONNECTIONREUSESTRATEGY_H__
#define __ORG_APACHE_HTTP_IMPL_DEFAULTCONNECTIONREUSESTRATEGY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::ITokenIterator;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Default implementation of a strategy deciding about connection re-use.
 * The default implementation first checks some basics, for example
 * whether the connection is still open or whether the end of the
 * request entity can be determined without closing the connection.
 * If these checks pass, the tokens in the "Connection" header will
 * be examined. In the absence of a "Connection" header, the
 * non-standard but commonly used "Proxy-Connection" header takes
 * it's role. A token "close" indicates that the connection cannot
 * be reused. If there is no such token, a token "keep-alive" indicates
 * that the connection should be re-used. If neither token is found,
 * or if there are no "Connection" headers, the default policy for
 * the HTTP version is applied. Since HTTP/1.1, connections are re-used
 * by default. Up until HTTP/1.0, connections are not re-used by default.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 * @version $Revision: 602537 $
 *
 * @since 4.0
 */
class DefaultConnectionReuseStrategy
    : public Object
    , public IConnectionReuseStrategy
{
public:
    DefaultConnectionReuseStrategy();

    CAR_INTERFACE_DECL()

    CARAPI KeepAlive(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);

protected:
    CARAPI CreateTokenIterator(
        /* [in] */ IHeaderIterator* hit,
        /* [out] */ ITokenIterator** tit);
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_DEFAULTCONNECTIONREUSESTRATEGY_H__
