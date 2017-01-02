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

#ifndef __ELASTOS_UTILITY_CURRENCY_H__
#define __ELASTOS_UTILITY_CURRENCY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"
#define HASH_FOR_UTILITY
#include <elastos/corehash.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Utility::ILocale;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Utility {

/**
 * A currency corresponding to an <a href="http://en.wikipedia.org/wiki/ISO_4217">ISO 4217</a>
 * currency code such as "EUR" or "USD".
 */
class Currency
    : public Object
    , public ICurrency
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetCurrencyCode(
        /* [out] */ String* str);

    CARAPI GetDisplayName(
        /* [out] */ String* str);

    CARAPI GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* str);

    CARAPI GetSymbol(
        /* [out] */ String* str);

    CARAPI GetSymbol(
        /* [in] */ ILocale* locale,
        /* [out] */ String* str);

    CARAPI GetDefaultFractionDigits(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI GetInstance(
        /* [in] */ const String& currencyCode,
        /* [out] */ ICurrency** instance);

    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ ICurrency** instance);

    static CARAPI GetAvailableCurrencies(
        /* [out] */ ISet** currencies);

private:
    Currency();

    CARAPI constructor(
        /* [in] */ const String& currencyCode);

    CARAPI_(AutoPtr<IInterface>) ReadResolve();

private:
    static HashMap<String, AutoPtr<ICurrency> > sCodesToCurrencies; // = new HashMap<String, Currency>();
    static HashMap<AutoPtr<ILocale>, AutoPtr<ICurrency> > sLocalesToCurrencies; // = new HashMap<Locale, Currency>();

    static Object sCodesLock;
    static Object sLocalesLock;

    String mCurrencyCode;

};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CURRENCY_H__
