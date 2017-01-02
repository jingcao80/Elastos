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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeValue.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::ICollection;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Utils::IObjectIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

/**
 * X.501 Attribute Value
 */
CarClass(CAttributeValue)
    , public Object
    , public IAttributeValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetWasEncoded(
        /* [out] */ Boolean * pWasEncoded);

    CARAPI SetWasEncoded(
        /* [in] */ Boolean wasEncoded);

    CARAPI GetEscapedString(
        /* [out] */ String * pWasEncoded);

    CARAPI SetEscapedString(
        /* [in] */ const String& wasEncoded);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI SetEncoded(
        /* [in] */ ArrayOf<Byte> * pEncoded);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte> ** ppBytes);

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte> * pBytes);

    CARAPI GetHasQE(
        /* [out] */ Boolean * pHasQE);

    CARAPI SetHasQE(
        /* [in] */ Boolean hasQE);

    CARAPI GetRawString(
        /* [out] */ String * pRawString);

    CARAPI SetRawString(
        /* [in] */ const String& rawString);

    CARAPI GetTag(
        /* [out] */ Int32 * pTag);

    CARAPI GetHexString(
        /* [out] */ String * pHexString);

    CARAPI AppendQEString(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI MakeCanonical(
        /* [out] */ String * pCannonical);

    CARAPI constructor(
        /* [in] */ const String& parsedString,
        /* [in] */ Boolean hasQorE,
        /* [in] */ IObjectIdentifier* oid);

    CARAPI constructor(
        /* [in] */ const String& hexString,
        /* [in] */ ArrayOf<Byte> * pEncoded);

    CARAPI constructor(
        /* [in] */ const String& rawString,
        /* [in] */ ArrayOf<Byte> * pEncoded,
        /* [in] */ Int32 tag);

    CARAPI GetValues(
        /* [in] */ IASN1Type* type,
        /* [out] */ ICollection** result) /*throws IOException*/;

    /**
     * Removes escape sequences used in RFC1779 escaping but not in RFC2253 and
     * returns the RFC2253 string to the caller..
     */
    CARAPI GetRFC2253String(
        /* [out] */ String* result);

private:
    /**
     * Checks if the string is PrintableString (see X.680)
     */
    static CARAPI_(Boolean) IsPrintableString(
        /* [in] */ const String& str);

    /**
     * Escapes:
     * 1) chars ",", "+", """, "\", "<", ">", ";" (RFC 2253)
     * 2) chars "#", "=" (required by RFC 1779)
     * 3) leading or trailing spaces
     * 4) consecutive spaces (RFC 1779)
     * 5) according to the requirement to be RFC 1779 compatible:
     *    '#' char is escaped in any position
     */
    CARAPI_(String) MakeEscaped(
        /* [in] */ const String& name);

public:
    Boolean mWasEncoded;

    String mEscapedString;

    AutoPtr<ArrayOf<Byte> > mEncoded;

    AutoPtr<ArrayOf<Byte> > mBytes; //FIXME remove??? bytes to be encoded

    Boolean mHasQE; // raw string contains '"' or '\'

    String mRawString;

private:
    Boolean mHasConsecutiveSpaces;
    String mRfc2253String;
    String mHexString;
    Int32 mTag;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__
