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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNERINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNERINFO_H__

#include "AuthenticatedAttributes.h"
#include "asn1/ASN1Sequence.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Math::IBigInteger;
using Elastos::Utility::IList;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class SignerInfo
    : public Object
    , public ISignerInfo
{
private:
    class ASN1SequenceDerived1
        : public ASN1Sequence
    {
    public:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("SignerInfo::ASN1SequenceDerived1")
    };

    class ASN1SequenceDerived2
        : public ASN1Sequence
    {
    public:
        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("SignerInfo::ASN1SequenceDerived2")
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IASN1Sequence>) InitStatic();

    CARAPI GetIssuer(
        /* [out] */ IX500Principal** issuer);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** serialNumber);

    CARAPI GetDigestAlgorithm(
        /* [out] */ String* algorithm);

    CARAPI GetDigestAlgorithmName(
        /* [out] */ String* algorithm);

    CARAPI GetDigestEncryptionAlgorithm(
        /* [out] */ String* digestEncryptionAlgorithm);

    CARAPI GetDigestEncryptionAlgorithmName(
        /* [out] */ String* digestEncryptionAlgorithm);

    CARAPI GetAuthenticatedAttributes(
        /* [out] */ IList** authenticatedAttributes);

    CARAPI GetEncodedAuthenticatedAttributes(
        /* [out, callee] */ ArrayOf<Byte>** encodedAuthenticatedAttributes);

    CARAPI GetEncryptedDigest(
        /* [out, callee] */ ArrayOf<Byte>** encryptedDigest);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    SignerInfo(
        /* [in] */ Int32 version,
        /* [in] */ ArrayOf<IInterface*>* issuerAndSerialNumber,
        /* [in] */ IAlgorithmIdentifier* digestAlgorithm,
        /* [in] */ AuthenticatedAttributes* authenticatedAttributes,
        /* [in] */ IAlgorithmIdentifier* digestEncryptionAlgorithm,
        /* [in] */ ArrayOf<Byte>* encryptedDigest,
        /* [in] */ IList* unauthenticatedAttributes);

public:
    static AutoPtr<IASN1Sequence> ISSUER_AND_SERIAL_NUMBER;
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;
    AutoPtr<IX500Principal> mIssuer;
    AutoPtr<IBigInteger> mSerialNumber;
    AutoPtr<IAlgorithmIdentifier> mDigestAlgorithm;
    AutoPtr<AuthenticatedAttributes> mAuthenticatedAttributes;
    AutoPtr<IAlgorithmIdentifier> mDigestEncryptionAlgorithm;
    AutoPtr<ArrayOf<Byte> > mEncryptedDigest;
    AutoPtr<IList> mUnauthenticatedAttributes;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNERINFO_H__
