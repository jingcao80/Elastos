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

#ifndef __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_

#include "Elastos.CoreLibrary.h"
#include <elastos/coredef.h>

using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

class HttpProtocolParams
{
public:
    /**
     * Returns the charset to be used for writing HTTP headers.
     * @return The charset
     */
    static CARAPI GetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used for writing HTTP headers.
     * @param charset The charset
     */
    static CARAPI SetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @return The charset
     */
    static CARAPI GetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @param charset The charset
     */
    static CARAPI SetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns {@link ProtocolVersion protocol version} to be used per default.
     *
     * @return {@link ProtocolVersion protocol version}
     */
    static CARAPI GetVersion(
        /* [in] */ IHttpParams* params,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Assigns the {@link ProtocolVersion protocol version} to be used by the
     * HTTP methods that this collection of parameters applies to.
     *
     * @param version the {@link ProtocolVersion protocol version}
     */
    static CARAPI SetVersion(
        /* [in] */ IHttpParams* params,
        /* [in] */ IProtocolVersion* version);

    static CARAPI GetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* agent);

    static CARAPI SetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& useragent);

    static CARAPI UseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* result);

    static CARAPI SetUseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean b);

private:
    HttpProtocolParams() {}
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_
