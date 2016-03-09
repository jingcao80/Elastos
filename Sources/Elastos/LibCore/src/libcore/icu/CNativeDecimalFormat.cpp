
#include "CNativeDecimalFormat.h"
#include "CAttributedString.h"
#include "ElStringByteSink.h"
#include <unicode/decimfmt.h>
#include <unicode/unum.h>
#include <unicode/fmtable.h>
#include "AutoLock.h"
#include "CLocaleData.h"
#include "CDouble.h"
#include "CInteger64.h"
#include "CBigDecimalHelper.h"
#include "CBigDecimal.h"
#include "CNumberFormatField.h"
#include "DecimalFormatSymbols.h"

using Elastos::Core::EIID_INumber;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Math::EIID_IBigInteger;
using Elastos::Math::EIID_IBigDecimal;
using Elastos::Math::IBigDecimalHelper;
using Elastos::Math::CBigDecimalHelper;
using Elastos::Math::CBigDecimal;
using Elastos::Text::IAttributedString;
using Elastos::Text::CAttributedString;
using Elastos::Core::EIID_ICloneable;
using Elastos::Text::CNumberFormatField;
using Elastos::Text::IFormatField;
using Elastos::Text::DecimalFormatSymbols;

namespace Libcore {
namespace ICU {

extern String UnicodeStringToString(const UnicodeString& us);

CAR_OBJECT_IMPL(CNativeDecimalFormat)

CAR_INTERFACE_IMPL_2(CNativeDecimalFormat, Object, INativeDecimalFormat, ICloneable)

const Int32 CNativeDecimalFormat::UNUM_DECIMAL_SEPARATOR_SYMBOL = 0;
const Int32 CNativeDecimalFormat::UNUM_GROUPING_SEPARATOR_SYMBOL = 1;
const Int32 CNativeDecimalFormat::UNUM_PATTERN_SEPARATOR_SYMBOL = 2;
const Int32 CNativeDecimalFormat::UNUM_PERCENT_SYMBOL = 3;
const Int32 CNativeDecimalFormat::UNUM_ZERO_DIGIT_SYMBOL = 4;
const Int32 CNativeDecimalFormat::UNUM_DIGIT_SYMBOL = 5;
const Int32 CNativeDecimalFormat::UNUM_MINUS_SIGN_SYMBOL = 6;
const Int32 CNativeDecimalFormat::UNUM_PLUS_SIGN_SYMBOL = 7;
const Int32 CNativeDecimalFormat::UNUM_CURRENCY_SYMBOL = 8;
const Int32 CNativeDecimalFormat::UNUM_INTL_CURRENCY_SYMBOL = 9;
const Int32 CNativeDecimalFormat::UNUM_MONETARY_SEPARATOR_SYMBOL = 10;
const Int32 CNativeDecimalFormat::UNUM_EXPONENTIAL_SYMBOL = 11;
const Int32 CNativeDecimalFormat::UNUM_PERMILL_SYMBOL = 12;
const Int32 CNativeDecimalFormat::UNUM_PAD_ESCAPE_SYMBOL = 13;
const Int32 CNativeDecimalFormat::UNUM_INFINITY_SYMBOL = 14;
const Int32 CNativeDecimalFormat::UNUM_NAN_SYMBOL = 15;
const Int32 CNativeDecimalFormat::UNUM_SIGNIFICANT_DIGIT_SYMBOL = 16;
const Int32 CNativeDecimalFormat::UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL = 17;
const Int32 CNativeDecimalFormat::UNUM_FORMAT_SYMBOL_COUNT = 18;
const Int32 CNativeDecimalFormat::UNUM_PARSE_INT_ONLY = 0;
const Int32 CNativeDecimalFormat::UNUM_GROUPING_USED = 1;
const Int32 CNativeDecimalFormat::UNUM_DECIMAL_ALWAYS_SHOWN = 2;
const Int32 CNativeDecimalFormat::UNUM_MAX_INTEGER_DIGITS = 3;
const Int32 CNativeDecimalFormat::UNUM_MIN_INTEGER_DIGITS = 4;
const Int32 CNativeDecimalFormat::UNUM_INTEGER_DIGITS = 5;
const Int32 CNativeDecimalFormat::UNUM_MAX_FRACTION_DIGITS = 6;
const Int32 CNativeDecimalFormat::UNUM_MIN_FRACTION_DIGITS = 7;
const Int32 CNativeDecimalFormat::UNUM_FRACTION_DIGITS = 8;
const Int32 CNativeDecimalFormat::UNUM_MULTIPLIER = 9;
const Int32 CNativeDecimalFormat::UNUM_GROUPING_SIZE = 10;
const Int32 CNativeDecimalFormat::UNUM_ROUNDING_MODE = 11;
const Int32 CNativeDecimalFormat::UNUM_ROUNDING_INCREMENT = 12;
const Int32 CNativeDecimalFormat::UNUM_FORMAT_WIDTH = 13;
const Int32 CNativeDecimalFormat::UNUM_PADDING_POSITION = 14;
const Int32 CNativeDecimalFormat::UNUM_SECONDARY_GROUPING_SIZE = 15;
const Int32 CNativeDecimalFormat::UNUM_SIGNIFICANT_DIGITS_USED = 16;
const Int32 CNativeDecimalFormat::UNUM_MIN_SIGNIFICANT_DIGITS = 17;
const Int32 CNativeDecimalFormat::UNUM_MAX_SIGNIFICANT_DIGITS = 18;
const Int32 CNativeDecimalFormat::UNUM_LENIENT_PARSE = 19;
const Int32 CNativeDecimalFormat::UNUM_POSITIVE_PREFIX = 0;
const Int32 CNativeDecimalFormat::UNUM_POSITIVE_SUFFIX = 1;
const Int32 CNativeDecimalFormat::UNUM_NEGATIVE_PREFIX = 2;
const Int32 CNativeDecimalFormat::UNUM_NEGATIVE_SUFFIX = 3;
const Int32 CNativeDecimalFormat::UNUM_PADDING_CHARACTER = 4;
const Int32 CNativeDecimalFormat::UNUM_CURRENCY_CODE = 5;
const Int32 CNativeDecimalFormat::UNUM_DEFAULT_RULESET = 6;
const Int32 CNativeDecimalFormat::UNUM_PUBLIC_RULESETS = 7;

Vector<AutoPtr<INumberFormatField> > getIDs()
{
    Vector<AutoPtr<INumberFormatField> > ids;

    // The old java field values were 0 for integer and 1 for fraction.
    // The new java field attributes are all objects.  ICU assigns the values
    // starting from 0 in the following order; note that integer and
    // fraction positions match the old field values.
    ids.PushBack(CNumberFormatField::INTEGER);               //  0 UNUM_INTEGER_FIELD
    ids.PushBack(CNumberFormatField::FRACTION);              //  1 UNUM_FRACTION_FIELD
    ids.PushBack(CNumberFormatField::DECIMAL_SEPARATOR);     //  2 UNUM_DECIMAL_SEPARATOR_FIELD
    ids.PushBack(CNumberFormatField::EXPONENT_SYMBOL);       //  3 UNUM_EXPONENT_SYMBOL_FIELD
    ids.PushBack(CNumberFormatField::EXPONENT_SIGN);         //  4 UNUM_EXPONENT_SIGN_FIELD
    ids.PushBack(CNumberFormatField::EXPONENT);              //  5 UNUM_EXPONENT_FIELD
    ids.PushBack(CNumberFormatField::GROUPING_SEPARATOR);    //  6 UNUM_GROUPING_SEPARATOR_FIELD
    ids.PushBack(CNumberFormatField::CURRENCY);              //  7 UNUM_CURRENCY_FIELD
    ids.PushBack(CNumberFormatField::PERCENT);               //  8 UNUM_PERCENT_FIELD
    ids.PushBack(CNumberFormatField::PERMILLE);              //  9 UNUM_PERMILL_FIELD
    ids.PushBack(CNumberFormatField::SIGN);                  // 10 UNUM_SIGN_FIELD

    return ids;
}

const Vector<AutoPtr<INumberFormatField> > CNativeDecimalFormat::ICU4C_FIELD_IDS = getIDs();

CNativeDecimalFormat::FieldPositionIterator::FieldPositionIterator()
    : mData(NULL)
    , mPos(-3)
{}

AutoPtr<CNativeDecimalFormat::FieldPositionIterator>
CNativeDecimalFormat::FieldPositionIterator::ForFieldPosition(
    /* [in] */ IFieldPosition* fp)
{
    return (fp != NULL) ? new FieldPositionIterator() : NULL;
}

ECode CNativeDecimalFormat::FieldPositionIterator::Next(
    /* [out] */ Boolean* next)
{
    VALIDATE_NOT_NULL(next);

    if (mData == NULL) {
        *next = FALSE;
        return NOERROR;
    }
    mPos += 3;
    *next = mPos < mData->GetLength();
    return NOERROR;
}

ECode CNativeDecimalFormat::FieldPositionIterator::FieldId(
    /* [out] */ Int32* fieldId)
{
    VALIDATE_NOT_NULL(fieldId);
    *fieldId = (*mData)[mPos];
    return NOERROR;
}

ECode CNativeDecimalFormat::FieldPositionIterator::Field(
    /* [out] */ IAttributedCharacterIteratorAttribute** field)
{
    VALIDATE_NOT_NULL(field);
    *field = IAttributedCharacterIteratorAttribute::Probe(ICU4C_FIELD_IDS[(*mData)[mPos]]);
    REFCOUNT_ADD(*field);
    return NOERROR;
}

ECode CNativeDecimalFormat::FieldPositionIterator::Start(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = (*mData)[mPos + 1];
    return NOERROR;
}

ECode CNativeDecimalFormat::FieldPositionIterator::Limit(
    /* [out] */ Int32* limit)
{
    VALIDATE_NOT_NULL(limit);
    *limit = (*mData)[mPos + 2];
    return NOERROR;
}

void CNativeDecimalFormat::FieldPositionIterator::SetData(
    /* [in] */ ArrayOf<Int32>* data)
{
    mData = data;
    mPos = -3;
}

CNativeDecimalFormat::CNativeDecimalFormat()
    : mAddress(0)
    , mNegPrefNull(FALSE)
    , mNegSuffNull(FALSE)
    , mPosPrefNull(FALSE)
    , mPosSuffNull(FALSE)
    , mParseBigDecimal(FALSE)
{}

CNativeDecimalFormat::~CNativeDecimalFormat()
{
    //try {
    Close();
    // } finally {
    //     super.finalize();
    // }
}

ECode CNativeDecimalFormat::constructor(
    /* [in] */ const String& pattern,
    /* [in] */ IDecimalFormatSymbols* dfs)
{
    String currencySymbol;
    dfs->GetCurrencySymbol(&currencySymbol);
    Char32 decimalSeparator;
    dfs->GetDecimalSeparator(&decimalSeparator);
    Char32 digit;
    dfs->GetDigit(&digit);
    String exponentSeparator;
    dfs->GetExponentSeparator(&exponentSeparator);
    Char32 groupingSeparator;
    dfs->GetGroupingSeparator(&groupingSeparator);
    String infinity;
    dfs->GetInfinity(&infinity);
    String internationalCurrencySymbol;
    dfs->GetInternationalCurrencySymbol(&internationalCurrencySymbol);
    String minusSign;
    ((Elastos::Text::DecimalFormatSymbols*)dfs)->GetMinusSignString(&minusSign);
    Char32 monetaryDecimalSeparator;
    dfs->GetMonetaryDecimalSeparator(&monetaryDecimalSeparator);
    String naN;
    dfs->GetNaN(&naN);
    Char32 patternSeparator;
    dfs->GetPatternSeparator(&patternSeparator);
    Char32 percent;
    dfs->GetPercent(&percent);
    Char32 perMill;
    dfs->GetPerMill(&perMill);
    Char32 zeroDigit;
    dfs->GetZeroDigit(&zeroDigit);
    //try {
    ECode ec = Open(pattern, currencySymbol,
            decimalSeparator, digit, exponentSeparator,
            groupingSeparator, infinity,
            internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, naN, patternSeparator,
            percent, perMill, zeroDigit, &mAddress);
    if (FAILED(ec)) {
        return ec == (ECode)E_RUNTIME_EXCEPTION ? E_ILLEGAL_ARGUMENT_EXCEPTION : ec;
    }
    mLastPattern = pattern;
    //} catch (NullPointerException npe) {
    //    throw npe;
    //}
    return NOERROR;
}

ECode CNativeDecimalFormat::constructor(
   /* [in] */ const String& pattern,
   /* [in] */ ILocaleData* data)
{
    String currencySymbol;
    data->GetCurrencySymbol(&currencySymbol);
    Char32 decimalSeparator, digit;
    data->GetDecimalSeparator(&decimalSeparator);
    digit = '#';
    String exponentSeparator;
    data->GetExponentSeparator(&exponentSeparator);
    Char32 groupingSeparator;
    data->GetGroupingSeparator(&groupingSeparator);
    String infinity, internationalCurrencySymbol;
    data->GetInfinity(&infinity);
    data->GetInternationalCurrencySymbol(&internationalCurrencySymbol);
    String minusSign;
    data->GetMinusSign(&minusSign);
    Char32 monetarySeparator;
    data->GetMonetarySeparator(&monetarySeparator);
    String naN;
    data->GetNaN(&naN);
    Char32 patternSeparator, percent, perMill, zeroDigit;
    data->GetPatternSeparator(&patternSeparator);
    data->GetPercent(&percent);
    data->GetPerMill(&perMill);
    data->GetZeroDigit(&zeroDigit);

    FAIL_RETURN(Open(pattern, currencySymbol,
           decimalSeparator, digit, exponentSeparator, groupingSeparator,
           infinity, internationalCurrencySymbol, minusSign,
           monetarySeparator, naN, patternSeparator,
           percent, perMill, zeroDigit, &mAddress));
    mLastPattern = pattern;
    return NOERROR;
}

ECode CNativeDecimalFormat::Close()
{
    AutoLock lock(this);

    if (mAddress != 0) {
        Close(mAddress);
        mAddress = 0;
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::SetDecimalFormatSymbols(
    /* [in] */ IDecimalFormatSymbols* dfs)
{
    String currencySymbol;
    dfs->GetCurrencySymbol(&currencySymbol);
    Char32 decimalSeparator;
    dfs->GetDecimalSeparator(&decimalSeparator);
    Char32 digit;
    dfs->GetDigit(&digit);
    String exponentSeparator;
    dfs->GetExponentSeparator(&exponentSeparator);
    Char32 groupingSeparator;
    dfs->GetGroupingSeparator(&groupingSeparator);
    String infinity;
    dfs->GetInfinity(&infinity);
    String internationalCurrencySymbol;
    dfs->GetInternationalCurrencySymbol(&internationalCurrencySymbol);
    String minusSign;
    ((Elastos::Text::DecimalFormatSymbols*)dfs)->GetMinusSignString(&minusSign);
    Char32 monetaryDecimalSeparator;
    dfs->GetMonetaryDecimalSeparator(&monetaryDecimalSeparator);
    String NaN;
    dfs->GetNaN(&NaN);
    Char32 patternSeparator;
    dfs->GetPatternSeparator(&patternSeparator);
    Char32 percent;
    dfs->GetPercent(&percent);
    Char32 perMill;
    dfs->GetPerMill(&perMill);
    Char32 zeroDigit;
    dfs->GetZeroDigit(&zeroDigit);
    SetDecimalFormatSymbols(mAddress, currencySymbol, decimalSeparator,
            digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, NaN, patternSeparator,
            percent, perMill, zeroDigit);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetDecimalFormatSymbols(
    /* [in] */ ILocaleData* localeData)
{
    String currencySymbol;
    localeData->GetCurrencySymbol(&currencySymbol);
    Char32 decimalSeparator, digit;
    localeData->GetDecimalSeparator(&decimalSeparator);
    digit = '#';
    String exponentSeparator;
    localeData->GetExponentSeparator(&exponentSeparator);
    Char32 groupingSeparator;
    localeData->GetGroupingSeparator(&groupingSeparator);
    String infinity, internationalCurrencySymbol;
    localeData->GetInfinity(&infinity);
    localeData->GetInternationalCurrencySymbol(&internationalCurrencySymbol);
    String minusSign;
    localeData->GetMinusSign(&minusSign);
    Char32 monetarySeparator;
    localeData->GetMonetarySeparator(&monetarySeparator);
    String naN;
    localeData->GetNaN(&naN);
    Char32 patternSeparator;
    localeData->GetPatternSeparator(&patternSeparator);
    Char32 percent, perMill, zeroDigit;
    localeData->GetPercent(&percent);
    localeData->GetPerMill(&perMill);
    localeData->GetZeroDigit(&zeroDigit);
    SetDecimalFormatSymbols(mAddress, currencySymbol, decimalSeparator,
            digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol,
            minusSign, monetarySeparator, naN, patternSeparator,
            ((LocaleData *)localeData)->mPercent,
            ((LocaleData *)localeData)->mPerMill,
            ((LocaleData *)localeData)->mZeroDigit);
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatBigDecimal(
    /* [in] */ IBigDecimal* value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ ArrayOf<Char32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    String str;
    IObject::Probe(value)->ToString(&str);
    AutoPtr<ArrayOf<Char32> > out;
    FAIL_RETURN(FormatDigitList(mAddress, str, fpi, (ArrayOf<Char32>**)&out));
    if (fpi != NULL && field != NULL) {
        FAIL_RETURN(UpdateFieldPosition(field, fpi));
    }
    *result = out;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatBigInteger(
    /* [in] */ IBigInteger* value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ ArrayOf<Char32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    String str;
    value->ToString(10,&str);
    AutoPtr<ArrayOf<Char32> >  out;
    FAIL_RETURN(FormatDigitList(mAddress, str, fpi, (ArrayOf<Char32>**)&out));
    if (fpi != NULL && field != NULL) {
        FAIL_RETURN(UpdateFieldPosition(field, fpi));
    }
    *result = out;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatInt64(
    /* [in] */ Int64 value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ ArrayOf<Char32>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    AutoPtr<ArrayOf<Char32> > result;
    FAIL_RETURN(FormatInt64(mAddress, value, fpi, (ArrayOf<Char32>**)&result));
    if (fpi != NULL && field != NULL) {
        FAIL_RETURN(UpdateFieldPosition(field, fpi));
    }
    *array = result;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatDouble(
    /* [in] */ Double value,
    /* [in] */ IFieldPosition* field,
    /* [out, callee] */ ArrayOf<Char32>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    AutoPtr<FieldPositionIterator> fpi = FieldPositionIterator::ForFieldPosition(field);
    AutoPtr<ArrayOf<Char32> > result;
    FAIL_RETURN(FormatDouble(mAddress, value, fpi, (ArrayOf<Char32>**)&result));
    if (fpi != NULL && field != NULL) {
        FAIL_RETURN(UpdateFieldPosition(field, fpi));
    }
    *array = result;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CNativeDecimalFormat::UpdateFieldPosition(
    /* [in] */ IFieldPosition* fp,
    /* [in] */ FieldPositionIterator* fpi)
{
    Int32 field;
    FAIL_RETURN(TranslateFieldId(fp, &field))
    if (field != -1) {
        Boolean next = false;
        while (fpi->Next(&next), next) {
            Int32 id;
            fpi->FieldId(&id);
            if (id == field) {
                fpi->Start(&id);
                fp->SetBeginIndex(id);
                fpi->Limit(&id);
                fp->SetEndIndex(id);
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::ApplyLocalizedPattern(
    /* [in] */ const String& pattern)
{
    FAIL_RETURN(ApplyPattern(mAddress, TRUE, pattern));
    mLastPattern = NULL;
    return NOERROR;
}

ECode CNativeDecimalFormat::ApplyPattern(
    /* [in] */ const String& pattern)
{
    if (!mLastPattern.IsNull() && pattern.Equals(mLastPattern)) {
        return NOERROR;
    }
    FAIL_RETURN(ApplyPattern(mAddress, FALSE, pattern));
    mLastPattern = pattern;
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* object,
    /* [out] */ IAttributedCharacterIterator** iter)
{
    VALIDATE_NOT_NULL(iter);
    AutoPtr<INumber> number = (INumber*)object->Probe(EIID_INumber);
    if (number == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<FieldPositionIterator> fpIter = new FieldPositionIterator();
    String text;
    if (number->Probe(EIID_IBigInteger) != NULL
            || number ->Probe(EIID_IBigDecimal) != NULL) {
        String str;
        IObject::Probe(number)->ToString(&str);
        AutoPtr<ArrayOf<Char32> >  out;
        FAIL_RETURN(FormatDigitList(mAddress, str, fpIter, (ArrayOf<Char32>**)&out));
        text = String(*out);
    }
    else if (number->Probe(EIID_IDouble) != NULL || number->Probe(EIID_IFloat) != NULL) {
        Double dv;
        number->DoubleValue(&dv);
        AutoPtr<ArrayOf<Char32> > ans;
        FAIL_RETURN(FormatDouble(mAddress, dv, fpIter, (ArrayOf<Char32>**)&ans));
        text = String( *ans);
    }
    else {
        Int64 lv;
        number->Int64Value(&lv);
        AutoPtr<ArrayOf<Char32> > ans;
        FAIL_RETURN(FormatInt64(mAddress, lv, fpIter, (ArrayOf<Char32>**)&ans));
        text = String(*ans);
    }

    AutoPtr<IAttributedString> as;
    FAIL_RETURN(CAttributedString::New(text, (IAttributedString**)&as));

    Boolean next;
    while (fpIter->Next(&next), next) {
        AutoPtr<IAttributedCharacterIteratorAttribute> field;
        fpIter->Field((IAttributedCharacterIteratorAttribute**)&field);
        Int32 start(0), limit(0);
        fpIter->Start(&start);
        fpIter->Limit(&limit);
        FAIL_RETURN(as->AddAttribute(field,(IInterface*)field, start, limit));
    }

    // return the CharacterIterator from StructAttributedString
    return as->GetIterator(iter);
}

Int32 CNativeDecimalFormat::MakeScalePositive(
    /* [in] */ Int32 scale,
    /* [in] */ StringBuffer& val)
{
    if (scale < 0) {
        scale = -scale;
        for (Int32 i = scale; i > 0; i--) {
            val.AppendChar('0');
        }
        scale = 0;
    }
    return scale;
}

ECode CNativeDecimalFormat::ToLocalizedPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = ToPatternImpl(mAddress, TRUE);
    return NOERROR;
}

ECode CNativeDecimalFormat::ToPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = ToPatternImpl(mAddress, FALSE);
    return NOERROR;
}

ECode CNativeDecimalFormat::Parse(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ INumber** number)
{
    VALIDATE_NOT_NULL(number);
    AutoPtr<INumber> temp = Parse(mAddress, string, position, mParseBigDecimal);
    *number = temp;
    REFCOUNT_ADD(*number);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetMaximumFractionDigits(
    /* [out] */ Int32* maxFractionDigits)
{
    VALIDATE_NOT_NULL(maxFractionDigits);
    *maxFractionDigits = GetAttribute(mAddress, UNUM_MAX_FRACTION_DIGITS);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetMaximumIntegerDigits(
    /* [out] */ Int32* maxIntegerDigits)
{
    VALIDATE_NOT_NULL(maxIntegerDigits);
    *maxIntegerDigits = GetAttribute(mAddress, UNUM_MAX_INTEGER_DIGITS);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetMinimumFractionDigits(
    /* [out] */ Int32* minFractionDigits)
{
    VALIDATE_NOT_NULL(minFractionDigits);
    *minFractionDigits = GetAttribute(mAddress, UNUM_MIN_FRACTION_DIGITS);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetMinimumIntegerDigits(
    /* [out] */ Int32* minIntegerDigits)
{
    VALIDATE_NOT_NULL(minIntegerDigits);
    *minIntegerDigits = GetAttribute(mAddress, UNUM_MIN_INTEGER_DIGITS);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetGroupingSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    // Work around http://bugs.icu-project.org/trac/ticket/10864 in icu4c 53.
    Boolean b;
    FAIL_RETURN(IsGroupingUsed(&b))
    if (!b) {
        *size = 0;
        return NOERROR;
    }
    *size = GetAttribute(mAddress, UNUM_GROUPING_SIZE);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetMultiplier(
    /* [out] */ Int32* multiplier)
{
    VALIDATE_NOT_NULL(multiplier);
    *multiplier = GetAttribute(mAddress, UNUM_MULTIPLIER);
    return NOERROR;
}

ECode CNativeDecimalFormat::GetNegativePrefix(
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);
    if (mNegPrefNull) {
        *prefix = NULL;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_NEGATIVE_PREFIX, prefix);
}

ECode CNativeDecimalFormat::GetNegativeSuffix(
    /* [out] */ String* suffix)
{
    VALIDATE_NOT_NULL(suffix);
    if (mNegSuffNull) {
        *suffix = NULL;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_NEGATIVE_SUFFIX, suffix);
}

ECode CNativeDecimalFormat::GetPositivePrefix(
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);
    if (mPosPrefNull) {
        *prefix = NULL;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_POSITIVE_PREFIX, prefix);
}

ECode CNativeDecimalFormat::GetPositiveSuffix(
    /* [out] */ String* suffix)
{
    VALIDATE_NOT_NULL(suffix);
    if (mPosSuffNull) {
        *suffix = NULL;
        return NOERROR;
    }
    return GetTextAttribute(mAddress, UNUM_POSITIVE_SUFFIX, suffix);
}

ECode CNativeDecimalFormat::IsDecimalSeparatorAlwaysShown(
    /* [out] */ Boolean* isAlwaysShown)
{
    VALIDATE_NOT_NULL(isAlwaysShown);
    *isAlwaysShown = GetAttribute(mAddress, UNUM_DECIMAL_ALWAYS_SHOWN) != 0;
    return NOERROR;
}

ECode CNativeDecimalFormat::IsParseBigDecimal(
    /* [out] */ Boolean* isParseBigDecimal)
{
    VALIDATE_NOT_NULL(isParseBigDecimal);
    *isParseBigDecimal = mParseBigDecimal;
    return NOERROR;
}

ECode CNativeDecimalFormat::IsParseIntegerOnly(
    /* [out] */ Boolean* isParseIntegerOnly)
{
    VALIDATE_NOT_NULL(isParseIntegerOnly);
    *isParseIntegerOnly = GetAttribute(mAddress, UNUM_PARSE_INT_ONLY) != 0;
    return NOERROR;
}

ECode CNativeDecimalFormat::IsGroupingUsed(
    /* [out] */ Boolean* isGroupingUsed)
{
    VALIDATE_NOT_NULL(isGroupingUsed);
    *isGroupingUsed = GetAttribute(mAddress, UNUM_GROUPING_USED) != 0;
    return NOERROR;
}

ECode CNativeDecimalFormat::SetDecimalSeparatorAlwaysShown(
    /* [in] */ Boolean value)
{
    Int32 i = value ? -1 : 0;
    SetAttribute(mAddress, UNUM_DECIMAL_ALWAYS_SHOWN, i);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetCurrency(
    /* [in] */ const String& currencySymbol,
    /* [in] */ const String& currencyCode)
{
    FAIL_RETURN(SetSymbol(mAddress, UNUM_CURRENCY_SYMBOL, currencySymbol));
    return SetSymbol(mAddress, UNUM_INTL_CURRENCY_SYMBOL, currencyCode);
}

ECode CNativeDecimalFormat::SetGroupingSize(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_GROUPING_SIZE, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetGroupingUsed(
    /* [in] */ Boolean value)
{
    Int32 i = value ? -1 : 0;
    SetAttribute(mAddress, UNUM_GROUPING_USED, i);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetMaximumFractionDigits(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_MAX_FRACTION_DIGITS, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetMaximumIntegerDigits(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_MAX_INTEGER_DIGITS, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetMinimumFractionDigits(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_MIN_FRACTION_DIGITS, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetMinimumIntegerDigits(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_MIN_INTEGER_DIGITS, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetMultiplier(
    /* [in] */ Int32 value)
{
    SetAttribute(mAddress, UNUM_MULTIPLIER, value);
    return NOERROR;
}

ECode CNativeDecimalFormat::SetNegativePrefix(
    /* [in] */ const String& value)
{
    mNegPrefNull = value.IsNull();
    if (!mNegPrefNull) {
        return SetTextAttribute(mAddress, UNUM_NEGATIVE_PREFIX, value);
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::SetNegativeSuffix(
    /* [in] */ const String& value)
{
    mNegSuffNull = value.IsNull();
    if (!mNegSuffNull) {
        return SetTextAttribute(mAddress, UNUM_NEGATIVE_SUFFIX, value);
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::SetPositivePrefix(
    /* [in] */ const String& value)
{
    mPosPrefNull = value.IsNull();
    if (!mPosPrefNull) {
        return SetTextAttribute(mAddress, UNUM_POSITIVE_PREFIX, value);
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::SetPositiveSuffix(
    /* [in] */ const String& value)
{
    mPosSuffNull = value.IsNull();
    if (!mPosSuffNull) {
        return SetTextAttribute(mAddress, UNUM_POSITIVE_SUFFIX, value);
    }
    return NOERROR;
}

ECode CNativeDecimalFormat::SetParseBigDecimal(
    /* [in] */ Boolean value)
{
    mParseBigDecimal = value;
    return NOERROR;
}

ECode CNativeDecimalFormat::SetParseIntegerOnly(
    /* [in] */ Boolean value)
{
    Int32 i = value ? -1 : 0;
    SetAttribute(mAddress, UNUM_PARSE_INT_ONLY, i);
    return NOERROR;
}

ECode CNativeDecimalFormat::ApplyPattern(
    /* [in] */ Int64 addr,
    /* [in] */ Boolean localized,
    /* [in] */ const String& pattern)
{
    //try {
    return ApplyPatternImpl(addr, localized, pattern);
    //} catch (NullPointerException npe) {
    //    throw npe;
    //} catch (RuntimeException re) {
    //    throw new IllegalArgumentException("syntax error: " + re.getMessage() + ": " + pattern);
    //}
}

ECode CNativeDecimalFormat::SetRoundingMode(
    /* [in] */ RoundingMode roundingMode,
    /* [in] */ Double roundingIncrement)
{
    Int32 nativeRoundingMode;
    switch (roundingMode) {
        case Elastos::Math::RoundingMode_CEILING:
            nativeRoundingMode = 0;
            break;
        case Elastos::Math::RoundingMode_FLOOR:
            nativeRoundingMode = 1;
            break;
        case Elastos::Math::RoundingMode_DOWN:
            nativeRoundingMode = 2;
            break;
        case Elastos::Math::RoundingMode_UP:
            nativeRoundingMode = 3;
            break;
        case Elastos::Math::RoundingMode_HALF_EVEN:
            nativeRoundingMode = 4;
            break;
        case Elastos::Math::RoundingMode_HALF_DOWN:
            nativeRoundingMode = 5;
            break;
        case Elastos::Math::RoundingMode_HALF_UP:
            nativeRoundingMode = 6;
            break;
        case Elastos::Math::RoundingMode_UNNECESSARY:
            nativeRoundingMode = 7;
            break;
        default: //throw new AssertionError();
            return E_ASSERTION_ERROR;
    }
    SetRoundingMode(mAddress, nativeRoundingMode, roundingIncrement);
    return NOERROR;
}

static U_ICU_NAMESPACE::DecimalFormat* toDecimalFormat(Int32 addr)
{
    return reinterpret_cast<U_ICU_NAMESPACE::DecimalFormat*>(static_cast<uintptr_t>(addr));
}

static UNumberFormat* toUNumberFormat(Int32 addr)
{
    return reinterpret_cast<UNumberFormat*>(static_cast<uintptr_t>(addr));
}

static U_ICU_NAMESPACE::DecimalFormatSymbols* makeDecimalFormatSymbols(
    /* [in] */ const String& currencySymbol,
    /* [in] */ Char32 decimalSeparator,
    /* [in] */ Char32 digit,
    /* [in] */ const String& exponentSeparator,
    /* [in] */ Char32 groupingSeparator,
    /* [in] */ const String& infinity,
    /* [in] */ const String& internationalCurrencySymbol,
    /* [in] */ const String& minusSign,
    /* [in] */ Char32 monetaryDecimalSeparator,
    /* [in] */ const String& nan,
    /* [in] */ Char32 patternSeparator,
    /* [in] */ Char32 percent,
    /* [in] */ Char32 perMill,
    /* [in] */ Char32 zeroDigit)
{
    U_ICU_NAMESPACE::DecimalFormatSymbols* result = new U_ICU_NAMESPACE::DecimalFormatSymbols;


    // result->setSymbol(DecimalFormatSymbols::kZeroDigitSymbol, UnicodeString(zeroDigit + 0));
    // result->setSymbol(DecimalFormatSymbols::kOneDigitSymbol, UnicodeString(zeroDigit + 1));
    // result->setSymbol(DecimalFormatSymbols::kTwoDigitSymbol, UnicodeString(zeroDigit + 2));
    // result->setSymbol(DecimalFormatSymbols::kThreeDigitSymbol, UnicodeString(zeroDigit + 3));
    // result->setSymbol(DecimalFormatSymbols::kFourDigitSymbol, UnicodeString(zeroDigit + 4));
    // result->setSymbol(DecimalFormatSymbols::kFiveDigitSymbol, UnicodeString(zeroDigit + 5));
    // result->setSymbol(DecimalFormatSymbols::kSixDigitSymbol, UnicodeString(zeroDigit + 6));
    // result->setSymbol(DecimalFormatSymbols::kSevenDigitSymbol, UnicodeString(zeroDigit + 7));
    // result->setSymbol(DecimalFormatSymbols::kEightDigitSymbol, UnicodeString(zeroDigit + 8));
    // result->setSymbol(DecimalFormatSymbols::kNineDigitSymbol, UnicodeString(zeroDigit + 9));

    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kCurrencySymbol,
            UnicodeString::fromUTF8(currencySymbol.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kDecimalSeparatorSymbol,
            UnicodeString((UChar32)decimalSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kDigitSymbol,
            UnicodeString((UChar32)digit));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kExponentialSymbol,
            UnicodeString::fromUTF8(exponentSeparator.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kGroupingSeparatorSymbol,
            UnicodeString((UChar32)groupingSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMonetaryGroupingSeparatorSymbol,
            UnicodeString((UChar32)groupingSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kInfinitySymbol,
            UnicodeString::fromUTF8(infinity.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kIntlCurrencySymbol,
            UnicodeString::fromUTF8(internationalCurrencySymbol.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMinusSignSymbol,
            UnicodeString::fromUTF8(minusSign.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kMonetarySeparatorSymbol,
            UnicodeString((UChar32)monetaryDecimalSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kNaNSymbol,
            UnicodeString::fromUTF8(nan.string()));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPatternSeparatorSymbol,
            UnicodeString((UChar32)patternSeparator));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPercentSymbol,
            UnicodeString((UChar32)percent));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kPerMillSymbol,
            UnicodeString((UChar32)perMill));

    // java.text.DecimalFormatSymbols just uses a zero digit,
    // but ICU >= 4.6 has a field for each decimal digit.
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kZeroDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 0)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kOneDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 1)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kTwoDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 2)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kThreeDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 3)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kFourDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 4)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kFiveDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 5)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kSixDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 6)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kSevenDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 7)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kEightDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 8)));
    result->setSymbol(U_ICU_NAMESPACE::DecimalFormatSymbols::kNineDigitSymbol,
            UnicodeString((UChar32)(zeroDigit + 9)));
    return result;
}

extern ECode maybeThrowIcuException(const char* provider, UErrorCode errorCode);

ECode CNativeDecimalFormat::ApplyPatternImpl(
    /* [in] */ Int64 addr,
    /* [in] */ Boolean localized,
    /* [in] */ const String& pattern)
{
    if (pattern.IsNull()) {
        return NOERROR;
    }
    U_ICU_NAMESPACE::DecimalFormat* fmt = toDecimalFormat(addr);
    UErrorCode status = U_ZERO_ERROR;
    const char* function;
    if (localized) {
        function = "DecimalFormat::applyLocalizedPattern";
        fmt->applyLocalizedPattern(
            UnicodeString::fromUTF8(pattern.string()), status);
    }
    else {
        function = "DecimalFormat::applyPattern";
        fmt->applyPattern(
            UnicodeString::fromUTF8(pattern.string()), status);
    }
    return maybeThrowIcuException(function, status);
}

Int64 CNativeDecimalFormat::CloneImpl(
    /* [in] */ Int64 addr)
{
    U_ICU_NAMESPACE::DecimalFormat* fmt = toDecimalFormat(addr);
    return reinterpret_cast<uintptr_t>(fmt->clone());
}

void CNativeDecimalFormat::Close(
    /* [in] */ Int64 addr)
{
    delete toDecimalFormat(addr);
}

static AutoPtr<ArrayOf<Char32> > formatResult(
    /* [in] */ const UnicodeString& s,
    /* [in] */ U_ICU_NAMESPACE::FieldPositionIterator* fpi,
    /* [in] */ CNativeDecimalFormat::FieldPositionIterator* javaFieldPositionIterator)
{
    if (fpi != NULL) {
        Vector<Int32> data;
        NATIVE(FieldPosition) fp;
        while (fpi->next(fp)) {
            data.PushBack(fp.getField());
            data.PushBack(fp.getBeginIndex());
            data.PushBack(fp.getEndIndex());
        }

        AutoPtr<ArrayOf<Int32> > aiData = NULL;
        if (!data.IsEmpty()) {
            aiData = ArrayOf<Int32>::Alloc(data.GetSize());
            if (NULL == aiData) {
                return NULL;
            }
            memcpy(aiData->GetPayload(), &data[0], data.GetSize() * sizeof(Int32));
        }
        javaFieldPositionIterator->SetData(aiData);
    }

    int len = s.length();
    AutoPtr<ArrayOf<Char32> > result = ArrayOf<Char32>::Alloc(len);
    if (result != NULL) {
        String str = UnicodeStringToString(s);
        AutoPtr<ArrayOf<Char32> > charArray = str.GetChars();
        for (Int32 i = 0; i < charArray->GetLength(); ++i) {
            (*result)[i] = (*charArray)[i];
        }
    }
    REFCOUNT_ADD(result);
    return result;
}

template <typename T>
static ECode format(
    Int64 addr,
    CNativeDecimalFormat::FieldPositionIterator* javaFieldPositionIterator,
    T value,
    ArrayOf<Char32>** rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = NULL;

    UErrorCode status = U_ZERO_ERROR;
    UnicodeString s;
    DecimalFormat* fmt = toDecimalFormat(addr);
    U_ICU_NAMESPACE::FieldPositionIterator nativeFieldPositionIterator;
    U_ICU_NAMESPACE::FieldPositionIterator* fpi = javaFieldPositionIterator ? &nativeFieldPositionIterator : NULL;
    fmt->format(value, s, fpi, status);
    FAIL_RETURN(maybeThrowIcuException("format", status))
    *rev = formatResult(s, fpi, javaFieldPositionIterator);
    return NOERROR;
}

ECode CNativeDecimalFormat::FormatInt64(
    /* [in] */ Int64 addr,
    /* [in] */ Int64 value,
    /* [in] */ CNativeDecimalFormat::FieldPositionIterator* javaFieldPositionIterator,
    /* [out] */ ArrayOf<Char32>** rev)
{
    return format<Int64>(addr, javaFieldPositionIterator, value, rev);
}

ECode CNativeDecimalFormat::FormatDouble(
    /* [in] */ Int64 addr,
    /* [in] */ Double value,
    /* [in] */ CNativeDecimalFormat::FieldPositionIterator* javaFieldPositionIterator,
    /* [out] */ ArrayOf<Char32>** rev)
{
    return format<Double>(addr, javaFieldPositionIterator, value, rev);
}

ECode CNativeDecimalFormat::FormatDigitList(
    /* [in] */ Int64 addr,
    /* [in] */ const String& value,
    /* [in] */ CNativeDecimalFormat::FieldPositionIterator* javaFieldPositionIterator,
    /* [out] */ ArrayOf<Char32>** rev)
{
    if (value.IsNull()) {
        *rev = NULL;
        return NOERROR;
    }
    StringPiece sp(value.string());
    return format(addr, javaFieldPositionIterator, sp, rev);
}

static AutoPtr<INumber> newBigDecimal(
    const char* value,
    size_t len)
{
    // this is painful...
    // value is a UTF-8 string of invariant characters, but isn't guaranteed to be
    // null-terminated.  NewStringUTF requires a terminated UTF-8 string.  So we copy the
    // data to jchars using UnicodeString, and call NewString instead.
    UnicodeString tmp(value, len, UnicodeString::kInvariant);
    String str = UnicodeStringToString(tmp);
    AutoPtr<IBigDecimal> pbd;
    CBigDecimal::New(str, (IBigDecimal**)&pbd);
    REFCOUNT_ADD(pbd);
    return INumber::Probe(pbd);
}

Int32 CNativeDecimalFormat::GetAttribute(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 symbol)
{
    UNumberFormatAttribute attr = static_cast<UNumberFormatAttribute>(symbol);
    return unum_getAttribute(toUNumberFormat(addr), attr);
}

ECode CNativeDecimalFormat::GetTextAttribute(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 symbol,
    /* [out] */ String* _attr)
{
    VALIDATE_NOT_NULL(_attr)
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat* fmt = toUNumberFormat(addr);
    UNumberFormatTextAttribute attr = static_cast<UNumberFormatTextAttribute>(symbol);

    // Find out how long the result will be...
    UChar* chars = NULL;
    uint32_t charCount = 0;
    uint32_t desiredCount = unum_getTextAttribute(fmt, attr, chars, charCount, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        // ...then get it.
        status = U_ZERO_ERROR;
        charCount = desiredCount + 1;
        chars = new UChar[charCount];
        charCount = unum_getTextAttribute(fmt, attr, chars, charCount, &status);
    }
    String str("");
    ElStringByteSink sink(&str);
    UnicodeString(chars, charCount).toUTF8(sink);
    *_attr = str;
    delete[] chars;
    return maybeThrowIcuException("unum_getTextAttribute", status);
}

ECode CNativeDecimalFormat::Open(
    /* [in] */ const String& pattern,
    /* [in] */ const String& currencySymbol,
    /* [in] */ Char32 decimalSeparator,
    /* [in] */ Char32 digit,
    /* [in] */ const String& exponentSeparator,
    /* [in] */ Char32 groupingSeparator,
    /* [in] */ const String& infinity,
    /* [in] */ const String& internationalCurrencySymbol,
    /* [in] */ const String& minusSign,
    /* [in] */ Char32 monetaryDecimalSeparator,
    /* [in] */ const String& nan,
    /* [in] */ Char32 patternSeparator,
    /* [in] */ Char32 percent,
    /* [in] */ Char32 perMill,
    /* [in] */ Char32 zeroDigit,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    UErrorCode status = U_ZERO_ERROR;
    UParseError parseError;
    if (pattern.IsNull()) {
        *result = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    U_ICU_NAMESPACE::DecimalFormatSymbols* symbols = makeDecimalFormatSymbols(
            currencySymbol, decimalSeparator, digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, nan, patternSeparator, percent, perMill,
            zeroDigit);
    U_ICU_NAMESPACE::DecimalFormat* fmt = new U_ICU_NAMESPACE::DecimalFormat(
            UnicodeString::fromUTF8(pattern.string()), symbols, parseError, status);
    if (fmt == NULL) {
        delete symbols;
    }
    FAIL_RETURN(maybeThrowIcuException("DecimalFormat::DecimalFormat", status))
    *result = reinterpret_cast<uintptr_t>(fmt);
    return NOERROR;
}

AutoPtr<INumber> CNativeDecimalFormat::Parse(
    /* [in] */ Int64 addr,
    /* [in] */ const String& text,
    /* [in] */ IParsePosition* position,
    /* [in] */ Boolean parseBigDecimal)
{
    if (text.IsNull()) {
        return NULL;
    }
    // make sure the ParsePosition is valid. Actually icu4c would parse a number
    // correctly even if the parsePosition is set to -1, but since the RI fails
    // for that case we have to fail too
    Int32 parsePos(0);
    position->GetIndex(&parsePos);
    if (parsePos < 0 || parsePos > text.GetLength()) {
        return NULL;
    }

    U_ICU_NAMESPACE::Formattable res;
    ParsePosition pp(parsePos);
    U_ICU_NAMESPACE::DecimalFormat* fmt = toDecimalFormat(addr);
    fmt->parse(UnicodeString::fromUTF8(text.string()), res, pp);

    if (pp.getErrorIndex() == -1) {
        position->SetIndex(pp.getIndex());
    }
    else {
        position->SetErrorIndex(pp.getErrorIndex());
        return NULL;
    }

    if (parseBigDecimal) {
        UErrorCode status = U_ZERO_ERROR;
        U_ICU_NAMESPACE::StringPiece str = res.getDecimalNumber(status);
        if (U_SUCCESS(status)) {
            int len = str.length();
            const char* data = str.data();
            if (strncmp(data, "NaN", 3) == 0 ||
                strncmp(data, "Inf", 3) == 0 ||
                strncmp(data, "-Inf", 4) == 0) {
                double resultDouble = res.getDouble(status);
                AutoPtr<IDouble> d;
                ASSERT_SUCCEEDED(CDouble::New(resultDouble, (IDouble**)&d));
                return (INumber*)d->Probe(EIID_INumber);
            }
            return newBigDecimal(data, len);
        }
        return NULL;
    }

    U_ICU_NAMESPACE::Formattable::Type numType = res.getType();
        switch(numType) {
            case U_ICU_NAMESPACE::Formattable::kDouble: {
                double resultDouble = res.getDouble();
                AutoPtr<IDouble> d;
                ASSERT_SUCCEEDED(CDouble::New(resultDouble, (IDouble**)&d));
                return (INumber*)d->Probe(EIID_INumber);
            }
            case U_ICU_NAMESPACE::Formattable::kLong: {
                long resultLong = res.getLong();
                AutoPtr<IInteger64> int64;
                ASSERT_SUCCEEDED(CInteger64::New(resultLong, (IInteger64**)&int64));
                return (INumber*)int64->Probe(EIID_INumber);
            }
            case U_ICU_NAMESPACE::Formattable::kInt64: {
                int64_t resultInt64 = res.getInt64();
                AutoPtr<IInteger64> int64;
                ASSERT_SUCCEEDED(CInteger64::New(resultInt64, (IInteger64**)&int64));
                return (INumber*)int64->Probe(EIID_INumber);
            }
            default: {
                return NULL;
        }
    }
    return NULL;
}

void CNativeDecimalFormat::SetDecimalFormatSymbols(
    /* [in] */ Int64 addr,
    /* [in] */ const String& currencySymbol,
    /* [in] */ Char32 decimalSeparator,
    /* [in] */ Char32 digit,
    /* [in] */ const String& exponentSeparator,
    /* [in] */ Char32 groupingSeparator,
    /* [in] */ const String& infinity,
    /* [in] */ const String& internationalCurrencySymbol,
    /* [in] */ const String& minusSign,
    /* [in] */ Char32 monetaryDecimalSeparator,
    /* [in] */ const String& nan,
    /* [in] */ Char32 patternSeparator,
    /* [in] */ Char32 percent,
    /* [in] */ Char32 perMill,
    /* [in] */ Char32 zeroDigit)
{
    U_ICU_NAMESPACE::DecimalFormatSymbols* symbols = makeDecimalFormatSymbols(
            currencySymbol, decimalSeparator, digit, exponentSeparator, groupingSeparator,
            infinity, internationalCurrencySymbol, minusSign,
            monetaryDecimalSeparator, nan, patternSeparator, percent, perMill,
            zeroDigit);
    toDecimalFormat(addr)->adoptDecimalFormatSymbols(symbols);
}

ECode CNativeDecimalFormat::SetSymbol(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 _symbol,
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return NOERROR;
    }
    UnicodeString _s = UnicodeString::fromUTF8(str.string());
    UnicodeString& s(_s);
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormatSymbol symbol = static_cast<UNumberFormatSymbol>(_symbol);
    unum_setSymbol(toUNumberFormat(addr), symbol, s.getBuffer(), s.length(), &status);
    return maybeThrowIcuException("unum_setSymbol", status);
}

void CNativeDecimalFormat::SetAttribute(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 symbol,
    /* [in] */ Int32 i)
{
    UNumberFormatAttribute attr = static_cast<UNumberFormatAttribute>(symbol);
    unum_setAttribute(toUNumberFormat(addr), attr, i);
}

void CNativeDecimalFormat::SetRoundingMode(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 mode,
    /* [in] */ Double increment)
{
    U_ICU_NAMESPACE::DecimalFormat* fmt = toDecimalFormat(addr);
    fmt->setRoundingMode(static_cast<U_ICU_NAMESPACE::DecimalFormat::ERoundingMode>(mode));
    fmt->setRoundingIncrement(increment);
}

ECode CNativeDecimalFormat::SetTextAttribute(
    /* [in] */ Int64 addr,
    /* [in] */ Int32 symbol,
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return NOERROR;
    }
    UnicodeString _s = UnicodeString::fromUTF8(str.string());
    UnicodeString& s(_s);
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormatTextAttribute attr = static_cast<UNumberFormatTextAttribute>(symbol);
    unum_setTextAttribute(toUNumberFormat(addr), attr, s.getBuffer(), s.length(), &status);
    return maybeThrowIcuException("unum_setTextAttribute", status);
}

String CNativeDecimalFormat::ToPatternImpl(
    /* [in] */ Int64 addr,
    /* [in] */ Boolean localized)
{
    U_ICU_NAMESPACE::DecimalFormat* fmt = toDecimalFormat(addr);
    UnicodeString pattern;
    if (localized) {
        fmt->toLocalizedPattern(pattern);
    }
    else {
        fmt->toPattern(pattern);
    }
    String s("");
    ElStringByteSink sink(&s);
    pattern.toUTF8(sink);
    return s;
}

ECode CNativeDecimalFormat::GetAddr(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mAddress;
    return NOERROR;
}

ECode CNativeDecimalFormat::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (object->Probe(EIID_INativeDecimalFormat) == this->Probe(EIID_INativeDecimalFormat)) {
        *result = TRUE;
        return NOERROR;
    }
    if (object->Probe(EIID_INativeDecimalFormat) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<INativeDecimalFormat> obj = INativeDecimalFormat::Probe(object);
    Int32 address;
    obj->GetAddr(&address);
    if (address == mAddress) {
        *result = TRUE;
        return NOERROR;
    }
    String pattern1, pattern2;
    obj->ToPattern(&pattern1);
    ToPattern(&pattern2);
    Boolean isAlwaysShown1 = FALSE, isAlwaysShown2 = FALSE;
    obj->IsDecimalSeparatorAlwaysShown(&isAlwaysShown1);
    IsDecimalSeparatorAlwaysShown(&isAlwaysShown2);
    Int32 size1, size2;
    obj->GetGroupingSize(&size1);
    GetGroupingSize(&size2);
    Int32 multiplier1, multiplier2;
    obj->GetMultiplier(&multiplier1);
    GetMultiplier(&multiplier2);
    String nPrefix1, nPrefix2;
    obj->GetNegativePrefix(&nPrefix1);
    GetNegativePrefix(&nPrefix2);
    String nSuffix1, nSuffix2;
    obj->GetNegativeSuffix(&nSuffix1);
    GetNegativeSuffix(&nSuffix2);
    String pPrefix1, pPrefix2;
    obj->GetPositivePrefix(&pPrefix1);
    GetPositivePrefix(&pPrefix2);
    String pSuffix1, pSuffix2;
    obj->GetPositiveSuffix(&pSuffix1);
    GetPositiveSuffix(&pSuffix2);
    Int32 maxIntegerDigits1, maxIntegerDigits2;
    obj->GetMaximumIntegerDigits(&maxIntegerDigits1);
    GetMaximumIntegerDigits(&maxIntegerDigits2);
    Int32 maxFractionDigits1, maxFractionDigits2;
    obj->GetMaximumFractionDigits(&maxFractionDigits1);
    GetMaximumFractionDigits(&maxFractionDigits2);
    Int32 minIntegerDigits1, minIntegerDigits2;
    obj->GetMinimumIntegerDigits(&minIntegerDigits1);
    GetMinimumIntegerDigits(&minIntegerDigits2);
    Int32 minFractionDigits1, minFractionDigits2;
    obj->GetMinimumFractionDigits(&minFractionDigits1);
    GetMinimumFractionDigits(&minFractionDigits2);
    Boolean isGroupingUsed1 = FALSE, isGroupingUsed2 = FALSE;
    obj->IsGroupingUsed(&isGroupingUsed1);
    IsGroupingUsed(&isGroupingUsed2);
    *result = pattern1.Equals(pattern2) &&
            isAlwaysShown1 == isAlwaysShown2 &&
            size1 == size2 &&
            multiplier1 == multiplier2 &&
            nPrefix1.Equals(nPrefix2) &&
            nSuffix1.Equals(nSuffix2) &&
            pPrefix1.Equals(pPrefix2) &&
            pSuffix1.Equals(pSuffix2) &&
            maxIntegerDigits1 == maxIntegerDigits2 &&
            maxFractionDigits1 == maxFractionDigits2 &&
            minIntegerDigits1 == minIntegerDigits2 &&
            minFractionDigits1 == minFractionDigits2 &&
            isGroupingUsed1 == isGroupingUsed2;

    return NOERROR;
}

ECode CNativeDecimalFormat::Clone(
    /* [out] */ IInterface** rev)
{
    VALIDATE_NOT_NULL(rev);

    AutoPtr<CNativeDecimalFormat> clone = new CNativeDecimalFormat;
    clone->mAddress = CloneImpl(mAddress);
    clone->mLastPattern = mLastPattern;
    clone->mNegPrefNull = mNegPrefNull;
    clone->mNegSuffNull = mNegSuffNull;
    clone->mPosPrefNull = mPosPrefNull;
    clone->mPosSuffNull = mPosSuffNull;
    *rev = clone->Probe(EIID_IInterface);
    REFCOUNT_ADD(clone);
    return NOERROR;
}

typedef ECode (CNativeDecimalFormat::*P_FUN_GET_STRING)(String*);
typedef ECode (CNativeDecimalFormat::*P_FUN_GET_BOOLEAN)(Boolean*);
typedef ECode (CNativeDecimalFormat::*P_FUN_GET_INT32)(Int32*);

static inline String GetValue(P_FUN_GET_STRING fun, CNativeDecimalFormat* pObj)
{
    String s;
    (pObj->*fun)(&s);
    return s;
}

static inline String GetValue(P_FUN_GET_BOOLEAN fun, CNativeDecimalFormat* pObj)
{
    Boolean b;
    (pObj->*fun)(&b);
    String s;
    s = b ? String("TRUE") : String("FALSE");
    return s;
}

static inline String GetValue(P_FUN_GET_INT32 fun, CNativeDecimalFormat* pObj)
{
    Int32 i32;
    (pObj->*fun)(&i32);
    String s;
    s.AppendFormat("%d", i32);
    return s;
}

ECode CNativeDecimalFormat::ToString(
    /* [out] */ String* rev)
{
    *rev = String("CNativeDecimalFormat") + "[\"" + GetValue(ToPattern, this) + "\"" +
        ",isDecimalSeparatorAlwaysShown=" + GetValue(IsDecimalSeparatorAlwaysShown, this) +
        ",groupingSize=" + GetValue(GetGroupingSize, this) +
        ",multiplier=" + GetValue(GetMultiplier, this) +
        ",negativePrefix=" + GetValue(GetNegativePrefix, this) +
        ",negativeSuffix=" + GetValue(GetNegativeSuffix, this) +
        ",positivePrefix=" + GetValue(GetPositivePrefix, this) +
        ",positiveSuffix=" + GetValue(GetPositiveSuffix, this) +
        ",maxIntegerDigits=" + GetValue(GetMaximumIntegerDigits, this) +
        ",maxFractionDigits=" + GetValue(GetMaximumFractionDigits, this) +
        ",minIntegerDigits=" + GetValue(GetMinimumIntegerDigits, this) +
        ",minFractionDigits=" + GetValue(GetMinimumFractionDigits, this) +
        ",grouping=" + GetValue(IsGroupingUsed, this) +
        "]";
    return NOERROR;
}

ECode CNativeDecimalFormat::TranslateFieldId(
    /* [in] */ IFieldPosition* fp,
    /* [out] */ Int32* rev)
{
    Int32 id;
    fp->GetField(&id);
    if (id < -1 || id > 1) {
        id = -1;
    }
    if (-1 == id) {
        AutoPtr<IAttributedCharacterIteratorAttribute> attr;
        fp->GetFieldAttribute((IAttributedCharacterIteratorAttribute**)&attr);
        if (attr != NULL) {
            for (UInt32 i = 0; i < ICU4C_FIELD_IDS.GetSize(); ++i) {
                Boolean is_equal = Object::Equals(
                    IAttributedCharacterIteratorAttribute::Probe(ICU4C_FIELD_IDS[i]), attr);
                if (is_equal) {
                    id = i;
                    break;
                }
            }
        }
    }
    *rev = id;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
