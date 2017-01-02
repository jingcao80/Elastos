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

#include "CAttributeValue.h"
#include "CAttributeTypeAndValue.h"
#include "DirectoryString.h"
#include "ASN1StringType.h"
#include "CoreUtils.h"
#include "CASN1SetOf.h"
#include "CDerInputStream.h"
#include "StringBuilder.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::ASN1StringType;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::CDerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IDerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CAttributeValue)

CAR_INTERFACE_IMPL(CAttributeValue, Object, IAttributeValue)

ECode CAttributeValue::GetWasEncoded(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWasEncoded;
    return NOERROR;
}

ECode CAttributeValue::SetWasEncoded(
    /* [in] */ Boolean wasEncoded)
{
    mWasEncoded = wasEncoded;
    return NOERROR;
}

ECode CAttributeValue::GetEscapedString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEscapedString;
    return NOERROR;
}

ECode CAttributeValue::SetEscapedString(
    /* [in] */ const String& escapedString)
{
    mEscapedString = escapedString;
    return NOERROR;
}

ECode CAttributeValue::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEncoded;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeValue::SetEncoded(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mEncoded = encoded;
    return NOERROR;
}

ECode CAttributeValue::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeValue::SetBytes(
    /* [in] */ ArrayOf<Byte> * bytes)
{
    mBytes = bytes;
    return NOERROR;
}

ECode CAttributeValue::GetHasQE(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasQE;
    return NOERROR;
}

ECode CAttributeValue::SetHasQE(
    /* [in] */ Boolean hasQE)
{
    mHasQE = hasQE;
    return NOERROR;
}

ECode CAttributeValue::GetRawString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRawString;
    return NOERROR;
}

ECode CAttributeValue::SetRawString(
    /* [in] */ const String& rawString)
{
    mRawString = rawString;
    return NOERROR;
}

ECode CAttributeValue::GetTag(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTag;
    return NOERROR;
}

ECode CAttributeValue::GetHexString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    if (mHexString == NULL) {
        if (!mWasEncoded) {
            //FIXME optimize me: what about reusable OutputStream???
            if (mTag == ((ASN1StringType*)ASN1StringType::IA5STRING.Get())->mId) {
                IASN1Type::Probe(ASN1StringType::IA5STRING)->Encode(CoreUtils::Convert(mRawString), (ArrayOf<Byte>**)&mEncoded);
            }
            else if (mTag == ((ASN1StringType*)ASN1StringType::PRINTABLESTRING.Get())->mId) {
                IASN1Type::Probe(ASN1StringType::PRINTABLESTRING)->Encode(CoreUtils::Convert(mRawString), (ArrayOf<Byte>**)&mEncoded);
            }
            else {
                IASN1Type::Probe(ASN1StringType::UTF8STRING)->Encode(CoreUtils::Convert(mRawString), (ArrayOf<Byte>**)&mEncoded);
            }
            mWasEncoded = TRUE;
        }

        StringBuilder buf(mEncoded->GetLength() * 2 + 1);
        buf.AppendChar('#');

        for (Int32 i = 0, c; i < mEncoded->GetLength(); i++) {
            c = ((*mEncoded)[i] >> 4) & 0x0F;
            if (c < 10) {
                buf.AppendChar((Char32) (c + 48));
            } else {
                buf.AppendChar((Char32) (c + 87));
            }

            c = (*mEncoded)[i] & 0x0F;
            if (c < 10) {
                buf.AppendChar((Char32) (c + 48));
            } else {
                buf.AppendChar((Char32) (c + 87));
            }
        }
        mHexString = buf.ToString();
    }
    *result = mHexString;
    return NOERROR;
}

