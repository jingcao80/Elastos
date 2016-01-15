
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
class PreferenceCategory
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
    const static String TAG;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CPREFERENCECATEGORY_H__
