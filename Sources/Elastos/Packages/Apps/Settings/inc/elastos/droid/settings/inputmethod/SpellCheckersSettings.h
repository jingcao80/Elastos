

package com.android.settings.inputmethod;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::Textservice::ISpellCheckerInfo;
using Elastos::Droid::View::Textservice::ISpellCheckerSubtype;
using Elastos::Droid::View::Textservice::ITextServicesManager;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Inputmethod::SpellCheckerPreference::IOnRadioButtonPreferenceListener;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::SwitchBar::IOnSwitchChangeListener;

public class SpellCheckersSettings extends SettingsPreferenceFragment
        implements OnSwitchChangeListener, OnPreferenceClickListener,
        OnRadioButtonPreferenceListener {
    private static const String TAG = SpellCheckersSettings.class->GetSimpleName();
    private static const Boolean DBG = FALSE;

    private static const String KEY_SPELL_CHECKER_LANGUAGE = "spellchecker_language";
    private static const Int32 ITEM_ID_USE_SYSTEM_LANGUAGE = 0;

    private SwitchBar mSwitchBar;
    private Preference mSpellCheckerLanaguagePref;
    private AlertDialog mDialog = NULL;
    private SpellCheckerInfo mCurrentSci;
    private SpellCheckerInfo[] mEnabledScis;
    private TextServicesManager mTsm;

    //@Override
    CARAPI OnCreate(final Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.spellchecker_prefs);
        mSpellCheckerLanaguagePref = FindPreference(KEY_SPELL_CHECKER_LANGUAGE);
        mSpellCheckerLanaguagePref->SetOnPreferenceClickListener(this);

        mTsm = (TextServicesManager) GetSystemService(Context.TEXT_SERVICES_MANAGER_SERVICE);
        mCurrentSci = mTsm->GetCurrentSpellChecker();
        mEnabledScis = mTsm->GetEnabledSpellCheckers();
        PopulatePreferenceScreen();
    }

    private void PopulatePreferenceScreen() {
        final PreferenceScreen screen = GetPreferenceScreen();
        final Context context = GetActivity();
        final Int32 count = (mEnabledScis == NULL) ? 0 : mEnabledScis.length;
        for (Int32 index = 0; index < count; ++index) {
            final SpellCheckerInfo sci = mEnabledScis[index];
            final SpellCheckerPreference pref = new SpellCheckerPreference(context, sci, this);
            screen->AddPreference(pref);
            InputMethodAndSubtypeUtil->RemoveUnnecessaryNonPersistentPreference(pref);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSwitchBar = ((SettingsActivity)GetActivity()).GetSwitchBar();
        mSwitchBar->Show();
        mSwitchBar->AddOnSwitchChangeListener(this);
        UpdatePreferenceScreen();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mSwitchBar->RemoveOnSwitchChangeListener(this);
    }

    //@Override
    CARAPI OnSwitchChanged(final Switch switchView, final Boolean isChecked) {
        mTsm->SetSpellCheckerEnabled(isChecked);
        UpdatePreferenceScreen();
    }

    private void UpdatePreferenceScreen() {
        mCurrentSci = mTsm->GetCurrentSpellChecker();
        final Boolean isSpellCheckerEnabled = mTsm->IsSpellCheckerEnabled();
        mSwitchBar->SetChecked(isSpellCheckerEnabled);

        final SpellCheckerSubtype currentScs = mTsm->GetCurrentSpellCheckerSubtype(
                FALSE /* allowImplicitlySelectedSubtype */);
        mSpellCheckerLanaguagePref->SetSummary(GetSpellCheckerSubtypeLabel(mCurrentSci, currentScs));

        final PreferenceScreen screen = GetPreferenceScreen();
        final Int32 count = screen->GetPreferenceCount();
        for (Int32 index = 0; index < count; index++) {
            final Preference preference = screen->GetPreference(index);
            preference->SetEnabled(isSpellCheckerEnabled);
            if (preference instanceof SpellCheckerPreference) {
                final SpellCheckerPreference pref = (SpellCheckerPreference)preference;
                final SpellCheckerInfo sci = pref->GetSpellCheckerInfo();
                pref->SetSelected(mCurrentSci != NULL && mCurrentSci->GetId()->Equals(sci->GetId()));
            }
        }
    }

    private CharSequence GetSpellCheckerSubtypeLabel(final SpellCheckerInfo sci,
            final SpellCheckerSubtype subtype) {
        if (sci == NULL) {
            return NULL;
        }
        if (subtype == NULL) {
            return GetString(R::string::use_system_language_to_select_input_method_subtypes);
        }
        return subtype->GetDisplayName(
                GetActivity(), sci->GetPackageName(), sci->GetServiceInfo().applicationInfo);
    }

    //@Override
    public Boolean OnPreferenceClick(final Preference pref) {
        if (pref == mSpellCheckerLanaguagePref) {
            ShowChooseLanguageDialog();
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    CARAPI OnRadioButtonClicked(final SpellCheckerPreference pref) {
        final SpellCheckerInfo sci = pref->GetSpellCheckerInfo();
        final Boolean isSystemApp =
                (sci->GetServiceInfo().applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0;
        if (isSystemApp) {
            ChangeCurrentSpellChecker(sci);
        } else {
            ShowSecurityWarnDialog(pref);
        }
    }

    private static Int32 ConvertSubtypeIndexToDialogItemId(final Int32 index) { return index + 1; }
    private static Int32 ConvertDialogItemIdToSubtypeIndex(final Int32 item) { return item - 1; }

    private void ShowChooseLanguageDialog() {
        if (mDialog != NULL && mDialog->IsShowing()) {
            mDialog->Dismiss();
        }
        final SpellCheckerInfo currentSci = mTsm->GetCurrentSpellChecker();
        final SpellCheckerSubtype currentScs = mTsm->GetCurrentSpellCheckerSubtype(
                FALSE /* allowImplicitlySelectedSubtype */);
        final AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
        builder->SetTitle(R::string::phone_language);
        final Int32 subtypeCount = currentSci->GetSubtypeCount();
        final CharSequence[] items = new CharSequence[subtypeCount + 1 /* default */ ];
        items[ITEM_ID_USE_SYSTEM_LANGUAGE] = GetSpellCheckerSubtypeLabel(currentSci, NULL);
        Int32 checkedItemId = ITEM_ID_USE_SYSTEM_LANGUAGE;
        for (Int32 index = 0; index < subtypeCount; ++index) {
            final SpellCheckerSubtype subtype = currentSci->GetSubtypeAt(index);
            final Int32 itemId = ConvertSubtypeIndexToDialogItemId(index);
            items[itemId] = GetSpellCheckerSubtypeLabel(currentSci, subtype);
            if (subtype->Equals(currentScs)) {
                checkedItemId = itemId;
            }
        }
        builder->SetSingleChoiceItems(items, checkedItemId, new AlertDialog->OnClickListener() {
            //@Override
            CARAPI OnClick(final DialogInterface dialog, final Int32 item) {
                if (item == ITEM_ID_USE_SYSTEM_LANGUAGE) {
                    mTsm->SetSpellCheckerSubtype(NULL);
                } else {
                    final Int32 index = ConvertDialogItemIdToSubtypeIndex(item);
                    mTsm->SetSpellCheckerSubtype(currentSci->GetSubtypeAt(index));
                }
                if (DBG) {
                    final SpellCheckerSubtype subtype = mTsm->GetCurrentSpellCheckerSubtype(
                            TRUE /* allowImplicitlySelectedSubtype */);
                    Logger::D(TAG, "Current spell check locale is "
                            + subtype == NULL ? "NULL" : subtype->GetLocale());
                }
                dialog->Dismiss();
                UpdatePreferenceScreen();
            }
        });
        mDialog = builder->Create();
        mDialog->Show();
    }

    private void ShowSecurityWarnDialog(final SpellCheckerPreference pref) {
        if (mDialog != NULL && mDialog->IsShowing()) {
            mDialog->Dismiss();
        }
        final SpellCheckerInfo sci = pref->GetSpellCheckerInfo();
        final AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
        builder->SetTitle(android.R::string::dialog_alert_title);
        builder->SetMessage(GetString(R::string::spellchecker_security_warning, pref->GetTitle()));
        builder->SetCancelable(TRUE);
        builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(final DialogInterface dialog, final Int32 which) {
                ChangeCurrentSpellChecker(sci);
            }
        });
        builder->SetNegativeButton(android.R::string::cancel, new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(final DialogInterface dialog, final Int32 which) {
            }
        });
        mDialog = builder->Create();
        mDialog->Show();
    }

    private void ChangeCurrentSpellChecker(final SpellCheckerInfo sci) {
        mTsm->SetCurrentSpellChecker(sci);
        if (DBG) {
            Logger::D(TAG, "Current spell check is " + mTsm->GetCurrentSpellChecker()->GetId());
        }
        UpdatePreferenceScreen();
    }
}
