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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/client/protocol/CClientContextConfigurer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL_2(CClientContextConfigurer, Object, IClientContextConfigurer, IClientContext)

CAR_OBJECT_IMPL(CClientContextConfigurer)

ECode CClientContextConfigurer::SetCookieSpecRegistry(
    /* [in] */ ICookieSpecRegistry* registry)
{
    return mContext->SetAttribute(COOKIESPEC_REGISTRY, registry);
}

ECode CClientContextConfigurer::SetAuthSchemeRegistry(
    /* [in] */ IAuthSchemeRegistry* registry)
{
    return mContext->SetAttribute(AUTHSCHEME_REGISTRY, registry);
}

ECode CClientContextConfigurer::SetCookieStore(
    /* [in] */ ICookieStore* store)
{
    return mContext->SetAttribute(COOKIE_STORE, store);
}

ECode CClientContextConfigurer::SetCredentialsProvider(
    /* [in] */ ICredentialsProvider* provider)
{
    return mContext->SetAttribute(CREDS_PROVIDER, provider);
}

ECode CClientContextConfigurer::SetAuthSchemePref(
    /* [in] */ IList* list)
{
    return mContext->SetAttribute(AUTH_SCHEME_PREF, list);
}

ECode CClientContextConfigurer::constructor(
    /* [in] */ IHttpContext* context)
{
    if (context == NULL) {
        Logger::E("CClientContextConfigurer", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContext = context;
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org