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

using Elastos::Droid::Accessibilityservice::IAccessibilityServiceInfo;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::IConfirmDeviceCredentialActivity;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Widget::IToggleSwitch;
using Elastos::Droid::Settings::Widget::ToggleSwitch::IOnBeforeCheckedChangeListener;

using Elastos::Utility::ICollections;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

public class ToggleAccessibilityServicePreferenceFragment
        extends ToggleFeaturePreferenceFragment implements DialogInterface.OnClickListener {

    private static const Int32 DIALOG_ID_ENABLE_WARNING = 1;
    private static const Int32 DIALOG_ID_DISABLE_WARNING = 2;

    public static const Int32 ACTIVITY_REQUEST_CONFIRM_CREDENTIAL_FOR_WEAKER_ENCRYPTION = 1;

    private LockPatternUtils mLockPatternUtils;

    private final SettingsContentObserver mSettingsContentObserver =
            new SettingsContentObserver(new Handler()) {
            //@Override
                CARAPI OnChange(Boolean selfChange, Uri uri) {
                    String settingValue = Settings.Secure->GetString(GetContentResolver(),
                            Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES);
                    final Boolean enabled = settingValue->Contains(mComponentName->FlattenToString());
                    mSwitchBar->SetCheckedInternal(enabled);
                }
            };

    private ComponentName mComponentName;

    private Int32 mShownDialogId;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mLockPatternUtils = new LockPatternUtils(GetActivity());
    }

    //@Override
    CARAPI OnResume() {
        mSettingsContentObserver->Register(GetContentResolver());
        super->OnResume();
    }

    //@Override
    CARAPI OnPause() {
        mSettingsContentObserver->Unregister(GetContentResolver());
        super->OnPause();
    }

    //@Override
    CARAPI OnPreferenceToggled(String preferenceKey, Boolean enabled) {
        // Parse the enabled services.
        Set<ComponentName> enabledServices = AccessibilityUtils->GetEnabledServicesFromSettings(
                GetActivity());

        if (enabledServices == (Set<?>) Collections->EmptySet()) {
            enabledServices = new HashSet<ComponentName>();
        }

        // Determine enabled services and accessibility state.
        ComponentName toggledService = ComponentName->UnflattenFromString(preferenceKey);
        Boolean accessibilityEnabled = FALSE;
        if (enabled) {
            enabledServices->Add(toggledService);
            // Enabling at least one service enables accessibility.
            accessibilityEnabled = TRUE;
        } else {
            enabledServices->Remove(toggledService);
            // Check how many enabled and installed services are present.
            Set<ComponentName> installedServices = AccessibilitySettings.sInstalledServices;
            for (ComponentName enabledService : enabledServices) {
                if (installedServices->Contains(enabledService)) {
                    // Disabling the last service disables accessibility.
                    accessibilityEnabled = TRUE;
                    break;
                }
            }
        }

        // Update the enabled services setting.
        StringBuilder enabledServicesBuilder = new StringBuilder();
        // Keep the enabled services even if they are not installed since we
        // have no way to know whether the application restore process has
        // completed. In general the system should be responsible for the
        // clean up not settings.
        for (ComponentName enabledService : enabledServices) {
            enabledServicesBuilder->Append(enabledService->FlattenToString());
            enabledServicesBuilder->Append(
                    AccessibilitySettings.ENABLED_ACCESSIBILITY_SERVICES_SEPARATOR);
        }
        final Int32 enabledServicesBuilderLength = enabledServicesBuilder->Length();
        if (enabledServicesBuilderLength > 0) {
            enabledServicesBuilder->DeleteCharAt(enabledServicesBuilderLength - 1);
        }
        Settings.Secure->PutString(GetContentResolver(),
                Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES,
                enabledServicesBuilder->ToString());

        // Update accessibility enabled.
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_ENABLED, accessibilityEnabled ? 1 : 0);
    }

    // IMPORTANT: Refresh the info since there are dynamically changing
    // capabilities. For
    // example, before JellyBean MR2 the user was granting the explore by touch
    // one.
    private AccessibilityServiceInfo GetAccessibilityServiceInfo() {
        List<AccessibilityServiceInfo> serviceInfos = AccessibilityManager->GetInstance(
                GetActivity()).GetInstalledAccessibilityServiceList();
        final Int32 serviceInfoCount = serviceInfos->Size();
        for (Int32 i = 0; i < serviceInfoCount; i++) {
            AccessibilityServiceInfo serviceInfo = serviceInfos->Get(i);
            ResolveInfo resolveInfo = serviceInfo->GetResolveInfo();
            if (mComponentName->GetPackageName()->Equals(resolveInfo.serviceInfo.packageName)
                    && mComponentName->GetClassName()->Equals(resolveInfo.serviceInfo.name)) {
                return serviceInfo;
            }
        }
        return NULL;
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        switch (dialogId) {
            case DIALOG_ID_ENABLE_WARNING: {
                mShownDialogId = DIALOG_ID_ENABLE_WARNING;
                AccessibilityServiceInfo info = GetAccessibilityServiceInfo();
                if (info == NULL) {
                    return NULL;
                }
                AlertDialog ad = new AlertDialog->Builder(GetActivity())
                        .SetTitle(GetString(R::string::enable_service_title,
                                info->GetResolveInfo()->LoadLabel(GetPackageManager())))
                        .SetView(CreateEnableDialogContentView(info))
                        .SetCancelable(TRUE)
                        .SetPositiveButton(android.R::string::ok, this)
                        .SetNegativeButton(android.R::string::cancel, this)
                        .Create();
                ad->Create();
                ad->GetButton(AlertDialog.BUTTON_POSITIVE).SetFilterTouchesWhenObscured(TRUE);
                return ad;
            }
            case DIALOG_ID_DISABLE_WARNING: {
                mShownDialogId = DIALOG_ID_DISABLE_WARNING;
                AccessibilityServiceInfo info = GetAccessibilityServiceInfo();
                if (info == NULL) {
                    return NULL;
                }
                return new AlertDialog->Builder(GetActivity())
                        .SetTitle(GetString(R::string::disable_service_title,
                                info->GetResolveInfo()->LoadLabel(GetPackageManager())))
                        .SetMessage(GetString(R::string::disable_service_message,
                                info->GetResolveInfo()->LoadLabel(GetPackageManager())))
                        .SetCancelable(TRUE)
                        .SetPositiveButton(android.R::string::ok, this)
                        .SetNegativeButton(android.R::string::cancel, this)
                        .Create();
            }
            default: {
                throw new IllegalArgumentException();
            }
        }
    }

    private View CreateEnableDialogContentView(AccessibilityServiceInfo info) {
        LayoutInflater inflater = (LayoutInflater) GetSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        View content = inflater->Inflate(R.layout.enable_accessibility_service_dialog_content,
                NULL);

        TextView encryptionWarningView = (TextView) content->FindViewById(
                R.id.encryption_warning);
        if (LockPatternUtils->IsDeviceEncrypted()) {
            String text = GetString(R::string::enable_service_encryption_warning,
                    info->GetResolveInfo()->LoadLabel(GetPackageManager()));
            encryptionWarningView->SetText(text);
            encryptionWarningView->SetVisibility(View.VISIBLE);
        } else {
            encryptionWarningView->SetVisibility(View.GONE);
        }

        TextView capabilitiesHeaderView = (TextView) content->FindViewById(
                R.id.capabilities_header);
        capabilitiesHeaderView->SetText(GetString(R::string::capabilities_list_title,
                info->GetResolveInfo()->LoadLabel(GetPackageManager())));

        LinearLayout capabilitiesView = (LinearLayout) content->FindViewById(R.id.capabilities);

        // This capability is implicit for all services.
        View capabilityView = inflater->Inflate(
                R.layout.app_permission_item_old, NULL);

        ImageView imageView = (ImageView) capabilityView->FindViewById(
                R.id.perm_icon);
        imageView->SetImageDrawable(GetResources()->GetDrawable(
                R.drawable.ic_text_dot));

        TextView labelView = (TextView) capabilityView->FindViewById(
                R.id.permission_group);
        labelView->SetText(GetString(R::string::capability_title_receiveAccessibilityEvents));

        TextView descriptionView = (TextView) capabilityView->FindViewById(
                R.id.permission_list);
        descriptionView->SetText(GetString(R::string::capability_desc_receiveAccessibilityEvents));

        List<AccessibilityServiceInfo.CapabilityInfo> capabilities =
                info->GetCapabilityInfos();

        capabilitiesView->AddView(capabilityView);

        // Service specific capabilities.
        final Int32 capabilityCount = capabilities->Size();
        for (Int32 i = 0; i < capabilityCount; i++) {
            AccessibilityServiceInfo.CapabilityInfo capability = capabilities->Get(i);

            capabilityView = inflater->Inflate(
                    R.layout.app_permission_item_old, NULL);

            imageView = (ImageView) capabilityView->FindViewById(
                    R.id.perm_icon);
            imageView->SetImageDrawable(GetResources()->GetDrawable(
                    R.drawable.ic_text_dot));

            labelView = (TextView) capabilityView->FindViewById(
                    R.id.permission_group);
            labelView->SetText(GetString(capability.titleResId));

            descriptionView = (TextView) capabilityView->FindViewById(
                    R.id.permission_list);
            descriptionView->SetText(GetString(capability.descResId));

            capabilitiesView->AddView(capabilityView);
        }

        return content;
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == ACTIVITY_REQUEST_CONFIRM_CREDENTIAL_FOR_WEAKER_ENCRYPTION) {
            if (resultCode == Activity.RESULT_OK) {
                HandleConfirmServiceEnabled(TRUE);
                // The user confirmed that they accept weaker encryption when
                // enabling the accessibility service, so change encryption.
                // Since we came here asynchronously, check encryption again.
                if (LockPatternUtils->IsDeviceEncrypted()) {
                    mLockPatternUtils->ClearEncryptionPassword();
                    Settings::Global::>PutInt(GetContentResolver(),
                            Settings::Global::REQUIRE_PASSWORD_TO_DECRYPT, 0);
                }
            } else {
                HandleConfirmServiceEnabled(FALSE);
            }
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        final Boolean checked;
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                if (mShownDialogId == DIALOG_ID_ENABLE_WARNING) {
                    if (LockPatternUtils->IsDeviceEncrypted()) {
                        String title = CreateConfirmCredentialReasonMessage();
                        Intent intent = ConfirmDeviceCredentialActivity->CreateIntent(title, NULL);
                        StartActivityForResult(intent,
                                ACTIVITY_REQUEST_CONFIRM_CREDENTIAL_FOR_WEAKER_ENCRYPTION);
                    } else {
                        HandleConfirmServiceEnabled(TRUE);
                    }
                } else {
                    HandleConfirmServiceEnabled(FALSE);
                }
                break;
            case DialogInterface.BUTTON_NEGATIVE:
                checked = (mShownDialogId == DIALOG_ID_DISABLE_WARNING);
                HandleConfirmServiceEnabled(checked);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private void HandleConfirmServiceEnabled(Boolean confirmed) {
        mSwitchBar->SetCheckedInternal(confirmed);
        GetArguments()->PutBoolean(AccessibilitySettings.EXTRA_CHECKED, confirmed);
        OnPreferenceToggled(mPreferenceKey, confirmed);
    }

    private String CreateConfirmCredentialReasonMessage() {
        Int32 resId = R::string::enable_service_password_reason;
        switch (mLockPatternUtils->GetKeyguardStoredPasswordQuality()) {
            case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING: {
                resId = R::string::enable_service_pattern_reason;
            } break;
            case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
            case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX: {
                resId = R::string::enable_service_pin_reason;
            } break;
        }
        return GetString(resId, GetAccessibilityServiceInfo()->GetResolveInfo()
                .LoadLabel(GetPackageManager()));
    }

    //@Override
    protected void OnInstallSwitchBarToggleSwitch() {
        super->OnInstallSwitchBarToggleSwitch();
        mToggleSwitch->SetOnBeforeCheckedChangeListener(new OnBeforeCheckedChangeListener() {
                //@Override
            public Boolean OnBeforeCheckedChanged(ToggleSwitch toggleSwitch, Boolean checked) {
                if (checked) {
                    mSwitchBar->SetCheckedInternal(FALSE);
                    GetArguments()->PutBoolean(AccessibilitySettings.EXTRA_CHECKED, FALSE);
                    ShowDialog(DIALOG_ID_ENABLE_WARNING);
                } else {
                    mSwitchBar->SetCheckedInternal(TRUE);
                    GetArguments()->PutBoolean(AccessibilitySettings.EXTRA_CHECKED, TRUE);
                    ShowDialog(DIALOG_ID_DISABLE_WARNING);
                }
                return TRUE;
            }
        });
    }

    //@Override
    protected void OnProcessArguments(Bundle arguments) {
        super->OnProcessArguments(arguments);
        // Settings title and intent.
        String settingsTitle = arguments->GetString(AccessibilitySettings.EXTRA_SETTINGS_TITLE);
        String settingsComponentName = arguments->GetString(
                AccessibilitySettings.EXTRA_SETTINGS_COMPONENT_NAME);
        if (!TextUtils->IsEmpty(settingsTitle) && !TextUtils->IsEmpty(settingsComponentName)) {
            Intent settingsIntent = new Intent(IIntent::ACTION_MAIN).SetComponent(
                    ComponentName->UnflattenFromString(settingsComponentName->ToString()));
            if (!GetPackageManager()->QueryIntentActivities(settingsIntent, 0).IsEmpty()) {
                mSettingsTitle = settingsTitle;
                mSettingsIntent = settingsIntent;
                SetHasOptionsMenu(TRUE);
            }
        }

        mComponentName = arguments->GetParcelable(AccessibilitySettings.EXTRA_COMPONENT_NAME);
    }
}
