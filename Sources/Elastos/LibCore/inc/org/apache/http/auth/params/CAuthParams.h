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

#ifndef __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_
#define __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_

#include "_Org_Apache_Http_Auth_Params_CAuthParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CarClass(CAuthParams)
    , public Object
    , public IAuthParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Obtains the charset for encoding
     * {@link org.apache.http.auth.Credentials}.
     * If not configured,
     * {@link HTTP#DEFAULT_PROTOCOL_CHARSET HTTP.DEFAULT_PROTOCOL_CHARSET}
     * is used instead.
     *
     * @return The charset
     *
     * @see AuthPNames#CREDENTIAL_CHARSET
     */
    CARAPI GetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used when encoding
     * {@link org.apache.http.auth.Credentials}.
     *
     * @param charset The charset
     */
    CARAPI SetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

};

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_
