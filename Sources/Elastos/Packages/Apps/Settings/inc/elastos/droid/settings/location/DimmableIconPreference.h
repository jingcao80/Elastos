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

#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_DIMMABLEICONPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_DIMMABLEICONPREFERENCE_H__

#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * A preference item that can dim the icon when it's disabled, either directly or because its parent
 * is disabled.
 */
class DimmableIconPreference
    : public Elastos::Droid::Preference::Preference
{
public:
    TO_STRING_IMPL("DimmableIconPreference")

    DimmableIconPreference();

    ~DimmableIconPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnParentChanged(
        /* [in] */ IPreference* parent,
        /* [in] */ Boolean disableChild);

    //@Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(void) DimIcon(
        /* [in] */ Boolean dimmed);

private:
    static const Int32 ICON_ALPHA_ENABLED;
    static const Int32 ICON_ALPHA_DISABLED;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_DIMMABLEICONPREFERENCE_H__