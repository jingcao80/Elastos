#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__

#include "_Elastos_Droid_Settings_Location_CRadioButtonPreference.h"
#include "elastos/droid/preference/CheckBoxPreference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * Check box preference with check box replaced by radio button.
 *
 * Functionally speaking, it's actually a CheckBoxPreference. We only modified
 * the widget to RadioButton to make it "look like" a RadioButtonPreference.
 *
 * In other words, there's no "RadioButtonPreferenceGroup" in this
 * implementation. When you check one RadioButtonPreference, if you want to
 * uncheck all the other preferences, you should do that by code yourself.
 */
CarClass(CRadioButtonPreference)
    , public Elastos::Droid::Preference::CheckBoxPreference
    , public IRadioButtonPreference
{
public:
    TO_STRING_IMPL("CRadioButtonPreference")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRadioButtonPreference();

    ~CRadioButtonPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI SetOnClickListener(
        /* [in] */ IRadioButtonPreferenceOnClickListener* listener);

    //@Override
    CARAPI OnClick();

    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<IRadioButtonPreferenceOnClickListener> mListener;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__