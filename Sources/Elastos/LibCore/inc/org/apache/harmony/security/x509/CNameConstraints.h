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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CNameConstraints.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IArrayOf;
using Elastos::Core::IStringBuilder;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CNameConstraints)
    , public ExtensionValue
    , public INameConstraints
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

        TO_STRING_IMPL("CAccessDescription::CNameConstraints")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI IsAcceptable(
        /* [in] */ IX509Certificate* pCert,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI IsAcceptable(
        /* [in] */ IList* pNames,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI constructor(
        /* [in] */ IGeneralSubtrees* permittedSubtrees,
        /* [in] */ IGeneralSubtrees* excludedSubtrees);

    CARAPI constructor(
        /* [in] */ IGeneralSubtrees* permittedSubtrees,
        /* [in] */ IGeneralSubtrees* excludedSubtrees,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ INameConstraints** ppObject);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    /**
     * Prepare the data structure to speed up the checking process.
     */
    CARAPI PrepareNames();

    /**
     * Returns the value of certificate extension
     */
    CARAPI GetExtensionValue(
        /* [in] */ IX509Certificate* cert,
        /* [in] */ const String& OID,
        /* [out] */ ArrayOf<Byte>** values);

    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * X.509 NameConstraints encoder/decoder.
     */
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of permittedSubtrees field of the structure */
    AutoPtr<IGeneralSubtrees> mPermittedSubtrees;
    /** the value of excludedSubtrees field of the structure */
    AutoPtr<IGeneralSubtrees> mExcludedSubtrees;
    /** the ASN.1 encoded form of NameConstraints */
    AutoPtr<ArrayOf<Byte> > mEncoding;

    AutoPtr<IArrayList> mPermitted_names;
    AutoPtr<IArrayList> mExcluded_names;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CNAMECONSTRAINTS_H__
