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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109SPEC_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109SPEC_H__

#include "org/apache/http/impl/cookie/CookieSpecBase.h"
#include "org/apache/http/cookie/CCookiePathComparator.h"

using Elastos::Utility::IList;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookieOrigin;
using Org::Apache::Http::Cookie::CCookiePathComparator;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * RFC 2109 compliant cookie policy
 *
 * @author  B.C. Holmes
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:dsale@us.britannica.com">Doug Sale</a>
 * @author Rod Waldhoff
 * @author dIon Gillard
 * @author Sean C. Sullivan
 * @author <a href="mailto:JEvans@Cyveillance.com">John Evans</a>
 * @author Marc A. Saegesser
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @since 4.0
 */
class RFC2109Spec : public CookieSpecBase
{
public:
    RFC2109Spec(
        /* [in] */ ArrayOf<String>* datepatterns,
        /* [in] */ Boolean oneHeader);

    RFC2109Spec();

    using CookieSpecBase::Parse;

    CARAPI Parse(
        /* [in] */ IHeader* header,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);

    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI FormatCookies(
        /* [in] */ IList* cookies,
        /* [out] */ IList** headers);

    CARAPI GetVersion(
        /* [out] */ Int32* version);

    CARAPI GetVersionHeader(
        /* [out] */ IHeader** header);

protected:
    /**
     * Return a name/value string suitable for sending in a <tt>"Cookie"</tt>
     * header as defined in RFC 2109 for backward compatibility with cookie
     * version 0
     * @param buffer The char array buffer to use for output
     * @param name The cookie name
     * @param value The cookie value
     * @param version The cookie version
     */
    CARAPI_(void) FormatParamAsVer(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ Int32 version);

    /**
     * Return a string suitable for sending in a <tt>"Cookie"</tt> header
     * as defined in RFC 2109 for backward compatibility with cookie version 0
     * @param buffer The char array buffer to use for output
     * @param cookie The {@link Cookie} to be formatted as string
     * @param version The version to use.
     */
    CARAPI_(void) FormatCookieAsVer(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ ICookie* cookie,
        /* [in] */ Int32 version);

private:
    CARAPI_(void) Init(
        /* [in] */ ArrayOf<String>* datepatterns,
        /* [in] */ Boolean oneHeader);

    CARAPI_(AutoPtr<IList>) DoFormatOneHeader(
        /* [in] */ IList* cookies);

    CARAPI_(AutoPtr<IList>) DoFormatManyHeaders(
        /* [in] */ IList* cookies);

private:
    static const AutoPtr<CCookiePathComparator> PATH_COMPARATOR;

    static const AutoPtr< ArrayOf<String> > DATE_PATTERNS;

    AutoPtr< ArrayOf<String> > mDatepatterns;
    Boolean mOneHeader;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109Spec_H__
