
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class KeyboardLayoutPickerFragment
    : public SettingsPreferenceFragment
{
public:
    class InnerListener
        : public Object
        , public IInputDeviceListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("KeyboardLayoutPickerFragment::InnerListener")

        InnerListener(
            /* [in] */ KeyboardLayoutPickerFragment* host);

        //@Override
        CARAPI OnInputDeviceAdded(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceChanged(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceRemoved(
            /* [in] */ Int32 deviceId);

    private:
        KeyboardLayoutPickerFragment* mHost;
    };

public:
    KeyboardLayoutPickerFragment();

    ~KeyboardLayoutPickerFragment();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

private:
    CARAPI_(AutoPtr<IPreferenceScreen>) CreatePreferenceHierarchy();

    CARAPI_(void) UpdateCheckedState();

public:
    /**
     * Intent extra: The input device descriptor of the keyboard whose keyboard
     * layout is to be changed.
     */
    static const String EXTRA_INPUT_DEVICE_IDENTIFIER;

private:
    AutoPtr<IInputDeviceIdentifier> mInputDeviceIdentifier;
    Int32 mInputDeviceId;
    AutoPtr<IInputManager> mIm;
    AutoPtr< ArrayOf<IKeyboardLayout*> > mKeyboardLayouts;
    // HashMap<CheckBoxPreference, KeyboardLayout> mPreferenceMap =
            // new HashMap<CheckBoxPreference, KeyboardLayout>();
    AutoPtr<IHashMap> mPreferenceMap;

    AutoPtr<InnerListener> mListener;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__