ECode CAttributeValue::GetValues(
    /* [in] */ IASN1Type* type,
    /* [out] */ ICollection** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IASN1SetOf> ao;
    CASN1SetOf::New(type, (IASN1SetOf**)&ao);
    AutoPtr<IInterface> c;
    IASN1Type::Probe(ao)->Decode(mEncoded, (IInterface**)&c);
    *result = ICollection::Probe(c);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeValue::AppendQEString(
    /* [in] */ Elastos::Core::IStringBuilder* sb)
{
    sb->AppendChar('"');
    if (mHasQE) {
        Char32 c;
        for (Int32 i = 0; i < mRawString.GetLength(); i++) {
            c = mRawString.GetChar(i);
            if (c == '"' || c == '\\') {
                sb->AppendChar('\\');
            }
            sb->AppendChar(c);
        }
    }
    else {
        sb->Append(mRawString);
    }
    sb->AppendChar('"');
    return NOERROR;
}

String CAttributeValue::MakeEscaped(
    /* [in] */ const String& name)
{
    Int32 length = name.GetLength();
    if (length == 0) {
        return name;
    }
    StringBuilder buf(length * 2);

    // Keeps track of whether we are escaping spaces.
    Boolean escapeSpaces = FALSE;

    for (Int32 index = 0; index < length; index++) {
        Char32 ch = name.GetChar(index);
        switch (ch) {
        case ' ': {
            /*
             * We should escape spaces in the following cases:
             *   1) at the beginning
             *   2) at the end
             *   3) consecutive spaces
             * Since multiple spaces at the beginning or end will be covered by
             * 3, we don't need a special case to check for that. Note that RFC 2253
             * doesn't escape consecutive spaces, so they are removed in
             * getRFC2253String instead of making two different strings here.
             */
            if (index < (length - 1)) {
                Boolean nextIsSpace = name.GetChar(index + 1) == ' ';
                escapeSpaces = escapeSpaces || nextIsSpace || index == 0;
                mHasConsecutiveSpaces |= nextIsSpace;
            }
            else {
                escapeSpaces = TRUE;
            }

            if (escapeSpaces) {
                buf.AppendChar('\\');
            }

            buf.AppendChar(' ');
            break;
        }

        case '"':
        case '\\':
            mHasQE = TRUE;
            buf.AppendChar('\\');
            buf.AppendChar(ch);
            break;

        case ',':
        case '+':
        case '<':
        case '>':
        case ';':
        case '#': // required by RFC 1779
        case '=': // required by RFC 1779
            buf.AppendChar('\\');
            buf.AppendChar(ch);
            break;

        default:
            buf.AppendChar(ch);
            break;
        }

        if (escapeSpaces && ch != ' ') {
            escapeSpaces = FALSE;
        }
    }

    return buf.ToString();
}

ECode CAttributeValue::MakeCanonical(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    Int32 length = mRawString.GetLength();
    if (length == 0) {
        *result = mRawString;
        return NOERROR;
    }
    StringBuilder buf(length * 2);

    Int32 index = 0;
    if (mRawString.GetChar(0) == '#') {
        buf.AppendChar('\\');
        buf.AppendChar('#');
        index++;
    }

    Int32 bufLength;
    for (; index < length; index++) {
        Char32 ch = mRawString.GetChar(index);

        switch (ch) {
        case ' ':
            buf.GetLength(&bufLength);
            if (bufLength == 0 || buf.GetCharAt(bufLength - 1) == ' ') {
                break;
            }
            buf.AppendChar(' ');
            break;

        case '"':
        case '\\':
        case ',':
        case '+':
        case '<':
        case '>':
        case ';':
            buf.AppendChar('\\');

        default:
            buf.AppendChar(ch);
        }
    }

    //remove trailing spaces
    for (bufLength = buf.GetLength() - 1; bufLength > -1
            && buf.GetCharAt(bufLength) == ' '; bufLength--) {
    }
    buf.SetLength(bufLength + 1);

    return buf.ToString(result);
}

ECode CAttributeValue::GetRFC2253String(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mHasConsecutiveSpaces) {
        *result = mEscapedString;
        return NOERROR;
    }

    if (mRfc2253String == NULL) {
        // Scan backwards first since runs of spaces at the end are escaped.
        Int32 lastIndex = mEscapedString.GetLength() - 2;
        for (Int32 i = lastIndex; i > 0; i -= 2) {
            if (mEscapedString.GetChar(i) == '\\' && mEscapedString.GetChar(i + 1) == ' ') {
                lastIndex = i - 1;
            }
        }

        Boolean beginning = TRUE;
        StringBuilder sb(mEscapedString.GetLength());
        for (Int32 i = 0; i < mEscapedString.GetLength(); i++) {
            Char32 ch = mEscapedString.GetChar(i);
            if (ch != '\\') {
                sb.AppendChar(ch);
                beginning = FALSE;
            }
            else {
                Char32 nextCh = mEscapedString.GetChar(i + 1);
                if (nextCh == ' ') {
                    if (beginning || i > lastIndex) {
                        sb.AppendChar(ch);
                    }
                    sb.AppendChar(nextCh);
                }
                else {
                    sb.AppendChar(ch);
                    sb.AppendChar(nextCh);
                    beginning = FALSE;
                }

                i++;
            }
        }
        mRfc2253String = sb.ToString();
    }
    *result = mRfc2253String;
    return NOERROR;
}

