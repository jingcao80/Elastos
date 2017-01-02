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

#include "elastos/droid/net/UrlQuerySanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerIllegalCharacterValueSanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerParameterValuePair.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::CObject;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ISet;
using Elastos::Utility::IStringTokenizer;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(UrlQuerySanitizer, Object, IUrlQuerySanitizer)

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAllIllegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::ALL_ILLEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAllButNulLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::ALL_BUT_NUL_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAllButWhitespaceLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::ALL_BUT_WHITESPACE_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sURLLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::URL_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sUrlAndSpaceLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::URL_AND_SPACE_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAmpLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::AMP_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAmpAndSpaceLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::AMP_AND_SPACE_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sSpaceLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::SPACE_LEGAL);

const AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::sAllButNulAndAngleBracketsLegal =
    CreateValueSanitizer(
            IUrlQuerySanitizerIllegalCharacterValueSanitizer::ALL_BUT_NUL_AND_ANGLE_BRACKETS_LEGAL);


UrlQuerySanitizer::UrlQuerySanitizer()
    : mSanitizers(CreateHashMap())
    , mEntries(CreateHashMap())
    , mEntriesList(CreateArrayList())
    , mAllowUnregisteredParamaters(FALSE)
    , mPreferFirstRepeatedParameter(FALSE)
{
    GetAllIllegal((IUrlQuerySanitizerValueSanitizer**)&mUnregisteredParameterValueSanitizer);
}

AutoPtr<IHashMap> UrlQuerySanitizer::CreateHashMap()
{
    AutoPtr<IHashMap> rev;
    CHashMap::New((IHashMap**)&rev);
    return rev;
}

AutoPtr<IArrayList> UrlQuerySanitizer::CreateArrayList()
{
    AutoPtr<IArrayList> rev;
    CArrayList::New((IArrayList**)&rev);
    return rev;
}

AutoPtr<IUrlQuerySanitizerValueSanitizer> UrlQuerySanitizer::CreateValueSanitizer(
    /* [in] */ Int32 value)
{
    AutoPtr<IUrlQuerySanitizerValueSanitizer> rev;
    AutoPtr<IUrlQuerySanitizerIllegalCharacterValueSanitizer> tmp;
    CUrlQuerySanitizerIllegalCharacterValueSanitizer::New(value, (IUrlQuerySanitizerIllegalCharacterValueSanitizer**)&tmp);
    rev = IUrlQuerySanitizerValueSanitizer::Probe(tmp);
    return rev;
}

ECode UrlQuerySanitizer::GetUnregisteredParameterValueSanitizer(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUnregisteredParameterValueSanitizer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::SetUnregisteredParameterValueSanitizer(
    /* [in] */ IUrlQuerySanitizerValueSanitizer* sanitizer)
{
    mUnregisteredParameterValueSanitizer = sanitizer;
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAllIllegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAllIllegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAllButNulLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAllButNulLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAllButWhitespaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAllButWhitespaceLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetUrlLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sURLLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetUrlAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sUrlAndSpaceLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAmpLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAmpLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAmpAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAmpAndSpaceLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sSpaceLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAllButNulAndAngleBracketsLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sAllButNulAndAngleBracketsLegal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::constructor()
{
    return NOERROR;
}

ECode UrlQuerySanitizer::constructor(
    /* [in] */ const String& url)
{
    SetAllowUnregisteredParamaters(TRUE);
    ParseUrl(url);
    return NOERROR;
}

ECode UrlQuerySanitizer::ParseUrl(
    /* [in] */ const String& url)
{
    VALIDATE_NOT_NULL(url);

    Int32 queryIndex = url.IndexOf('?');
    String query;
    if (queryIndex >= 0) {
        query = url.Substring(queryIndex + 1);
    }
    else {
        query = "";
    }
    ParseQuery(query);

    return NOERROR;
}

ECode UrlQuerySanitizer::ParseQuery(
    /* [in] */ const String& query)
{
    Clear();

    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(query, String("&"), (ArrayOf<String>**)&values);

    Int32 tokens = values ? values->GetLength() : 0;
    for (Int32 i = 0; i < tokens; ++i) {
        String attributeValuePair = (*values)[i];
        if (attributeValuePair.GetLength() > 0) {
            Int32 assignmentIndex = attributeValuePair.IndexOf('=');
            if (assignmentIndex < 0) {
                // No assignment found, treat as if empty value
                ParseEntry(attributeValuePair, String(""));
            }
            else {
                ParseEntry(attributeValuePair.Substring(0, assignmentIndex),
                        attributeValuePair.Substring(assignmentIndex + 1));
            }
        }
    }

    return NOERROR;
}

ECode UrlQuerySanitizer::GetParameterSet(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);

    return mEntries->GetKeySet(result);
}

ECode UrlQuerySanitizer::GetParameterList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(IList::Probe(mEntriesList))
}

