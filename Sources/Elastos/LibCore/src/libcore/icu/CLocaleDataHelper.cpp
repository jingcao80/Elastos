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

#include "Elastos.CoreLibrary.Utility.h"
#include "CLocaleDataHelper.h"
#include "LocaleData.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CLocaleDataHelper)

CAR_INTERFACE_IMPL(CLocaleDataHelper, Singleton, ILocaleDataHelper)

ECode CLocaleDataHelper::MapInvalidAndNullLocales(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocale** rst)
{
    VALIDATE_NOT_NULL(rst);
    AutoPtr<ILocale> localeObj = LocaleData::MapInvalidAndNullLocales(locale);
    *rst = localeObj;
    REFCOUNT_ADD(*rst);

    return NOERROR;
}

ECode CLocaleDataHelper::Get(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocaleData** localeData)
{
    VALIDATE_NOT_NULL(localeData);
    if (NULL == locale)
        return E_NULL_POINTER_EXCEPTION;

    AutoPtr<ILocaleData> temp = LocaleData::Get(locale);
    *localeData = temp;
    REFCOUNT_ADD(*localeData);

    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
