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

#ifndef __ORG_APACHE_HTTP_AUTH_CAUTHSTATE_H_
#define __ORG_APACHE_HTTP_AUTH_CAUTHSTATE_H_

#include "_Org_Apache_Http_Auth_CAuthState.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/**
 * This class provides detailed information about the state of the
 * authentication process.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
CarClass(CAuthState)
    , public Object
    , public IAuthState
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Invalidates the authentication state by resetting its parameters.
     */
    CARAPI Invalidate();

    CARAPI IsValid(
        /* [out] */ Boolean* isValid);

    /**
     * Assigns the given {@link AuthScheme authentication scheme}.
     *
     * @param authScheme the {@link AuthScheme authentication scheme}
     */
    CARAPI SetAuthScheme(
        /* [in] */ IAuthScheme* authScheme);

    /**
     * Returns the {@link AuthScheme authentication scheme}.
     *
     * @return {@link AuthScheme authentication scheme}
     */
    CARAPI GetAuthScheme(
        /* [out] */ IAuthScheme** scheme);

    /**
     * Returns user {@link Credentials} selected for authentication if available
     *
     * @return user credentials if available, <code>null</code otherwise
     */
    CARAPI GetCredentials(
        /* [out] */ ICredentials** credentials);

    /**
     * Sets user {@link Credentials} to be used for authentication
     *
     * @param credentials User credentials
     */
    CARAPI SetCredentials(
        /* [in] */ ICredentials* credentials);

    /**
     * Returns actual {@link AuthScope} if available
     *
     * @return actual authentication scope if available, <code>null</code otherwise
     */
    CARAPI GetAuthScope(
        /* [out] */ IAuthScope** authScope);

    /**
      * Sets actual {@link AuthScope}.
      *
      * @param authScope Authentication scope
      */
    CARAPI SetAuthScope(
        /* [in] */ IAuthScope* authScope);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor();

private:
    /** Actual authentication scheme */
    AutoPtr<IAuthScheme> mAuthScheme;

    /** Actual authentication scope */
    AutoPtr<IAuthScope> mAuthScope;

    /** Credentials selected for authentication */
    AutoPtr<ICredentials> mCredentials;

};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CAUTHSTATE_H_
