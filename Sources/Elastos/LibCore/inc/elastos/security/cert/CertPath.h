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

#ifndef __ELASTOS_SECURITY_CERT_CERTPATH_H__
#define __ELASTOS_SECURITY_CERT_CERTPATH_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::IO::IObjectStreamField;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {
namespace Cert {

class CertPath
    : public Object
    , public ICertPath
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the type of {@code Certificate} in this instance.
     *
     * @return the certificate type.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Returns {@code true} if {@code Certificate}s in the list are the same
     * type and the lists are equal (and by implication the certificates
     * contained within are the same).
     *
     * @param other
     *            {@code CertPath} to be compared for equality.
     * @return {@code true} if the object are equal, {@code false} otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEqual);

    /**
     * Overrides {@code Object.hashCode()}. The function is defined as follows:
     * <pre>
     * {@code hashCode = 31 * path.getType().hashCode() +
     * path.getCertificates().hashCode();}
     * </pre>
     *
     * @return the hash code for this instance.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns a {@code String} representation of this {@code CertPath}
     * instance. It is the result of calling {@code toString} on all {@code
     * Certificate}s in the {@code List}.
     *
     * @return a string representation of this instance.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns an immutable List of the {@code Certificate}s contained
     * in the {@code CertPath}.
     *
     * @return a list of {@code Certificate}s in the {@code CertPath}.
     */
    virtual CARAPI GetCertificates(
        /* [out] */ IList** certs) = 0;

    /**
     * Returns an encoding of the {@code CertPath} using the default encoding.
     *
     * @return default encoding of the {@code CertPath}.
     * @throws CertificateEncodingException
     *             if the encoding fails.
     */
    virtual CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encode) = 0;

    /**
     * Returns an encoding of the {@code CertPath} using the specified encoding.
     *
     * @param encoding
     *            encoding that should be generated.
     * @return default encoding of the {@code CertPath}.
     * @throws CertificateEncodingException
     *             if the encoding fails.
     */
    virtual CARAPI GetEncoded(
        /* [in] */ const String& encoding,
        /* [out, callee] */ ArrayOf<Byte>** encode) = 0;

    /**
     * Returns an {@code Iterator} over the supported encodings for a
     * representation of the certificate path.
     *
     * @return {@code Iterator} over supported encodings (as {@code String}s).
     */
    virtual CARAPI GetEncodings(
        /* [out] */ IIterator** it) = 0;

protected:
    /**
     * Creates a new {@code CertPath} instance for the specified certificate
     * type.
     *
     * @param type
     *            the certificate type.
     */
    CertPath(
        /* [in] */ const String& type);

    /**
     * Returns an alternate object to be serialized.
     *
     * @return an alternate object to be serialized.
     * @throws ObjectStreamException
     *             if the creation of the alternate object fails.
     */
    CARAPI WriteReplace(
        /* [out] */ IInterface** object);

    /**
     * The alternate {@code Serializable} class to be used for serialization and
     * deserialization on {@code CertPath} objects.
     */
    class CertPathRep
        : public Object
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Creates a new {@code CertPathRep} instance with the specified type
         * and encoded data.
         *
         * @param type
         *            the certificate type.
         * @param data
         *            the encoded data.
         */
        CertPathRep(
            /* [in] */ const String& type,
            /* [in] */ ArrayOf<Byte>* data);

    protected:
        /**
         * Deserializes a {@code CertPath} from a serialized {@code CertPathRep}
         * object.
         *
         * @return the deserialized {@code CertPath}.
         * @throws ObjectStreamException
         *             if deserialization fails.
         */
        CARAPI ReadResolve(
            /* [out] */ IInterface** object);

    private:
        static CARAPI_(AutoPtr<ArrayOf<IObjectStreamField*> >) InitFields();

    private:
        static const Int64 sSerialVersionUID;/* = 3015633072427920915L;*/
        // Standard name of the type of certificates in this path
        const String mType;
        // cert path data
        const AutoPtr<ArrayOf<Byte> > mData;

        // Force default serialization to use writeUnshared/readUnshared
        // for cert path data
        static const AutoPtr<ArrayOf<IObjectStreamField*> > sSerialPersistentFields;/* = {
            new ObjectStreamField("type", String.class),
            new ObjectStreamField("data", byte[].class, true),
        };*/
    };

private:
    // Standard name of the type of certificates in this path
    const String mType;
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CERTPATH_H__
