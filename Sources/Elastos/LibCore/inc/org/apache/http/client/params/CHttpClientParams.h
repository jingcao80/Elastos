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

#ifndef __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_
#define __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_

#include "_Org_Apache_Http_Client_Params_CHttpClientParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

CarClass(CHttpClientParams)
    , public Object
    , public IHttpClientParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsRedirecting(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isRedirecting);

    CARAPI SetRedirecting(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    CARAPI IsAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isAuthenticating);

    CARAPI SetAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    CARAPI GetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* cookiePolicy);

    CARAPI SetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& cookiePolicy);

};

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_
