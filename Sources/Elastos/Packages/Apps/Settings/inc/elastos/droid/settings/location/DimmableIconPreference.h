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