ECode UrlQuerySanitizer::HasParameter(
    /* [in] */ const String& parameter,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> csq;
    CString::New(parameter, (ICharSequence**)&csq);
    return mEntries->ContainsKey(csq, result);
}

ECode UrlQuerySanitizer::GetValue(
    /* [in] */ const String& parameter,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> csq;
    CString::New(parameter, (ICharSequence**)&csq);
    AutoPtr<IInterface> obj;
    mEntries->Get(csq, (IInterface**)&obj);
    ICharSequence::Probe(obj)->ToString(result);
    return NOERROR;
}

ECode UrlQuerySanitizer::RegisterParameter(
    /* [in] */ const String& parameter,
    /* [in] */ IUrlQuerySanitizerValueSanitizer* valueSanitizer)
{
    if (valueSanitizer == NULL) {
        mSanitizers->Remove(StringUtils::ParseCharSequence(parameter));
    }
    mSanitizers->Put(StringUtils::ParseCharSequence(parameter), valueSanitizer);
    return NOERROR;
}

ECode UrlQuerySanitizer::RegisterParameters(
    /* [in] */ ArrayOf<String>* parameters,
    /* [in] */ IUrlQuerySanitizerValueSanitizer* valueSanitizer)
{
    Int32 length = parameters->GetLength();
    for(Int32 i = 0; i < length; i++) {
        mSanitizers->Put(StringUtils::ParseCharSequence((*parameters)[i]), valueSanitizer);
    }
    return NOERROR;
}

ECode UrlQuerySanitizer::SetAllowUnregisteredParamaters(
    /* [in] */ Boolean allowUnregisteredParamaters)
{
    mAllowUnregisteredParamaters = allowUnregisteredParamaters;
    return NOERROR;
}

ECode UrlQuerySanitizer::GetAllowUnregisteredParamaters(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAllowUnregisteredParamaters;
    return NOERROR;
}

ECode UrlQuerySanitizer::SetPreferFirstRepeatedParameter(
    /* [in] */ Boolean preferFirstRepeatedParameter)
{
    mPreferFirstRepeatedParameter = preferFirstRepeatedParameter;
    return NOERROR;
}

ECode UrlQuerySanitizer::GetPreferFirstRepeatedParameter(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPreferFirstRepeatedParameter;
    return NOERROR;
}

ECode UrlQuerySanitizer::ParseEntry(
    /* [in] */ const String& parameter,
    /* [in] */ const String& value)
{
    String unescapedParameter;
    Unescape(parameter,&unescapedParameter);

    AutoPtr<IUrlQuerySanitizerValueSanitizer> valueSanitizer;
    GetEffectiveValueSanitizer(unescapedParameter, (IUrlQuerySanitizerValueSanitizer**)&valueSanitizer);
    if (valueSanitizer == NULL) {
        return NOERROR;
    }
    String unescapedValue;
    Unescape(value, &unescapedValue);
    String sanitizedValue;
    valueSanitizer->Sanitize(unescapedValue, &sanitizedValue);

    AddSanitizedEntry(unescapedParameter, sanitizedValue);
    return NOERROR;
}

ECode UrlQuerySanitizer::AddSanitizedEntry(
    /* [in] */ const String& parameter,
    /* [in] */ const String& value)
{
    AutoPtr<IUrlQuerySanitizerParameterValuePair> pair;
    CUrlQuerySanitizerParameterValuePair::New(parameter, value, (IUrlQuerySanitizerParameterValuePair**)&pair);
    mEntriesList->Add(pair);
    if (mPreferFirstRepeatedParameter) {
        Boolean isContainsKey;
        mEntries->ContainsKey(StringUtils::ParseCharSequence(parameter), &isContainsKey);
        if (isContainsKey) {
            return NOERROR;
        }
    }
    AutoPtr<ICharSequence> csqParam;
    CString::New(parameter, (ICharSequence**)&csqParam);
    AutoPtr<ICharSequence> csqValue;
    CString::New(value, (ICharSequence**)&csqValue);
    mEntries->Put(csqParam, csqValue);
    return NOERROR;
}

