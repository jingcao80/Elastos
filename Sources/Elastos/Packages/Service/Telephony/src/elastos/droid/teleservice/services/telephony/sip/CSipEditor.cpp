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

#include "elastos/droid/teleservice/services/telephony/sip/CSipEditor.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/utility/Arrays.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {


CAR_INTERFACE_IMPL(CSipEditor::InnerListener, Object, IPreferenceOnPreferenceChangeListener)

CSipEditor::InnerListener::InnerListener(
    /* [in] */ CSipEditor* host)
    : mHost(host)
{}

ECode CSipEditor::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* pref,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(pref, newValue, result);
}

CAR_INTERFACE_IMPL(CSipEditor::AdvancedSettings::InnerListener, Object, IPreferenceOnPreferenceClickListener)


CSipEditor::AdvancedSettings::InnerListener::InnerListener(
    /* [in] */ AdvancedSettings* host)
    : mHost(host)
{}

ECode CSipEditor::AdvancedSettings::InnerListener::OnPreferenceClick(
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceClick(pref, result);
}

CSipEditor::AdvancedSettings::AdvancedSettings(
    /* [in] */ CSipEditor* host)
    : mHost(host)
    , mShowing(FALSE)
{
    AutoPtr<IPreferenceScreen> screen;
    mHost->GetPreferenceScreen((IPreferenceScreen**)&screen);

    String str;
    mHost->GetString(R::string::advanced_settings, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    IPreferenceGroup::Probe(screen)->FindPreference(cchar, (IPreference**)&mAdvancedSettingsTrigger);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mAdvancedSettingsTrigger->SetOnPreferenceClickListener(listener);

    LoadAdvancedPreferences();
}

ECode CSipEditor::AdvancedSettings::Show()
{
    mShowing = TRUE;
    mAdvancedSettingsTrigger->SetSummary(R::string::advanced_settings_hide);

    AutoPtr<IPreferenceScreen> _screen;
    mHost->GetPreferenceScreen((IPreferenceScreen**)&_screen);
    AutoPtr<IPreferenceGroup> screen = IPreferenceGroup::Probe(_screen);

    for (Int32 i = 0; i < mPreferences->GetLength(); i++) {
        AutoPtr<IPreference> pref = (*mPreferences)[i];

        Boolean res;
        screen->AddPreference(pref, &res);
        if (VERBOSE) {
            StringBuilder sb;
            sb += "AdvancedSettings.show, pref: ";
            String key;
            pref->GetKey(&key);
            sb += key;
            sb += ", order: ";
            Int32 order;
            pref->GetOrder(&order);
            sb += order;
            Log(sb.ToString());
        }
    }
    return NOERROR;
}

ECode CSipEditor::AdvancedSettings::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (VERBOSE) Log(String("AdvancedSettings.onPreferenceClick"));
    if (!mShowing) {
        Show();
    }
    else {
        Hide();
    }
    *result = TRUE;
    return NOERROR;
}

void CSipEditor::AdvancedSettings::LoadAdvancedPreferences()
{
    AutoPtr<IPreferenceScreen> _screen;
    mHost->GetPreferenceScreen((IPreferenceScreen**)&_screen);
    AutoPtr<IPreferenceGroup> screen = IPreferenceGroup::Probe(_screen);

    mHost->AddPreferencesFromResource(R::xml::sip_advanced_edit);

    String str;
    mHost->GetString(R::string::advanced_settings_container, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(screen)->FindPreference(cchar, (IPreference**)&preference);
    AutoPtr<IPreferenceGroup> group = IPreferenceGroup::Probe(preference);
    Boolean res;
    screen->RemovePreference(IPreference::Probe(group), &res);

    Int32 count;
    group->GetPreferenceCount(&count);
    mPreferences = ArrayOf<IPreference*>::Alloc(count);
    Int32 order;
    screen->GetPreferenceCount(&order);
    for (Int32 i = 0, n = mPreferences->GetLength(); i < n; i++) {
        AutoPtr<IPreference> pref;
        group->GetPreference(i, (IPreference**)&pref);
        pref->SetOrder(order++);
        mHost->SetupPreference(pref);
        mPreferences->Set(i, pref);
    }
}

void CSipEditor::AdvancedSettings::Hide()
{
    mShowing = FALSE;
    mAdvancedSettingsTrigger->SetSummary(R::string::advanced_settings_show);

    AutoPtr<IPreferenceScreen> _screen;
    mHost->GetPreferenceScreen((IPreferenceScreen**)&_screen);
    AutoPtr<IPreferenceGroup> screen = IPreferenceGroup::Probe(_screen);

    for (Int32 i = 0; i < mPreferences->GetLength(); i++) {
        AutoPtr<IPreference> pref = (*mPreferences)[i];

        Boolean res;
        screen->RemovePreference(pref, &res);

    }
}

ECode CSipEditor::PreferenceKey::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (IEditTextPreference::Probe(mPreference) != NULL) {
        return IEditTextPreference::Probe(mPreference)->GetText(value);
    }
    else if (IListPreference::Probe(mPreference) != NULL) {
        return IListPreference::Probe(mPreference)->GetValue(value);
    }
    //throw new RuntimeException("getValue() for the preference " + this);
    return E_RUNTIME_EXCEPTION;
}

