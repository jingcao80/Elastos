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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCECATEGORY_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCECATEGORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceGroup.h"

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * Used to group {@link Preference} objects
 * and provide a disabled title above the group.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For information about building a settings UI with Preferences,
 * read the <a href="{@docRoot}guide/topics/ui/settings.html">Settings</a>
 * guide.</p>
 * </div>
 */
class ECO_PUBLIC PreferenceCategory
    : public PreferenceGroup
    , public IPreferenceCategory
{
public:
    CAR_INTERFACE_DECL()

    PreferenceCategory();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* isEnabled);

    //@Override
    CARAPI ShouldDisableDependents(
        /* [out] */ Boolean* should);

protected:
    CARAPI OnPrepareAddPreference(
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

private:
    ECO_LOCAL const static String TAG;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CPREFERENCECATEGORY_H__
