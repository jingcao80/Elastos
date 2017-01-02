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

#include "org/apache/http/client/params/CHttpClientParams.h"
#include "org/apache/http/client/params/HttpClientParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

CAR_INTERFACE_IMPL(CHttpClientParams, Object, IHttpClientParams)

CAR_SINGLETON_IMPL(CHttpClientParams)

ECode CHttpClientParams::IsRedirecting(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* isRedirecting)
{
    VALIDATE_NOT_NULL(isRedirecting)
    return HttpClientParams::IsRedirecting(params, isRedirecting);
}

ECode CHttpClientParams::SetRedirecting(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    return HttpClientParams::SetRedirecting(params, value);
}

ECode CHttpClientParams::IsAuthenticating(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* isAuthenticating)
{
    VALIDATE_NOT_NULL(isAuthenticating)
    return HttpClientParams::IsAuthenticating(params, isAuthenticating);
}

ECode CHttpClientParams::SetAuthenticating(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    return HttpClientParams::SetAuthenticating(params, value);
}

ECode CHttpClientParams::GetCookiePolicy(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* cookiePolicy)
{
    VALIDATE_NOT_NULL(cookiePolicy)
    return HttpClientParams::GetCookiePolicy(params, cookiePolicy);
}

ECode CHttpClientParams::SetCookiePolicy(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& cookiePolicy)
{
    return HttpClientParams::SetCookiePolicy(params, cookiePolicy);
}

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org