void CSipEditor::PreferenceKey::SetValue(
    /* [in] */ const String& value)
{
    if (IEditTextPreference::Probe(mPreference) != NULL) {
        String oldValue;
        GetValue(&oldValue);
        IEditTextPreference::Probe(mPreference)->SetText(value);

        assert(0);
        // if (this != Password) {
        //     if (VERBOSE) {
        //         log(this + ": setValue() " + value + ": " + oldValue + " --> " +
        //                 getValue());
        //     }
        // }
    }
    else if (IListPreference::Probe(mPreference) != NULL) {
        IListPreference::Probe(mPreference)->SetValue(value);
    }

    assert(0);
    // if (TextUtils::IsEmpty(value)) {
    //     preference->SetSummary(defaultSummary);
    // }
    // else if (this == Password) {
    //     preference->SetSummary(scramble(value));
    // }
    // else if ((this == DisplayName)
    //         && value.equals(getDefaultDisplayName())) {
    //     preference->SetSummary(defaultSummary);
    // }
    // else {
    //     preference->SetSummary(value);
    // }
}

ECode CSipEditor::MyRunnable::Run()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);
    builder->SetIconAttribute(Elastos::Droid::R::attr::alertDialogIcon);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mMessage);
    builder->SetMessage(cchar);
    builder->SetPositiveButton(R::string::alert_dialog_ok, NULL);
    AutoPtr<IAlertDialog> dialog;
    return builder->Show((IAlertDialog**)&dialog);
}

const String CSipEditor::PREFIX("[SipEditor] ");
const Boolean CSipEditor::VERBOSE = FALSE; /* STOP SHIP if true */

const Int32 CSipEditor::MENU_SAVE = IMenu::FIRST;
const Int32 CSipEditor::MENU_DISCARD = IMenu::FIRST + 1;
const Int32 CSipEditor::MENU_REMOVE = IMenu::FIRST + 2;

const String CSipEditor::KEY_PROFILE("profile");
const String CSipEditor::GET_METHOD_PREFIX("get");
const Char32 CSipEditor::SCRAMBLED = '*';
const Int32 CSipEditor::NA = 0;

CAR_OBJECT_IMPL(CSipEditor)

CAR_INTERFACE_IMPL(CSipEditor, PreferenceActivity, ISipEditor)

CSipEditor::CSipEditor()
{
    mUsername = new CSipEditor::PreferenceKey(R::string::username, 0, R::string::default_preference_summary),
    mPassword = new CSipEditor::PreferenceKey(R::string::password, 0, R::string::default_preference_summary),
    mDomainAddress = new CSipEditor::PreferenceKey(R::string::domain_address, 0, R::string::default_preference_summary),
    mDisplayName = new CSipEditor::PreferenceKey(R::string::display_name, 0, R::string::display_name_summary),
    mProxyAddress = new CSipEditor::PreferenceKey(R::string::proxy_address, 0, R::string::optional_summary),
    mPort = new CSipEditor::PreferenceKey(R::string::port, R::string::default_port, R::string::default_port),
    mTransport = new CSipEditor::PreferenceKey(R::string::transport, R::string::default_transport, NA),
    mSendKeepAlive = new CSipEditor::PreferenceKey(R::string::send_keepalive, R::string::sip_system_decide, NA),
    mAuthUserName = new CSipEditor::PreferenceKey(R::string::auth_username, 0, R::string::optional_summary);
}

