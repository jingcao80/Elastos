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

#ifndef __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_
#define __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Maintains a map of HTTP request handlers keyed by a request URI pattern.
 * {@link HttpRequestHandler} instances can be looked up by request URI
 * using the {@link HttpRequestHandlerResolver} interface.<br/>
 * Patterns may have three formats:
 * <ul>
 *   <li><code>*</code></li>
 *   <li><code>*&lt;uri&gt;</code></li>
 *   <li><code>&lt;uri&gt;*</code></li>
 * </ul>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 630662 $
 */
class HttpRequestHandlerRegistry
    : public Object
    , public IHttpRequestHandlerRegistry
    , public IHttpRequestHandlerResolver
{
public:
    CAR_INTERFACE_DECL()

    virtual ~HttpRequestHandlerRegistry() {}

    CARAPI Register(
        /* [in] */ const String& pattern,
        /* [in] */ IHttpRequestHandler* handler);

    CARAPI Unregister(
        /* [in] */ const String& pattern);

    CARAPI SetHandlers(
        /* [in] */ IMap* map);

    CARAPI Lookup(
        /* [in] */ const String& requestURI,
        /* [out] */ IHttpRequestHandler** handler);

protected:
    CARAPI Init();

    CARAPI MatchUriRequestPattern(
        /* [in] */ const String& pattern,
        /* [in] */ const String& requestUri,
        /* [out] */ Boolean* isMatch);

private:
    AutoPtr<IUriPatternMatcher> mMatcher;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_
