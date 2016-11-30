#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSBSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSBSETTINGS_H__

#include "Elastos.Droid.Hardware.h"
#include "_Elastos_Droid_Settings_Deviceinfo_CUsbSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * USB storage settings.
 */
CarClass(CUsbSettings)
    , public SettingsPreferenceFragment
{
private:
    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CUsbSettings::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ CUsbSettings* host);

        CARAPI OnReceive(
            /* [in] */ IContext* content,
            /* [in] */ IIntent* intent);

    private:
        CUsbSettings* mHost;
    };

public:
    TO_STRING_IMPL("CUsbSettings")

    CUsbSettings();

    ~CUsbSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

private:
    CARAPI_(AutoPtr<IPreferenceScreen>) CreatePreferenceHierarchy();

    CARAPI_(void) UpdateToggles(
        /* [in] */ const String& function);

private:
    static const String TAG;

    static const String KEY_MTP;
    static const String KEY_PTP;

    AutoPtr<IUsbManager> mUsbManager;
    AutoPtr<ICheckBoxPreference> mMtp;
    AutoPtr<ICheckBoxPreference> mPtp;
    Boolean mUsbAccessoryMode;

    AutoPtr<InitBroadcastReceiver> mStateReceiver;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSBSETTINGS_H__