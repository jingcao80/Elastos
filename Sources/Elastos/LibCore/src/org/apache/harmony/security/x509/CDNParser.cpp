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

#include "org/apache/harmony/security/x509/CDNParser.h"
#include "org/apache/harmony/security/x501/CAttributeValue.h"
#include "org/apache/harmony/security/x501/CAttributeTypeAndValue.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "elastos/utility/CArrayList.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::IAttributeValue;
using Org::Apache::Harmony::Security::X501::CAttributeValue;
using Org::Apache::Harmony::Security::X501::IAttributeTypeAndValue;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValue;
using Org::Apache::Harmony::Security::Utils::IObjectIdentifier;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDNParser)

CAR_INTERFACE_IMPL(CDNParser, Object, IDNParser)

ECode CDNParser::constructor(
    /* [in] */ const String& dn)
{
    mChars = dn.GetChars();
    return NOERROR;
}

ECode CDNParser::NextAT(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    mHasQE = FALSE; // reset

    // skip preceding space chars, they can present after
    // comma or semicolon (compatibility with RFC 1779)
    for (; mPos < mChars->GetLength() && (*mChars)[mPos] == ' '; mPos++) {
    }
    if (mPos == mChars->GetLength()) {
        *str = NULL; // reached the end of DN
        return NOERROR;
    }

    // mark the beginning of attribute type
    mBeg = mPos;

    // attribute type chars
    mPos++;
    for (; mPos < mChars->GetLength() && (*mChars)[mPos] != '=' && (*mChars)[mPos] != ' '; mPos++) {
        // we don't follow exact BNF syntax here:
        // accept any char except space and '='
    }
    if (mPos >= mChars->GetLength()) {
        // unexpected end of DN
        //throw new IOException("Invalid distinguished name string");
        *str = NULL;
        return E_IO_EXCEPTION;
    }

    // mark the end of attribute type
    mEnd = mPos;

    // skip trailing space chars between attribute type and '='
    // (compatibility with RFC 1779)
    if ((*mChars)[mPos] == ' ') {
        for (; mPos < mChars->GetLength() && (*mChars)[mPos] != '=' && (*mChars)[mPos] == ' '; mPos++) {
        }

        if ((*mChars)[mPos] != '=' || mPos == mChars->GetLength()) {
            // unexpected end of DN
            //throw new IOException("Invalid distinguished name string");
            *str = NULL;
            return E_IO_EXCEPTION;
        }
    }

    mPos++; //skip '=' char

    // skip space chars between '=' and attribute value
    // (compatibility with RFC 1779)
    for (; mPos < mChars->GetLength() && (*mChars)[mPos] == ' '; mPos++) {
    }

    // in case of oid attribute type skip its prefix: "oid." or "OID."
    // (compatibility with RFC 1779)
    if ((mEnd - mBeg > 4) && ((*mChars)[mBeg + 3] == '.')
            && ((*mChars)[mBeg] == 'O' || (*mChars)[mBeg] == 'o')
            && ((*mChars)[mBeg + 1] == 'I' || (*mChars)[mBeg + 1] == 'i')
            && ((*mChars)[mBeg + 2] == 'D' || (*mChars)[mBeg + 2] == 'd')) {
        mBeg += 4;
    }

    *str = String(*mChars, mBeg, mEnd - mBeg);
    return NOERROR;
}

ECode CDNParser::QuotedAV(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    mPos++;
    mBeg = mPos;
    mEnd = mBeg;
    while (TRUE) {
        if (mPos == mChars->GetLength()) {
            // unexpected end of DN
            //throw new IOException("Invalid distinguished name string");
            *str = NULL;
            return E_IO_EXCEPTION;
        }

        if ((*mChars)[mPos] == '"') {
            // enclosing quotation was found
            mPos++;
            break;
        }
        else if ((*mChars)[mPos] == '\\') {
            Char32 c;
            FAIL_RETURN(GetEscaped(&c))
            (*mChars)[mEnd] = c;
        }
        else {
            // shift char: required for string with escaped chars
            (*mChars)[mEnd] = (*mChars)[mPos];
        }
        mPos++;
        mEnd++;
    }

    // skip trailing space chars before comma or semicolon.
    // (compatibility with RFC 1779)
    for (; mPos < mChars->GetLength() && (*mChars)[mPos] == ' '; mPos++) {
    }

    *str = String(*mChars, mBeg, mEnd - mBeg);
    return NOERROR;
}