ECode CSipEditor::OnResume()
{
    PreferenceActivity::OnResume();
    mHomeButtonClicked = FALSE;
    if (!SipUtil::IsPhoneIdle(this)) {
        mAdvancedSettings->Show();
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreference::Probe(screen)->SetEnabled(FALSE);
        if (mRemoveButton != NULL) IView::Probe(mRemoveButton)->SetEnabled(FALSE);
    }
    else {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreference::Probe(screen)->SetEnabled(TRUE);
        if (mRemoveButton != NULL) IView::Probe(mRemoveButton)->SetEnabled(TRUE);
    }
    return NOERROR;
}

ECode CSipEditor::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    if (VERBOSE) Log(String("onCreate, start profile editor"));
    PreferenceActivity::OnCreate(savedInstanceState);

    mSharedPreferences = new SipSharedPreferences((IContext*)this);
    mProfileDb = new SipProfileDb((IContext*)this);
    mSipAccountRegistry = SipAccountRegistry::GetInstance();

    SetContentView(R::layout::sip_settings_ui);
    AddPreferencesFromResource(R::xml::sip_edit);

    assert(0);
    // SipProfile p = mOldProfile = (SipProfile) ((savedInstanceState == null)
    //         ? getIntent().getParcelableExtra(SipSettings.KEY_SIP_PROFILE)
    //         : savedInstanceState.getParcelable(KEY_PROFILE));

    AutoPtr<IPreferenceScreen> _screen;
    GetPreferenceScreen((IPreferenceScreen**)&_screen);
    AutoPtr<IPreferenceGroup> screen = IPreferenceGroup::Probe(_screen);
    Int32 count;
    screen->GetPreferenceCount(&count);
    for (Int32 i = 0, n = count; i < n; i++) {
        AutoPtr<IPreference> preference;
        screen->GetPreference(i, (IPreference**)&preference);
        SetupPreference(preference);
    }

    assert(0);
    // if (p == null) {
    //     screen->SetTitle(R::string::sip_edit_new_title);
    // }

    mAdvancedSettings = new AdvancedSettings(this);

    assert(0);
    //LoadPreferencesFromProfile(p);
    return NOERROR;
}

ECode CSipEditor::OnPause()
{
    Boolean res;
    IsFinishing(&res);
    if (VERBOSE) Log(String("onPause, finishing: ") + StringUtils::ToString(res));
    if (!res) {
        mHomeButtonClicked = TRUE;
        ValidateAndSetResult();
    }
    return PreferenceActivity::OnPause();
}

ECode CSipEditor::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    PreferenceActivity::OnCreateOptionsMenu(menu, &res);

    AutoPtr<IMenuItem> item;
    menu->Add(0, MENU_DISCARD, 0, R::string::sip_menu_discard, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);

    AutoPtr<IMenuItem> item2;
    menu->Add(0, MENU_SAVE, 0, R::string::sip_menu_save, (IMenuItem**)&item2);
    item2->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);

    AutoPtr<IMenuItem> item3;
    menu->Add(0, MENU_REMOVE, 0, R::string::remove_sip_account, (IMenuItem**)&item3);
    item3->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
    *result = TRUE;
    return NOERROR;
}

ECode CSipEditor::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMenuItem> removeMenu;
    menu->FindItem(MENU_REMOVE, (IMenuItem**)&removeMenu);
    assert(0);
    //removeMenu->SetVisible(mOldProfile != NULL);

    AutoPtr<IMenuItem> saveMenu;
    menu->FindItem(MENU_SAVE, (IMenuItem**)&saveMenu);
    saveMenu->SetEnabled(mUpdateRequired);
    return PreferenceActivity::OnPrepareOptionsMenu(menu, result);
}

ECode CSipEditor::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_SAVE:
            ValidateAndSetResult();
            *result = TRUE;
            return NOERROR;

        case MENU_DISCARD:
            Finish();
            *result = TRUE;
            return NOERROR;

        case MENU_REMOVE: {
            SetRemovedProfileAndFinish();
            *result = TRUE;
            return NOERROR;
        }
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

ECode CSipEditor::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (keyCode) {
        case IKeyEvent::KEYCODE_BACK:
            ValidateAndSetResult();
            *result = TRUE;
            return NOERROR;
    }
    return PreferenceActivity::OnKeyDown(keyCode, event, result);
}

// CARAPI SaveAndRegisterProfile(
//     /* [in] */ SipProfile* p);

