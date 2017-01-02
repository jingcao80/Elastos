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

#ifndef __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CRESPONSEPROCESSCOOKIES_H_
#define __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CRESPONSEPROCESSCOOKIES_H_

#include "_Org_Apache_Http_Client_Protocol_CResponseProcessCookies.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpResponseInterceptor;
using Org::Apache::Http::Client::ICookieStore;
using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

/**
 * Response interceptor that populates the current {@link CookieStore} with data
 * contained in response cookies received in the given the HTTP response.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 673450 $
 *
 * @since 4.0
 */
CarClass(CResponseProcessCookies)
    , public Object
    , public IHttpResponseInterceptor
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Process(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* contexT);

    CARAPI constructor();

private:
    CARAPI_(void) ProcessCookies(
        /* [in] */ IHeaderIterator* iterator,
        /* [in] */ ICookieSpec* cookieSpec,
        /* [in] */ ICookieOrigin* cookieOrigin,
        /* [in] */ ICookieStore* cookieStore);

    /**
     * Don't log the cookie's value; that's potentially sensitive information.
     */
    CARAPI_(String) CookieToString(
        /* [in] */ ICookie* cookie);
};

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CRESPONSEPROCESSCOOKIES_H_