ECode UrlQuerySanitizer::GetValueSanitizer(
    /* [in] */ const String& parameter,
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> obj;
    mSanitizers->Get(StringUtils::ParseCharSequence(parameter), (IInterface**)&obj);
    FUNC_RETURN(IUrlQuerySanitizerValueSanitizer::Probe(obj))
}

ECode UrlQuerySanitizer::GetEffectiveValueSanitizer(
    /* [in] */ const String& parameter,
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUrlQuerySanitizerValueSanitizer> sanitizer;
    GetValueSanitizer(parameter, (IUrlQuerySanitizerValueSanitizer**)&sanitizer);
    if (sanitizer == NULL && mAllowUnregisteredParamaters) {
        GetUnregisteredParameterValueSanitizer((IUrlQuerySanitizerValueSanitizer**)&sanitizer);
    }
    *result = sanitizer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UrlQuerySanitizer::Unescape(
    /* [in] */ const String& string,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 firstEscape = string.IndexOf('%');
    if ( firstEscape < 0) {
        firstEscape = string.IndexOf('+');
        if (firstEscape < 0) {
            *result = string;
            return NOERROR;
        }
    }

    Int32 length = string.GetLength();
    StringBuilder stringBuilder(length);
    stringBuilder += (string.Substring(0, firstEscape));
    for (Int32 i = firstEscape; i < length; i++) {
        char c = string.GetChar(i);
        if (c == '+') {
            c = ' ';
        }
        else if ( c == '%' && i + 2 < length) {
            char c1 = string.GetChar(i + 1);
            char c2 = string.GetChar(i + 2);
            if (IsHexDigit(c1) && IsHexDigit(c2)) {
                c = (char) (DecodeHexDigit(c1) * 16 + DecodeHexDigit(c2));
                i += 2;
            }
        }
        stringBuilder += (c);

    }

    *result = stringBuilder.ToString();
    return NOERROR;
}

Boolean UrlQuerySanitizer::IsHexDigit(
    /* [in] */ Char32 c)
{
    if (DecodeHexDigit(c) >= 0)
        return TRUE;
    else
        return FALSE;
}

Int32 UrlQuerySanitizer::DecodeHexDigit(
    /* [in] */ Char32 c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return -1;
    }
}

ECode UrlQuerySanitizer::Clear()
{
    mEntries->Clear();
    mEntriesList->Clear();
    return NOERROR;
}

//=================================================================
// UrlQuerySanitizerParameterValuePair
//=================================================================
CAR_INTERFACE_IMPL(UrlQuerySanitizerParameterValuePair, Object, IUrlQuerySanitizerParameterValuePair)

ECode UrlQuerySanitizerParameterValuePair::constructor(
    /* [in] */ const String& parameter,
    /* [in] */ const String& value)
{
    mParameter = parameter;
    mValue = value;
    return NOERROR;
}

ECode UrlQuerySanitizerParameterValuePair::GetParameter(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mParameter;
    return NOERROR;
}

ECode UrlQuerySanitizerParameterValuePair::SetParameter(
    /* [in] */ const String& mParameter)
{
    this->mParameter = mParameter;
    return NOERROR;
}

ECode UrlQuerySanitizerParameterValuePair::GetValue(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mValue;
    return NOERROR;
}

ECode UrlQuerySanitizerParameterValuePair::SetValue(
    /* [in] */ const String& mValue)
{
    this->mValue = mValue;
    return NOERROR;
}

//=============================================================
// UrlQuerySanitizerIllegalCharacterValueSanitizer
//=============================================================

CAR_INTERFACE_IMPL_2(UrlQuerySanitizerIllegalCharacterValueSanitizer, Object, IUrlQuerySanitizerValueSanitizer, IUrlQuerySanitizerIllegalCharacterValueSanitizer)

