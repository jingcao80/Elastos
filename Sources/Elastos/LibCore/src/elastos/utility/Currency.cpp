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

#include "Currency.h"
#include "CLocale.h"
#include "ICUUtil.h"
#include "CLocaleData.h"
#include "CLinkedHashSet.h"
#include "AutoLock.h"
#include "Logger.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::CLocale;
using Elastos::Utility::Logging::Logger;
using Libcore::ICU::ICUUtil;
using Libcore::ICU::LocaleData;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::CLocaleData;

namespace Elastos {
namespace Utility {

static const String TAG("Currency");

HashMap<String, AutoPtr<ICurrency> > Currency::sCodesToCurrencies;
HashMap<AutoPtr<ILocale>, AutoPtr<ICurrency> > Currency::sLocalesToCurrencies;
Object Currency::sCodesLock;
Object Currency::sLocalesLock;

CAR_INTERFACE_IMPL_2(Currency, Object, ICurrency, ISerializable)

Currency::Currency()
{
}

ECode Currency::constructor(
    /* [in] */ const String& currencyCode)
{
    mCurrencyCode = currencyCode;
    String usstr;
    CLocale::US->ToString(&usstr);
    String symbol = ICUUtil::GetCurrencySymbol(usstr, mCurrencyCode);
    if (symbol.IsNull()) {
        Logger::E("Currency", "Unsupported ISO 4217 currency code: \"%s\"", mCurrencyCode.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Currency::GetInstance(
    /* [in] */ const String& currencyCode,
    /* [out] */ ICurrency** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;
    AutoLock lock(sCodesLock);

    AutoPtr<ICurrency> currency;
    HashMap<String, AutoPtr<ICurrency> >::Iterator it = sCodesToCurrencies.Find(currencyCode);
    if (it == sCodesToCurrencies.End()) {
        AutoPtr<Currency> c = new Currency();
        FAIL_RETURN(c->constructor(currencyCode));
        currency = (ICurrency*)c.Get();
        sCodesToCurrencies[currencyCode] = currency;
    }
    else {
        currency = it->mSecond;
    }

    *instance = currency;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode Currency::GetInstance(
    /* [in] */ ILocale* l,
    /* [out] */ ICurrency** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;
    VALIDATE_NOT_NULL(l)

    AutoLock lock(sLocalesLock);

    AutoPtr<ILocale> locale = l;
    HashMap<AutoPtr<ILocale>, AutoPtr<ICurrency> >::Iterator it = sLocalesToCurrencies.Find(locale);
    if (it != sLocalesToCurrencies.End()) {
        *instance = it->mSecond;
        REFCOUNT_ADD(*instance)
        return NOERROR;
    }

    String country;
    locale->GetCountry(&country);
    String variant;
    locale->GetVariant(&variant);
    if (!variant.IsEmpty()
        && (variant.Equals("EURO")
        || variant.Equals("HK")
        || variant.Equals("PREEURO"))) {
        country = country + String("_") + variant;
    }

    String currencyCode = ICUUtil::GetCurrencyCode(country);
    if (currencyCode.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    else if (currencyCode.Equals("XXX")) {
        return NOERROR;
    }

    AutoPtr<ICurrency> result;
    FAIL_RETURN(GetInstance(currencyCode, (ICurrency**)&result));
    sLocalesToCurrencies[locale] = result;
    *instance = result;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode Currency::GetAvailableCurrencies(
    /* [out] */ ISet** currencies)
{
    VALIDATE_NOT_NULL(currencies);
    *currencies = NULL;

    FAIL_RETURN(CLinkedHashSet::New(currencies));
    AutoPtr<ArrayOf<String> > currencyCodes;
    FAIL_RETURN(ICUUtil::GetAvailableCurrencyCodes((ArrayOf<String>**)&currencyCodes));
    Int32 length = currencyCodes->GetLength();
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<ICurrency> currency;
        FAIL_RETURN(GetInstance((*currencyCodes)[i], (ICurrency**)&currency));
        ICollection::Probe(*currencies)->Add(currency);
    }
    return NOERROR;
}

ECode Currency::GetCurrencyCode(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mCurrencyCode;
    return NOERROR;
}

ECode Currency::GetDisplayName(
    /* [out] */ String* str)
{
    return GetDisplayName(CLocale::GetDefault(), str);
}

ECode Currency::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String locstr;
    locale->ToString(&locstr);
    *str = ICUUtil::GetCurrencyDisplayName(locstr, mCurrencyCode);
    return NOERROR;
}

ECode Currency::GetSymbol(
    /* [out] */ String* str)
{
    return GetSymbol(CLocale::GetDefault(), str);
}

ECode Currency::GetSymbol(
    /* [in] */ ILocale* locale,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    VALIDATE_NOT_NULL(locale)

    String coustr;
    locale->GetCountry(&coustr);
    if (coustr.IsNullOrEmpty()) {
        *str = mCurrencyCode;
        return NOERROR;
    }

    // Check the locale first, in case the locale has the same currency.
    AutoPtr<ILocaleData> localeData = LocaleData::Get(locale);
    LocaleData* ld = (LocaleData*)localeData.Get();
    if (ld->mInternationalCurrencySymbol.Equals(mCurrencyCode)) {
        *str = ld->mCurrencySymbol;
        return NOERROR;
    }

    // Try ICU, and fall back to the currency code if ICU has nothing.
    locale->ToString(&coustr);
    String symbol = ICUUtil::GetCurrencySymbol(coustr, mCurrencyCode);
    *str = !symbol.IsNull() ? symbol : mCurrencyCode;
    return NOERROR;
}

ECode Currency::GetDefaultFractionDigits(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    // In some places the code XXX is used as the fall back currency.
    // The RI returns -1, but ICU defaults to 2 for unknown currencies.
    if (mCurrencyCode.Equals("XXX")) {
        *value = -1;
        return NOERROR;
    }
    *value = ICUUtil::GetCurrencyFractionDigits(mCurrencyCode);
    return NOERROR;
}

ECode Currency::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mCurrencyCode;
    return NOERROR;
}

AutoPtr<IInterface> Currency::ReadResolve()
{
    AutoPtr<ICurrency> currency;
    ECode ec = GetInstance(mCurrencyCode, (ICurrency**)&currency);
    return SUCCEEDED(ec) ? currency.Get() : NULL;
}

} // namespace Utility
} // namespace Elastos
