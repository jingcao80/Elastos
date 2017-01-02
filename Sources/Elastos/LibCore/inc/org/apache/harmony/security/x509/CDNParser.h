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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__

#include "_Org_Apache_Harmony_Security_X509_CDNParser.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
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

CarClass(CDNParser)
    , public Object
    , public IDNParser
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Parse(
        /* [out] */ IList** ppList);

    CARAPI constructor(
        /* [in] */ const String& dn);

protected:
    /**
     * Decodes a UTF-8 char.
     */
    CARAPI GetUTF8(
        /* [out] */ Char32* c);

private:
    /**
     * Returns the next attribute type: (ALPHA 1*keychar) / oid
     */
    CARAPI NextAT(
        /* [out] */ String* str);

    /**
     * Returns a quoted attribute value: QUOTATION *( quotechar / pair ) QUOTATION
     */
    CARAPI QuotedAV(
        /* [out] */ String* str);

    /**
     * Returns a hex string attribute value: "#" hexstring
     */
    CARAPI HexAV(
        /* [out] */ String* str);

    /**
     * Returns a string attribute value: *( stringchar / pair ).
     */
    CARAPI EscapedAV(
        /* [out] */ String* str);

    /**
     * Returns an escaped char
     */
    CARAPI GetEscaped(
        /* [out] */ Char32* c);

    /**
     * Returns byte representation of a char pair
     * The char pair is composed of DN char in
     * specified 'position' and the next char
     * According to BNF syntax:
     * hexchar    = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
     *                     / "a" / "b" / "c" / "d" / "e" / "f"
     */
    CARAPI GetByte(
        /* [in] */ Int32 position,
        /* [out] */ Int32* b);

private:
    Int32 mPos;
    Int32 mBeg;
    Int32 mEnd;

    /** distinguished name chars */
    AutoPtr<ArrayOf<Char32> > mChars;

    /** raw string contains '"' or '\' */
    Boolean mHasQE;

    /** DER encoding of currently parsed item */
    AutoPtr<ArrayOf<Byte> > mEncoded;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__
