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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_BROWSERCOMPATSPEC_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_BROWSERCOMPATSPEC_H__

#include "org/apache/http/impl/cookie/CookieSpecBase.h"

using Elastos::Utility::IList;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class BestMatchSpec;

/**
 * Cookie specification that strives to closely mimic (mis)behavior of
 * common web browser applications such as Microsoft Internet Explorer
 * and Mozilla FireFox.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class BrowserCompatSpec : public CookieSpecBase
{
public:
    BrowserCompatSpec(
        /* [in] */ ArrayOf<String>* datepatterns);

    BrowserCompatSpec();

    using CookieSpecBase::Parse;

    CARAPI Parse(
        /* [in] */ IHeader* header,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);

    CARAPI FormatCookies(
        /* [in] */ IList* cookies,
        /* [out] */ IList** headers);

    CARAPI GetVersion(
        /* [out] */ Int32* version);

    CARAPI GetVersionHeader(
        /* [out] */ IHeader** header);

private:
    CARAPI_(void) Init(
        /* [in] */ ArrayOf<String>* datepatterns);

protected:
    /** Valid date patterns used per default */
    static const AutoPtr< ArrayOf<String> > DATE_PATTERNS;

private:
    AutoPtr< ArrayOf<String> > mDatepatterns;

    friend class BestMatchSpec;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BROWSERCOMPATSPEC_H__
