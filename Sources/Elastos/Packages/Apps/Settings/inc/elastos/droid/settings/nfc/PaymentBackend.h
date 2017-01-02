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

package com.android.settings.nfc;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Nfc::Cardemulation::IApduServiceInfo;
using Elastos::Droid::Nfc::Cardemulation::ICardEmulation;
using Elastos::Droid::Provider::ISettings;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class PaymentBackend {
    public static const String TAG = "Settings.PaymentBackend";

    public static class PaymentAppInfo {
        CharSequence caption;
        Drawable banner;
        Boolean isDefault;
        public ComponentName componentName;
    }

    private final Context mContext;
    private final NfcAdapter mAdapter;
    private final CardEmulation mCardEmuManager;

    public PaymentBackend(Context context) {
        mContext = context;

        mAdapter = NfcAdapter->GetDefaultAdapter(context);
        mCardEmuManager = CardEmulation->GetInstance(mAdapter);
    }

    public List<PaymentAppInfo> GetPaymentAppInfos() {
        PackageManager pm = mContext->GetPackageManager();
        List<ApduServiceInfo> serviceInfos =
                mCardEmuManager->GetServices(CardEmulation.CATEGORY_PAYMENT);
        List<PaymentAppInfo> appInfos = new ArrayList<PaymentAppInfo>();

        if (serviceInfos == NULL) return appInfos;

        ComponentName defaultApp = GetDefaultPaymentApp();

        for (ApduServiceInfo service : serviceInfos) {
            PaymentAppInfo appInfo = new PaymentAppInfo();
            appInfo.banner = service->LoadBanner(pm);
            appInfo.caption = service->GetDescription();
            if (appInfo.caption == NULL) {
                appInfo.caption = service->LoadLabel(pm);
            }
            appInfo.isDefault = service->GetComponent()->Equals(defaultApp);
            appInfo.componentName = service->GetComponent();
            appInfos->Add(appInfo);
        }

        return appInfos;
    }

    Boolean IsForegroundMode() {
        try {
            return Settings.Secure->GetInt(mContext->GetContentResolver(),
                    Settings.Secure.NFC_PAYMENT_FOREGROUND) != 0;
        } catch (SettingNotFoundException e) {
            return FALSE;
        }
    }

    void SetForegroundMode(Boolean foreground) {
        Settings.Secure->PutInt(mContext->GetContentResolver(),
                Settings.Secure.NFC_PAYMENT_FOREGROUND, foreground ? 1 : 0) ;
    }

    ComponentName GetDefaultPaymentApp() {
        String componentString = Settings.Secure->GetString(mContext->GetContentResolver(),
                Settings.Secure.NFC_PAYMENT_DEFAULT_COMPONENT);
        if (componentString != NULL) {
            return ComponentName->UnflattenFromString(componentString);
        } else {
            return NULL;
        }
    }

    CARAPI SetDefaultPaymentApp(ComponentName app) {
        Settings.Secure->PutString(mContext->GetContentResolver(),
                Settings.Secure.NFC_PAYMENT_DEFAULT_COMPONENT,
                app != NULL ? app->FlattenToString() : NULL);
    }
}