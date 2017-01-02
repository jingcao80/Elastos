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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Settings::IHelpUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Nfc::PaymentBackend::IPaymentAppInfo;

using Elastos::Utility::IList;

public class PaymentSettings extends SettingsPreferenceFragment implements
        OnClickListener, OnPreferenceChangeListener {
    public static const String TAG = "PaymentSettings";
    private LayoutInflater mInflater;
    private PaymentBackend mPaymentBackend;
    private final PackageMonitor mSettingsPackageMonitor = new SettingsPackageMonitor();


    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mPaymentBackend = new PaymentBackend(GetActivity());
        mInflater = (LayoutInflater) GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        SetHasOptionsMenu(TRUE);
    }

    CARAPI Refresh() {
        PreferenceManager manager = GetPreferenceManager();
        PreferenceScreen screen = manager->CreatePreferenceScreen(GetActivity());

        // Get all payment services
        List<PaymentAppInfo> appInfos = mPaymentBackend->GetPaymentAppInfos();
        if (appInfos != NULL && appInfos->Size() > 0) {
            // Add all payment apps
            for (PaymentAppInfo appInfo : appInfos) {
                PaymentAppPreference preference =
                        new PaymentAppPreference(GetActivity(), appInfo, this);
                preference->SetTitle(appInfo.caption);
                if (appInfo.banner != NULL) {
                    screen->AddPreference(preference);
                } else {
                    // Ignore, no banner
                    Logger::E(TAG, "Couldn't load banner drawable of service " + appInfo.componentName);
                }
            }
        }
        TextView emptyText = (TextView) GetView()->FindViewById(R.id.nfc_payment_empty_text);
        TextView learnMore = (TextView) GetView()->FindViewById(R.id.nfc_payment_learn_more);
        ImageView emptyImage = (ImageView) GetView()->FindViewById(R.id.nfc_payment_tap_image);
        if (screen->GetPreferenceCount() == 0) {
            emptyText->SetVisibility(View.VISIBLE);
            learnMore->SetVisibility(View.VISIBLE);
            emptyImage->SetVisibility(View.VISIBLE);
            GetListView()->SetVisibility(View.GONE);
        } else {
            CheckBoxPreference foreground = new CheckBoxPreference(GetActivity());
            Boolean foregroundMode = mPaymentBackend->IsForegroundMode();
            foreground->SetPersistent(FALSE);
            foreground->SetTitle(GetString(R::string::nfc_payment_favor_foreground));
            foreground->SetChecked(foregroundMode);
            foreground->SetOnPreferenceChangeListener(this);
            screen->AddPreference(foreground);
            emptyText->SetVisibility(View.GONE);
            learnMore->SetVisibility(View.GONE);
            emptyImage->SetVisibility(View.GONE);
            GetListView()->SetVisibility(View.VISIBLE);
        }
        SetPreferenceScreen(screen);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        super->OnCreateView(inflater, container, savedInstanceState);
        View v = mInflater->Inflate(R.layout.nfc_payment, container, FALSE);
        TextView learnMore = (TextView) v->FindViewById(R.id.nfc_payment_learn_more);
        learnMore->SetOnClickListener(new OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                String helpUrl;
                if (!TextUtils->IsEmpty(helpUrl = GetResources()->GetString(
                        R::string::help_url_nfc_payment))) {
                    final Uri fullUri = HelpUtils->UriWithAddedParameters(
                            PaymentSettings.this->GetActivity(), Uri->Parse(helpUrl));
                    Intent intent = new Intent(IIntent::ACTION_VIEW, fullUri);
                    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                    StartActivity(intent);
                } else {
                    Logger::E(TAG, "Help url not set.");
                }
            }
        });
        return v;
    }

    //@Override
    CARAPI OnClick(View v) {
        if (v->GetTag() instanceof PaymentAppInfo) {
            PaymentAppInfo appInfo = (PaymentAppInfo) v->GetTag();
            if (appInfo.componentName != NULL) {
                mPaymentBackend->SetDefaultPaymentApp(appInfo.componentName);
            }
            Refresh();
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSettingsPackageMonitor->Register(GetActivity(), GetActivity()->GetMainLooper(), FALSE);
        Refresh();
    }

    //@Override
    CARAPI OnPause() {
        mSettingsPackageMonitor->Unregister();
        super->OnPause();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);
        String searchUri = Settings.Secure->GetString(GetContentResolver(),
                Settings.Secure.PAYMENT_SERVICE_SEARCH_URI);
        if (!TextUtils->IsEmpty(searchUri)) {
            MenuItem menuItem = menu->Add(R::string::nfc_payment_menu_item_add_service);
            menuItem->SetShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
            menuItem->SetIntent(new Intent(IIntent::ACTION_VIEW,Uri->Parse(searchUri)));
        }
    }

    private final Handler mHandler = new Handler() {
        //@Override
        CARAPI DispatchMessage(Message msg) {
            Refresh();
        }
    };

    private class SettingsPackageMonitor extends PackageMonitor {
        //@Override
        CARAPI OnPackageAdded(String packageName, Int32 uid) {
           mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageAppeared(String packageName, Int32 reason) {
            mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageDisappeared(String packageName, Int32 reason) {
            mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageRemoved(String packageName, Int32 uid) {
            mHandler->ObtainMessage()->SendToTarget();
        }
    }

    public static class PaymentAppPreference extends Preference {
        private final OnClickListener listener;
        private final PaymentAppInfo appInfo;

        public PaymentAppPreference(Context context, PaymentAppInfo appInfo,
                OnClickListener listener) {
            Super(context);
            SetLayoutResource(R.layout.nfc_payment_option);
            this.appInfo = appInfo;
            this.listener = listener;
        }

        //@Override
        protected void OnBindView(View view) {
            super->OnBindView(view);

            RadioButton radioButton = (RadioButton) view->FindViewById(android.R.id.button1);
            radioButton->SetChecked(appInfo.isDefault);
            radioButton->SetOnClickListener(listener);
            radioButton->SetTag(appInfo);

            ImageView banner = (ImageView) view->FindViewById(R.id.banner);
            banner->SetImageDrawable(appInfo.banner);
            banner->SetOnClickListener(listener);
            banner->SetTag(appInfo);
        }
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference instanceof CheckBoxPreference) {
            mPaymentBackend->SetForegroundMode(((Boolean) newValue).BooleanValue());
            return TRUE;
        } else {
            return FALSE;
        }
    }
}
