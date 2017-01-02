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

#include "CCurrencyHelper.h"
#include "Currency.h"

namespace Elastos{
namespace Utility{

CAR_SINGLETON_IMPL(CCurrencyHelper)

CAR_INTERFACE_IMPL(CCurrencyHelper, Singleton, ICurrencyHelper)

ECode CCurrencyHelper::GetInstance(
    /* [in] */ const String& currencyCode,
    /* [out] */ ICurrency** outcur)
{
    return Currency::GetInstance(currencyCode, outcur);
}

ECode CCurrencyHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICurrency** outcur)
{
    return Currency::GetInstance(locale, outcur);
}

ECode CCurrencyHelper::GetAvailableCurrencies(
    /* [out] */ ISet** currencies)
{
    return Currency::GetAvailableCurrencies(currencies);
}

} // namespace Utility
} // namespace Elastos
