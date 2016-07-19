#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONSETTINGS_H__

#include "_Elastos_Droid_Settings_CApplicationSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CApplicationSettings)
    , public SettingsPreferenceFragment
{
private:
    class OnCreateOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnCreateOnPreferenceChangeListener(
            /* [in] */ CApplicationSettings* host);

        ~OnCreateOnPreferenceChangeListener();

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CApplicationSettings* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CApplicationSettings();

    ~CApplicationSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* res);

protected:
    virtual CARAPI_(void) HandleUpdateAppInstallLocation(
        /* [in] */ const String& value);

private:
    CARAPI_(Boolean) IsAdvancedSettingsEnabled();

    CARAPI_(void) SetAdvancedSettingsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(String) GetAppInstallLocation();

private:
    static const String KEY_TOGGLE_ADVANCED_SETTINGS;
    static const String KEY_APP_INSTALL_LOCATION;

    // App installation location. Default is ask the user.
    static const Int32 APP_INSTALL_AUTO;
    static const Int32 APP_INSTALL_DEVICE;
    static const Int32 APP_INSTALL_SDCARD;

    static const String APP_INSTALL_DEVICE_ID;
    static const String APP_INSTALL_SDCARD_ID;
    static const String APP_INSTALL_AUTO_ID;

    AutoPtr<ICheckBoxPreference> mToggleAdvancedSettings;
    AutoPtr<IListPreference> mInstallLocation;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONSETTINGS_H__