const String UrlQuerySanitizerIllegalCharacterValueSanitizer::JAVASCRIPT_PREFIX("javascript:");
const String UrlQuerySanitizerIllegalCharacterValueSanitizer::VBSCRIPT_PREFIX("vbscript:");
const Int32 UrlQuerySanitizerIllegalCharacterValueSanitizer::MIN_SCRIPT_PREFIX_LENGTH = Elastos::Core::Math::Min(JAVASCRIPT_PREFIX.GetLength(), VBSCRIPT_PREFIX.GetLength());

UrlQuerySanitizerIllegalCharacterValueSanitizer::UrlQuerySanitizerIllegalCharacterValueSanitizer()
    : mFlags(0)
{}

ECode UrlQuerySanitizerIllegalCharacterValueSanitizer::constructor(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode UrlQuerySanitizerIllegalCharacterValueSanitizer::Sanitize(
    /* [in] */ const String& value,
    /* [out] */ String* result)
{
    if (value == NULL) {
        result = NULL;
        return NOERROR;
    }

    String _value = value;

    Int32 length = _value.GetLength();
    if ((mFlags & SCRIPT_URL_OK) != 0) {
        if (length >= MIN_SCRIPT_PREFIX_LENGTH) {
            String asLower = _value.ToLowerCase(); // String asLower = value.toLowerCase(Locale.ROOT);
            if (asLower.StartWith(JAVASCRIPT_PREFIX)  ||
                    asLower.StartWith(VBSCRIPT_PREFIX)) {
                *result = String("");
                return NOERROR;
            }
        }
    }

    // If whitespace isn't OK, get rid of whitespace at beginning
    // and end of value.
    if ( (mFlags & ALL_WHITESPACE_OK) == 0) {
        _value = TrimWhitespace(_value);
        // The length could have changed, so we need to correct
        // the length variable.
        length = _value.GetLength();
    }

    AutoPtr<StringBuilder> stringBuilder = new StringBuilder(length);
    for(Int32 i = 0; i < length; i++) {
        char c = _value.GetChar(i);
        if (!CharacterIsLegal(c)) {
            if ((mFlags & SPACE_OK) != 0) {
                c = ' ';
            }
            else {
                c = '_';
            }
        }
        stringBuilder->AppendChar(c);
    }

    *result = stringBuilder->ToString();
    return NOERROR;
}

String UrlQuerySanitizerIllegalCharacterValueSanitizer::TrimWhitespace(
    /* [in] */ const String& value)
{
    Int32 start = 0;
    Int32 last = value.GetLength() - 1;
    Int32 end = last;

    while (start <= end && IsWhitespace(value.GetChar(start))) {
        start++;
    }
    while (end >= start && IsWhitespace(value.GetChar(end))) {
        end--;
    }

    if (start == 0 && end == last) {
        return value;
    }
    return value.Substring(start, end + 1);
}

Boolean UrlQuerySanitizerIllegalCharacterValueSanitizer::IsWhitespace(
    /* [in] */ Char32 c)
{
    Boolean result;

    switch(c) {
        case ' ':
        case '\t':
        case '\f':
        case '\n':
        case '\r':
        case 11: /* VT */
            result = TRUE;
        break;
        default:
            result = FALSE;
        break;
    }
    return result;
}

Boolean UrlQuerySanitizerIllegalCharacterValueSanitizer::CharacterIsLegal(
    /* [in] */ Char32 c)
{
    Boolean result;

    switch(c) {
        case ' ' :
            result =  (mFlags & SPACE_OK) != 0;
        break;
        case '\t':
        case '\f':
        case '\n':
        case '\r':
        case 11: /* VT */
            result =  ((mFlags & OTHER_WHITESPACE_OK) != 0 );
        break;
        case '\"':
            result = ( (mFlags & DQUOTE_OK) != 0 );
        break;
        case '\'':
            result = ( (mFlags & SQUOTE_OK) != 0 );
        break;
        case '<' :
            result = ( (mFlags & LT_OK) != 0 );
        break;
        case '>' :
            result = ( (mFlags & GT_OK) != 0 );
        break;
        case '&' :
            result = ( (mFlags & AMP_OK) != 0 );
        break;
        case '%' :
            result = ( (mFlags & PCT_OK) != 0 );
        break;
        case '\0':
            result = ( (mFlags & NUL_OK) != 0);
        break;
        default  :
            result = ( (c >= 32 && c < 127) ||
            ((c >= 128) && ((mFlags & NON_7_BIT_ASCII_OK) != 0)) );
        break;
    }
    return result;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
