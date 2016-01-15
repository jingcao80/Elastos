#include "StringToIntegral.h"
#include "Character.h"
#include "Math.h"
#include "HexStringParser.h"
#include <utils/Log.h>

namespace Elastos {
namespace Core {

//=============================================================================
//             Int16
//=============================================================================
ECode StringToIntegral::Decode(
    /* [in] */ const String& string,
    /* [out] */ Int16* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 intValue;
    ECode ec = Decode(string, &intValue);
    Int16 shortValue = (Int16)intValue;
    if (shortValue == intValue) {
        *result = shortValue;
        return ec;
    }

    *result = 0;
    ALOGE("StringToIntegral::Decode() Value out of range for short: \"%s\"", string.string());
    return E_NUMBER_FORMAT_EXCEPTION;
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [out] */ Int16* result)
{
    return Parse(string, 10, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [in] */ Int32 radix,
    /* [out] */ Int16* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 intValue;
    ECode ec = Parse(string, radix, &intValue);
    short shortValue = (short) intValue;
    if (shortValue == intValue) {
        *result = shortValue;
        return ec;
    }

    *result = 0;
    ALOGE("StringToIntegral::Parse() Value out of range for short: \"%s\"", string.string());
    return E_NUMBER_FORMAT_EXCEPTION;
}

//=============================================================================
//             Int32
//=============================================================================
ECode StringToIntegral::Decode(
    /* [in] */ const String& string,
    /* [out] */ Int32* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength(), i = 0;
    if (length == 0) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Char32 firstDigit = (*charArray)[i];
    Boolean negative = firstDigit == '-';
    if (negative) {
        if (length == 1) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        firstDigit = (*charArray)[++i];
    }

    Int32 base = 10;
    if (firstDigit == '0') {
        if (++i == length) {
            return 0;
        }
        if ((firstDigit = (*charArray)[i]) == 'x' || firstDigit == 'X') {
            if (++i == length) {
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            base = 16;
        } else {
            base = 8;
        }
    } else if (firstDigit == '#') {
        if (++i == length) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        base = 16;
    }

    return Parse(string, i, base, negative, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [out] */ Int32* result)
{
    return Parse(string, 10, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [in] */ Int32 radix,
    /* [out] */ Int32* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    if (string.IsNull()) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength(), i = 0;
    if (length == 0) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Boolean negative = (*charArray)[i] == '-';
    if (negative && ++i == length) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    return Parse(string, i, radix, negative, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 radix,
    /* [in] */ Boolean negative,
    /* [out] */ Int32* resultValue)
{
    assert(resultValue != NULL);
    *resultValue = 0;

    using Elastos::Core::Math;
    Int32 max = Math::INT32_MIN_VALUE / radix;
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 digit, next;
    Int32 result = 0, length = charArray->GetLength();
    while (offset < length) {
        digit = Character::ToDigit((*charArray)[offset++], radix);
        if (digit == -1) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (max > result) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        next = result * radix - digit;
        if (next > result) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        result = next;
    }

    if (!negative) {
        result = -result;
        if (result < 0) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
    }
    *resultValue = result;
    return NOERROR;
}

//=============================================================================
//             Int64
//=============================================================================
ECode StringToIntegral::Decode(
    /* [in] */ const String& string,
    /* [out] */ Int64* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength(), i = 0;
    if (length == 0) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Char32 firstDigit = (*charArray)[i];
    Boolean negative = firstDigit == '-';
    if (negative) {
        if (length == 1) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        firstDigit = (*charArray)[++i];
    }

    Int64 base = 10;
    if (firstDigit == '0') {
        if (++i == length) {
            return 0;
        }
        if ((firstDigit = (*charArray)[i]) == 'x' || firstDigit == 'X') {
            if (i == length) {
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            i++;
            base = 16;
        } else {
            base = 8;
        }
    } else if (firstDigit == '#') {
        if (i == length) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        i++;
        base = 16;
    }

    return Parse(string, i, base, negative, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [out] */ Int64* result)
{
    return Parse(string, 10, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [in] */ Int32 radix,
    /* [out] */ Int64* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        ALOGE("StringToIntegral::Decode() invalid radix %d for Int64: \"%s\"", radix, string.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    if (string.IsNull()) {
        // throw invalidLong(string);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength(), i = 0;
    if (length == 0) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Boolean negative = (*charArray)[i] == '-';
    if (negative && ++i == length) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    return Parse(string, i, radix, negative, result);
}

ECode StringToIntegral::Parse(
    /* [in] */ const String& string,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 radix,
    /* [in] */ Boolean negative,
    /* [out] */ Int64* resultValue)
{
    assert(resultValue);
    *resultValue = 0;

    using Elastos::Core::Math;
    Int64 max = Math::INT64_MIN_VALUE / radix;
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int64 result = 0, length = charArray->GetLength();
    Int32 digit;
    Int64 next;
    while (offset < length) {
        digit = Character::ToDigit((*charArray)[offset++], radix);
        if (digit == -1) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (max > result) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }

        next = result * radix - digit;
        if (next > result) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        result = next;
    }
    if (!negative) {
        result = -result;
        if (result < 0) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
    }

    *resultValue = result;
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
