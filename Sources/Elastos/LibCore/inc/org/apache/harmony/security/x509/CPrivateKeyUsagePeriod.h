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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__

#include "_Org_Apache_Harmony_Security_X509_CPrivateKeyUsagePeriod.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ASN1Sequence.h"

using Elastos::Utility::IDate;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPrivateKeyUsagePeriod)
    , public Object
    , public IPrivateKeyUsagePeriod
{
private:
    /**
     * ASN.1 DER X.509 PrivateKeyUsagePeriod encoder/decoder class.
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

        TO_STRING_IMPL("CPrivateKeyUsagePeriod::ASN1SequenceWrapper")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNotBefore(
        /* [out] */ IDate** ppNotBefore);

    CARAPI GetNotAfter(
        /* [out] */ IDate** ppNotAfter);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IDate* notBeforeDate,
        /* [in] */ IDate* notAfterDate);

    CARAPI constructor(
        /* [in] */ IDate* notBeforeDate,
        /* [in] */ IDate* notAfterDate,
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of notBeforeDate field of the structure */
    AutoPtr<IDate> mNotBeforeDate;
    /** the value of notAfterDate field of the structure */
    AutoPtr<IDate> mNotAfterDate;
    /** the ASN.1 encoded form of PrivateKeyUsagePeriod */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
