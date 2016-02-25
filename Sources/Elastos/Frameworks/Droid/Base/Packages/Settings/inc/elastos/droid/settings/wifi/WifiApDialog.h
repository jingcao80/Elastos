/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings.wifi;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::WifiConfiguration::IAuthAlgorithm;
using Elastos::Droid::Net::Wifi::WifiConfiguration::IKeyMgmt;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * Dialog to configure the SSID and security settings
 * for Access Point operation
 */
public class WifiApDialog extends AlertDialog implements View.OnClickListener,
        TextWatcher, AdapterView.OnItemSelectedListener {

    static const Int32 BUTTON_SUBMIT = DialogInterface.BUTTON_POSITIVE;

    private final DialogInterface.OnClickListener mListener;

    public static const Int32 OPEN_INDEX = 0;
    public static const Int32 WPA2_INDEX = 1;

    private View mView;
    private TextView mSsid;
    private Int32 mSecurityTypeIndex = OPEN_INDEX;
    private EditText mPassword;

    WifiConfiguration mWifiConfig;

    public WifiApDialog(Context context, DialogInterface.OnClickListener listener,
            WifiConfiguration wifiConfig) {
        Super(context);
        mListener = listener;
        mWifiConfig = wifiConfig;
        if (wifiConfig != NULL) {
            mSecurityTypeIndex = GetSecurityTypeIndex(wifiConfig);
        }
    }

    public static Int32 GetSecurityTypeIndex(WifiConfiguration wifiConfig) {
        if (wifiConfig.allowedKeyManagement->Get(KeyMgmt.WPA2_PSK)) {
            return WPA2_INDEX;
        }
        return OPEN_INDEX;
    }

    public WifiConfiguration GetConfig() {

        WifiConfiguration config = new WifiConfiguration();

        /**
         * TODO: SSID in WifiConfiguration for soft ap
         * is being stored as a raw string without quotes.
         * This is not the case on the client side. We need to
         * make things consistent and clean it up
         */
        config.SSID = mSsid->GetText()->ToString();

        switch (mSecurityTypeIndex) {
            case OPEN_INDEX:
                config.allowedKeyManagement->Set(KeyMgmt.NONE);
                return config;

            case WPA2_INDEX:
                config.allowedKeyManagement->Set(KeyMgmt.WPA2_PSK);
                config.allowedAuthAlgorithms->Set(AuthAlgorithm.OPEN);
                if (mPassword->Length() != 0) {
                    String password = mPassword->GetText()->ToString();
                    config.preSharedKey = password;
                }
                return config;
        }
        return NULL;
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {

        mView = GetLayoutInflater()->Inflate(R.layout.wifi_ap_dialog, NULL);
        Spinner mSecurity = ((Spinner) mView->FindViewById(R.id.security));

        SetView(mView);
        SetInverseBackgroundForced(TRUE);

        Context context = GetContext();

        SetTitle(R::string::wifi_tether_configure_ap_text);
        mView->FindViewById(R.id.type).SetVisibility(View.VISIBLE);
        mSsid = (TextView) mView->FindViewById(R.id.ssid);
        mPassword = (EditText) mView->FindViewById(R.id.password);

        SetButton(BUTTON_SUBMIT, context->GetString(R::string::wifi_save), mListener);
        SetButton(DialogInterface.BUTTON_NEGATIVE,
        context->GetString(R::string::wifi_cancel), mListener);

        if (mWifiConfig != NULL) {
            mSsid->SetText(mWifiConfig.SSID);
            mSecurity->SetSelection(mSecurityTypeIndex);
            if (mSecurityTypeIndex == WPA2_INDEX) {
                  mPassword->SetText(mWifiConfig.preSharedKey);
            }
        }

        mSsid->AddTextChangedListener(this);
        mPassword->AddTextChangedListener(this);
        ((CheckBox) mView->FindViewById(R.id.show_password)).SetOnClickListener(this);
        mSecurity->SetOnItemSelectedListener(this);

        super->OnCreate(savedInstanceState);

        ShowSecurityFields();
        Validate();
    }

    private void Validate() {
        if ((mSsid != NULL && mSsid->Length() == 0) ||
                   ((mSecurityTypeIndex == WPA2_INDEX)&&
                        mPassword->Length() < 8)) {
            GetButton(BUTTON_SUBMIT).SetEnabled(FALSE);
        } else {
            GetButton(BUTTON_SUBMIT).SetEnabled(TRUE);
        }
    }

    CARAPI OnClick(View view) {
        mPassword->SetInputType(
                InputType.TYPE_CLASS_TEXT | (((CheckBox) view).IsChecked() ?
                InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                InputType.TYPE_TEXT_VARIATION_PASSWORD));
    }

    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
    }

    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
    }

    CARAPI AfterTextChanged(Editable editable) {
        Validate();
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        mSecurityTypeIndex = position;
        ShowSecurityFields();
        Validate();
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
    }

    private void ShowSecurityFields() {
        if (mSecurityTypeIndex == OPEN_INDEX) {
            mView->FindViewById(R.id.fields).SetVisibility(View.GONE);
            return;
        }
        mView->FindViewById(R.id.fields).SetVisibility(View.VISIBLE);
    }
}
