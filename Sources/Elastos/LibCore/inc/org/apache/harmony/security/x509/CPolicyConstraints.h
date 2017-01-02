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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CPolicyConstraints.h"
#include "ExtensionValue.h"
#include "ASN1Sequence.h"

using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPolicyConstraints)
    , public ExtensionValue
    , public IPolicyConstraints
{
public:
    /**
     * X.509 PolicyConstraints encoder/decoder.
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

        TO_STRING_IMPL("CPolicyConstraints::ASN1SequenceWrapper")

    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IBigInteger* pRequireExplicitPolicy,
        /* [in] */ IBigInteger* pInhibitPolicyMapping);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    CARAPI constructor(
        /* [in] */ IBigInteger* pRequireExplicitPolicy,
        /* [in] */ IBigInteger* pInhibitPolicyMapping,
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of requireExplicitPolicy field of the structure */
    AutoPtr<IBigInteger> mRequireExplicitPolicy;
    /** the value of inhibitPolicyMapping field of the structure */
    AutoPtr<IBigInteger> mInhibitPolicyMapping;
    /** the ASN.1 encoded form of PolicyConstraints */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
