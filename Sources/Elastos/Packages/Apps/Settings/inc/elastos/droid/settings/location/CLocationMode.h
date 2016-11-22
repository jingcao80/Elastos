#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONMODE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONMODE_H__

#include "_Elastos_Droid_Settings_Location_CLocationMode.h"
#include "elastos/droid/settings/location/LocationSettingsBase.h"

using Elastos::Droid::Preference::IPreferenceScreen;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * A page with 3 radio buttons to choose the location mode.
 *
 * There are 3 location modes when location access is enabled:
 *
 * High accuracy: use both GPS and network location.
 *
 * Battery saving: use network location only to reduce the power consumption.
 *
 * Sensors only: use GPS location only.
 */
CarClass(CLocationMode)
    , public LocationSettingsBase
{
private:
    class InnerListener
        : public Object
        , public IRadioButtonPreferenceOnClickListener
    {
    public:
        TO_STRING_IMPL("CLocationMode::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CLocationMode* host);

        //@Override
        CARAPI OnRadioButtonClicked(
            /* [in] */ IRadioButtonPreference* emiter);

    private:
        CLocationMode* mHost;
    };

public:
    TO_STRING_IMPL("CLocationMode")

    CAR_OBJECT_DECL()

    CLocationMode();

    ~CLocationMode();

    CARAPI constructor();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnRadioButtonClicked(
        /* [in] */ IRadioButtonPreference* emiter);

    //@Override
    CARAPI OnModeChanged(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean restricted);

    //@Override
    CARAPI_(Int32) GetHelpResource();

private:
    CARAPI_(AutoPtr<IPreferenceScreen>) CreatePreferenceHierarchy();

    CARAPI_(void) UpdateRadioButtons(
        /* [in] */ IRadioButtonPreference* activated);

private:
    static const String KEY_HIGH_ACCURACY;
    static const String KEY_BATTERY_SAVING;
    static const String KEY_SENSORS_ONLY;
    AutoPtr<IRadioButtonPreference> mHighAccuracy;
    AutoPtr<IRadioButtonPreference> mBatterySaving;
    AutoPtr<IRadioButtonPreference> mSensorsOnly;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONMODE_H__