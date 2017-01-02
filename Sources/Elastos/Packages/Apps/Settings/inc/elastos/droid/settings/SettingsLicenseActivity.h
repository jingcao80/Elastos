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
 * Copyright (C) 2007 The Android Open Source Project
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

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::DialogInterface::IOnDismissListener;
using Elastos::Droid::Content::Res::IConfiguration;

using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::IInputStreamReader;
using Elastos::Utility::Zip::IGZIPInputStream;

/**
 * The "dialog" that shows from "License" in the Settings app.
 */
public class SettingsLicenseActivity extends Activity {

    private static const String TAG = "SettingsLicenseActivity";
    private static const Boolean LOGV = FALSE || FALSE;

    private static const String DEFAULT_LICENSE_PATH = "/system/etc/NOTICE.html.gz";
    private static const String PROPERTY_LICENSE_PATH = "ro.config.license_path";

    private Handler mHandler;
    private WebView mWebView;
    private ProgressDialog mSpinnerDlg;
    private AlertDialog mTextDlg;

    private class LicenseFileLoader implements Runnable {

        private static const String INNER_TAG = "SettingsLicenseActivity.LicenseFileLoader";
        public static const Int32 STATUS_OK = 0;
        public static const Int32 STATUS_NOT_FOUND = 1;
        public static const Int32 STATUS_READ_ERROR = 2;
        public static const Int32 STATUS_EMPTY_FILE = 3;

        private String mFileName;
        private Handler mHandler;

        public LicenseFileLoader(String fileName, Handler handler) {
            mFileName = fileName;
            mHandler = handler;
        }

        CARAPI Run() {

            Int32 status = STATUS_OK;

            InputStreamReader inputReader = NULL;
            StringBuilder data = new StringBuilder(2048);
            try {
                Char32[] tmp = new Char32[2048];
                Int32 numRead;
                if (mFileName->EndsWith(".gz")) {
                    inputReader = new InputStreamReader(
                        new GZIPInputStream(new FileInputStream(mFileName)));
                } else {
                    inputReader = new FileReader(mFileName);
                }

                while ((numRead = inputReader->Read(tmp)) >= 0) {
                    data->Append(tmp, 0, numRead);
                }
            } catch (FileNotFoundException e) {
                Logger::E(INNER_TAG, "License HTML file not found at " + mFileName, e);
                status = STATUS_NOT_FOUND;
            } catch (IOException e) {
                Logger::E(INNER_TAG, "Error reading license HTML file at " + mFileName, e);
                status = STATUS_READ_ERROR;
            } finally {
                try {
                    if (inputReader != NULL) {
                        inputReader->Close();
                    }
                } catch (IOException e) {
                }
            }

            if ((status == STATUS_OK) && TextUtils->IsEmpty(data)) {
                Logger::E(INNER_TAG, "License HTML is empty (from " + mFileName + ")");
                status = STATUS_EMPTY_FILE;
            }

            // Tell the UI thread that we are finished.
            Message msg = mHandler->ObtainMessage(status, NULL);
            if (status == STATUS_OK) {
                msg.obj = data->ToString();
            }
            mHandler->SendMessage(msg);
        }
    }

    public SettingsLicenseActivity() {
        Super();
        mHandler = NULL;
        mWebView = NULL;
        mSpinnerDlg = NULL;
        mTextDlg = NULL;
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        String fileName = SystemProperties->Get(PROPERTY_LICENSE_PATH, DEFAULT_LICENSE_PATH);
        if (TextUtils->IsEmpty(fileName)) {
            Logger::E(TAG, "The system property for the license file is empty.");
            ShowErrorAndFinish();
            return;
        }

        // The activity does not have any view itself,
        // so set it invisible to avoid displaying the title text in the background.
        SetVisible(FALSE);

        mWebView = new WebView(this);

        mHandler = new Handler() {

            //@Override
            CARAPI HandleMessage(Message msg) {
                super->HandleMessage(msg);

                if (msg.what == LicenseFileLoader.STATUS_OK) {
                    String text = (String) msg.obj;
                    ShowPageOfText(text);
                } else {
                    ShowErrorAndFinish();
                }
            }
        };

        CharSequence title = GetText(R::string::settings_license_activity_title);
        CharSequence msg = GetText(R::string::settings_license_activity_loading);

        ProgressDialog pd = ProgressDialog->Show(this, title, msg, TRUE, FALSE);
        pd->SetProgressStyle(ProgressDialog.STYLE_SPINNER);
        mSpinnerDlg = pd;

        // Start separate thread to do the actual loading.
        Thread thread = new Thread(new LicenseFileLoader(fileName, mHandler));
        thread->Start();
    }

    //@Override
    protected void OnDestroy() {
        if (mTextDlg != NULL && mTextDlg->IsShowing()) {
            mTextDlg->Dismiss();
        }
        if (mSpinnerDlg != NULL && mSpinnerDlg->IsShowing()) {
            mSpinnerDlg->Dismiss();
        }
        super->OnDestroy();
    }

    private void ShowPageOfText(String text) {
        // Create an AlertDialog to display the WebView in.
        AlertDialog.Builder builder = new AlertDialog->Builder(SettingsLicenseActivity.this);
        builder->SetCancelable(TRUE)
               .SetView(mWebView)
               .SetTitle(R::string::settings_license_activity_title);

        mTextDlg = builder->Create();
        mTextDlg->SetOnDismissListener(new OnDismissListener() {

            CARAPI OnDismiss(DialogInterface dlgi) {
                SettingsLicenseActivity.this->Finish();
            }
        });

        // Begin the loading.  This will be done in a separate thread in WebView.
        mWebView->LoadDataWithBaseURL(NULL, text, "text/html", "utf-8", NULL);
        mWebView->SetWebViewClient(new WebViewClient() {
            //@Override
            CARAPI OnPageFinished(WebView view, String url) {
                mSpinnerDlg->Dismiss();
                if (SettingsLicenseActivity.this->IsResumed()) {
                    mTextDlg->Show();
                }
            }
        });

        mWebView = NULL;
    }

    private void ShowErrorAndFinish() {
        mSpinnerDlg->Dismiss();
        mSpinnerDlg = NULL;
        Toast->MakeText(this, R::string::settings_license_activity_unavailable, Toast.LENGTH_LONG)
                .Show();
        Finish();
    }
}
