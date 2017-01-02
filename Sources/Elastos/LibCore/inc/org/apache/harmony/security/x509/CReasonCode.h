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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__

#include "_Org_Apache_Harmony_Security_X509_CReasonCode.h"
#include "ExtensionValue.h"

using Elastos::Core::IStringBuilder;
using Elastos::Security::Cert::CRLReason;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CReasonCode)
    , public ExtensionValue
    , public IReasonCode
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CReasonCode();

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

    CARAPI_(CRLReason) GetReason();

public:
    /**
     * ASN.1 Encoder/Decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    // the reason code value
    Byte mCode;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREASONCODE_H__
