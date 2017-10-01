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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__

#include "org/apache/http/impl/cookie/AbstractCookieSpec.h"

using Elastos::Utility::IList;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * Cookie management functions shared by all specification.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class CookieSpecBase : public AbstractCookieSpec
{
public:
    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI Match(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);

protected:
    static CARAPI_(String) GetDefaultPath(
        /* [in] */ ICookieOrigin* origin);

    static CARAPI_(String) GetDefaultDomain(
        /* [in] */ ICookieOrigin* origin);

    using AbstractCookieSpec::Parse;

    CARAPI Parse(
        /* [in] */ ArrayOf<IHeaderElement*>* elems,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__
