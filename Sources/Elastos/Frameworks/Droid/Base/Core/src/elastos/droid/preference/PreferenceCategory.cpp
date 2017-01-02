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

#include "elastos/droid/preference/PreferenceCategory.h"
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

const String PreferenceCategory::TAG("PreferenceCategory");

CAR_INTERFACE_IMPL(PreferenceCategory, PreferenceGroup, IPreferenceCategory)

PreferenceCategory::PreferenceCategory()
{
}

ECode PreferenceCategory::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return PreferenceGroup::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode PreferenceCategory::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PreferenceCategory::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::preferenceCategoryStyle);
}

ECode PreferenceCategory::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PreferenceCategory::IsEnabled(
    /* [out] */ Boolean* isEnable)
{
    VALIDATE_NOT_NULL(isEnable)
    *isEnable = FALSE;
    return NOERROR;
}

ECode PreferenceCategory::ShouldDisableDependents(
    /* [out] */ Boolean* should)
{
    Preference::IsEnabled(should);
    *should = !(*should);
    return NOERROR;
}

ECode PreferenceCategory::OnPrepareAddPreference(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    if (IPreferenceCategory::Probe(preference) != NULL) {
        Slogger::E(TAG, "Cannot add a %s directly to a ", TAG.string(), TAG.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return PreferenceGroup::OnPrepareAddPreference(preference, result);
}

} // Preference
} // Droid
} // Elastos
