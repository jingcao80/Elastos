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

#ifndef __ELASTOS_SECURITY_IDENTITY_H__
#define __ELASTOS_SECURITY_IDENTITY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::IO::ISerializable;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Security {

class Identity
    : public Object
    , public IIdentity
    , public IPrincipal
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Identity() = 0;

    /**
     * Constructs a new instance of {@code Identity}.
     */
    Identity();

    /**
     * Creates a new instance of {@code Identity} with the specified name.
     *
     * @param name
     *            the name of this {@code Identity}.
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Creates a new instance of {@code Identity} with the specified name and
     * the scope of this {@code Identity}.
     *
     * @param name
     *            the name of this {@code Identity}.
     * @param scope
     *            the {@code IdentityScope} of this {@code Identity}.
     * @throws KeyManagementException
     *             if an {@code Identity} with the same name is already present
     *             in the specified scope.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IIdentityScope* scope) /*throws KeyManagementException*/;

    /**
     * Adds a {@code Certificate} to this {@code Identity}.
     *
     * @param certificate
     *            the {@code Certificate} to be added to this {@code Identity}.
     * @throws KeyManagementException
     *             if the certificate is not valid.
     */
    CARAPI AddCertificate(
        /* [in] */ ICertificate* certificate) /*throws KeyManagementException*/;

    /**
     * Removes the specified {@code Certificate} from this {@code Identity}.
     *
     * @param certificate
     *            the {@code Certificate} to be removed.
     * @throws KeyManagementException
     *             if the certificate is not found.
     */
    CARAPI RemoveCertificate(
        /* [in] */ ICertificate* certificate) /*throws KeyManagementException*/;

    /**
     * Returns the certificates for this {@code Identity}. External
     * modifications of the returned array has no impact on this {@code
     * Identity}.
     *
     * @return the {@code Certificates} for this {@code Identity}
     */
    CARAPI Certificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    /**
     * Returns a string containing a concise, human-readable description of the
     * this {@code Identity}.
     *
     * @param detailed
     *            whether or not this method should return detailed information.
     * @return a printable representation for this {@code Permission}.
     */
    CARAPI ToString(
        /* [in] */ Boolean detailed,
        /* [out] */ String* result);

    /**
     * Returns the {@code IdentityScope} of this {@code Identity}.
     *
     * @return the {@code IdentityScope} of this {@code Identity}.
     */
    CARAPI GetScope(
        /* [out] */ IIdentityScope** result);

    /**
     * Sets the specified {@code PublicKey} to this {@code Identity}.
     *
     * @param key
     *            the {@code PublicKey} to be set.
     * @throws KeyManagementException
     *             if another {@code Identity} in the same scope as this {@code
     *             Identity} already has the same {@code PublicKey}.
     */
    CARAPI SetPublicKey(
        /* [in] */ IPublicKey* key) /*throws KeyManagementException*/;

    /**
     * Returns the {@code PublicKey} associated with this {@code Identity}.
     *
     * @return the {@code PublicKey} associated with this {@code Identity}.
     */
    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** result);

    /**
     * Sets an information string for this {@code Identity}.
     * @param info
     *            the information to be set.
     */
    CARAPI SetInfo(
        /* [in] */ const String& info);

    /**
     * Returns the information string of this {@code Identity}.
     *
     * @return the information string of this {@code Identity}.
     */
    CARAPI GetInfo(
        /* [out] */ String* result);

    /**
     * Compares the specified object with this {@code Identity} for equality and
     * returns {@code TRUE} if the specified object is equal, {@code FALSE}
     * otherwise. {@code Identity} objects are considered equal, if they have
     * the same name and are in the same scope.
     *
     * @param obj
     *            object to be compared for equality with this {@code
     *            Identity}.
     * @return {@code TRUE} if the specified object is equal to this {@code
     *         Identity}, otherwise {@code FALSE}.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns the name of this {@code Identity}.
     *
     * @return the name of this {@code Identity}.
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Returns the hash code value for this {@code Identity}. Returns the same
     * hash code for {@code Identity}s that are equal to each other as required
     * by the general contract of {@link Object#hashCode}.
     *
     * @return the hash code value for this {@code Identity}.
     * @see Object#equals(Object)
     * @see Identity#equals(Object)
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Returns a string containing a concise, human-readable description of the
     * this {@code Identity} including its name and its scope.
     *
     * @return a printable representation for this {@code Identity}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    /**
     * Compares the specified {@code Identity} with this {@code Identity} for
     * equality and returns {@code TRUE} if the specified object is equal,
     * {@code FALSE} otherwise.
     * <p>
     * To be equal, two {@code Identity} objects need to have the same name and
     * the same public keys.
     *
     * @param identity
     *            the identity to check for equality.
     * @return {@code TRUE} if the {@code Identity} objects are equal, {@code
     *         FALSE} otherwise.
     */
    virtual CARAPI_(Boolean) IdentityEquals(
        /* [in] */ Identity* identity);

private:
    static CARAPI_(Boolean) CheckKeysEqual(
        /* [in] */ IPublicKey* pk1,
        /* [in] */ IPublicKey* pk2);

private:
    // private static final long serialVersionUID = 3609922007826600659L;

    String mName;

    AutoPtr<IPublicKey> mPublicKey;

    String mInfo;

    AutoPtr<IIdentityScope> mScope;

    AutoPtr<IVector> mCertificates; ///*<Certificate*/>
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_IDENTITY_H__
