/*
 * Copyright (C) 2006 The Android Open Source Project
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

using Elastos::Droid::Settings::SettingsPreferenceFragment::ISettingsDialogFragment;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IProxy;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::View::IOnFocusChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

using Elastos::Net::IInetSocketAddress;

public class ProxySelector extends Fragment implements DialogCreatable {
    private static const String TAG = "ProxySelector";

    EditText    mHostnameField;
    EditText    mPortField;
    EditText    mExclusionListField;
    Button      mOKButton;
    Button      mClearButton;
    Button      mDefaultButton;

    private static const Int32 ERROR_DIALOG_ID = 0;

    private SettingsDialogFragment mDialogFragment;
    private View mView;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mView = inflater->Inflate(R.layout.proxy, container, FALSE);
        InitView(mView);
        // TODO: Populate based on connection status
        PopulateFields();
        return mView;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        final DevicePolicyManager dpm =
                (DevicePolicyManager)GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);

        final Boolean userSetGlobalProxy = (dpm->GetGlobalProxyAdmin() == NULL);
        // Disable UI if the Global Proxy is being controlled by a Device Admin
        mHostnameField->SetEnabled(userSetGlobalProxy);
        mPortField->SetEnabled(userSetGlobalProxy);
        mExclusionListField->SetEnabled(userSetGlobalProxy);
        mOKButton->SetEnabled(userSetGlobalProxy);
        mClearButton->SetEnabled(userSetGlobalProxy);
        mDefaultButton->SetEnabled(userSetGlobalProxy);
    }

    // Dialog management

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        if (id == ERROR_DIALOG_ID) {
            String hostname = mHostnameField->GetText()->ToString().Trim();
            String portStr = mPortField->GetText()->ToString().Trim();
            String exclList = mExclusionListField->GetText()->ToString().Trim();
            String msg = GetActivity()->GetString(Validate(hostname, portStr, exclList));

            return new AlertDialog->Builder(GetActivity())
                    .SetTitle(R::string::proxy_error)
                    .SetPositiveButton(R::string::proxy_error_dismiss, NULL)
                    .SetMessage(msg)
                    .Create();
        }
        return NULL;
    }

    private void ShowDialog(Int32 dialogId) {
        if (mDialogFragment != NULL) {
            Logger::E(TAG, "Old dialog fragment not NULL!");
        }
        mDialogFragment = new SettingsDialogFragment(this, dialogId);
        mDialogFragment->Show(GetActivity()->GetFragmentManager(), Integer->ToString(dialogId));
    }

    private void InitView(View view) {
        mHostnameField = (EditText)view->FindViewById(R.id.hostname);
        mHostnameField->SetOnFocusChangeListener(mOnFocusChangeHandler);

        mPortField = (EditText)view->FindViewById(R.id.port);
        mPortField->SetOnClickListener(mOKHandler);
        mPortField->SetOnFocusChangeListener(mOnFocusChangeHandler);

        mExclusionListField = (EditText)view->FindViewById(R.id.exclusionlist);
        mExclusionListField->SetOnFocusChangeListener(mOnFocusChangeHandler);

        mOKButton = (Button)view->FindViewById(R.id.action);
        mOKButton->SetOnClickListener(mOKHandler);

        mClearButton = (Button)view->FindViewById(R.id.clear);
        mClearButton->SetOnClickListener(mClearHandler);

        mDefaultButton = (Button)view->FindViewById(R.id.defaultView);
        mDefaultButton->SetOnClickListener(mDefaultHandler);
    }

    void PopulateFields() {
        final Activity activity = GetActivity();
        String hostname = "";
        Int32 port = -1;
        String exclList = "";
        // Use the last setting given by the user
        ConnectivityManager cm =
                (ConnectivityManager)GetActivity()->GetSystemService(Context.CONNECTIVITY_SERVICE);

        ProxyInfo proxy = cm->GetGlobalProxy();
        if (proxy != NULL) {
            hostname = proxy->GetHost();
            port = proxy->GetPort();
            exclList = proxy->GetExclusionListAsString();
        }

        if (hostname == NULL) {
            hostname = "";
        }

        mHostnameField->SetText(hostname);

        String portStr = port == -1 ? "" : Integer->ToString(port);
        mPortField->SetText(portStr);

        mExclusionListField->SetText(exclList);

        final Intent intent = activity->GetIntent();

        String buttonLabel = intent->GetStringExtra("button-label");
        if (!TextUtils->IsEmpty(buttonLabel)) {
            mOKButton->SetText(buttonLabel);
        }

        String title = intent->GetStringExtra("title");
        if (!TextUtils->IsEmpty(title)) {
            activity->SetTitle(title);
        }
    }

    /**
     * validate syntax of hostname and port entries
     * @return 0 on success, string resource ID on failure
     */
    public static Int32 Validate(String hostname, String port, String exclList) {
        switch (Proxy->Validate(hostname, port, exclList)) {
            case Proxy.PROXY_VALID:
                return 0;
            case Proxy.PROXY_HOSTNAME_EMPTY:
                return R::string::proxy_error_empty_host_set_port;
            case Proxy.PROXY_HOSTNAME_INVALID:
                return R::string::proxy_error_invalid_host;
            case Proxy.PROXY_PORT_EMPTY:
                return R::string::proxy_error_empty_port;
            case Proxy.PROXY_PORT_INVALID:
                return R::string::proxy_error_invalid_port;
            case Proxy.PROXY_EXCLLIST_INVALID:
                return R::string::proxy_error_invalid_exclusion_list;
            default:
                // should neven happen
                Logger::E(TAG, "Unknown proxy settings error");
                return -1;
        }
    }

    /**
     * returns TRUE on success, FALSE if the user must correct something
     */
    Boolean SaveToDb() {

        String hostname = mHostnameField->GetText()->ToString().Trim();
        String portStr = mPortField->GetText()->ToString().Trim();
        String exclList = mExclusionListField->GetText()->ToString().Trim();
        Int32 port = 0;

        Int32 result = Validate(hostname, portStr, exclList);
        if (result != 0) {
            ShowDialog(ERROR_DIALOG_ID);
            return FALSE;
        }

        if (portStr->Length() > 0) {
            try {
                port = Integer->ParseInt(portStr);
            } catch (NumberFormatException ex) {
                // should never happen - caught by validate above
                return FALSE;
            }
        }
        ProxyInfo p = new ProxyInfo(hostname, port, exclList);
        // FIXME: The best solution would be to make a better UI that would
        // disable editing of the text boxes if the user chooses to use the
        // default settings. i.e. checking a box to always use the default
        // carrier. http:/b/issue?id=756480
        // FIXME: If the user types in a proxy that matches the default, should
        // we keep that setting? Can be fixed with a new UI.
        ConnectivityManager cm =
                (ConnectivityManager)GetActivity()->GetSystemService(Context.CONNECTIVITY_SERVICE);

        cm->SetGlobalProxy(p);
        return TRUE;
    }

    OnClickListener mOKHandler = new OnClickListener() {
            CARAPI OnClick(View v) {
                if (SaveToDb()) {
                    GetActivity()->OnBackPressed();
                }
            }
        };

    OnClickListener mClearHandler = new OnClickListener() {
            CARAPI OnClick(View v) {
                mHostnameField->SetText("");
                mPortField->SetText("");
                mExclusionListField->SetText("");
            }
        };

    OnClickListener mDefaultHandler = new OnClickListener() {
            CARAPI OnClick(View v) {
                // TODO: populate based on connection status
                PopulateFields();
            }
        };

    OnFocusChangeListener mOnFocusChangeHandler = new OnFocusChangeListener() {
            CARAPI OnFocusChange(View v, Boolean hasFocus) {
                if (hasFocus) {
                    TextView textView = (TextView) v;
                    Selection->SelectAll((Spannable) textView->GetText());
                }
            }
        };
}
