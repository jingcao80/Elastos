

package com.android.settings.tts;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Speech::Tts::TextToSpeech::IEngineInfo;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IRadioButton;


using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;


public class TtsEnginePreference extends Preference {

    private static const String TAG = "TtsEnginePreference";

    /**
     * Key for the name of the TTS engine passed in to the engine
     * settings fragment {@link TtsEngineSettingsFragment}.
     */
    static const String FRAGMENT_ARGS_NAME = "name";

    /**
     * Key for the label of the TTS engine passed in to the engine
     * settings fragment. This is used as the title of the fragment
     * {@link TtsEngineSettingsFragment}.
     */
    static const String FRAGMENT_ARGS_LABEL = "label";

    /**
     * Key for the voice data data passed in to the engine settings
     * fragmetn {@link TtsEngineSettingsFragment}.
     */
    static const String FRAGMENT_ARGS_VOICES = "voices";

    /**
     * The preference activity that owns this preference. Required
     * for instantiating the engine specific settings screen.
     */
    private final SettingsActivity mSettingsActivity;

    /**
     * The engine information for the engine this preference represents.
     * Contains it's name, label etc. which are used for display.
     */
    private final EngineInfo mEngineInfo;

    /**
     * The shared radio button state, which button is checked etc.
     */
    private final RadioButtonGroupState mSharedState;

    /**
     * When TRUE, the change callbacks on the radio button will not
     * fire.
     */
    private volatile Boolean mPreventRadioButtonCallbacks;

    private View mSettingsIcon;
    private RadioButton mRadioButton;
    private Intent mVoiceCheckData;

    private final CompoundButton.OnCheckedChangeListener mRadioChangeListener =
        new CompoundButton->OnCheckedChangeListener() {
            //@Override
            CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                OnRadioButtonClicked(buttonView, isChecked);
            }
        };

    public TtsEnginePreference(Context context, EngineInfo info, RadioButtonGroupState state,
            SettingsActivity prefActivity) {
        Super(context);
        SetLayoutResource(R.layout.preference_tts_engine);

        mSharedState = state;
        mSettingsActivity = prefActivity;
        mEngineInfo = info;
        mPreventRadioButtonCallbacks = FALSE;

        SetKey(mEngineInfo.name);
        SetTitle(mEngineInfo.label);
    }

    //@Override
    public View GetView(View convertView, ViewGroup parent) {
        if (mSharedState == NULL) {
            throw new IllegalStateException("Call to GetView() before a call to" +
                    "SetSharedState()");
        }

        View view = super->GetView(convertView, parent);
        final RadioButton rb = (RadioButton) view->FindViewById(R.id.tts_engine_radiobutton);
        rb->SetOnCheckedChangeListener(mRadioChangeListener);

        Boolean isChecked = GetKey()->Equals(mSharedState->GetCurrentKey());
        if (isChecked) {
            mSharedState->SetCurrentChecked(rb);
        }

        mPreventRadioButtonCallbacks = TRUE;
        rb->SetChecked(isChecked);
        mPreventRadioButtonCallbacks = FALSE;

        mRadioButton = rb;

        View textLayout = view->FindViewById(R.id.tts_engine_pref_text);
        textLayout->SetOnClickListener(new View->OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                OnRadioButtonClicked(rb, !rb->IsChecked());
            }
        });

        mSettingsIcon = view->FindViewById(R.id.tts_engine_settings);
        // Will be enabled only the engine has passed the voice check, and
        // is currently enabled.
        mSettingsIcon->SetEnabled(isChecked && mVoiceCheckData != NULL);
        if (!isChecked) {
            mSettingsIcon->SetAlpha(Utils.DISABLED_ALPHA);
        }
        mSettingsIcon->SetOnClickListener(new View->OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                Bundle args = new Bundle();
                args->PutString(FRAGMENT_ARGS_NAME, mEngineInfo.name);
                args->PutString(FRAGMENT_ARGS_LABEL, mEngineInfo.label);
                if (mVoiceCheckData != NULL) {
                    args->PutParcelable(FRAGMENT_ARGS_VOICES, mVoiceCheckData);
                }

                // Note that we use this instead of the (easier to use)
                // SettingsActivity.startPreferenceFragment because the
                // title will not be updated correctly in the fragment
                // breadcrumb since it isn't inflated from the XML layout.
                mSettingsActivity->StartPreferencePanel(
                        TtsEngineSettingsFragment.class->GetName(),
                        args, 0, mEngineInfo.label, NULL, 0);
            }
        });

        if (mVoiceCheckData != NULL) {
            mSettingsIcon->SetEnabled(mRadioButton->IsChecked());
        }

        return view;
    }

    CARAPI SetVoiceDataDetails(Intent data) {
        mVoiceCheckData = data;
        // This might end up running before getView aboive, in which
        // case mSettingsIcon && mRadioButton will be NULL. In this case
        // getView will set the right values.
        if (mSettingsIcon != NULL && mRadioButton != NULL) {
            if (mRadioButton->IsChecked()) {
                mSettingsIcon->SetEnabled(TRUE);
            } else {
                mSettingsIcon->SetEnabled(FALSE);
                mSettingsIcon->SetAlpha(Utils.DISABLED_ALPHA);
            }
        }
    }

    private Boolean ShouldDisplayDataAlert() {
        return !mEngineInfo.system;
    }


    private void DisplayDataAlert(
            DialogInterface.OnClickListener positiveOnClickListener,
            DialogInterface.OnClickListener negativeOnClickListener) {
        Logger::I(TAG, "Displaying data alert for :" + mEngineInfo.name);

        AlertDialog.Builder builder = new AlertDialog->Builder(GetContext());
        builder->SetTitle(android.R::string::dialog_alert_title)
                .SetMessage(GetContext()->GetString(
                        R::string::tts_engine_security_warning, mEngineInfo.label))
                .SetCancelable(TRUE)
                .SetPositiveButton(android.R::string::ok, positiveOnClickListener)
                .SetNegativeButton(android.R::string::cancel, negativeOnClickListener);

        AlertDialog dialog = builder->Create();
        dialog->Show();
    }


    private void OnRadioButtonClicked(final CompoundButton buttonView,
            Boolean isChecked) {
        if (mPreventRadioButtonCallbacks ||
                (mSharedState->GetCurrentChecked() == buttonView)) {
            return;
        }

        if (isChecked) {
            // Should we alert user? if that's TRUE, delay making engine current one.
            if (ShouldDisplayDataAlert()) {
                DisplayDataAlert(new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        MakeCurrentEngine(buttonView);
                    }
                },new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        // Undo the click.
                        buttonView->SetChecked(FALSE);
                    }
                });
            } else {
                // Privileged engine, set it current
                MakeCurrentEngine(buttonView);
            }
        } else {
            mSettingsIcon->SetEnabled(FALSE);
        }
    }

    private void MakeCurrentEngine(Checkable current) {
        if (mSharedState->GetCurrentChecked() != NULL) {
            mSharedState->GetCurrentChecked()->SetChecked(FALSE);
        }
        mSharedState->SetCurrentChecked(current);
        mSharedState->SetCurrentKey(GetKey());
        CallChangeListener(mSharedState->GetCurrentKey());
        mSettingsIcon->SetEnabled(TRUE);
    }


    /**
     * Holds all state that is common to this group of radio buttons, such
     * as the currently selected key and the currently checked compound button.
     * (which corresponds to this key).
     */
    public interface RadioButtonGroupState {
        String GetCurrentKey();
        Checkable GetCurrentChecked();

        void SetCurrentKey(String key);
        void SetCurrentChecked(Checkable current);
    }

}