ECode CDNParser::HexAV(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    if (mPos + 4 >= mChars->GetLength()) {
        // encoded byte array  must be not less then 4 c
        //throw new IOException("Invalid distinguished name string");
        *str = NULL;
        return E_IO_EXCEPTION;
    }

    mBeg = mPos; // store '#' position
    mPos++;
    while (TRUE) {
        // check for end of attribute value
        // looks for space and component separators
        if (mPos == mChars->GetLength() || (*mChars)[mPos] == '+' || (*mChars)[mPos] == ','
                || (*mChars)[mPos] == ';') {
            mEnd = mPos;
            break;
        }

        if ((*mChars)[mPos] == ' ') {
            mEnd = mPos;
            mPos++;
            // skip trailing space chars before comma or semicolon.
            // (compatibility with RFC 1779)
            for (; mPos < mChars->GetLength() && (*mChars)[mPos] == ' '; mPos++) {
            }
            break;
        }
        else if ((*mChars)[mPos] >= 'A' && (*mChars)[mPos] <= 'F') {
            (*mChars)[mPos] += 32; //to low case
        }

        mPos++;
    }

    // verify length of hex string
    // encoded byte array  must be not less then 4 and must be even number
    Int32 hexLen = mEnd - mBeg; // skip first '#' char
    if (hexLen < 5 || (hexLen & 1) == 0) {
        //throw new IOException("Invalid distinguished name string");
        *str = NULL;
        return E_IO_EXCEPTION;
    }

    // get byte encoding from string representation
    mEncoded = ArrayOf<Byte>::Alloc(hexLen / 2);
    for (Int32 i = 0, p = mBeg + 1; i < mEncoded->GetLength(); p += 2, i++) {
        Int32 b;
        FAIL_RETURN(GetByte(p, &b));
        (*mEncoded)[i] = (Byte)b;
    }

    *str = String(*mChars, mBeg, hexLen);
    return NOERROR;
}

ECode CDNParser::EscapedAV(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    mBeg = mPos;
    mEnd = mPos;
    while (TRUE) {
        if (mPos >= mChars->GetLength()) {
            // the end of DN has been found
            *str = String(*mChars, mBeg, mEnd - mBeg);
            return NOERROR;
        }

        switch ((*mChars)[mPos]) {
            case '+':
            case ',':
            case ';':
            {
                // separator char has been found
                *str = String(*mChars, mBeg, mEnd - mBeg);
                return NOERROR;
            }
            case '\\':
            {
                // escaped char
                Char32 c;
                FAIL_RETURN(GetEscaped(&c))
                (*mChars)[mEnd++] = c;
                mPos++;
                break;
            }
            case ' ':
            {
                // need to figure out whether space defines
                // the end of attribute value or not
                Int32 cur = mEnd;

                mPos++;
                (*mChars)[mEnd++] = ' ';

                for (; mPos < mChars->GetLength() && (*mChars)[mPos] == ' '; mPos++) {
                    (*mChars)[mEnd++] = ' ';
                }
                if (mPos == mChars->GetLength() || (*mChars)[mPos] == ',' || (*mChars)[mPos] == '+'
                        || (*mChars)[mPos] == ';') {
                    // separator char or the end of DN has been found
                    *str = String(*mChars, mBeg, cur - mBeg);
                    return NOERROR;
                }
                break;
            }
            default:
                (*mChars)[mEnd++] = (*mChars)[mPos];
                mPos++;
        }
    }
    return NOERROR;
}