/**
 * Deletes a {@link SipProfile} and un-registers the associated
 * {@link android.telecom.PhoneAccount}.
 *
 * @param p The {@link SipProfile} to delete.
 */
// CARAPI_(void) DeleteAndUnregisterProfile(
//     /* [in] */ SipProfile* p);

void CSipEditor::SetRemovedProfileAndFinish()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IContext*)this, ECLSID_CSipSettings, (IIntent**)&intent);
    SetResult(RESULT_FIRST_USER, intent);

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(this,  R::string::removing_account, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    assert(0);
    //ReplaceProfile(mOldProfile, NULL);
    // do finish() in replaceProfile() in a background thread
}

void CSipEditor::ShowAlert(
    /* [in] */ IThrowable* e)
{
    String msg;
    e->GetMessage(&msg);
    if (TextUtils::IsEmpty(msg)) IObject::Probe(e)->ToString(&msg);
    ShowAlert(msg);
}

void CSipEditor::ShowAlert(
    /* [in] */ const String& message)
{
    if (mHomeButtonClicked) {
        if (VERBOSE) Log(String("Home button clicked, don't show dialog: ") + message);
        return;
    }
    AutoPtr<IRunnable> r = new MyRunnable(this, message);
    RunOnUiThread(r);
}

Boolean CSipEditor::IsEditTextEmpty(
    /* [in] */ PreferenceKey* key)
{
    AutoPtr<IEditTextPreference> pref = IEditTextPreference::Probe(key->mPreference);
    String str;
    pref->GetText(&str);
    AutoPtr<ICharSequence> cchar;
    IPreference::Probe(pref)->GetSummary((ICharSequence**)&cchar);
    String summary;
    cchar->ToString(&summary);
    String dsummary;
    GetString(key->mDefaultSummary, &dsummary);
    return TextUtils::IsEmpty(str) || summary.Equals(dsummary);
}

void CSipEditor::ValidateAndSetResult()
{
    Boolean allEmpty = TRUE;
    AutoPtr<ICharSequence> firstEmptyFieldTitle;
    assert(0);
    // for (PreferenceKey key : PreferenceKey.values()) {
    //     Preference p = key.preference;
    //     if (p instanceof EditTextPreference) {
    //         EditTextPreference pref = (EditTextPreference) p;
    //         boolean fieldEmpty = isEditTextEmpty(key);
    //         if (allEmpty && !fieldEmpty) allEmpty = FALSE;

    //         // use default value if display name is empty
    //         if (fieldEmpty) {
    //             switch (key) {
    //                 case DisplayName:
    //                     pref.setText(getDefaultDisplayName());
    //                     break;
    //                 case AuthUserName:
    //                 case ProxyAddress:
    //                     // optional; do nothing
    //                     break;
    //                 case Port:
    //                     pref.setText(getString(R.string.default_port));
    //                     break;
    //                 default:
    //                     if (firstEmptyFieldTitle == null) {
    //                         firstEmptyFieldTitle = pref.getTitle();
    //                     }
    //             }
    //         } else if (key == PreferenceKey.Port) {
    //             int port = Integer.parseInt(PreferenceKey.Port.getValue());
    //             if ((port < 1000) || (port > 65534)) {
    //                 showAlert(getString(R.string.not_a_valid_port));
    //                 return;
    //             }
    //         }
    //     }
    // }

    if (allEmpty || !mUpdateRequired) {
        Finish();
        return;
    }
    else if (firstEmptyFieldTitle != NULL) {
        String str;
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, TO_IINTERFACE(firstEmptyFieldTitle));
        GetString(R::string::empty_alert, array, &str);
        ShowAlert(str);
        return;
    }
    assert(0);
    // try {
    //     SipProfile profile = createSipProfile();
    //     Intent intent = new Intent(this, SipSettings.class);
    //     intent.putExtra(SipSettings.KEY_SIP_PROFILE, (Parcelable) profile);
    //     setResult(RESULT_OK, intent);
    //     Toast.makeText(this, R.string.saving_account, Toast.LENGTH_SHORT).show();

    //     replaceProfile(mOldProfile, profile);
    //     // do finish() in replaceProfile() in a background thread
    // } catch (Exception e) {
    //     log("validateAndSetResult, can not create new SipProfile, exception: " + e);
    //     showAlert(e);
    // }
}

// CARAPI_(void) ReplaceProfile(
//     /* [in] */ SipProfile* oldProfile,
//     /* [in] */ SipProfile* newProfile);

