
#include "elastos/droid/settings/inputmethod/SpellCheckersSettings.h"
#include "elastos/droid/settings/inputmethod/CSpellCheckerPreference.h"
#include "elastos/droid/settings/inputmethod/InputMethodAndSubtypeUtil.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Widget::CSwitchBar;
using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String SpellCheckersSettings::TAG("SpellCheckersSettings");
const Boolean SpellCheckersSettings::DBG = FALSE;
const String SpellCheckersSettings::KEY_SPELL_CHECKER_LANGUAGE("spellchecker_language");
const Int32 SpellCheckersSettings::ITEM_ID_USE_SYSTEM_LANGUAGE = 0;

//===============================================================================
//                  SpellCheckersSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_2(SpellCheckersSettings::InnerListener, Object, ISwitchBarOnSwitchChangeListener, IPreferenceOnPreferenceClickListener)

SpellCheckersSettings::InnerListener::InnerListener(
    /* [in] */ SpellCheckersSettings* host)
    : mHost(host)
{}

ECode SpellCheckersSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

ECode SpellCheckersSettings::InnerListener::OnPreferenceClick(
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceClick(pref, result);
}

//===============================================================================
//                  SpellCheckersSettings::AlertDialogOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(SpellCheckersSettings::AlertDialogOnClickListener, Object, IDialogInterfaceOnClickListener)

SpellCheckersSettings::AlertDialogOnClickListener::AlertDialogOnClickListener(
    /* [in] */ SpellCheckersSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ ISpellCheckerInfo* info)
    : mHost(host)
    , mId(id)
    , mSci(info)
{}

SpellCheckersSettings::AlertDialogOnClickListener::~AlertDialogOnClickListener()
{}

ECode SpellCheckersSettings::AlertDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 item)
{
    switch (mId) {
        case 0: {
            if (item == ITEM_ID_USE_SYSTEM_LANGUAGE) {
                mHost->mTsm->SetSpellCheckerSubtype(NULL);
            }
            else {
                const Int32 index = ConvertDialogItemIdToSubtypeIndex(item);
                AutoPtr<ISpellCheckerSubtype> subtype;
                mSci->GetSubtypeAt(index, (ISpellCheckerSubtype**)&subtype);
                mHost->mTsm->SetSpellCheckerSubtype(subtype);
            }
            if (DBG) {
                AutoPtr<ISpellCheckerSubtype> subtype;
                mHost->mTsm->GetCurrentSpellCheckerSubtype(
                        TRUE /* allowImplicitlySelectedSubtype */, (ISpellCheckerSubtype**)&subtype);
                String str = String(NULL);
                if (subtype != NULL) {
                    subtype->GetLocale(&str);
                }
                Logger::D(TAG, "Current spell check locale is %s", str.string());
            }
            dialog->Dismiss();
            mHost->UpdatePreferenceScreen();
            break;
        }
        case 1:
            mHost->ChangeCurrentSpellChecker(mSci);
            break;
        case 2:
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  SpellCheckersSettings
//===============================================================================

CAR_INTERFACE_IMPL(SpellCheckersSettings, SettingsPreferenceFragment, ISpellCheckerPreferenceOnRadioButtonPreferenceListener);

SpellCheckersSettings::SpellCheckersSettings()
{
    mListener = new InnerListener(this);
}

SpellCheckersSettings::~SpellCheckersSettings()
{}

ECode SpellCheckersSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode SpellCheckersSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::spellchecker_prefs);
    FindPreference(CoreUtils::Convert(KEY_SPELL_CHECKER_LANGUAGE), (IPreference**)&mSpellCheckerLanaguagePref);
    mSpellCheckerLanaguagePref->SetOnPreferenceClickListener(mListener);

    AutoPtr<IInterface> obj = GetSystemService(IContext::TEXT_SERVICES_MANAGER_SERVICE);
    mTsm = ITextServicesManager::Probe(obj);
    mTsm->GetCurrentSpellChecker((ISpellCheckerInfo**)&mCurrentSci);
    mTsm->GetEnabledSpellCheckers((ArrayOf<ISpellCheckerInfo*>**)&mEnabledScis);
    PopulatePreferenceScreen();
    return NOERROR;
}