ECode CDNParser::GetEscaped(
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);

    mPos++;
    if (mPos == mChars->GetLength()) {
        //throw new IOException("Invalid distinguished name string");
        *c = 0;
        return E_IO_EXCEPTION;
    }

    Char32 ch = (*mChars)[mPos];
    switch (ch) {
    case '"':
    case '\\':
        mHasQE = TRUE;
        return ch;
    case ',':
    case '=':
    case '+':
    case '<':
    case '>':
    case '#':
    case ';':
    case ' ':
    case '*':
    case '%':
    case '_':
        //FIXME: escaping is allowed only for leading or trailing space char
        *c = ch;
        return NOERROR;
    default:
        // RFC doesn't explicitly say that escaped hex pair is
        // interpreted as UTF-8 char. It only contains an example of such DN.
        return GetUTF8(c);
    }
    return NOERROR;
}

ECode CDNParser::GetUTF8(
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);
    *c = 0;

    Int32 res;
    FAIL_RETURN(GetByte(mPos, &res))
    mPos++; //FIXME tmp

    if (res < 128) { // one byte: 0-7F
        *c = (Char32)res;
        return NOERROR;
    }
    else if (res >= 192 && res <= 247) {

        Int32 count;
        if (res <= 223) { // two bytes: C0-DF
            count = 1;
            res = res & 0x1F;
        }
        else if (res <= 239) { // three bytes: E0-EF
            count = 2;
            res = res & 0x0F;
        }
        else { // four bytes: F0-F7
            count = 3;
            res = res & 0x07;
        }

        Int32 b;
        for (Int32 i = 0; i < count; i++) {
            mPos++;
            if (mPos == mChars->GetLength() || (*mChars)[mPos] != '\\') {
                *c = 0x3F; //FIXME failed to decode UTF-8 char - return '?'
                return NOERROR;
            }
            mPos++;

            FAIL_RETURN(GetByte(mPos, &b))
            mPos++; //FIXME tmp
            if ((b & 0xC0) != 0x80) {
                *c = 0x3F; //FIXME failed to decode UTF-8 char - return '?'
                return NOERROR;
            }

            res = (res << 6) + (b & 0x3F);
        }
        *c = (Char32)res;
        return NOERROR;
    }
    else {
        *c = 0x3F; //FIXME failed to decode UTF-8 char - return '?'
        return NOERROR;
    }
    return NOERROR;
}

ECode CDNParser::GetByte(
    /* [in] */ Int32 position,
    /* [out] */ Int32* b)
{
    VALIDATE_NOT_NULL(b);
    *b = 0;

    if ((position + 1) >= mChars->GetLength()) {
        // to avoid ArrayIndexOutOfBoundsException
        //throw new IOException("Invalid distinguished name string");
        return E_IO_EXCEPTION;
    }

    Int32 b1 = (*mChars)[position];
    if (b1 >= '0' && b1 <= '9') {
        b1 = b1 - '0';
    }
    else if (b1 >= 'a' && b1 <= 'f') {
        b1 = b1 - 87; // 87 = 'a' - 10
    }
    else if (b1 >= 'A' && b1 <= 'F') {
        b1 = b1 - 55; // 55 = 'A' - 10
    }
    else {
        //throw new IOException("Invalid distinguished name string");
        return E_IO_EXCEPTION;
    }

    Int32 b2 = (*mChars)[position + 1];
    if (b2 >= '0' && b2 <= '9') {
        b2 = b2 - '0';
    }
    else if (b2 >= 'a' && b2 <= 'f') {
        b2 = b2 - 87; // 87 = 'a' - 10
    }
    else if (b2 >= 'A' && b2 <= 'F') {
        b2 = b2 - 55; // 55 = 'A' - 10
    }
    else {
        //throw new IOException("Invalid distinguished name string");
        return E_IO_EXCEPTION;
    }

    *b = (b1 << 4) + b2;
    return NOERROR;
}

