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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__

#include "org/apache/http/impl/client/AbstractAuthenticationHandler.h"

using Elastos::Utility::IMap;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
class DefaultTargetAuthenticationHandler : public AbstractAuthenticationHandler
{
public:
    DefaultTargetAuthenticationHandler();

    CARAPI IsAuthenticationRequested(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetChallenges(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ IMap** callenges);
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__