void SpellCheckersSettings::PopulatePreferenceScreen()
{
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    Int32 count = 0;
    if (mEnabledScis != NULL) {
        count = mEnabledScis->GetLength();
    }

    for (Int32 index = 0; index < count; ++index) {
        AutoPtr<ISpellCheckerInfo> sci = (*mEnabledScis)[index];
        AutoPtr<ISpellCheckerPreference> pref;
        CSpellCheckerPreference::New(context, sci, this, (ISpellCheckerPreference**)&pref);
        IPreference* _pref = IPreference::Probe(pref);
        Boolean res;
        IPreferenceGroup::Probe(screen)->AddPreference(_pref, &res);
        InputMethodAndSubtypeUtil::RemoveUnnecessaryNonPersistentPreference(_pref);
    }
}

ECode SpellCheckersSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    ((CSettingsActivity*)activity.Get())->GetSwitchBar((ISwitchBar**)&mSwitchBar);
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    switchBar->Show();
    switchBar->AddOnSwitchChangeListener(mListener);
    UpdatePreferenceScreen();
    return NOERROR;
}

ECode SpellCheckersSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    ((CSwitchBar*)mSwitchBar.Get())->RemoveOnSwitchChangeListener(mListener);
    return NOERROR;
}

ECode SpellCheckersSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    mTsm->SetSpellCheckerEnabled(isChecked);
    UpdatePreferenceScreen();
    return NOERROR;
}

void SpellCheckersSettings::UpdatePreferenceScreen()
{
    mCurrentSci = NULL;
    mTsm->GetCurrentSpellChecker((ISpellCheckerInfo**)&mCurrentSci);
    Boolean isSpellCheckerEnabled;
    mTsm->IsSpellCheckerEnabled(&isSpellCheckerEnabled);
    ((CSwitchBar*)mSwitchBar.Get())->SetChecked(isSpellCheckerEnabled);

    AutoPtr<ISpellCheckerSubtype> currentScs;
    mTsm->GetCurrentSpellCheckerSubtype(
            FALSE /* allowImplicitlySelectedSubtype */, (ISpellCheckerSubtype**)&currentScs);
    mSpellCheckerLanaguagePref->SetSummary(GetSpellCheckerSubtypeLabel(mCurrentSci, currentScs));

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreferenceGroup* _screen = IPreferenceGroup::Probe(screen);
    Int32 count;
    _screen->GetPreferenceCount(&count);
    for (Int32 index = 0; index < count; index++) {
        AutoPtr<IPreference> preference;
        _screen->GetPreference(index, (IPreference**)&preference);
        preference->SetEnabled(isSpellCheckerEnabled);
        AutoPtr<ISpellCheckerPreference> pref = ISpellCheckerPreference::Probe(preference);
        if (pref != NULL) {
            AutoPtr<ISpellCheckerInfo> sci;
            pref->GetSpellCheckerInfo((ISpellCheckerInfo**)&sci);
            Boolean res = FALSE;
            if (mCurrentSci != NULL) {
                String id1, id2;
                mCurrentSci->GetId(&id1);
                sci->GetId(&id2);
                res = id1.Equals(id2);
            }
            pref->SetSelected(res);
        }
    }
}

