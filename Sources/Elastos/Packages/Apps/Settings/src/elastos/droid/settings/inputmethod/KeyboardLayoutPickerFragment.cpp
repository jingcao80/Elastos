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

#include "elastos/droid/settings/inputmethod/KeyboardLayoutPickerFragment.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::CCheckBoxPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::View::IInputDevice;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String KeyboardLayoutPickerFragment::EXTRA_INPUT_DEVICE_IDENTIFIER("input_device_identifier");

//===============================================================================
//                  KeyboardLayoutPickerFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(KeyboardLayoutPickerFragment::InnerListener, Object, IInputDeviceListener);

KeyboardLayoutPickerFragment::InnerListener::InnerListener(
    /* [in] */ KeyboardLayoutPickerFragment* host)
    : mHost(host)
{}

ECode KeyboardLayoutPickerFragment::InnerListener::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceAdded(deviceId);
}

ECode KeyboardLayoutPickerFragment::InnerListener::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceChanged(deviceId);
}

ECode KeyboardLayoutPickerFragment::InnerListener::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceRemoved(deviceId);
}

//===============================================================================
//                  KeyboardLayoutPickerFragment
//===============================================================================

KeyboardLayoutPickerFragment::KeyboardLayoutPickerFragment()
    : mInputDeviceId(-1)
{
    CHashMap::New((IHashMap**)&mPreferenceMap);
    mListener = new InnerListener(this);
}

KeyboardLayoutPickerFragment::~KeyboardLayoutPickerFragment()
{}

ECode KeyboardLayoutPickerFragment::constructor()
{
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(EXTRA_INPUT_DEVICE_IDENTIFIER, (IParcelable**)&parcelable);
    mInputDeviceIdentifier = IInputDeviceIdentifier::Probe(parcelable);
    if (mInputDeviceIdentifier == NULL) {
        activity->Finish();
    }

    mIm = IInputManager::Probe(GetSystemService(IContext::INPUT_SERVICE));
    mIm->GetKeyboardLayouts((ArrayOf<IKeyboardLayout*>**)&mKeyboardLayouts);
    Arrays::Sort(mKeyboardLayouts.Get());
    SetPreferenceScreen(CreatePreferenceHierarchy());
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    mIm->RegisterInputDeviceListener(mListener, NULL);

    String str;
    mInputDeviceIdentifier->GetDescriptor(&str);

    AutoPtr<IInputDevice> inputDevice;
    mIm->GetInputDeviceByDescriptor(str, (IInputDevice**)&inputDevice);
    if (inputDevice == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        activity->Finish();
        return NOERROR;
    }
    inputDevice->GetId(&mInputDeviceId);

    UpdateCheckedState();
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnPause()
{
    mIm->UnregisterInputDeviceListener(mListener);
    mInputDeviceId = -1;

    SettingsPreferenceFragment::OnPause();
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICheckBoxPreference> checkboxPref = ICheckBoxPreference::Probe(preference);
    if (checkboxPref != NULL) {
        AutoPtr<IInterface> obj;
        mPreferenceMap->Get(checkboxPref, (IInterface**)&obj);
        IKeyboardLayout* layout = IKeyboardLayout::Probe(obj);
        if (layout != NULL) {
            String descriptor;
            layout->GetDescriptor(&descriptor);
            Boolean checked;
            ITwoStatePreference::Probe(checkboxPref)->IsChecked(&checked);
            if (checked) {
                mIm->AddKeyboardLayoutForInputDevice(mInputDeviceIdentifier, descriptor);
            }
            else {
                mIm->RemoveKeyboardLayoutForInputDevice(mInputDeviceIdentifier, descriptor);
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
}

ECode KeyboardLayoutPickerFragment::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        UpdateCheckedState();
    }
    return NOERROR;
}

ECode KeyboardLayoutPickerFragment::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        activity->Finish();
    }
    return NOERROR;
}

AutoPtr<IPreferenceScreen> KeyboardLayoutPickerFragment::CreatePreferenceHierarchy()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IPreferenceManager> pm;
    GetPreferenceManager((IPreferenceManager**)&pm);
    AutoPtr<IPreferenceScreen> root;
    pm->CreatePreferenceScreen(context, (IPreferenceScreen**)&root);

    for (Int32 i = 0; i < mKeyboardLayouts->GetLength(); ++i) {
        AutoPtr<IKeyboardLayout> layout = (*mKeyboardLayouts)[i];
        AutoPtr<ICheckBoxPreference> pref;
        CCheckBoxPreference::New(context, (ICheckBoxPreference**)&pref);
        String label, collection;
        layout->GetLabel(&label);
        layout->GetCollection(&collection);
        IPreference* _pref = IPreference::Probe(pref);
        _pref->SetTitle(CoreUtils::Convert(label));
        _pref->SetSummary(CoreUtils::Convert(collection));
        Boolean res;
        IPreferenceGroup::Probe(root)->AddPreference(_pref, &res);
        mPreferenceMap->Put(pref, layout);
    }
    return root;
}

void KeyboardLayoutPickerFragment::UpdateCheckedState()
{
    AutoPtr< ArrayOf<String> > enabledKeyboardLayouts;
    mIm->GetKeyboardLayoutsForInputDevice(
            mInputDeviceIdentifier, (ArrayOf<String>**)&enabledKeyboardLayouts);
    assert(0 && "TODO");
    // Arrays::Sort(enabledKeyboardLayouts);

    AutoPtr<ISet> set;
    mPreferenceMap->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNxt = FALSE;
    while (it->HasNext(&hasNxt), hasNxt) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IMapEntry* entry = IMapEntry::Probe(obj);

        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        ICheckBoxPreference* cbp = ICheckBoxPreference::Probe(key);

        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        IKeyboardLayout* kl = IKeyboardLayout::Probe(value);
        String descriptor;
        kl->GetDescriptor(&descriptor);
        Int32 result;
        Arrays::BinarySearch(enabledKeyboardLayouts, descriptor, &result);
        ITwoStatePreference::Probe(cbp)->SetChecked(result >= 0);
    }
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos