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

#ifndef __ELASTOS_SECURITY_IDENTITYSCOPE_H__
#define __ELASTOS_SECURITY_IDENTITYSCOPE_H__

#include "Identity.h"

namespace Elastos {
namespace Security {

/**
 * {@code IdentityScope} represents a scope for {@link Identity} objects.
 *
 * @deprecated Use {@link Principal}, {@link KeyStore} and the {@code java.security.cert} package
 * instead.
 */
class IdentityScope
    : public Identity
    , public IIdentityScope
{
public:
    CAR_INTERFACE_DECL()

    virtual ~IdentityScope() = 0;

    /**
     * Constructs a new instance of {@code IdentityScope}.
     */
    IdentityScope();

    /**
     * Constructs a new instance of {@code IdentityScope} with the specified
     * name.
     *
     * @param name
     *            the name of this {@code IdentityScope}.
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Constructs a new instance of {@code IdentityScope} with the specified
     * name and the specified scope.
     *
     * @param name
     *            the name of this {@code IdentityScope}.
     * @param scope
     *            the scope of this {@code IdentityScope}.
     * @throws KeyManagementException
     *             if an identity with the same key already exists.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IdentityScope* scope) /*throws KeyManagementException*/;

    /**
     * Returns the system's scope.
     *
     * @return the system's scope.
     */
    static CARAPI_(AutoPtr<IdentityScope>) GetSystemScope();

    using IIdentityScope::GetIdentity;

    /**
     * Returns the {@code Identity} with the name of the specified principal or
     * {@code null} if no {@code Identity} with the name of the specified
     * principal is present in this scope.
     *
     * @param principal
     *            the {@code Principal} whose name is used to lookup the {@code
     *            Identity} to be returned.
     * @return the {@code Identity} with the specified name or {@code null} if
     *         not present.
     */
    CARAPI GetIdentity(
        /* [in] */ IPrincipal* principal,
        /* [out] */ IIdentity** result);

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code IdentityScope}.
     *
     * @return a printable representation for this {@code IdentityScope}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);


protected:
    /**
     * Sets the system's scope.
     *
     * @param scope
     *            the scope to set.
     */
    static CARAPI_(void) SetSystemScope(
        /* [in] */ IdentityScope* scope);

private:
    // private static final long serialVersionUID = -2337346281189773310L;

    // systemScope holds reference to the current system scope
    static AutoPtr<IdentityScope> systemScope;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_IDENTITYSCOPE_H__