AutoPtr<ICharSequence> SpellCheckersSettings::GetSpellCheckerSubtypeLabel(
    /* [in] */ ISpellCheckerInfo* sci,
    /* [in] */ ISpellCheckerSubtype* subtype)
{
    if (sci == NULL) {
        return NULL;
    }

    AutoPtr<ICharSequence> cs;
    if (subtype == NULL) {
        String str;
        GetString(R::string::use_system_language_to_select_input_method_subtypes, &str);
        cs = CoreUtils::Convert(str);
        return cs;
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    String name;
    sci->GetPackageName(&name);
    AutoPtr<IServiceInfo> serviceInfo;
    sci->GetServiceInfo((IServiceInfo**)&serviceInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    subtype->GetDisplayName(
            IContext::Probe(activity), name, appInfo, (ICharSequence**)&cs);
    return cs;
}

ECode SpellCheckersSettings::OnPreferenceClick(
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (pref == mSpellCheckerLanaguagePref) {
        ShowChooseLanguageDialog();
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode SpellCheckersSettings::OnRadioButtonClicked(
    /* [in] */ ISpellCheckerPreference* pref)
{
    AutoPtr<ISpellCheckerInfo> sci;
    pref->GetSpellCheckerInfo((ISpellCheckerInfo**)&sci);

    AutoPtr<IServiceInfo> serviceInfo;
    sci->GetServiceInfo((IServiceInfo**)&serviceInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);

    Boolean isSystemApp = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
    if (isSystemApp) {
        ChangeCurrentSpellChecker(sci);
    }
    else {
        ShowSecurityWarnDialog(pref);
    }
    return NOERROR;
}

Int32 SpellCheckersSettings::ConvertSubtypeIndexToDialogItemId(
    /* [in] */ Int32 index)
{
    return index + 1;
}
Int32 SpellCheckersSettings::ConvertDialogItemIdToSubtypeIndex(
    /* [in] */ Int32 item)
{
    return item - 1;
}

void SpellCheckersSettings::ShowChooseLanguageDialog()
{
    Boolean res;
    if (mDialog != NULL && (IDialog::Probe(mDialog)->IsShowing(&res), res)) {
        IDialogInterface::Probe(mDialog)->Dismiss();
    }
    AutoPtr<ISpellCheckerInfo> currentSci;
    mTsm->GetCurrentSpellChecker((ISpellCheckerInfo**)&currentSci);
    AutoPtr<ISpellCheckerSubtype> currentScs;
    mTsm->GetCurrentSpellCheckerSubtype(
            FALSE /* allowImplicitlySelectedSubtype */, (ISpellCheckerSubtype**)&currentScs);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::phone_language);
    Int32 subtypeCount;
    currentSci->GetSubtypeCount(&subtypeCount);
    AutoPtr< ArrayOf<ICharSequence*> > items = ArrayOf<ICharSequence*>::Alloc(subtypeCount + 1 /* default */);
    items->Set(ITEM_ID_USE_SYSTEM_LANGUAGE, GetSpellCheckerSubtypeLabel(currentSci, NULL));
    Int32 checkedItemId = ITEM_ID_USE_SYSTEM_LANGUAGE;
    for (Int32 index = 0; index < subtypeCount; ++index) {
        AutoPtr<ISpellCheckerSubtype> subtype;
        currentSci->GetSubtypeAt(index, (ISpellCheckerSubtype**)&subtype);
        const Int32 itemId = ConvertSubtypeIndexToDialogItemId(index);
        items->Set(itemId, GetSpellCheckerSubtypeLabel(currentSci, subtype));
        if (IObject::Probe(subtype)->Equals(currentScs, &res), res) {
            checkedItemId = itemId;
        }
    }

    AutoPtr<AlertDialogOnClickListener> listener = new AlertDialogOnClickListener(this, 0, currentSci);
    builder->SetSingleChoiceItems(items, checkedItemId, listener);
    mDialog = NULL;
    builder->Create((IAlertDialog**)&mDialog);
    IDialog::Probe(mDialog)->Show();
}

void SpellCheckersSettings::ShowSecurityWarnDialog(
    /* [in] */ ISpellCheckerPreference* pref)
{
    Boolean res;
    if (mDialog != NULL && (IDialog::Probe(mDialog)->IsShowing(&res), res)) {
        IDialogInterface::Probe(mDialog)->Dismiss();
    }
    AutoPtr<ISpellCheckerInfo> sci;
    pref->GetSpellCheckerInfo((ISpellCheckerInfo**)&sci);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);
    AutoPtr<ICharSequence> cs;
    IPreference::Probe(pref)->GetTitle((ICharSequence**)&cs);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cs);
    String str;
    GetString(R::string::spellchecker_security_warning, args, &str);
    builder->SetMessage(CoreUtils::Convert(str));
    builder->SetCancelable(TRUE);
    AutoPtr<AlertDialogOnClickListener> listener1 = new AlertDialogOnClickListener(this, 1, sci);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener1);
    AutoPtr<AlertDialogOnClickListener> listener2 = new AlertDialogOnClickListener(this, 2, NULL);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, listener2);
    mDialog = NULL;
    builder->Create((IAlertDialog**)&mDialog);
    IDialog::Probe(mDialog)->Show();
}

void SpellCheckersSettings::ChangeCurrentSpellChecker(
    /* [in] */ ISpellCheckerInfo* sci)
{
    mTsm->SetCurrentSpellChecker(sci);
    if (DBG) {
        AutoPtr<ISpellCheckerInfo> info;
        mTsm->GetCurrentSpellChecker((ISpellCheckerInfo**)&info);
        String id;
        info->GetId(&id);
        Logger::D(TAG, "Current spell check is %s", id.string());
    }
    UpdatePreferenceScreen();
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos