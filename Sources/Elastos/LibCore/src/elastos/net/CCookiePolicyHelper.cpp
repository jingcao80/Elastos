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

#include "CCookiePolicyHelper.h"
#include "CHttpCookie.h"

namespace Elastos {
namespace Net {

//========================================================================
// CCookiePolicyHelper::CookiePolicyAcceptAll
//========================================================================

CAR_INTERFACE_IMPL(CCookiePolicyHelper::CookiePolicyAcceptAll, Object, ICookiePolicy)

ECode CCookiePolicyHelper::CookiePolicyAcceptAll::ShouldAccept(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

//========================================================================
// CCookiePolicyHelper::CookiePolicyAcceptNone
//========================================================================

CAR_INTERFACE_IMPL(CCookiePolicyHelper::CookiePolicyAcceptNone, Object, ICookiePolicy)

ECode CCookiePolicyHelper::CookiePolicyAcceptNone::ShouldAccept(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

//========================================================================
// CCookiePolicyHelper::CookiePolicyAcceptOriginalServer
//========================================================================

CAR_INTERFACE_IMPL(CCookiePolicyHelper::CookiePolicyAcceptOriginalServer, Object, ICookiePolicy)

ECode CCookiePolicyHelper::CookiePolicyAcceptOriginalServer::ShouldAccept(
    /* [in] */ IURI* uri,
    /* [in] */ IHttpCookie* cookie,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((NULL == uri) || (NULL == cookie)) return E_NULL_POINTER_EXCEPTION;

    String domain, host;
    cookie->GetDomainAttr(&domain);
    uri->GetHost(&host);
    *result = CHttpCookie::IsDomainMatches(domain, host);
    return NOERROR;
}


//========================================================================
// CCookiePolicyHelper
//========================================================================

INIT_PROI_3 AutoPtr<ICookiePolicy> CCookiePolicyHelper::ACCEPT_ALL = new CookiePolicyAcceptAll();
INIT_PROI_3 AutoPtr<ICookiePolicy> CCookiePolicyHelper::ACCEPT_NONE = new CookiePolicyAcceptNone();
INIT_PROI_3 AutoPtr<ICookiePolicy> CCookiePolicyHelper::ACCEPT_ORIGINAL_SERVER = new CookiePolicyAcceptOriginalServer();

CAR_INTERFACE_IMPL(CCookiePolicyHelper, Singleton, ICookiePolicyHelper)

CAR_SINGLETON_IMPL(CCookiePolicyHelper)

ECode CCookiePolicyHelper::GetCookiePolicy(
    /* [in] */ CookiePolicyKind kind,
    /* [out] */ ICookiePolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    AutoPtr<ICookiePolicy> _policy = GetCookiePolicy(kind);
    if (_policy == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;

    *policy = _policy.Get();
    REFCOUNT_ADD(*policy);

    return NOERROR;
}

AutoPtr<ICookiePolicy> CCookiePolicyHelper::GetCookiePolicy(
    /* [in] */ CookiePolicyKind kind)
{
    if (kind == CookiePolicyKind_ACCEPT_ALL) {
        return ACCEPT_ALL;
    }
    else if (kind == CookiePolicyKind_ACCEPT_NONE) {
        return ACCEPT_NONE;
    }
    else if (kind == CookiePolicyKind_ACCEPT_ORIGINAL_SERVER) {
        return ACCEPT_ORIGINAL_SERVER;
    }
    else {
        return NULL;
    }
}

} // namespace Net
} // namespace Elastos
