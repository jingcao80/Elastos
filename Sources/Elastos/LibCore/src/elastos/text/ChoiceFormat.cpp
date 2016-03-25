#include "ChoiceFormat.h"
#include "Math.h"
#include "StringBuffer.h"
#include "StringBuilder.h"
#include "Character.h"
#include "StringUtils.h"
#include "CChoiceFormat.h"
#include "CString.h"
#include "CDouble.h"
#include "CLocaleHelper.h"
#include "CFieldPosition.h"
#include "CParsePosition.h"
#include "CoreUtils.h"
#include "EmptyArray.h"
#include "Arrays.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::Math;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Arrays;
using Libcore::Utility::EmptyArray;
using Elastos::Text::IChoiceFormat;
using Elastos::Text::EIID_IChoiceFormat;
using Elastos::Text::CParsePosition;

namespace Elastos {
namespace Text {


CAR_INTERFACE_IMPL(ChoiceFormat, NumberFormat, IChoiceFormat)

ChoiceFormat::~ChoiceFormat()
{
}

ECode ChoiceFormat::constructor(
    /* [in] */ ArrayOf<Double> * limits,
    /* [in] */ ArrayOf<String> * formats)
{
    return SetChoices(limits, formats);
}

ECode ChoiceFormat::constructor(
    /* [in] */ const String& tem)
{
    return ApplyPattern(tem);
}

ECode ChoiceFormat::ApplyPattern(
    /* [in] */ const String& tem)
{
    AutoPtr<ArrayOf<Double> > limits = ArrayOf<Double>::Alloc(5);
    List<String> formats;
    AutoPtr<ArrayOf<Char32> > charArray = tem.GetChars();
    Int32 length = charArray->GetLength();
    Int32 limitCount = 0, index = 0;
    StringBuffer buffer("");

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> uslocale;
    localeHelper->GetUS((ILocale**)&uslocale);
    AutoPtr<INumberFormat> format;
    NumberFormat::GetInstance(uslocale, (INumberFormat**)&format);

    AutoPtr<IParsePosition> position;
    CParsePosition::New(0, (IParsePosition**)&position);

    Char32 ch;
    Int32 errorIndex;
    Double next;
    while (TRUE) {
        index = SkipWhitespace(tem, index);
        if (index >= length) {
            if (limitCount == limits->GetLength()) {
                mChoiceLimits = limits;
            }
            else {
                mChoiceLimits = ArrayOf<Double>::Alloc(limitCount);
                for (Int32 i = 0; i < limitCount; i++) {
                    (*mChoiceLimits)[i] = (*limits)[i];
                }
            }
            mChoiceFormats = ArrayOf<String>::Alloc(formats.GetSize());
            List<String>::Iterator it = formats.Begin();
            for (Int32 i = 0; it != formats.End(); ++it, ++i) {
                (*mChoiceFormats)[i] = *it;
            }
            return NOERROR;
        }

        position->SetIndex(index);
        AutoPtr<INumber> value;
        format->Parse(tem, position, (INumber**)&value);
        position->GetIndex(&index);
        index = SkipWhitespace(tem, index);
        position->GetErrorIndex(&errorIndex);
        if (errorIndex != -1 || index >= length) {
            // Fix Harmony 540
            mChoiceLimits = EmptyArray::DOUBLE;
            mChoiceFormats = EmptyArray::STRING;
            return NOERROR;
        }
        ch = (*charArray)[index++];
        if (limitCount == limits->GetLength()) {
            AutoPtr<ArrayOf<Double> > newLimits = ArrayOf<Double>::Alloc(limitCount * 2);
            newLimits->Copy(limits);
            limits = newLimits;
        }
        next = 0.0;
        switch (ch) {
            case '#':
            case 0x2264/*'\u2264'*/://'\u2264'
                value->DoubleValue(&next);
                break;
            case '<':
                Double d;
                value->DoubleValue(&d);
                next = NextDouble(d);
                break;
            default:
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (limitCount > 0 && next <= (*limits)[limitCount - 1]) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        buffer.Reset();
        position->SetIndex(index);
        Boolean succeeded;
        UpTo(tem, position, buffer, '|', &succeeded);
        position->GetIndex(&index);
        (*limits)[limitCount++] = next;
        formats.PushBack(buffer.ToString());
    }

    return NOERROR;
}

ECode ChoiceFormat::Clone(
    /* [out] */ IInterface ** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IChoiceFormat> clone;
    FAIL_RETURN(CChoiceFormat::New(mChoiceLimits, mChoiceFormats,(IChoiceFormat **)&clone));

    CloneImpl(clone);

    *object = clone;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode ChoiceFormat::CloneImpl(
    /* [in] */ IChoiceFormat * object)
{
    assert(object);
    return NumberFormat::CloneImpl(INumberFormat::Probe(object));
}

ECode ChoiceFormat::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)

    IChoiceFormat* cf = IChoiceFormat::Probe(object);
    if (this == cf) {
        *result = TRUE;
        return NOERROR;
    }
    if (cf == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Double> > limits;
    cf->GetLimits((ArrayOf<Double>**)&limits);
    AutoPtr<ArrayOf<String> > formats;
    cf->GetFormats((ArrayOf<String>**)&formats);
    return Arrays::Equals(mChoiceLimits, limits)
        && Arrays::Equals(mChoiceFormats, formats);
}

ECode ChoiceFormat::Format(
    /* [in] */ Double value,
    /* [in] */ IStringBuffer * inbuffer,
    /* [in] */ IFieldPosition * field)
{
    VALIDATE_NOT_NULL(inbuffer);
    VALIDATE_NOT_NULL(field);

    for (Int32 i = mChoiceLimits->GetLength() - 1; i >= 0; i--) {
        if ((*mChoiceLimits)[i] <= value) {
            inbuffer->Append((*mChoiceFormats)[i]);
            return NOERROR;
        }
    }

    if (mChoiceFormats->GetLength() > 0) {
        inbuffer->Append((*mChoiceFormats)[0]);
    }

    return NOERROR;
}

ECode ChoiceFormat::Format(
    /* [in] */ Int64 value,
    /* [in] */ IStringBuffer * inbuffer,
    /* [in] */ IFieldPosition * field)
{
    return Format((Double)value, inbuffer, field);
}

ECode ChoiceFormat::GetHashCode(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    Int32 hashCode = 0;
    for (Int32 i = 0; i < mChoiceLimits->GetLength(); i++) {
        Int64 v = Elastos::Core::Math::DoubleToInt64Bits((*mChoiceLimits)[i]);
        hashCode += (Int32) (v ^ (v >> 32)) + (*mChoiceFormats)[i].GetHashCode();
    }
    *value = hashCode;
    return NOERROR;
}

ECode ChoiceFormat::GetFormats(
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = mChoiceFormats;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode ChoiceFormat::GetLimits(
    /* [out, callee] */ ArrayOf<Double>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = mChoiceLimits;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

Double ChoiceFormat::NextDouble(
    /* [in] */ Double value)
{
    using Elastos::Core::Math;
    if (value == Math::DOUBLE_POSITIVE_INFINITY) {
        return value;
    }
    Int64 bits;
    // Handle -0.0
    if (value == 0) {
        bits = 0;
    }
    else {
        bits = Math::DoubleToInt64Bits(value);
    }

    return Math::Int64BitsToDouble(value < 0 ? bits - 1 : bits + 1);
}

Double ChoiceFormat::NextDouble(
    /* [in] */ Double value,
    /* [in] */ Boolean increment)
{
    return increment ? NextDouble(value) : PreviousDouble(value);
}

ECode ChoiceFormat::Parse(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ INumber** value)
{
    VALIDATE_NOT_NULL(value)

    Int32 offset = 0;
    position->GetIndex(&offset);
    for (Int32 i = 0; i < mChoiceFormats->GetLength(); i++) {
        if (string.StartWith((*mChoiceFormats)[i]), offset) {
            position->SetIndex(offset + (*mChoiceFormats)[i].GetLength());
            AutoPtr<IDouble> outdouble = CoreUtils::Convert((*mChoiceLimits)[i]);
            *value = INumber::Probe(outdouble);
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
    }
    position->SetErrorIndex(offset);
    AutoPtr<IDouble> outdouble = CoreUtils::Convert(Elastos::Core::Math::DOUBLE_NAN);
    *value = INumber::Probe(outdouble);
    REFCOUNT_ADD(*value);
    return NOERROR;
}

Double ChoiceFormat::PreviousDouble(
    /* [in] */ Double value)
{
    using Elastos::Core::Math;
    if (value == Math::DOUBLE_NEGATIVE_INFINITY) {
        return value;
    }

    Int64 bits;
    // Handle 0.0
    if (value == 0) {
        bits = 0x8000000000000000ll;
    }
    else {
        bits = Math::DoubleToInt64Bits(value);
    }
    return Math::Int64BitsToDouble(value <= 0 ? bits + 1 : bits - 1);
}

ECode ChoiceFormat::SetChoices(
    /* [in] */ ArrayOf<Double>* limits,
    /* [in] */ ArrayOf<String>* formats)
{
    if (limits == NULL || formats == NULL
        || limits->GetLength() != formats->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mChoiceLimits = limits;
    mChoiceFormats = formats;
    return NOERROR;
}

Int32 ChoiceFormat::SkipWhitespace(
    /* [in] */ const String& string,
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength();
    while (index < length && Character::IsWhitespace((*charArray)[index])) {
        index++;
    }
    return index;
}

ECode ChoiceFormat::ToPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);

    StringBuilder buffer;
    for (Int32 i = 0; i < mChoiceLimits->GetLength(); i++) {
        if (i != 0) {
            buffer.AppendChar('|');
        }
        String previous = StringUtils::ToString(PreviousDouble((*mChoiceLimits)[i]));
        String limit = StringUtils::ToString((*mChoiceLimits)[i]);
        if (previous.GetLength() < limit.GetLength()) {
            buffer.Append(previous);
            buffer.AppendChar('<');
        } else {
            buffer.Append(limit);
            buffer.AppendChar('#');
        }
        Boolean quote = ((*mChoiceFormats)[i].IndexOf('|') != -1);
        if (quote) {
            buffer.AppendChar('\'');
        }
        buffer.Append((*mChoiceFormats)[i]);
        if (quote) {
            buffer.AppendChar('\'');
        }
    }

    return buffer.Substring(0, pattern);
}

} // namespace Text
} // namespace Elastos
