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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CStorageItemPreference.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CarClass(CStorageItemPreference)
    , public Elastos::Droid::Preference::Preference
{
public:
    TO_STRING_IMPL("CStorageItemPreference")

    CAR_OBJECT_DECL()

    CStorageItemPreference();

    ~CStorageItemPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 colorRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 colorRes,
        /* [in] */ Int32 userHandle);

private:
    static CARAPI_(AutoPtr<IShapeDrawable>) CreateRectShape(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 color);
public:
    Int32 mColor;
    Int32 mUserHandle;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__