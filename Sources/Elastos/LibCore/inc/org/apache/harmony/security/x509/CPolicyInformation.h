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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__

#include "_Org_Apache_Harmony_Security_X509_CPolicyInformation.h"
#include "ASN1Sequence.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPolicyInformation)
    , public Object
    , public IPolicyInformation
{
private:
    /**
     * ASN.1 DER X.509 PolicyInformation encoder/decoder class.
     */
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* types);

    protected:
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);

        TO_STRING_IMPL("CPolicyInformation::ASN1SequenceWrapper")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPolicyIdentifier(
        /* [out] */ String* pPolicyIdentifier);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor(
        /* [in] */ const String& policyIdentifier);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of policyIdentifier field of the structure */
    String mPolicyIdentifier;
    /** the ASN.1 encoded form of PolicyInformation */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__
