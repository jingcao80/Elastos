
#include "DecimalFormatSymbols.h"
#include "CDecimalFormatSymbols.h"
#include "CLocale.h"
#include "StringBuilder.h"
#include "CLocaleDataHelper.h"
#include "ICUUtil.h"
#include "Currency.h"
#include "LocaleData.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Libcore::ICU::ICUUtil;
using Elastos::Utility::CLocale;
using Elastos::Utility::Currency;
using Libcore::ICU::LocaleData;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_2(DecimalFormatSymbols, Object, IDecimalFormatSymbols, ICloneable)

ECode DecimalFormatSymbols::constructor()
{
    AutoPtr<ILocale> locale = CLocale::GetDefault();
    return constructor(locale);
}

ECode DecimalFormatSymbols::constructor(
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<ILocaleData> localeData;
    FAIL_RETURN(helper->Get(locale, (ILocaleData**)&localeData));
    localeData->GetZeroDigit(&mZeroDigit);
    mDigit = '#';
    localeData->GetDecimalSeparator(&mDecimalSeparator);
    localeData->GetGroupingSeparator(&mGroupingSeparator);
    localeData->GetPatternSeparator(&mPatternSeparator);
    localeData->GetPercent(&mPercent);
    localeData->GetPerMill(&mPerMill);
    localeData->GetMonetarySeparator(&mMonetarySeparator);
    localeData->GetMinusSign(&mMinusSign);
    localeData->GetInfinity(&mInfinity);
    localeData->GetNaN(&mNaN);
    localeData->GetExponentSeparator(&mExponentSeparator);
    mLocale = LocaleData::MapInvalidAndNullLocales(locale);
    ECode ec = Currency::GetInstance(locale, (ICurrency**)&mCurrency);
    if (SUCCEEDED(ec)) {
        mCurrency->GetSymbol(locale, &mCurrencySymbol);
        mCurrency->GetCurrencyCode(&mIntlCurrencySymbol);
    }
    else {
        FAIL_RETURN(Currency::GetInstance(String("XXX"), (ICurrency**)&mCurrency));
        localeData->GetCurrencySymbol(&mCurrencySymbol);
        localeData->GetInternationalCurrencySymbol(&mIntlCurrencySymbol);
    }
    return NOERROR;
}

ECode DecimalFormatSymbols::GetInstance(
    /* [out] */ IDecimalFormatSymbols** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<ILocale> locale = CLocale::GetDefault();
    return GetInstance(locale, instance);
}

