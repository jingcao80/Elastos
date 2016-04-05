/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.settings;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Content::IDialogInterface;

/**
 * The "dialog" that shows from "Safety information" in the Settings app.
 */
public class SettingsSafetyLegalActivity extends AlertActivity 
        implements DialogInterface.OnCancelListener, DialogInterface.OnClickListener {
    private static const String PROPERTY_LSAFETYLEGAL_URL = "ro.url.safetylegal";

    private WebView mWebView;

    private AlertDialog mErrorDialog = NULL;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        String userSafetylegalUrl = SystemProperties->Get(PROPERTY_LSAFETYLEGAL_URL);

        final Configuration configuration = GetResources()->GetConfiguration();
        final String language = configuration.locale->GetLanguage();
        final String country = configuration.locale->GetCountry();

        String loc = String->Format("locale=%s-%s", language, country);

        userSafetylegalUrl = String->Format("%s&%s", userSafetylegalUrl, loc);

        mWebView = new WebView(this);

        // Begin accessing
        mWebView->GetSettings()->SetJavaScriptEnabled(TRUE);
        if (savedInstanceState == NULL) {
            mWebView->LoadUrl(userSafetylegalUrl);
        } else {
            mWebView->RestoreState(savedInstanceState);
        }
        mWebView->SetWebViewClient(new WebViewClient() {
            //@Override
            CARAPI OnPageFinished(WebView view, String url) {
                // Change from 'Loading...' to the real title
                mAlert->SetTitle(GetString(R::string::settings_safetylegal_activity_title));
            }

            //@Override
            CARAPI OnReceivedError(WebView view, Int32 errorCode,
                    String description, String failingUrl) {
                ShowErrorAndFinish(failingUrl);
            }
        });

        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::settings_safetylegal_activity_loading);
        p.mView = mWebView;
        p.mForceInverseBackground = TRUE;
        SetupAlert();
    }

    private void ShowErrorAndFinish(String url) {
        if (mErrorDialog == NULL) {
            mErrorDialog = new AlertDialog->Builder(this)
                    .SetTitle(R::string::settings_safetylegal_activity_title)
                    .SetPositiveButton(android.R::string::ok, this)
                    .SetOnCancelListener(this)
                    .SetCancelable(TRUE)
                    .Create();
        } else {
            if (mErrorDialog->IsShowing()) {
                mErrorDialog->Dismiss();
            }
        }
        mErrorDialog->SetMessage(GetResources()
                .GetString(R::string::settings_safetylegal_activity_unreachable, url));
        mErrorDialog->Show();
    }

    //@Override
    protected void OnDestroy() {
        super->OnDestroy();

        if (mErrorDialog != NULL) {
            mErrorDialog->Dismiss();
            mErrorDialog = NULL;
        }
    }

    //@Override
    public Boolean DispatchKeyEvent(KeyEvent event) {
        if (event->GetKeyCode() == KeyEvent.KEYCODE_BACK 
                && event->GetAction() == KeyEvent.ACTION_DOWN) {
            if (mWebView->CanGoBack()) {
                mWebView->GoBack();
                return TRUE;
            }
        }
        return super->DispatchKeyEvent(event);
    }

    CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
        Finish();
    }

    CARAPI OnCancel(DialogInterface dialog) {
        Finish();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle icicle) {
        mWebView->SaveState(icicle);
        super->OnSaveInstanceState(icicle);
    }
}
