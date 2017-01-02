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

#ifndef __ELASTOS_SECURITY_CERT_CERTIFICATE_H__
#define __ELASTOS_SECURITY_CERT_CERTIFICATE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IObjectStreamField;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {
namespace Cert {

class ECO_PUBLIC Certificate
    : public Object
    , public ICertificate
    , public ISerializable
{
protected:
    /**
     * The alternate {@code Serializable} class to be used for serialization and
     * deserialization of {@code Certificate} objects.
     */
    class CertificateRep
        : public Object
        , public ISerializable
    {
        friend class Certificate;

    public:
        CAR_INTERFACE_DECL()

    protected:
        /**
         * Creates a new {@code CertificateRep} instance with the specified
         * certificate type and encoded data.
         *
         * @param type
         *            the certificate type.
         * @param data
         *            the encoded data.
         */
        CertificateRep(
            /* [in] */ const String& type,
            /* [in] */ ArrayOf<Byte>* data);

        /**
         * Deserializes a {@code Certificate} from a serialized {@code
         * CertificateRep} object.
         *
         * @return the deserialized {@code Certificate}.
         * @throws ObjectStreamException
         *             if deserialization fails.
         */
        CARAPI ReadResolve(
            /* [out] */ IInterface** object);

    private:
        // The standard name of the certificate type
        const String mType;
        // The certificate data
        const AutoPtr< ArrayOf<Byte> > mData;
        // Force default serialization to use writeUnshared/readUnshared
        // for the certificate data
        // static const AutoPtr<ArrayOf<IObjectStreamField*> > sSerialPersistentFields;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the certificate type.
     *
     * @return the certificate type.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Compares the argument to the certificate, and returns {@code true} if they
     * represent the <em>same</em> object using a class specific comparison. The
     * implementation in Object returns {@code true} only if the argument is the
     * exact same object as the callee (==).
     *
     * @param other
     *            the object to compare with this object.
     * @return {@code true} if the object is the same as this object, {@code
     *         false} if it is different from this object.
     * @see #hashCode
     */
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* equal);

    /**
     * Returns an integer hash code for the certificate. Any two objects which
     * return {@code true} when passed to {@code equals} must return the same
     * value for this method.
     *
     * @return the certificate's hash
     * @see #equals
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns the encoded representation for this certificate.
     *
     * @return the encoded representation for this certificate.
     * @throws CertificateEncodingException
     *             if the encoding fails.
     */
    using ICertificate::GetEncoded;

    /**
     * Verifies that this certificate was signed with the given public key.
     *
     * @param key
     *            PublicKey public key for which verification should be
     *            performed.
     * @throws CertificateException
     *             if encoding errors are detected.
     * @throws NoSuchAlgorithmException
     *             if an unsupported algorithm is detected.
     * @throws InvalidKeyException
     *             if an invalid key is detected.
     * @throws NoSuchProviderException
     *             if there is no default provider.
     * @throws SignatureException
     *             if signature errors are detected.
     */


    /**
     * Verifies that this certificate was signed with the given public key. It
     * Uses the signature algorithm given by the provider.
     *
     * @param key
     *            PublicKey public key for which verification should be
     *            performed.
     * @param sigProvider
     *            String the name of the signature provider.
     * @exception CertificateException
     *                if encoding errors are detected.
     * @exception NoSuchAlgorithmException
     *                if an unsupported algorithm is detected.
     * @exception InvalidKeyException
     *                if an invalid key is detected.
     * @exception NoSuchProviderException
     *                if the specified provider does not exists.
     * @exception SignatureException
     *                if signature errors are detected.
     */
    using ICertificate::Verify;

    /**
     * Returns a string containing a concise, human-readable description of the
     * certificate.
     *
     * @return a printable representation for the certificate.
     */
    virtual CARAPI ToString(
        /* [out] */ String* str) = 0;

    /**
     * Returns the public key corresponding to this certificate.
     *
     * @return the public key corresponding to this certificate.
     */
    using ICertificate::GetPublicKey;

protected:
    /**
     * Creates a new {@code Certificate} with the specified type.
     *
     * @param type
     *        the certificate type.
     */
    CARAPI constructor(
        /* [in] */ const String& type);

    /**
     * Returns an alternate object to be serialized.
     *
     * @return the object to serialize.
     * @throws ObjectStreamException
     *             if the creation of the alternate object fails.
     */
    CARAPI WriteReplace(
        /* [out] */ IInterface** object);

private:
    // The standard name of the certificate type
    String mType;
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CERTIFICATE_H__
