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

#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

/**
 * Abstract authentication scheme class that serves as a basis
 * for all authentication schemes supported by HttpClient. This class
 * defines the generic way of parsing an authentication challenge. It
 * does not make any assumptions regarding the format of the challenge
 * nor does it impose any specific way of responding to that challenge.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
*/
class AuthSchemeBase
    : public Object
    , public IAuthScheme
{
public:
    AuthSchemeBase();

    CAR_INTERFACE_DECL()

    CARAPI ProcessChallenge(
        /* [in] */ IHeader* header);

    /**
     * Returns <code>true</code> if authenticating against a proxy, <code>false</code>
     * otherwise.
     *
     * @return <code>true</code> if authenticating against a proxy, <code>false</code>
     * otherwise
     */
    CARAPI_(Boolean) IsProxy();

protected:
    virtual CARAPI ParseChallenge(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len) = 0;

private:
    /**
     * Flag whether authenticating against a proxy.
     */
    Boolean mProxy;
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__
