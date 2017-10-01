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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__

#include "ASN1Type.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

/**
 * This class is the super class for all string ASN.1 types
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1StringType
    : public ASN1Type
    , public IASN1StringType
{
public:
    ASN1StringType(
        /* [in] */ Int32 tagNumber);

    CARAPI constructor(
        /* [in] */ Int32 tagNumber);

    CAR_INTERFACE_DECL()

    /**
     * Tests provided identifier.
     *
     * @param identifier identifier to be verified
     * @return true if identifier correspond to primitive or constructed
     *     identifier of this ASN.1 string type, otherwise false
     */
    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [ou] */ Boolean* result);

    using ASN1Type::Decode;

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

protected:
    ASN1StringType() {}

public:
    // TODO: what about defining them as separate classes?
    // TODO: check decoded/encoded characters
    static const AutoPtr<IASN1StringType> BMPSTRING;

    static const AutoPtr<IASN1StringType> IA5STRING;

    static const AutoPtr<IASN1StringType> GENERALSTRING;

    static const AutoPtr<IASN1StringType> PRINTABLESTRING;

    static const AutoPtr<IASN1StringType> TELETEXSTRING;

    static const AutoPtr<IASN1StringType> UNIVERSALSTRING;

    static const AutoPtr<IASN1StringType> UTF8STRING;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__
