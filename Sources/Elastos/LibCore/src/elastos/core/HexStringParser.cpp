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

#include "HexStringParser.h"
#include "Character.h"
#include "Math.h"
#include "StringBuilder.h"
#include "StringToIntegral.h"
#include "StringUtils.h"
#include "CPatternHelper.h"

using Elastos::Core::Math;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Core {

static String InitHEX_PATTERN()
{
    StringBuilder sb("[\\x00-\\x20]*([+-]?)");
    sb += "0[xX](\\p{XDigit}+\\.?|\\p{XDigit}*\\.\\p{XDigit}+)";
    sb += "[pP]([+-]?\\d+)";
    sb += "[fFdD]?";
    sb += "[\\x00-\\x20]*";
    return sb.ToString();
}

static AutoPtr<IPattern> InitPATTERN()
{
    AutoPtr<IPattern> pattern;
    AutoPtr<CPatternHelper> helper;
    CPatternHelper::AcquireSingletonByFriend((CPatternHelper**)&helper);
    helper->Compile(InitHEX_PATTERN(), (IPattern**)&pattern);
    return pattern;
}

const Int32 HexStringParser::DOUBLE_EXPONENT_WIDTH = 11;
const Int32 HexStringParser::DOUBLE_MANTISSA_WIDTH = 52;
const Int32 HexStringParser::FLOAT_EXPONENT_WIDTH = 8;
const Int32 HexStringParser::FLOAT_MANTISSA_WIDTH = 23;
const Int32 HexStringParser::HEX_RADIX = 16;
const Int32 HexStringParser::MAX_SIGNIFICANT_LENGTH = 15;

const String HexStringParser::HEX_SIGNIFICANT = String("0[xX](\\p{XDigit}+\\.?|\\p{XDigit}*\\.\\p{XDigit}+)");
const String HexStringParser::BINARY_EXPONENT = String("[pP]([+-]?\\d+)");
const String HexStringParser::FLOAT_TYPE_SUFFIX = String("[fFdD]?");
const String HexStringParser::HEX_PATTERN = InitHEX_PATTERN();

AutoPtr<IPattern> HexStringParser::PATTERN = NULL;

HexStringParser::HexStringParser(
    /* [in] */ Int32 exponentWidth,
    /* [in] */ Int32 mantissaWidth)
    : mSign(0)
    , mExponent(0)
    , mMantissa(0)
{
    EXPONENT_WIDTH = exponentWidth;
    MANTISSA_WIDTH = mantissaWidth;

    EXPONENT_BASE = ~(-1L << (exponentWidth - 1));
    MAX_EXPONENT = ~(-1L << exponentWidth);
    MIN_EXPONENT = -(MANTISSA_WIDTH + 1);
    MANTISSA_MASK = ~(-1L << mantissaWidth);

    mAbandonedNumber = String("");
}

HexStringParser::~HexStringParser()
{
}

ECode HexStringParser::Parse(
    /* [in] */ const String& hexString,
    /* [out] */ Double* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    AutoPtr<HexStringParser> parser = new HexStringParser(DOUBLE_EXPONENT_WIDTH, DOUBLE_MANTISSA_WIDTH);
    Int64 intValue;
    ECode ec = parser->Parse(hexString, TRUE, &intValue);
    if (SUCCEEDED(ec)) {
        *result = Math::Int64BitsToDouble(intValue);
        return NOERROR;
    }
    return ec;
}

ECode HexStringParser::Parse(
    /* [in] */ const String& hexString,
    /* [out] */ Float* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    AutoPtr<HexStringParser> parser = new HexStringParser(FLOAT_EXPONENT_WIDTH, FLOAT_MANTISSA_WIDTH);
    Int64 intValue;
    ECode ec = parser->Parse(hexString, FALSE, &intValue);
    if (SUCCEEDED(ec)) {
        *result = Math::Int32BitsToFloat((Int32)intValue);
        return NOERROR;
    }
    return ec;
}

ECode HexStringParser::Parse(
    /* [in] */ const String& hexString,
    /* [in] */ Boolean isDouble,
    /* [out] */ Int64* result)
{
    if (result == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = 0;

    if (PATTERN == NULL) {
        PATTERN = InitPATTERN();
    }

    AutoPtr<IMatcher> matcher;
    PATTERN->Matcher(hexString, (IMatcher**)&matcher);
    Boolean matches;
    matcher->Matches(&matches);
    if (!matches) {
        // throw new NumberFormatException("Invalid hex " + (isDouble ? "double" : "float")+ ":" +
        //         hexString);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    IMatchResult* mr = IMatchResult::Probe(matcher);
    String signStr, significantStr, exponentStr;

    mr->Group(1, &signStr);
    mr->Group(2, &significantStr);
    mr->Group(3, &exponentStr);

    ParseHexSign(signStr);
    ParseExponent(exponentStr);
    ParseMantissa(significantStr);

    mSign <<= (MANTISSA_WIDTH + EXPONENT_WIDTH);
    mExponent <<= MANTISSA_WIDTH;
    *result = mSign | mExponent | mMantissa;
    return NOERROR;
}

void HexStringParser::ParseHexSign(
    /* [in] */ const String& signStr)
{
    mSign = signStr.Equals("-") ? 1 : 0;
}

void HexStringParser::ParseExponent(
    /* [in] */ const String& str)
{
    String exponentStr = str;
    Char32 leadingChar = exponentStr.GetChar(0);
    Int32 expSign = (leadingChar == '-' ? -1 : 1);
    if (!Character::IsDigit(leadingChar)) {
        exponentStr = exponentStr.Substring(1);
    }

    // try {
        ECode ec = StringToIntegral::Parse(exponentStr, &mExponent);
        if (SUCCEEDED(ec)) {
            mExponent = expSign * mExponent;
            CheckedAddExponent(EXPONENT_BASE);
        }
        else {
            mExponent = expSign * Math::INT64_MAX_VALUE;
        }
    // } catch (NumberFormatException e) {
    //     mExponent = expSign * Long.MAX_VALUE;
    // }
}

void HexStringParser::ParseMantissa(
    /* [in] */ const String& significantStr)
{
    AutoPtr<ArrayOf<String> > strings;
    StringUtils::Split(significantStr, String("\\."), (ArrayOf<String>**)&strings);
    if (NULL == strings) {
        SetZero();
        return;
    }

    String strIntegerPart = (*strings)[0];
    String strDecimalPart = strings->GetLength() > 1 ? (*strings)[1] : String("");

    String significand = GetNormalizedSignificand(strIntegerPart, strDecimalPart);
    if (significand.Equals("0")) {
        SetZero();
        return;
    }

    Int32 offset = GetOffset(strIntegerPart, strDecimalPart);
    CheckedAddExponent(offset);

    if (mExponent >= MAX_EXPONENT) {
        SetInfinite();
        return;
    }

    if (mExponent <= MIN_EXPONENT) {
        SetZero();
        return;
    }

    if (significand.GetLength() > MAX_SIGNIFICANT_LENGTH) {
        mAbandonedNumber = significand.Substring(MAX_SIGNIFICANT_LENGTH);
        significand = significand.Substring(0, MAX_SIGNIFICANT_LENGTH);
    }

    StringToIntegral::Parse(significand, HEX_RADIX, &mMantissa);

    if (mExponent >= 1) {
        ProcessNormalNumber();
    } else{
        ProcessSubNormalNumber();
    }
}

void HexStringParser::SetInfinite()
{
    mExponent = MAX_EXPONENT;
    mMantissa = 0;
}

void HexStringParser::SetZero()
{
    mExponent = 0;
    mMantissa = 0;
}

void HexStringParser::CheckedAddExponent(
    /* [in] */ Int64 offset)
{
    Int64 result = mExponent + offset;
    Int32 expSign = Math::Signum(mExponent);
    if (expSign * Math::Signum(offset) > 0 && expSign * Math::Signum(result) < 0) {
        mExponent = expSign * Math::INT64_MAX_VALUE;
    } else {
        mExponent = result;
    }
}

void HexStringParser::ProcessNormalNumber()
{
    Int32 desiredWidth = MANTISSA_WIDTH + 2;
    FitMantissaInDesiredWidth(desiredWidth);
    Round();
    mMantissa = mMantissa & MANTISSA_MASK;
}

void HexStringParser::ProcessSubNormalNumber()
{
    Int32 desiredWidth = MANTISSA_WIDTH + 1;
    desiredWidth += (Int32)mExponent;//lends bit from mantissa to exponent
    mExponent = 0;
    FitMantissaInDesiredWidth(desiredWidth);
    Round();
    mMantissa = mMantissa & MANTISSA_MASK;
}

void HexStringParser::FitMantissaInDesiredWidth(
    /* [in] */ Int32 desiredWidth)
{
    Int32 bitLength = CountBitsLength(mMantissa);
    if (bitLength > desiredWidth) {
        DiscardTrailingBits(bitLength - desiredWidth);
    } else {
        mMantissa <<= (desiredWidth - bitLength);
    }
}

void HexStringParser::DiscardTrailingBits(
    /* [in] */ Int64 num)
{
    Int64 mask = ~(-1L << num);
    StringBuilder sb(mAbandonedNumber);
    sb += (mMantissa & mask);
    mAbandonedNumber = sb.ToString();
    mMantissa >>= num;
}

void HexStringParser::Round()
{
    String result;
    StringUtils::ReplaceAll(mAbandonedNumber, String("0+"), String(""), &result);
    Boolean moreThanZero = (result.GetLength() > 0 ? TRUE : FALSE);

    Int32 lastDiscardedBit = (Int32) (mMantissa & 1L);
    mMantissa >>= 1;
    Int32 tailBitInMantissa = (Int32) (mMantissa & 1L);

    if (lastDiscardedBit == 1 && (moreThanZero || tailBitInMantissa == 1)) {
        Int32 oldLength = CountBitsLength(mMantissa);
        mMantissa += 1L;
        Int32 newLength = CountBitsLength(mMantissa);

        //Rounds up to exponent when whole bits of mMantissa are one-bits.
        if (oldLength >= MANTISSA_WIDTH && newLength > oldLength) {
            CheckedAddExponent(1);
        }
    }
}

String HexStringParser::GetNormalizedSignificand(
    /* [in] */ const String& strIntegerPart,
    /* [in] */ const String& strDecimalPart)
{
    String significand(strIntegerPart);
    significand += strDecimalPart;

    String result;
    StringUtils::ReplaceFirst(significand, String("^0+"), String(""), &result);
    significand = result;
    if (significand.GetLength() == 0) {
        significand = "0";
    }
    return significand;
}

Int32 HexStringParser::GetOffset(
    /* [in] */ const String& integerPart,
    /* [in] */ const String& strDecimalPart)
{
    String strIntegerPart;
    StringUtils::ReplaceFirst(integerPart, String("^0+"), String(""), &strIntegerPart);

    //If the Integer part is a nonzero number.
    if (strIntegerPart.GetLength() != 0) {
        String leadingNumber = strIntegerPart.Substring(0, 1);
        Int64 longValue;
        StringToIntegral::Parse(leadingNumber, HEX_RADIX, &longValue);
        return (strIntegerPart.GetLength() - 1) * 4 + CountBitsLength(longValue) - 1;
    }

    //If the Integer part is a zero number.
    AutoPtr<ArrayOf<Char32> > charArray = strDecimalPart.GetChars();
    Int32 i;
    for (i = 0; i < charArray->GetLength() && (*charArray)[i] == '0'; i++);
    if (i == charArray->GetLength()) {
        return 0;
    }
    String leadingNumber = strDecimalPart.Substring(i, i + 1);
    Int64 longValue;
    StringToIntegral::Parse(leadingNumber, HEX_RADIX, &longValue);
    return (-i - 1) * 4 + CountBitsLength(longValue) - 1;
}

Int32 HexStringParser::CountBitsLength(
    /* [in] */ Int64 value)
{
    Int32 leadingZeros = Math::NumberOfLeadingZeros(value);
    return 64/* Long.SIZE */ - leadingZeros;
}

} //Core
} //Elastos
