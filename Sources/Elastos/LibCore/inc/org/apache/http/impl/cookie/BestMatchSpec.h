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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_BestMatchSpec_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_BestMatchSpec_H__

#include "org/apache/http/impl/cookie/RFC2965Spec.h"
#include "org/apache/http/impl/cookie/BrowserCompatSpec.h"
#include "org/apache/http/impl/cookie/NetScapeDraftSpec.h"

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

class BestMatchSpec
    : public Object
    , public ICookieSpec
{
public:
    BestMatchSpec(
        /* [in] */ ArrayOf<String>* datepatterns,
        /* [in] */ Boolean oneHeader);

    BestMatchSpec();

    CAR_INTERFACE_DECL()

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

    CARAPI FormatCookies(
        /* [in] */ IList* cookies,
        /* [out] */ IList** headers);

    CARAPI GetVersion(
        /* [out] */ Int32* version);

    CARAPI GetVersionHeader(
        /* [out] */ IHeader** header);

private:
    CARAPI_(AutoPtr<RFC2965Spec>) GetStrict();

    CARAPI_(AutoPtr<BrowserCompatSpec>) GetCompat();

    CARAPI_(AutoPtr<NetScapeDraftSpec>) GetNetscape();

private:
    AutoPtr< ArrayOf<String> > mDatepatterns;
    Boolean mOneHeader;

    AutoPtr<RFC2965Spec> mStrict;
    AutoPtr<BrowserCompatSpec> mCompat;
    AutoPtr<NetScapeDraftSpec> mNetscape;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BestMatchSpec_H__
