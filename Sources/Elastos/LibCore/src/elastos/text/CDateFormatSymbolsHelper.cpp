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

#include "CDateFormatSymbolsHelper.h"
#include "DateFormatSymbols.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CDateFormatSymbolsHelper, Singleton, IDateFormatSymbolsHelper)

CAR_SINGLETON_IMPL(CDateFormatSymbolsHelper)

ECode CDateFormatSymbolsHelper::GetInstance(
    /* [out] */ IDateFormatSymbols** instance)
{
    return DateFormatSymbols::GetInstance(instance);
}

ECode CDateFormatSymbolsHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormatSymbols** instance)
{
    return DateFormatSymbols::GetInstance(locale,instance);
}

ECode CDateFormatSymbolsHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** arrayOfLocales)
{
    return DateFormatSymbols::GetAvailableLocales(arrayOfLocales);
}

} // namespace Text
} // namespace Elastos