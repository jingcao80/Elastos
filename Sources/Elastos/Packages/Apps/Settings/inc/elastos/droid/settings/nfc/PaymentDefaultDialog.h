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

package com.android.settings.nfc;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Nfc::Cardemulation::ICardEmulation;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Nfc::PaymentBackend::IPaymentAppInfo;

using Elastos::Utility::IList;

public class PaymentDefaultDialog extends AlertActivity implements
        DialogInterface.OnClickListener {

    public static const String TAG = "PaymentDefaultDialog";
    private static const Int32 PAYMENT_APP_MAX_CAPTION_LENGTH = 40;

    private PaymentBackend mBackend;
    private ComponentName mNewDefault;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mBackend = new PaymentBackend(this);
        Intent intent = GetIntent();
        ComponentName component = intent->GetParcelableExtra(
                CardEmulation.EXTRA_SERVICE_COMPONENT);
        String category = intent->GetStringExtra(CardEmulation.EXTRA_CATEGORY);

        SetResult(RESULT_CANCELED);
        if (!BuildDialog(component, category)) {
            Finish();
        }

    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case BUTTON_POSITIVE:
                mBackend->SetDefaultPaymentApp(mNewDefault);
                SetResult(RESULT_OK);
                break;
            case BUTTON_NEGATIVE:
                break;
        }
    }

    private Boolean BuildDialog(ComponentName component, String category) {
        if (component == NULL || category == NULL) {
            Logger::E(TAG, "Component or category are NULL");
            return FALSE;
        }

        if (!CardEmulation.CATEGORY_PAYMENT->Equals(category)) {
            Logger::E(TAG, "Don't support defaults for category " + category);
            return FALSE;
        }

        // Check if passed in service exists
        PaymentAppInfo requestedPaymentApp = NULL;
        PaymentAppInfo defaultPaymentApp = NULL;

        List<PaymentAppInfo> services = mBackend->GetPaymentAppInfos();
        for (PaymentAppInfo service : services) {
            if (component->Equals(service.componentName)) {
                requestedPaymentApp = service;
            }
            if (service.isDefault) {
                defaultPaymentApp = service;
            }
        }

        if (requestedPaymentApp == NULL) {
            Logger::E(TAG, "Component " + component + " is not a registered payment service.");
            return FALSE;
        }

        // Get current mode and default component
        ComponentName defaultComponent = mBackend->GetDefaultPaymentApp();
        if (defaultComponent != NULL && defaultComponent->Equals(component)) {
            Logger::E(TAG, "Component " + component + " is already default.");
            return FALSE;
        }

        mNewDefault = component;
        // Compose dialog; get
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::nfc_payment_set_default_label);
        if (defaultPaymentApp == NULL) {
            String formatString = GetString(R::string::nfc_payment_set_default);
            String msg = String->Format(formatString,
                    SanitizePaymentAppCaption(requestedPaymentApp.caption->ToString()));
            p.mMessage = msg;
        } else {
            String formatString = GetString(R::string::nfc_payment_set_default_instead_of);
            String msg = String->Format(formatString,
                    SanitizePaymentAppCaption(requestedPaymentApp.caption->ToString()),
                    SanitizePaymentAppCaption(defaultPaymentApp.caption->ToString()));
            p.mMessage = msg;
        }
        p.mPositiveButtonText = GetString(R::string::yes);
        p.mNegativeButtonText = GetString(R::string::no);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonListener = this;
        SetupAlert();

        return TRUE;
    }

    private String SanitizePaymentAppCaption(String input) {
        String sanitizedString = input->Replace('\n', ' ').Replace('\r', ' ').Trim();


        if (sanitizedString->Length() > PAYMENT_APP_MAX_CAPTION_LENGTH) {
            return sanitizedString->Substring(0, PAYMENT_APP_MAX_CAPTION_LENGTH);
        }

        return sanitizedString;
    }

}
