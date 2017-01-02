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

/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.accessibility;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::IToggleSwitch;

public abstract class ToggleFeaturePreferenceFragment
        extends SettingsPreferenceFragment {

    protected SwitchBar mSwitchBar;
    protected ToggleSwitch mToggleSwitch;

    protected String mPreferenceKey;
    protected Preference mSummaryPreference;

    protected CharSequence mSettingsTitle;
    protected Intent mSettingsIntent;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        PreferenceScreen preferenceScreen = GetPreferenceManager()->CreatePreferenceScreen(
                GetActivity());
        SetPreferenceScreen(preferenceScreen);
        mSummaryPreference = new Preference(GetActivity()) {
            //@Override
            protected void OnBindView(View view) {
                super->OnBindView(view);
                final TextView summaryView = (TextView) view->FindViewById(android.R.id.summary);
                summaryView->SetText(GetSummary());
                SendAccessibilityEvent(summaryView);
            }

            private void SendAccessibilityEvent(View view) {
                // Since the view is still not attached we create, populate,
                // and send the event directly since we do not know when it
                // will be attached and posting commands is not as clean.
                AccessibilityManager accessibilityManager =
                        AccessibilityManager->GetInstance(GetActivity());
                if (accessibilityManager->IsEnabled()) {
                    AccessibilityEvent event = AccessibilityEvent->Obtain();
                    event->SetEventType(AccessibilityEvent.TYPE_VIEW_FOCUSED);
                    view->OnInitializeAccessibilityEvent(event);
                    view->DispatchPopulateAccessibilityEvent(event);
                    accessibilityManager->SendAccessibilityEvent(event);
                }
            }
        };
        mSummaryPreference->SetPersistent(FALSE);
        mSummaryPreference->SetLayoutResource(R.layout.text_description_preference);
        preferenceScreen->AddPreference(mSummaryPreference);
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);

        SettingsActivity activity = (SettingsActivity) GetActivity();
        mSwitchBar = activity->GetSwitchBar();
        mToggleSwitch = mSwitchBar->GetSwitch();

        OnProcessArguments(GetArguments());
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        InstallActionBarToggleSwitch();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        RemoveActionBarToggleSwitch();
    }

    protected abstract void OnPreferenceToggled(String preferenceKey, Boolean enabled);

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);
        MenuItem menuItem = menu->Add(mSettingsTitle);
        menuItem->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        menuItem->SetIntent(mSettingsIntent);
    }

    protected void OnInstallSwitchBarToggleSwitch() {
        // Implement this to set a checked listener.
    }

    protected void OnRemoveSwitchBarToggleSwitch() {
        // Implement this to reset a checked listener.
    }

    private void InstallActionBarToggleSwitch() {
        mSwitchBar->Show();
        OnInstallSwitchBarToggleSwitch();
    }

    private void RemoveActionBarToggleSwitch() {
        mToggleSwitch->SetOnBeforeCheckedChangeListener(NULL);
        OnRemoveSwitchBarToggleSwitch();
        mSwitchBar->Hide();
    }

    CARAPI SetTitle(String title) {
        GetActivity()->SetTitle(title);
    }

    protected void OnProcessArguments(Bundle arguments) {
        if (arguments == NULL) {
            GetPreferenceScreen()->RemovePreference(mSummaryPreference);
            return;
        }

        // Key.
        mPreferenceKey = arguments->GetString(AccessibilitySettings.EXTRA_PREFERENCE_KEY);

        // Enabled.
        if (arguments->ContainsKey(AccessibilitySettings.EXTRA_CHECKED)) {
            final Boolean enabled = arguments->GetBoolean(AccessibilitySettings.EXTRA_CHECKED);
            mSwitchBar->SetCheckedInternal(enabled);
        }

        // Title.
        if (arguments->ContainsKey(AccessibilitySettings.EXTRA_TITLE)) {
            SetTitle(arguments->GetString(AccessibilitySettings.EXTRA_TITLE));
        }

        // Summary.
        if (arguments->ContainsKey(AccessibilitySettings.EXTRA_SUMMARY)) {
            final CharSequence summary = arguments->GetCharSequence(
                    AccessibilitySettings.EXTRA_SUMMARY);
            mSummaryPreference->SetSummary(summary);

            // Set a transparent drawable to prevent use of the default one.
            GetListView()->SetSelector(new ColorDrawable(Color.TRANSPARENT));
            GetListView()->SetDivider(NULL);
        } else {
            GetPreferenceScreen()->RemovePreference(mSummaryPreference);
        }
    }
}
