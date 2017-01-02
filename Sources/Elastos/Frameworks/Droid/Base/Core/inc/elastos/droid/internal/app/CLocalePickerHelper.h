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

#ifndef __ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKERHELPER_H__

#include "_Elastos_Droid_Internal_App_CLocalePickerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CLocalePickerHelper)
    , public Singleton
    , public ILocalePickerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAllAssetLocales(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isInDeveloperMode,
        /* [out] */ IList** list);

    CARAPI ConstructAdapter(
        /* [in] */ IContext* context,
        /* [out] */ IArrayAdapter** adapter);

    CARAPI ConstructAdapter(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layoutId,
        /* [in] */ Int32 fieldId,
        /* [out] */ IArrayAdapter** adapter);

    CARAPI UpdateLocale(
        /* [in] */ ILocale* locale);
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_APP_CLOCALEPICKERHELPER_H__