ECode CDNParser::Parse(
    /* [out] */ IList** ppList)
{
    VALIDATE_NOT_NULL(ppList);
    *ppList = NULL;

    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);

    String attType;
    FAIL_RETURN(NextAT(&attType));
    if (attType.IsNull()) {
        *ppList = list; //empty list of RDNs
        REFCOUNT_ADD(*ppList);
        return NOERROR;
    }
    AutoPtr<IObjectIdentifier> oid;
    CAttributeTypeAndValue::GetObjectIdentifier(attType, (IObjectIdentifier**)&oid);

    AutoPtr<IList> atav;
    CArrayList::New((IList**)&atav);
    while (TRUE) {
        if (mPos == mChars->GetLength()) {
            //empty Attribute Value
            AutoPtr<IAttributeValue> av;
            CAttributeValue::New(String(""), FALSE, oid, (IAttributeValue**)&av);
            AutoPtr<IAttributeTypeAndValue> atv;
            CAttributeTypeAndValue::New(oid, av, (IAttributeTypeAndValue**)&atv);
            atav->Add(TO_IINTERFACE(atv));
            list->Add(0, TO_IINTERFACE(atav));
            *ppList = list;
            REFCOUNT_ADD(*ppList);
            return NOERROR;
        }

        switch ((*mChars)[mPos]) {
            case '"':
            {
                String str;
                FAIL_RETURN(QuotedAV(&str))
                AutoPtr<IAttributeValue> av;
                CAttributeValue::New(str, mHasQE, oid, (IAttributeValue**)&av);
                AutoPtr<IAttributeTypeAndValue> atv;
                CAttributeTypeAndValue::New(oid, av, (IAttributeTypeAndValue**)&atv);
                atav->Add(TO_IINTERFACE(atv));
                break;
            }
            case '#':
            {
                String str;
                FAIL_RETURN(HexAV(&str))
                AutoPtr<IAttributeValue> av;
                CAttributeValue::New(str, mEncoded, (IAttributeValue**)&av);
                AutoPtr<IAttributeTypeAndValue> atv;
                CAttributeTypeAndValue::New(oid, av, (IAttributeTypeAndValue**)&atv);
                atav->Add(TO_IINTERFACE(atv));
                break;
            }
            case '+':
            case ',':
            case ';': // compatibility with RFC 1779: semicolon can separate RDNs
            {
                //empty attribute value
                AutoPtr<IAttributeValue> av;
                CAttributeValue::New(String(""), FALSE, oid, (IAttributeValue**)&av);
                AutoPtr<IAttributeTypeAndValue> atv;
                CAttributeTypeAndValue::New(oid, av, (IAttributeTypeAndValue**)&atv);
                atav->Add(TO_IINTERFACE(atv));
                break;
            }
            default:
            {
                String str;
                FAIL_RETURN(EscapedAV(&str))
                AutoPtr<IAttributeValue> av;
                CAttributeValue::New(str, mHasQE, oid, (IAttributeValue**)&av);
                AutoPtr<IAttributeTypeAndValue> atv;
                CAttributeTypeAndValue::New(oid, av, (IAttributeTypeAndValue**)&atv);
                atav->Add(TO_IINTERFACE(atv));
            }
        }

        if (mPos >= mChars->GetLength()) {
            list->Add(0, TO_IINTERFACE(atav));
            *ppList = list;
            REFCOUNT_ADD(*ppList);
            return NOERROR;
        }

        if ((*mChars)[mPos] == ',' || (*mChars)[mPos] == ';') {
            list->Add(0, TO_IINTERFACE(atav));
            CArrayList::New((IList**)&atav);
        }
        else if ((*mChars)[mPos] != '+') {
            //throw new IOException("Invalid distinguished name string");
            return E_IO_EXCEPTION;
        }

        mPos++;
        FAIL_RETURN(NextAT(&attType));
        if (attType.IsNull()) {
            //throw new IOException("Invalid distinguished name string");
            return E_IO_EXCEPTION;
        }
        CAttributeTypeAndValue::GetObjectIdentifier(attType, (IObjectIdentifier**)&oid);
    }

    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org