String CSipEditor::GetProfileName()
{
    assert(0);
    // return PreferenceKey.Username.getValue() + "@"
    //         + PreferenceKey.DomainAddress.getValue();
    return String(NULL);
}

// CARAPI CreateSipProfile(
//     /* [out] */ SipProfile** profile);

ECode CSipEditor::OnPreferenceChange(
    /* [in] */ IPreference* pref,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!mUpdateRequired) {
        mUpdateRequired = TRUE;
    }

    if (ICheckBoxPreference::Probe(pref) != NULL) {
        InvalidateOptionsMenu();
        *result = TRUE;
        return NOERROR;
    }
    String value;
    if (newValue == NULL) {
        value = String("");
    }
    else {
        IObject::Probe(newValue)->ToString(&value);
    }
    assert(0);
    // if (TextUtils::IsEmpty(value)) {
    //     AutoPtr<PreferenceKey> key;
    //     GetPreferenceKey(pref, (PreferenceKey**)&key);
    //     pref->SetSummary(key->mDefaultSummary);
    // }
    // else if (pref == PreferenceKey.Password.preference) {
    //     pref->SetSummary(scramble(value));
    // }
    // else {
    //     pref->SetSummary(value);
    // }

    // if (pref == PreferenceKey.DisplayName.preference) {
    //     ((EditTextPreference) pref).setText(value);
    //     checkIfDisplayNameSet();
    // }

    // SAVE menu should be enabled once the user modified some preference.
    InvalidateOptionsMenu();
    *result = TRUE;
    return NOERROR;
}

ECode CSipEditor::GetPreferenceKey(
    /* [in] */ IPreference* pref,
    /* [out] */ PreferenceKey** key)
{
    VALIDATE_NOT_NULL(key)
    *key = NULL;

    assert(0);
    // for (PreferenceKey key : PreferenceKey.values()) {
    //     if (key.preference == pref) return key;
    // }
    //throw new RuntimeException("not possible to reach here");
    return E_RUNTIME_EXCEPTION;
}

// CARAPI_(void) LoadPreferencesFromProfile(
//     /* [in] */ SipProfile* p);

Boolean CSipEditor::IsAlwaysSendKeepAlive()
{
    assert(0);
    AutoPtr<IListPreference> pref;// = (ListPreference) PreferenceKey.SendKeepAlive.preference;

    String str;
    GetString(R::string::sip_always_send_keepalive, &str);
    String value;
    pref->GetValue(&value);
    return str.Equals(value);
}

void CSipEditor::SetCheckBox(
    /* [in] */ PreferenceKey* key,
    /* [in] */ Boolean checked)
{
    AutoPtr<ICheckBoxPreference> pref = ICheckBoxPreference::Probe(key->mPreference);
    ITwoStatePreference::Probe(pref)->SetChecked(checked);
}

void CSipEditor::SetupPreference(
    /* [in] */ IPreference* pref)
{
    AutoPtr<InnerListener> listener = new InnerListener(this);
    pref->SetOnPreferenceChangeListener(listener);
    assert(0);
    // for (PreferenceKey key : PreferenceKey.values()) {
    //     String name = getString(key.text);
    //     if (name.equals(pref.getKey())) {
    //         key.preference = pref;
    //         return;
    //     }
    // }
}

void CSipEditor::CheckIfDisplayNameSet()
{
    assert(0);
    String displayName;// = PreferenceKey.DisplayName.getValue();
    mDisplayNameSet = !TextUtils::IsEmpty(displayName)
            && !displayName.Equals(GetDefaultDisplayName());
    if (VERBOSE) Log(String("checkIfDisplayNameSet, displayName set: ") + StringUtils::ToString(mDisplayNameSet));
    assert(0);
    // if (mDisplayNameSet) {
    //     PreferenceKey.DisplayName.preference.setSummary(displayName);
    // } else {
    //     PreferenceKey.DisplayName.setValue("");
    // }
}

String CSipEditor::GetDefaultDisplayName()
{
    assert(0);
    //return PreferenceKey.Username.getValue();
    return String(NULL);
}

String CSipEditor::Scramble(
    /* [in] */ const String& s)
{
    AutoPtr<ArrayOf<Char32> > cc = ArrayOf<Char32>::Alloc(s.GetLength());
    Arrays::Fill(cc, SCRAMBLED);
    return String(*cc.Get());
}

void CSipEditor::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos