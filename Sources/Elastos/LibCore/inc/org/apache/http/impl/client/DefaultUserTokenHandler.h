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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;
using Org::Apache::Http::Auth::IAuthState;
using Org::Apache::Http::Client::IUserTokenHandler;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

class DefaultUserTokenHandler
    : public Object
    , public IUserTokenHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetUserToken(
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** token);

private:
    static CARAPI GetAuthPrincipal(
        /* [in] */ IAuthState* authState,
        /* [out] */ IPrincipal** principal);
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__
