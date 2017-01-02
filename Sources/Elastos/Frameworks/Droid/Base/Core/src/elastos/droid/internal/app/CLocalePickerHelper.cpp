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

#include "elastos/droid/internal/app/CLocalePickerHelper.h"
#include "elastos/droid/internal/app/LocalePicker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(CLocalePickerHelper, Singleton, ILocalePickerHelper)
CAR_SINGLETON_IMPL(CLocalePickerHelper)

ECode CLocalePickerHelper::GetAllAssetLocales(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isInDeveloperMode,
    /* [out] */ IList** list)
{
    return LocalePicker::GetAllAssetLocales(context, isInDeveloperMode, list);
}

ECode CLocalePickerHelper::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [out] */ IArrayAdapter** adapter)
{
    return LocalePicker::ConstructAdapter(context, adapter);
}

ECode CLocalePickerHelper::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutId,
    /* [in] */ Int32 fieldId,
    /* [out] */ IArrayAdapter** adapter)
{
    return LocalePicker::ConstructAdapter(context, layoutId, fieldId, adapter);
}

ECode CLocalePickerHelper::UpdateLocale(
    /* [in] */ ILocale* locale)
{
    return LocalePicker::UpdateLocale(locale);
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
