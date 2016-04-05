

package com.android.settings.inputmethod;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::Textservice::ISpellCheckerInfo;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

/**
 * Spell checker service preference.
 *
 * This preference represents a spell checker service. It is used for two purposes. 1) A radio
 * button on the left side is used to choose the current spell checker service. 2) A settings
 * icon on the right side is used to invoke the setting activity of the spell checker service.
 */
class SpellCheckerPreference extends Preference implements OnClickListener {
    interface OnRadioButtonPreferenceListener {
        /**
         * Called when this preference needs to be saved its state.
         *
         * Note that this preference is non-persistent and needs explicitly to be saved its state.
         * Because changing one IME state may change other IMEs' state, this is a place to update
         * other IMEs' state as well.
         *
         * @param pref This preference.
         */
        CARAPI OnRadioButtonClicked(SpellCheckerPreference pref);
    }

    private final SpellCheckerInfo mSci;
    private final OnRadioButtonPreferenceListener mOnRadioButtonListener;

    private RadioButton mRadioButton;
    private View mPrefLeftButton;
    private View mSettingsButton;
    private Boolean mSelected;

    public SpellCheckerPreference(final Context context, final SpellCheckerInfo sci,
            final OnRadioButtonPreferenceListener onRadioButtonListener) {
        Super(context, NULL, 0);
        SetPersistent(FALSE);
        SetLayoutResource(R.layout.preference_spellchecker);
        SetWidgetLayoutResource(R.layout.preference_spellchecker_widget);
        mSci = sci;
        mOnRadioButtonListener = onRadioButtonListener;
        SetKey(sci->GetId());
        SetTitle(sci->LoadLabel(context->GetPackageManager()));
        final String settingsActivity = mSci->GetSettingsActivity();
        if (TextUtils->IsEmpty(settingsActivity)) {
            SetIntent(NULL);
        } else {
            final Intent intent = new Intent(IIntent::ACTION_MAIN);
            intent->SetClassName(mSci->GetPackageName(), settingsActivity);
            SetIntent(intent);
        }
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        mRadioButton = (RadioButton)view->FindViewById(R.id.pref_radio);
        mPrefLeftButton = view->FindViewById(R.id.pref_left_button);
        mPrefLeftButton->SetOnClickListener(this);
        mSettingsButton = view->FindViewById(R.id.pref_right_button);
        mSettingsButton->SetOnClickListener(this);
        UpdateSelectedState(mSelected);
    }

    //@Override
    CARAPI OnClick(final View v) {
        if (v == mPrefLeftButton) {
            mOnRadioButtonListener->OnRadioButtonClicked(this);
            return;
        }
        if (v == mSettingsButton) {
            OnSettingsButtonClicked();
            return;
        }
    }

    private void OnSettingsButtonClicked() {
        final Context context = GetContext();
        try {
            final Intent intent = GetIntent();
            if (intent != NULL) {
                // Invoke a settings activity of an spell checker.
                context->StartActivity(intent);
            }
        } catch (final ActivityNotFoundException e) {
            final String message = context->GetString(R::string::failed_to_open_app_settings_toast,
                    mSci->LoadLabel(context->GetPackageManager()));
            Toast->MakeText(context, message, Toast.LENGTH_LONG).Show();
        }
    }

    public SpellCheckerInfo GetSpellCheckerInfo() {
        return mSci;
    }

    CARAPI SetSelected(final Boolean selected) {
        mSelected = selected;
        UpdateSelectedState(selected);
    }

    private void UpdateSelectedState(final Boolean selected) {
        if (mRadioButton != NULL) {
            mRadioButton->SetChecked(selected);
            EnableSettingsButton(IsEnabled() && selected);
        }
    }

    private void EnableSettingsButton(final Boolean enabled) {
        if (mSettingsButton == NULL) {
            return;
        }
        if (GetIntent() == NULL) {
            mSettingsButton->SetVisibility(View.GONE);
        } else {
            mSettingsButton->SetEnabled(enabled);
            mSettingsButton->SetClickable(enabled);
            mSettingsButton->SetFocusable(enabled);
            if (!enabled) {
                mSettingsButton->SetAlpha(Utils.DISABLED_ALPHA);
            }
        }
    }
}
