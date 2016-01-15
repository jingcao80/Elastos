
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
