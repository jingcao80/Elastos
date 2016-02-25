

package com.android.settings.inputmethod;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Inputmethod::IInputMethodUtils;
using Elastos::Droid::Settings::IR;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Input method preference.
 *
 * This preference represents an IME. It is used for two purposes. 1) An instance with a switch
 * is used to enable or disable the IME. 2) An instance without a switch is used to invoke the
 * setting activity of the IME.
 */
class InputMethodPreference extends SwitchPreference implements OnPreferenceClickListener,
        OnPreferenceChangeListener {
    private static const String TAG = InputMethodPreference.class->GetSimpleName();
    private static const String EMPTY_TEXT = "";

    interface OnSavePreferenceListener {
        /**
         * Called when this preference needs to be saved its state.
         *
         * Note that this preference is non-persistent and needs explicitly to be saved its state.
         * Because changing one IME state may change other IMEs' state, this is a place to update
         * other IMEs' state as well.
         *
         * @param pref This preference.
         */
        CARAPI OnSaveInputMethodPreference(InputMethodPreference pref);
    }

    private final InputMethodInfo mImi;
    private final Boolean mHasPriorityInSorting;
    private final OnSavePreferenceListener mOnSaveListener;
    private final InputMethodSettingValuesWrapper mInputMethodSettingValues;
    private final Boolean mIsAllowedByOrganization;

    private AlertDialog mDialog = NULL;

    /**
     * A preference entry of an input method.
     *
     * @param context The Context this is associated with.
     * @param imi The {@link InputMethodInfo} of this preference.
     * @param isImeEnabler TRUE if this preference is the IME enabler that has enable/disable
     *     switches for all available IMEs, not the list of enabled IMEs.
     * @param isAllowedByOrganization FALSE if the IME has been disabled by a device or profile
           owner.
     * @param onSaveListener The listener called when this preference has been changed and needs
     *     to save the state to shared preference.
     */
    InputMethodPreference(final Context context, final InputMethodInfo imi,
            final Boolean isImeEnabler, final Boolean isAllowedByOrganization,
            final OnSavePreferenceListener onSaveListener) {
        Super(context);
        SetPersistent(FALSE);
        mImi = imi;
        mIsAllowedByOrganization = isAllowedByOrganization;
        mOnSaveListener = onSaveListener;
        if (!isImeEnabler) {
            // Hide switch widget.
            SetWidgetLayoutResource(0 /* widgetLayoutResId */);
        }
        // Disable on/off switch texts.
        SetSwitchTextOn(EMPTY_TEXT);
        SetSwitchTextOff(EMPTY_TEXT);
        SetKey(imi->GetId());
        SetTitle(imi->LoadLabel(context->GetPackageManager()));
        final String settingsActivity = imi->GetSettingsActivity();
        if (TextUtils->IsEmpty(settingsActivity)) {
            SetIntent(NULL);
        } else {
            // Set an intent to invoke settings activity of an input method.
            final Intent intent = new Intent(IIntent::ACTION_MAIN);
            intent->SetClassName(imi->GetPackageName(), settingsActivity);
            SetIntent(intent);
        }
        mInputMethodSettingValues = InputMethodSettingValuesWrapper->GetInstance(context);
        mHasPriorityInSorting = InputMethodUtils->IsSystemIme(imi)
                && mInputMethodSettingValues->IsValidSystemNonAuxAsciiCapableIme(imi, context);
        SetOnPreferenceClickListener(this);
        SetOnPreferenceChangeListener(this);
    }

    public InputMethodInfo GetInputMethodInfo() {
        return mImi;
    }

    private Boolean IsImeEnabler() {
        // If this {@link SwitchPreference} doesn't have a widget layout, we explicitly hide the
        // switch widget at constructor.
        return GetWidgetLayoutResource() != 0;
    }

    //@Override
    public Boolean OnPreferenceChange(final Preference preference, final Object newValue) {
        // Always returns FALSE to prevent default behavior.
        // See {@link TwoStatePreference#OnClick()}.
        if (!IsImeEnabler()) {
            // Prevent disabling an IME because this preference is for invoking a settings activity.
            return FALSE;
        }
        if (IsChecked()) {
            // Disable this IME.
            SetChecked(FALSE);
            mOnSaveListener->OnSaveInputMethodPreference(this);
            return FALSE;
        }
        if (InputMethodUtils->IsSystemIme(mImi)) {
            // Enable a system IME. No need to show a security warning dialog.
            SetChecked(TRUE);
            mOnSaveListener->OnSaveInputMethodPreference(this);
            return FALSE;
        }
        // Enable a 3rd party IME.
        ShowSecurityWarnDialog(mImi);
        return FALSE;
    }

    //@Override
    public Boolean OnPreferenceClick(final Preference preference) {
        // Always returns TRUE to prevent invoking an intent without catching exceptions.
        // See {@link Preference#PerformClick(PreferenceScreen)}/
        if (IsImeEnabler()) {
            // Prevent invoking a settings activity because this preference is for enabling and
            // disabling an input method.
            return TRUE;
        }
        final Context context = GetContext();
        try {
            final Intent intent = GetIntent();
            if (intent != NULL) {
                // Invoke a settings activity of an input method.
                context->StartActivity(intent);
            }
        } catch (final ActivityNotFoundException e) {
            Logger::D(TAG, "IME's Settings Activity Not Found", e);
            final String message = context->GetString(
                    R::string::failed_to_open_app_settings_toast,
                    mImi->LoadLabel(context->GetPackageManager()));
            Toast->MakeText(context, message, Toast.LENGTH_LONG).Show();
        }
        return TRUE;
    }

    void UpdatePreferenceViews() {
        final Boolean isAlwaysChecked = mInputMethodSettingValues->IsAlwaysCheckedIme(
                mImi, GetContext());
        // Only when this preference has a switch and an input method should be always enabled,
        // this preference should be disabled to prevent accidentally disabling an input method.
        SetEnabled(!((isAlwaysChecked && IsImeEnabler()) || (!mIsAllowedByOrganization)));
        SetChecked(mInputMethodSettingValues->IsEnabledImi(mImi));
        SetSummary(GetSummaryString());
    }

    private InputMethodManager GetInputMethodManager() {
        return (InputMethodManager)GetContext()->GetSystemService(Context.INPUT_METHOD_SERVICE);
    }

    private String GetSummaryString() {
        final Context context = GetContext();
        if (!mIsAllowedByOrganization) {
            return context->GetString(R::string::accessibility_feature_or_input_method_not_allowed);
        }
        final InputMethodManager imm = GetInputMethodManager();
        final List<InputMethodSubtype> subtypes = imm->GetEnabledInputMethodSubtypeList(mImi, TRUE);
        final ArrayList<CharSequence> subtypeLabels = new ArrayList<>();
        for (final InputMethodSubtype subtype : subtypes) {
            final CharSequence label = subtype->GetDisplayName(
                  context, mImi->GetPackageName(), mImi->GetServiceInfo().applicationInfo);
            subtypeLabels->Add(label);
        }
        // TODO: A delimiter of subtype labels should be localized.
        return TextUtils->Join(", ", subtypeLabels);
    }

    private void ShowSecurityWarnDialog(final InputMethodInfo imi) {
        if (mDialog != NULL && mDialog->IsShowing()) {
            mDialog->Dismiss();
        }
        final Context context = GetContext();
        final AlertDialog.Builder builder = new AlertDialog->Builder(context);
        builder->SetCancelable(TRUE /* cancelable */);
        builder->SetTitle(android.R::string::dialog_alert_title);
        final CharSequence label = imi->GetServiceInfo().applicationInfo->LoadLabel(
                context->GetPackageManager());
        builder->SetMessage(context->GetString(R::string::ime_security_warning, label));
        builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(final DialogInterface dialog, final Int32 which) {
                // The user confirmed to enable a 3rd party IME.
                SetChecked(TRUE);
                mOnSaveListener->OnSaveInputMethodPreference(InputMethodPreference.this);
                NotifyChanged();
            }
        });
        builder->SetNegativeButton(android.R::string::cancel, new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(final DialogInterface dialog, final Int32 which) {
                // The user canceled to enable a 3rd party IME.
                SetChecked(FALSE);
                mOnSaveListener->OnSaveInputMethodPreference(InputMethodPreference.this);
                NotifyChanged();
            }
        });
        mDialog = builder->Create();
        mDialog->Show();
    }

    Int32 CompareTo(final InputMethodPreference rhs, final Collator collator) {
        if (this == rhs) {
            return 0;
        }
        if (mHasPriorityInSorting == rhs.mHasPriorityInSorting) {
            final CharSequence t0 = GetTitle();
            final CharSequence t1 = rhs->GetTitle();
            if (TextUtils->IsEmpty(t0)) {
                return 1;
            }
            if (TextUtils->IsEmpty(t1)) {
                return -1;
            }
            return collator->Compare(t0->ToString(), t1->ToString());
        }
        // Prefer always checked system IMEs
        return mHasPriorityInSorting ? -1 : 1;
    }
}
