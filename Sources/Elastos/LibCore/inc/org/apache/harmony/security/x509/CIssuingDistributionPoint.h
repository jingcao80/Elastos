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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CISSUINGDISTRIBUTIONPOINT_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CISSUINGDISTRIBUTIONPOINT_H__

#include "_Org_Apache_Harmony_Security_X509_CIssuingDistributionPoint.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IBoolean;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CIssuingDistributionPoint)
    , public ExtensionValue
    , public IIssuingDistributionPoint
{
private:
    class MyASN1Sequence
        : public ASN1Sequence
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("CIssuingDistributionPoint::MyASN1Sequence")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI SetOnlyContainsUserCerts(
        /* [in] */ Boolean onlyContainsUserCerts);

    CARAPI SetOnlyContainsCACerts(
        /* [in] */ Boolean onlyContainsCACerts);

    CARAPI SetIndirectCRL(
        /* [in] */ Boolean indirectCRL);

    CARAPI SetOnlyContainsAttributeCerts(
        /* [in] */ Boolean onlyContainsAttributeCerts);

    CARAPI constructor(
        /* [in] */ IDistributionPointName* pDistributionPoint,
        /* [in] */ IReasonFlags* pOnlySomeReasons);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ IIssuingDistributionPoint** ppObject);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * ASN.1 Encoder/Decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    // values of the fields of the structure
    AutoPtr<IDistributionPointName> mDistributionPoint;
    AutoPtr<IBoolean> mOnlyContainsUserCerts;
    AutoPtr<IBoolean> mOnlyContainsCACerts;
    AutoPtr<IReasonFlags> mOnlySomeReasons;
    AutoPtr<IBoolean> mIndirectCRL;
    AutoPtr<IBoolean> mOnlyContainsAttributeCerts;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CISSUINGDISTRIBUTIONPOINT_H__
