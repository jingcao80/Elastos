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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__

#include "_Org_Apache_Harmony_Security_X509_CValidity.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ASN1Sequence.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

/**
 * The class encapsulates the ASN.1 DER encoding/decoding work
 * with Validity structure which is the part of X.509 certificate
 * (as specified in RFC 3280 -
 *  Internet X.509 Public Key Infrastructure.
 *  Certificate and Certificate Revocation List (CRL) Profile.
 *  http://www.ietf.org/rfc/rfc3280.txt):
 *
 * <pre>
 *  Validity ::= SEQUENCE {
 *       notBefore      Time,
 *       notAfter       Time
 *  }
 * </pre>
 */
CarClass(CValidity)
    , public Object
    , public IValidity
{
public:
    /**
     * ASN.1 DER X.509 Validity encoder/decoder class.
     */
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

    protected:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);

        TO_STRING_IMPL("CValidity::ASN1SequenceWrapper")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNotBefore(
        /* [out] */ IDate** ppDate);

    CARAPI GetNotAfter(
        /* [out] */ IDate** ppDate);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IDate* pNotBefore,
        /* [in] */ IDate* pNotAfter);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of notBefore field of the structure */
    AutoPtr<IDate> mNotBefore;
    /** the value of notAfter field of the structure */
    AutoPtr<IDate> mNotAfter;
    /** the ASN.1 encoded form of Validity */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