ECode CAttributeValue::constructor(
    /* [in] */ const String& parsedString,
    /* [in] */ Boolean hasQorE,
    /* [in] */ IObjectIdentifier* oid)
{
    mWasEncoded = FALSE;

    mHasQE = hasQorE;
    mRawString = parsedString;
    mEscapedString = MakeEscaped(mRawString); // overwrites hasQE

    Int32 tag;
    if (oid == CAttributeTypeAndValue::EMAILADDRESS.Get() || oid == CAttributeTypeAndValue::DC.Get()) {
        // http://www.rfc-editor.org/rfc/rfc5280.txt
        // says that EmailAddress and DomainComponent should be a IA5String
        tag = ((ASN1StringType*)ASN1StringType::IA5STRING.Get())->mId;
    }
    else if (IsPrintableString(mRawString)) {
        tag = ((ASN1StringType*)ASN1StringType::PRINTABLESTRING.Get())->mId;
    }
    else {
        tag = ((ASN1StringType*)ASN1StringType::UTF8STRING.Get())->mId;
    }
    mTag = tag;
    return NOERROR;
}

ECode CAttributeValue::constructor(
    /* [in] */ const String& hexString,
    /* [in] */ ArrayOf<Byte> * encoded)
{
    mWasEncoded = TRUE;

    mHexString = hexString;
    mEncoded = encoded;

    ECode ec = NOERROR;
    do {
        AutoPtr<IDerInputStream> in;
        ec = CDerInputStream::New(encoded, (IDerInputStream**)&in);
        if (FAILED(ec)) {
            break;
        }

        mTag = ((CDerInputStream*)in.Get())->mTag;

        Boolean tmp = FALSE;
        if (ec = IASN1Type::Probe(DirectoryString::ASN1)->CheckTag(mTag, &tmp), tmp) {
            if (FAILED(ec)) {
                break;
            }

            // has string representation
            AutoPtr<IInterface> obj;
            ec = IASN1Type::Probe(DirectoryString::ASN1)->Decode(IBerInputStream::Probe(in), (IInterface**)&obj);
            if (FAILED(ec)) {
                break;
            }
            ICharSequence::Probe(obj)->ToString(&mRawString);
            mEscapedString = MakeEscaped(mRawString);
        }
        else {
            mRawString = hexString;
            mEscapedString = hexString;
        }
    } while (0);

    if (ec == (ECode)E_IO_EXCEPTION) {
        // IllegalArgumentException iae = new IllegalArgumentException(); //FIXME message
        // iae.initCause(e);
        // throw iae;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return ec;
}

ECode CAttributeValue::constructor(
    /* [in] */ const String& rawString,
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 tag)
{
    mWasEncoded = TRUE;

    mEncoded = encoded;
    mTag = tag;

    if (rawString == NULL) {
        GetHexString(&mRawString);
        mEscapedString = mHexString;
    }
    else {
        mRawString = rawString;
        mEscapedString = MakeEscaped(rawString);
    }
    return NOERROR;
}

Boolean CAttributeValue::IsPrintableString(
    /* [in] */ const String& str)
{
    for (Int32 i = 0; i < str.GetLength(); ++i) {
        Char32 ch = str.GetChar(i);
        if (!(ch == 0x20
        || (ch >= 0x27 && ch<= 0x29) // '()
        || (ch >= 0x2B && ch<= 0x3A) // +,-./0-9:
        || ch == '='
        || ch == '?'
        || (ch >= 'A' && ch<= 'Z')
        || (ch >= 'a' && ch<= 'z'))) {
            return FALSE;
        }
    }
    return TRUE;
}

}
}
}
}
}