ECode DecimalFormatSymbols::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IDecimalFormatSymbols** instance)
{
    VALIDATE_NOT_NULL(instance);

    if (locale == NULL) {
        //throw new NullPointerException("locale == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return CDecimalFormatSymbols::New(locale, instance);
}

ECode DecimalFormatSymbols::GetAvailableLocales(
    /* [out] */ ArrayOf<ILocale*>** locales)
{
    return ICUUtil::GetAvailableDecimalFormatSymbolsLocales(locales);
}

ECode DecimalFormatSymbols::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IDecimalFormatSymbols> outdec ;
    FAIL_RETURN(CDecimalFormatSymbols::New(mLocale, (IDecimalFormatSymbols **)&outdec));

    CloneImpl(outdec);

    *object = outdec;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode DecimalFormatSymbols::CloneImpl(
    /* [in] */ IDecimalFormatSymbols* object)
{
    assert(object);

    DecimalFormatSymbols* other = (DecimalFormatSymbols*)object;
    other->mZeroDigit = mZeroDigit;
    other->mDigit = mDigit;
    other->mDecimalSeparator = mDecimalSeparator;
    other->mGroupingSeparator = mGroupingSeparator;
    other->mPatternSeparator = mPatternSeparator;
    other->mPercent = mPercent;
    other->mPerMill = mPerMill;
    other->mMonetarySeparator = mMonetarySeparator;
    other->mMinusSign = mMinusSign;
    other->mInfinity = mInfinity;
    other->mNaN = mNaN;
    other->mCurrencySymbol = mCurrencySymbol;
    other->mIntlCurrencySymbol = mIntlCurrencySymbol;

    other->mCurrency = mCurrency;
    other->mLocale = mLocale;
    other->mExponentSeparator = mExponentSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    IDecimalFormatSymbols* other = IDecimalFormatSymbols::Probe(object);
    if (other == NULL) {
        return NOERROR;
    }

    DecimalFormatSymbols* df = (DecimalFormatSymbols*)other;
    if (df == this) {
        *res = TRUE;
        return NOERROR;
    }

    *res = Object::Equals(mCurrency, df->mCurrency) &&
            mCurrencySymbol.Equals(df->mCurrencySymbol) &&
            mDecimalSeparator == df->mDecimalSeparator &&
            mDigit == df->mDigit &&
            mExponentSeparator.Equals(df->mExponentSeparator) &&
            mGroupingSeparator == df->mGroupingSeparator &&
            mInfinity.Equals(df->mInfinity) &&
            mIntlCurrencySymbol.Equals(df->mIntlCurrencySymbol) &&
            mMinusSign.Equals(df->mMinusSign) &&
            mMonetarySeparator == df->mMonetarySeparator &&
            mNaN.Equals(df->mNaN) &&
            mPatternSeparator == df->mPatternSeparator &&
            mPerMill == df->mPerMill &&
            mPercent == df->mPercent &&
            mZeroDigit == df->mZeroDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("DecimalFormatSymbols[currency=");
    sb.Append(Object::ToString(mCurrency));
    sb.Append(",currencySymbol=");
    sb.Append(mCurrencySymbol);

    sb.Append(",decimalSeparator=");
    sb.AppendChar(mDecimalSeparator);
    sb.Append(",digit=");
    sb.AppendChar(mDigit);
    sb.Append(",exponentSeparator=");
    sb.Append(mExponentSeparator);
    sb.Append(",groupingSeparator=");
    sb.AppendChar(mGroupingSeparator);
    sb.Append(",infinity=");
    sb.Append(mInfinity);
    sb.Append(",intlCurrencySymbol=");
    sb.Append(mIntlCurrencySymbol);
    sb.Append(",minusSign=");
    sb.Append(mMinusSign);
    sb.Append(",monetarySeparator=");
    sb.AppendChar(mMonetarySeparator);
    sb.Append(",NaN=");
    sb.Append(mNaN);
    sb.Append(",patternSeparator=");
    sb.AppendChar(mPatternSeparator);
    sb.Append(",perMill=");
    sb.AppendChar(mPerMill);
    sb.Append(",percent=");
    sb.AppendChar(mPercent);
    sb.Append(",zeroDigit=");
    sb.AppendChar(mZeroDigit);
    sb.Append("]");

    *str = sb.ToString();
    return NOERROR;
}

ECode DecimalFormatSymbols::GetCurrency(
    /* [out] */ ICurrency** currency)
{
    VALIDATE_NOT_NULL(currency)
    *currency = mCurrency;
    REFCOUNT_ADD(*currency);
    return NOERROR;
}

ECode DecimalFormatSymbols::GetInternationalCurrencySymbol(
    /* [out] */ String* symbol)
{
    VALIDATE_NOT_NULL(symbol)
    *symbol = mIntlCurrencySymbol;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetCurrencySymbol(
    /* [out] */ String* symbol)
{
    VALIDATE_NOT_NULL(symbol)
    *symbol = mCurrencySymbol;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetDecimalSeparator(
    /* [out] */ Char32* separator)
{
    VALIDATE_NOT_NULL(separator)
    *separator = mDecimalSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetDigit(
    /* [out] */ Char32* digit)
{
    VALIDATE_NOT_NULL(digit)
    *digit = mDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetGroupingSeparator(
    /* [out] */ Char32* separator)
{
    VALIDATE_NOT_NULL(separator)
    *separator = mGroupingSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetInfinity(
    /* [out] */ String* infinity)
{
    VALIDATE_NOT_NULL(infinity)
    *infinity = mInfinity;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetMinusSign(
    /* [out] */ Char32* minusSign)
{
    VALIDATE_NOT_NULL(minusSign)

    if (mMinusSign.GetLength() == 1) {
        *minusSign  = mMinusSign.GetChar(0);
        return NOERROR;
    }

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
    // throw new UnsupportedOperationException(
    //         "Minus sign spans multiple characters: " + minusSign);
}

ECode DecimalFormatSymbols::GetMonetaryDecimalSeparator(
    /* [out] */ Char32* separator)
{
    VALIDATE_NOT_NULL(separator)
    *separator = mMonetarySeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetNaN(
    /* [out] */ String* infinity)
{
    VALIDATE_NOT_NULL(infinity)
    *infinity = mNaN;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPatternSeparator(
    /* [out] */ Char32* separator)
{
    VALIDATE_NOT_NULL(separator)
    *separator = mPatternSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPercent(
    /* [out] */ Char32* percent)
{
    VALIDATE_NOT_NULL(percent)
    *percent = mPercent;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetPerMill(
    /* [out] */ Char32* perMill)
{
    VALIDATE_NOT_NULL(perMill)
    *perMill = mPerMill;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetZeroDigit(
    /* [out] */ Char32* zeroDigit)
{
    VALIDATE_NOT_NULL(zeroDigit)
    *zeroDigit = mZeroDigit;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetExponentSeparator(
    /* [out] */ String* separator)
{
    VALIDATE_NOT_NULL(separator)
    *separator = mExponentSeparator;
    return NOERROR;
}

ECode DecimalFormatSymbols::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    int result = 17;
    result = 31*result + mZeroDigit;
    result = 31*result + mDigit;
    result = 31*result + mDecimalSeparator;
    result = 31*result + mGroupingSeparator;
    result = 31*result + mPatternSeparator;
    result = 31*result + mPercent;
    result = 31*result + mPerMill;
    result = 31*result + mMonetarySeparator;
    result = 31*result + mMinusSign.GetHashCode();
    result = 31*result + mExponentSeparator.GetHashCode();
    result = 31*result + mInfinity.GetHashCode();
    result = 31*result + mNaN.GetHashCode();
    result = 31*result + mCurrencySymbol.GetHashCode();
    result = 31*result + mIntlCurrencySymbol.GetHashCode();
    *hash = result;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetCurrency(
    /* [in] */ ICurrency* currency)
{
    if (currency == NULL) {
        //throw new NullPointerException("currency == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (currency == mCurrency) {
        return NOERROR;
    }
    mCurrency = currency;
    currency->GetCurrencyCode(&mIntlCurrencySymbol);
    currency->GetSymbol(mLocale, &mCurrencySymbol);
    return NOERROR;
}

ECode DecimalFormatSymbols::SetInternationalCurrencySymbol(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        mCurrency = NULL;
        mIntlCurrencySymbol = NULL;
        return NOERROR;
    }

    if (value.Equals(mIntlCurrencySymbol)) {
        return NOERROR;
    }

    mCurrency = NULL;
    FAIL_RETURN(Currency::GetInstance(value, (ICurrency**)&mCurrency));
    ECode ec = mCurrency->GetSymbol(mLocale, &mCurrencySymbol);
    if (FAILED(ec)) {
        mCurrency = NULL;
    }
    mIntlCurrencySymbol = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetCurrencySymbol(
    /* [in] */ const String& value)
{
    mCurrencySymbol = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetDecimalSeparator(
    /* [in] */ Char32 value)
{
    mDecimalSeparator = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetDigit(
    /* [in] */ Char32 value)
{
    mDigit = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetGroupingSeparator(
    /* [in] */ Char32 value)
{
    mGroupingSeparator = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetInfinity(
    /* [in] */ const String& value)
{
    mInfinity = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetMinusSign(
    /* [in] */ Char32 value)
{
    StringBuilder sb;
    sb.AppendChar(value);
    mMinusSign = sb.ToString();
    return NOERROR;
}

ECode DecimalFormatSymbols::SetMonetaryDecimalSeparator(
    /* [in] */ Char32 value)
{
    mMonetarySeparator = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetNaN(
    /* [in] */ const String& value)
{
    mNaN = value;
    return NOERROR;
}
ECode DecimalFormatSymbols::SetPatternSeparator(
    /* [in] */ Char32 value)
{
    mPatternSeparator = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetPercent(
    /* [in] */ Char32 value)
{
    mPercent = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetPerMill(
    /* [in] */ Char32 value)
{
    mPerMill = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetZeroDigit(
    /* [in] */ Char32 value)
{
    mZeroDigit = value;
    return NOERROR;
}

ECode DecimalFormatSymbols::SetExponentSeparator(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        //throw new NullPointerException("value == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mExponentSeparator = value;
    return NOERROR;
}


ECode DecimalFormatSymbols::GetMinusSignString(
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = mMinusSign;
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
