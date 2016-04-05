
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
// using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
// using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
// using Elastos::Droid::View::IInputDevice;
// using Elastos::Utility::IArrays;
// using Elastos::Utility::IHashMap;
// using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class KeyboardLayoutPickerFragment
    : public SettingsPreferenceFragment
    , public IInputDeviceListener
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle)
    {
        SettingsPreferenceFragment::OnCreate(icicle);

        assert(0 && "TODO");
        // mInputDeviceIdentifier = GetActivity()->GetIntent().GetParcelableExtra(
        //         EXTRA_INPUT_DEVICE_IDENTIFIER);
        // if (mInputDeviceIdentifier == NULL) {
        //     GetActivity()->Finish();
        // }

        // mIm = (InputManager)GetSystemService(Context.INPUT_SERVICE);
        // mKeyboardLayouts = mIm->GetKeyboardLayouts();
        // Arrays->Sort(mKeyboardLayouts);
        // SetPreferenceScreen(CreatePreferenceHierarchy());
        return NOERROR;
    }

    //@Override
    CARAPI OnResume()
    {
        SettingsPreferenceFragment::OnResume();

        assert(0 && "TODO");
        // mIm->RegisterInputDeviceListener(this, NULL);

        // InputDevice inputDevice =
        //         mIm->GetInputDeviceByDescriptor(mInputDeviceIdentifier->GetDescriptor());
        // if (inputDevice == NULL) {
        //     GetActivity()->Finish();
        //     return;
        // }
        // mInputDeviceId = inputDevice->GetId();

        // UpdateCheckedState();
        return NOERROR;
    }

    //@Override
    CARAPI OnPause()
    {
        // mIm->UnregisterInputDeviceListener(this);
        // mInputDeviceId = -1;

        SettingsPreferenceFragment::OnPause();
        assert(0 && "TODO");
        return NOERROR;
    }

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result)
    {
        VALIDATE_NOT_NULL(result);
        assert(0 && "TODO");
        // if (preference instanceof CheckBoxPreference) {
        //     CheckBoxPreference checkboxPref = (CheckBoxPreference)preference;
        //     KeyboardLayout layout = mPreferenceMap->Get(checkboxPref);
        //     if (layout != NULL) {
        //         Boolean checked = checkboxPref->IsChecked();
        //         if (checked) {
        //             mIm->AddKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
        //                     layout->GetDescriptor());
        //         }
        //         else {
        //             mIm->RemoveKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
        //                     layout->GetDescriptor());
        //         }
        //         return TRUE;
        //     }
        // }
        // return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference);
        return NOERROR;
    }

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId)
    {
        return NOERROR;
    }

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId)
    {
        assert(0 && "TODO");
        return NOERROR;
        // if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        //     UpdateCheckedState();
        // }
    }

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId)
    {
        assert(0 && "TODO");
        return NOERROR;
        // if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        //     GetActivity()->Finish();
        // }
    }

    // private PreferenceScreen CreatePreferenceHierarchy() {
    //     PreferenceScreen root = GetPreferenceManager()->CreatePreferenceScreen(GetActivity());
    //     Context context = GetActivity();

    //     for (KeyboardLayout layout : mKeyboardLayouts) {
    //         CheckBoxPreference pref = new CheckBoxPreference(context);
    //         pref->SetTitle(layout->GetLabel());
    //         pref->SetSummary(layout->GetCollection());
    //         root->AddPreference(pref);
    //         mPreferenceMap->Put(pref, layout);
    //     }
    //     return root;
    // }

    // private void UpdateCheckedState() {
    //     String[] enabledKeyboardLayouts = mIm->GetKeyboardLayoutsForInputDevice(
    //             mInputDeviceIdentifier);
    //     Arrays->Sort(enabledKeyboardLayouts);

    //     for (Map.Entry<CheckBoxPreference, KeyboardLayout> entry : mPreferenceMap->EntrySet()) {
    //         entry->GetKey()->SetChecked(Arrays->BinarySearch(enabledKeyboardLayouts,
    //                 entry->GetValue()->GetDescriptor()) >= 0);
    //     }
    // }

public:
    /**
     * Intent extra: The input device descriptor of the keyboard whose keyboard
     * layout is to be changed.
     */
    static const String EXTRA_INPUT_DEVICE_IDENTIFIER;

private:
    // private InputDeviceIdentifier mInputDeviceIdentifier;
    // private Int32 mInputDeviceId = -1;
    // private InputManager mIm;
    // private KeyboardLayout[] mKeyboardLayouts;
    // private HashMap<CheckBoxPreference, KeyboardLayout> mPreferenceMap =
    //         new HashMap<CheckBoxPreference, KeyboardLayout>();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_KEYBOARDLAYOUTPICKERFRAGMENT_H__
