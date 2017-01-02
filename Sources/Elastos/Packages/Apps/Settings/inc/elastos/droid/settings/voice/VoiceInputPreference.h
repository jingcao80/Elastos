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

package com.android.settings.voice;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IRadioButton;


using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

public class VoiceInputPreference extends Preference {

    private static const String TAG = "VoiceInputPreference";

    private final CharSequence mLabel;

    private final CharSequence mAppLabel;

    private final CharSequence mAlertText;

    private final ComponentName mSettingsComponent;

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

    private final CompoundButton.OnCheckedChangeListener mRadioChangeListener =
        new CompoundButton->OnCheckedChangeListener() {
            //@Override
            CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                OnRadioButtonClicked(buttonView, isChecked);
            }
        };

    public VoiceInputPreference(Context context, VoiceInputHelper.BaseInfo info,
            CharSequence summary, CharSequence alertText, RadioButtonGroupState state) {
        Super(context);
        SetLayoutResource(R.layout.preference_tts_engine);

        mSharedState = state;
        mLabel = info.label;
        mAppLabel = info.appLabel;
        mAlertText = alertText;
        mSettingsComponent = info.settings;
        mPreventRadioButtonCallbacks = FALSE;

        SetKey(info.key);
        SetTitle(info.label);
        SetSummary(summary);
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
        mSettingsIcon->SetOnClickListener(new View->OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                Intent intent = new Intent(IIntent::ACTION_MAIN);
                intent->SetComponent(mSettingsComponent);
                GetContext()->StartActivity(new Intent(intent));
            }
        });
        UpdateCheckedState(isChecked);

        return view;
    }

    private Boolean ShouldDisplayAlert() {
        return mAlertText != NULL;
    }

    private void DisplayAlert(
            final DialogInterface.OnClickListener positiveOnClickListener,
            final DialogInterface.OnClickListener negativeOnClickListener) {
        Logger::I(TAG, "Displaying data alert for :" + GetKey());

        AlertDialog.Builder builder = new AlertDialog->Builder(GetContext());
        String msg = String->Format(GetContext()->GetResources().GetConfiguration().locale,
                mAlertText->ToString(), mAppLabel);
        builder->SetTitle(android.R::string::dialog_alert_title)
                .SetMessage(msg)
                .SetCancelable(TRUE)
                .SetPositiveButton(android.R::string::ok, positiveOnClickListener)
                .SetNegativeButton(android.R::string::cancel, negativeOnClickListener)
                .SetOnCancelListener(new DialogInterface->OnCancelListener() {
                    //@Override CARAPI OnCancel(DialogInterface dialog) {
                        negativeOnClickListener->OnClick(dialog, DialogInterface.BUTTON_NEGATIVE);
                    }
                });

        AlertDialog dialog = builder->Create();
        dialog->Show();
    }

    CARAPI DoClick() {
        mRadioButton->PerformClick();
    }

    void UpdateCheckedState(Boolean isChecked) {
        if (mSettingsComponent != NULL) {
            mSettingsIcon->SetVisibility(View.VISIBLE);
            if (isChecked) {
                mSettingsIcon->SetEnabled(TRUE);
                mSettingsIcon->SetAlpha(1);
            } else {
                mSettingsIcon->SetEnabled(FALSE);
                mSettingsIcon->SetAlpha(Utils.DISABLED_ALPHA);
            }
        } else {
            mSettingsIcon->SetVisibility(View.GONE);
        }
    }

    void OnRadioButtonClicked(final CompoundButton buttonView, Boolean isChecked) {
        if (mPreventRadioButtonCallbacks) {
            return;
        }
        if (mSharedState->GetCurrentChecked() == buttonView) {
            UpdateCheckedState(isChecked);
            return;
        }

        if (isChecked) {
            // Should we alert user? if that's TRUE, delay making engine current one.
            if (ShouldDisplayAlert()) {
                DisplayAlert(new DialogInterface->OnClickListener() {
                                 //@Override
                                 CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                     MakeCurrentChecked(buttonView);
                                 }
                             }, new DialogInterface->OnClickListener() {
                                 //@Override
                                 CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                     // Undo the click.
                                     buttonView->SetChecked(FALSE);
                                 }
                             }
                );
            } else {
                // Privileged engine, set it current
                MakeCurrentChecked(buttonView);
            }
        } else {
            UpdateCheckedState(isChecked);
        }
    }

    void MakeCurrentChecked(Checkable current) {
        if (mSharedState->GetCurrentChecked() != NULL) {
            mSharedState->GetCurrentChecked()->SetChecked(FALSE);
        }
        mSharedState->SetCurrentChecked(current);
        mSharedState->SetCurrentKey(GetKey());
        UpdateCheckedState(TRUE);
        CallChangeListener(mSharedState->GetCurrentKey());
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
