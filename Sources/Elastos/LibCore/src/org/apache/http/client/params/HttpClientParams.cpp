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

#include "org/apache/http/client/params/HttpClientParams.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

ECode HttpClientParams::IsRedirecting(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* isRedirecting)
{
    VALIDATE_NOT_NULL(isRedirecting)
    *isRedirecting = FALSE;

    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetBooleanParameter(IClientPNames::HANDLE_REDIRECTS, TRUE, isRedirecting);
}

ECode HttpClientParams::SetRedirecting(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetBooleanParameter(IClientPNames::HANDLE_REDIRECTS, value,
            (IHttpParams**)&p);
}

ECode HttpClientParams::IsAuthenticating(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* isAuthenticating)
{
    VALIDATE_NOT_NULL(isAuthenticating)
    *isAuthenticating = FALSE;

    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetBooleanParameter(IClientPNames::HANDLE_AUTHENTICATION, TRUE, isAuthenticating);
}

ECode HttpClientParams::SetAuthenticating(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetBooleanParameter(IClientPNames::HANDLE_AUTHENTICATION, value,
            (IHttpParams**)&p);
}

ECode HttpClientParams::GetCookiePolicy(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* cookiePolicy)
{
    VALIDATE_NOT_NULL(cookiePolicy)
    *cookiePolicy = String(NULL);

    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(IClientPNames::COOKIE_POLICY, (IInterface**)&o);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(o);
    if (cs == NULL) {
        *cookiePolicy = ICookiePolicy::BEST_MATCH;
        return NOERROR;
    }
    return cs->ToString(cookiePolicy);
}

ECode HttpClientParams::SetCookiePolicy(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& cookiePolicy)
{
    if (params == NULL) {
        Logger::E("HttpClientParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(cookiePolicy, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> p;
    return params->SetParameter(IClientPNames::COOKIE_POLICY, cs, (IHttpParams**)&p);
}

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org