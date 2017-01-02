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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificatePolicies.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificatePolicies)
    , public ExtensionValue
    , public ICertificatePolicies
{
private:
    class MyASN1SequenceOf
        : public ASN1SequenceOf
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** coll);

        TO_STRING_IMPL("CCertificatePolicies::MyASN1SequenceOf")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI GetPolicyInformations(
        /* [out] */ IList** ppInfomations);

    CARAPI AddPolicyInformation(
        /* [in] */ IPolicyInformation* pPolicyInformation);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* policyInformations,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ ICertificatePolicies** ppPolicies);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 DER X.509 CertificatePolicies encoder/decoder class.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    /** the values of policyInformation field of the structure */
    AutoPtr<IList> mPolicyInformations;

    /** the ASN.1 encoded form of CertificatePolicies */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__
