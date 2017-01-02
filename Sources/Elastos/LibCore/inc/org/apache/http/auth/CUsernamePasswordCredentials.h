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

#ifndef __ORG_APACHE_HTTP_AUTH_CUSERNAMEPASSWORDCREDENTIALS_H_
#define __ORG_APACHE_HTTP_AUTH_CUSERNAMEPASSWORDCREDENTIALS_H_

#include "_Org_Apache_Http_Auth_CUsernamePasswordCredentials.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/** {@link Credentials} specific to the Windows platform.
 *
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 */
CarClass(CUsernamePasswordCredentials)
    , public Object
    , public IUsernamePasswordCredentials
    , public ICredentials
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetUserPrincipal(
        /* [out] */ IPrincipal** principal);

    CARAPI GetUserName(
        /* [out] */ String* name);

    CARAPI GetPassword(
        /* [out] */ String* password);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI constructor(
        /* [in] */ const String& usernamePassword);

    CARAPI constructor(
        /* [in] */ const String& userName,
        /* [in] */ const String& password);

private:
    AutoPtr<IBasicUserPrincipal> mPrincipal;
    String mPassword;
};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CUSERNAMEPASSWORDCREDENTIALS_H_
