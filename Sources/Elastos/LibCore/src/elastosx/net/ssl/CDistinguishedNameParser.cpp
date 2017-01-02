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

#include "CDistinguishedNameParser.h"
#include <elastos/utility/etl/List.h>
#include "Collections.h"
#include "CArrayList.h"
#include "CoreUtils.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::Collections;
using Elastos::Utility::Etl::List;
using Elastos::Utility::CArrayList;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CDistinguishedNameParser, Object, IDistinguishedNameParser)

CAR_OBJECT_IMPL(CDistinguishedNameParser)

ECode CDistinguishedNameParser::constructor(
    /* [in] */ IX500Principal* principal)
{
    // RFC2253 is used to ensure we get attributes in the reverse
    // order of the underlying ASN.1 encoding, so that the most
    // significant values of repeated attributes occur first.
    FAIL_RETURN(principal->GetName(IX500Principal::RFC2253, &mDn))
    mLength = mDn.GetLength();
    return NOERROR;
}

ECode CDistinguishedNameParser::NextAT(
    /* [out] */ String* at)
{
    VALIDATE_NOT_NULL(at)

    // skip preceding space mChars, they can present after
    // comma or semicolon (compatibility with RFC 1779)
    for (; mPos < mLength && (*mChars)[mPos] == ' '; mPos++) {
    }
    if (mPos == mLength) {
        *at = String(NULL); // reached the end of DN
        return NOERROR;
    }

    // mark the beginning of attribute type
    mBeg = mPos;

    // attribute type mChars
    mPos++;
    for (; mPos < mLength && (*mChars)[mPos] != '=' && (*mChars)[mPos] != ' '; mPos++) {
        // we don't follow exact BNF syntax here:
        // accept any char except space and '='
    }
    if (mPos >= mLength) {
        //throw new IllegalStateException("Unexpected end of DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // mark the end of attribute type
    mEnd = mPos;

    // skip trailing space mChars between attribute type and '='
    // (compatibility with RFC 1779)
    if ((*mChars)[mPos] == ' ') {
        for (; mPos < mLength && (*mChars)[mPos] != '=' && (*mChars)[mPos] == ' '; mPos++) {
        }

        if ((*mChars)[mPos] != '=' || mPos == mLength) {
            //throw new IllegalStateException("Unexpected end of DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    mPos++; //skip '=' char

    // skip space mChars between '=' and attribute value
    // (compatibility with RFC 1779)
    for (; mPos < mLength && (*mChars)[mPos] == ' '; mPos++) {
    }

    // in case of oid attribute type skip its prefix: "oid." or "OID."
    // (compatibility with RFC 1779)
    if ((mEnd - mBeg > 4) && ((*mChars)[mBeg + 3] == '.')
            && ((*mChars)[mBeg] == 'O' || (*mChars)[mBeg] == 'o')
            && ((*mChars)[mBeg + 1] == 'I' || (*mChars)[mBeg + 1] == 'i')
            && ((*mChars)[mBeg + 2] == 'D' || (*mChars)[mBeg + 2] == 'd')) {
        mBeg += 4;
    }

    *at = String(*mChars, mBeg, mEnd - mBeg);
    return NOERROR;
}

ECode CDistinguishedNameParser::QuotedAV(
    /* [out] */ String* av)
{
    VALIDATE_NOT_NULL(av)

    mPos++;
    mBeg = mPos;
    mEnd = mBeg;
    while (TRUE) {

        if (mPos == mLength) {
            //throw new IllegalStateException("Unexpected end of DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        if ((*mChars)[mPos] == '"') {
            // enclosing quotation was found
            mPos++;
            break;
        } else if ((*mChars)[mPos] == '\\') {
            FAIL_RETURN(GetEscaped(&(*mChars)[mEnd]))
        } else {
            // shift char: required for string with escaped chars
            (*mChars)[mEnd] = (*mChars)[mPos];
        }
        mPos++;
        mEnd++;
    }

    // skip trailing space chars before comma or semicolon.
    // (compatibility with RFC 1779)
    for (; mPos < mLength && (*mChars)[mPos] == ' '; mPos++) {
    }

    *av = String(*mChars, mBeg, mEnd - mBeg);
    return NOERROR;
}

ECode CDistinguishedNameParser::HexAV(
    /* [out] */ String* av)
{
    VALIDATE_NOT_NULL(av)

    if (mPos + 4 >= mLength) {
        // encoded byte array  must be not less then 4 c
        //throw new IllegalStateException("Unexpected end of DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mBeg = mPos; // store '#' position
    mPos++;
    while (TRUE) {

        // check for end of attribute value
        // looks for space and component separators
        if (mPos == mLength || (*mChars)[mPos] == '+' || (*mChars)[mPos] == ','
                || (*mChars)[mPos] == ';') {
            mEnd = mPos;
            break;
        }

        if ((*mChars)[mPos] == ' ') {
            mEnd = mPos;
            mPos++;
            // skip trailing space chars before comma or semicolon.
            // (compatibility with RFC 1779)
            for (; mPos < mLength && (*mChars)[mPos] == ' '; mPos++) {
            }
            break;
        } else if ((*mChars)[mPos] >= 'A' && (*mChars)[mPos] <= 'F') {
            (*mChars)[mPos] += 32; //to low case
        }

        mPos++;
    }

    // verify length of hex string
    // encoded byte array  must be not less then 4 and must be even number
    Int32 hexLen = mEnd - mBeg; // skip first '#' char
    if (hexLen < 5 || (hexLen & 1) == 0) {
        //throw new IllegalStateException("Unexpected end of DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // get byte encoding from string representation
    AutoPtr<ArrayOf<Byte> > encoded = ArrayOf<Byte>::Alloc(hexLen / 2);
    for (Int32 i = 0, p = mBeg + 1; i < encoded->GetLength(); p += 2, i++) {
        Int32 value;
        FAIL_RETURN(GetByte(p, &value))
        (*encoded)[i] = (Byte)value;
    }

    *av = String(*mChars, mBeg, hexLen);
    return NOERROR;
}

ECode CDistinguishedNameParser::EscapedAV(
    /* [out] */ String* av)
{
    VALIDATE_NOT_NULL(av)

    mBeg = mPos;
    mEnd = mPos;
    while (TRUE) {
        if (mPos >= mLength) {
            // the end of DN has been found
            *av = String(*mChars, mBeg, mEnd - mBeg);
            return NOERROR;
        }

        switch ((*mChars)[mPos]) {
        case '+':
        case ',':
        case ';':
            // separator char has been found
            *av = String(*mChars, mBeg, mEnd - mBeg);
            return NOERROR;
        case '\\':
            // escaped char
            FAIL_RETURN(GetEscaped(&(*mChars)[mEnd++]))
            mPos++;
            break;
        case ' ':
            // need to figure out whether space defines
            // the end of attribute value or not
            mCur = mEnd;

            mPos++;
            (*mChars)[mEnd++] = ' ';

            for (; mPos < mLength && (*mChars)[mPos] == ' '; mPos++) {
                (*mChars)[mEnd++] = ' ';
            }
            if (mPos == mLength || (*mChars)[mPos] == ',' || (*mChars)[mPos] == '+'
                    || (*mChars)[mPos] == ';') {
                // separator char or the end of DN has been found
                *av = String(*mChars, mBeg, mCur - mBeg);
                return NOERROR;
            }
            break;
        default:
            (*mChars)[mEnd++] = (*mChars)[mPos];
            mPos++;
        }
    }
}

ECode CDistinguishedNameParser::GetEscaped(
    /* [out] */ Char32* escaped)
{
    VALIDATE_NOT_NULL(escaped)

    mPos++;
    if (mPos == mLength) {
        //throw new IllegalStateException("Unexpected end of DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    switch ((*mChars)[mPos]) {
    case '"':
    case '\\':
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
        return (*mChars)[mPos];
    default:
        // RFC doesn't explicitly say that escaped hex pair is
        // interpreted as UTF-8 char. It only contains an example of such DN.
        return GetUTF8(escaped);
    }
    return NOERROR;
}

ECode CDistinguishedNameParser::GetUTF8(
    /* [out] */ Char32* utf8)
{
    VALIDATE_NOT_NULL(utf8)

    Int32 res;
    FAIL_RETURN(GetByte(mPos, &res))
    mPos++; //FIXME tmp

    if (res < 128) { // one byte: 0-7F
        *utf8 = (Char32) res;
        return NOERROR;
    } else if (res >= 192 && res <= 247) {

        Int32 count;
        if (res <= 223) { // two bytes: C0-DF
            count = 1;
            res = res & 0x1F;
        } else if (res <= 239) { // three bytes: E0-EF
            count = 2;
            res = res & 0x0F;
        } else { // four bytes: F0-F7
            count = 3;
            res = res & 0x07;
        }

        Int32 b;
        for (Int32 i = 0; i < count; i++) {
            mPos++;
            if (mPos == mLength || (*mChars)[mPos] != '\\') {
                *utf8 =  0x3F; //FIXME failed to decode UTF-8 char - return '?'
                return NOERROR;
            }
            mPos++;

            FAIL_RETURN(GetByte(mPos, &b))
            mPos++; //FIXME tmp
            if ((b & 0xC0) != 0x80) {
                *utf8 = 0x3F; //FIXME failed to decode UTF-8 char - return '?'
                return NOERROR;
            }

            res = (res << 6) + (b & 0x3F);
        }
        *utf8 = (Char32)res;
        return NOERROR;
    } else {
        *utf8 = 0x3F; //FIXME failed to decode UTF-8 char - return '?'
        return NOERROR;
    }
    return NOERROR;
}

ECode CDistinguishedNameParser::GetByte(
    /* [in] */ Int32 position,
    /* [out] */ Int32* _byte)
{
    VALIDATE_NOT_NULL(_byte)

    if (position + 1 >= mLength) {
        //throw new IllegalStateException("Malformed DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 b1, b2;

    b1 = (*mChars)[position];
    if (b1 >= '0' && b1 <= '9') {
        b1 = b1 - '0';
    } else if (b1 >= 'a' && b1 <= 'f') {
        b1 = b1 - 87; // 87 = 'a' - 10
    } else if (b1 >= 'A' && b1 <= 'F') {
        b1 = b1 - 55; // 55 = 'A' - 10
    } else {
        //throw new IllegalStateException("Malformed DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    b2 = (*mChars)[position + 1];
    if (b2 >= '0' && b2 <= '9') {
        b2 = b2 - '0';
    } else if (b2 >= 'a' && b2 <= 'f') {
        b2 = b2 - 87; // 87 = 'a' - 10
    } else if (b2 >= 'A' && b2 <= 'F') {
        b2 = b2 - 55; // 55 = 'A' - 10
    } else {
        //throw new IllegalStateException("Malformed DN: " + dn);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *_byte = (b1 << 4) + b2;
    return NOERROR;
}

ECode CDistinguishedNameParser::FindMostSpecific(
    /* [in] */ const String& attributeType,
    /* [out] */ String* specific)
{
    VALIDATE_NOT_NULL(specific)

    // Initialize internal state.
    mPos = 0;
    mBeg = 0;
    mEnd = 0;
    mCur = 0;
    mChars = mDn.GetChars();

    String attType;
    FAIL_RETURN(NextAT(&attType))
    if (attType.IsNull()) {
        *specific = String(NULL);
        return NOERROR;
    }
    while (TRUE) {
        String attValue = String("");

        if (mPos == mLength) {
            *specific = String(NULL);
            return NOERROR;
        }

        switch ((*mChars)[mPos]) {
        case '"':
            FAIL_RETURN(QuotedAV(&attValue))
            break;
        case '#':
            FAIL_RETURN(HexAV(&attValue))
            break;
        case '+':
        case ',':
        case ';': // compatibility with RFC 1779: semicolon can separate RDNs
            //empty attribute value
            break;
        default:
            FAIL_RETURN(EscapedAV(&attValue))
        }

        // Values are ordered from most specific to least specific
        // due to the RFC2253 formatting. So take the first match
        // we see.
        if (attributeType.EqualsIgnoreCase(attType)) {
            *specific = attValue;
            return NOERROR;
        }

        if (mPos >= mLength) {
            *specific = String(NULL);
            return NOERROR;
        }

        if ((*mChars)[mPos] == ',' || (*mChars)[mPos] == ';') {
        } else if ((*mChars)[mPos] != '+') {
            //throw new IllegalStateException("Malformed DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        mPos++;
        FAIL_RETURN(NextAT(&attType))
        if (attType.IsNull()) {
            //throw new IllegalStateException("Malformed DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CDistinguishedNameParser::GetAllMostSpecificFirst(
    /* [in] */ const String& attributeType,
    /* [out] */ IList** frist)
{
    VALIDATE_NOT_NULL(frist)

    // Initialize internal state.
    mPos = 0;
    mBeg = 0;
    mEnd = 0;
    mCur = 0;
    mChars = mDn.GetChars();
    AutoPtr<IList> result;
    FAIL_RETURN(Collections::GetEmptyList((IList**)&result))

    String attType;
    FAIL_RETURN(NextAT(&attType))
    if (attType.IsNull()) {
        *frist = result;
        REFCOUNT_ADD(*frist);
        return NOERROR;
    }
    while (mPos < mLength) {
        String attValue = String("");

        switch ((*mChars)[mPos]) {
        case '"':
            FAIL_RETURN(QuotedAV(&attValue))
            break;
        case '#':
            FAIL_RETURN(HexAV(&attValue))
            break;
        case '+':
        case ',':
        case ';': // compatibility with RFC 1779: semicolon can separate RDNs
            //empty attribute value
            break;
        default:
            FAIL_RETURN(EscapedAV(&attValue))
        }

        // Values are ordered from most specific to least specific
        // due to the RFC2253 formatting. So take the first match
        // we see.
        if (attributeType.EqualsIgnoreCase(attType)) {
            Boolean isEmp;
            if (ICollection::Probe(result)->IsEmpty(&isEmp), isEmp) {
                //result = new ArrayList<String>();
                CArrayList::New((IList**)&result);
            }
            ICollection* c = ICollection::Probe(result);
            AutoPtr<ICharSequence> c1 = CoreUtils::Convert(attValue);
            c->Add(c1);
        }

        if (mPos >= mLength) {
            break;
        }

        if ((*mChars)[mPos] == ',' || (*mChars)[mPos] == ';') {
        } else if ((*mChars)[mPos] != '+') {
            //throw new IllegalStateException("Malformed DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        mPos++;
        FAIL_RETURN(NextAT(&attType))
        if (attType.IsNull()) {
            //throw new IllegalStateException("Malformed DN: " + dn);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    *frist = result;
    REFCOUNT_ADD(*frist);
    return NOERROR;
}


} // namespace Ssl
} // namespace Net
} // namespace Elastosx
