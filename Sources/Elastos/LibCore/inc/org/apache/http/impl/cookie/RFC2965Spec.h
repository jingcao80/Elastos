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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965SPEC_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965SPEC_H__

#include "org/apache/http/impl/cookie/RFC2109Spec.h"
#include "org/apache/http/impl/cookie/BasicClientCookie.h"

using Org::Apache::Http::Cookie::ISetCookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class BestMatchSpec;

/**
 * <p>RFC 2965 specific cookie management functions.</p>
 *
 * @author jain.samit@gmail.com (Samit Jain)
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 3.1
 */
class RFC2965Spec : public RFC2109Spec
{
public:
    RFC2965Spec(
        /* [in] */ ArrayOf<String>* datepatterns,
        /* [in] */ Boolean oneHeader);

    RFC2965Spec();

    using CookieSpecBase::Parse;

    CARAPI Parse(
        /* [in] */ IHeader* header,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);

    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI Match(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);

    CARAPI GetVersion(
        /* [out] */ Int32* version);

    CARAPI GetVersionHeader(
        /* [out] */ IHeader** header);

protected:
    /**
     * Adds valid Port attribute value, e.g. "8000,8001,8002"
     */
    CARAPI_(void) FormatCookieAsVer(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ ICookie* cookie,
        /* [in] */ Int32 version);

private:
    CARAPI_(void) Init(
        /* [in] */ ArrayOf<String>* datepatterns,
        /* [in] */ Boolean oneHeader);

    CARAPI_(AutoPtr<BasicClientCookie>) CreateCookie(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ ICookieOrigin* origin);

    CARAPI_(AutoPtr<BasicClientCookie>) CreateCookie2(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ ICookieOrigin* origin);

    /**
     * Set 'effective host name' as defined in RFC 2965.
     * <p>
     * If a host name contains no dots, the effective host name is
     * that name with the string .local appended to it.  Otherwise
     * the effective host name is the same as the host name.  Note
     * that all effective host names contain at least one dot.
     *
     * @param origin origin where cookie is received from or being sent to.
     * @return
     */
    static CARAPI_(AutoPtr<ICookieOrigin>) AdjustEffectiveHost(
        /* [in] */ ICookieOrigin* origin);

private:
    friend class BestMatchSpec;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965SPEC_H__
