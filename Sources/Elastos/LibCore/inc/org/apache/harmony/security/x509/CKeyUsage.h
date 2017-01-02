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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CKEYUSAGE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CKEYUSAGE_H__

#include "_Org_Apache_Harmony_Security_X509_CKeyUsage.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CKeyUsage)
    , public ExtensionValue
    , public IKeyUsage
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI GetKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean>** ppKeyUsage);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * X.509 Extension value encoder/decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    /** The names of the usages. */
    static AutoPtr<ArrayOf<String> > USAGES;

    /** the value of extension */
    AutoPtr<ArrayOf<Boolean> > mKeyUsage;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CKEYUSAGE_H__
