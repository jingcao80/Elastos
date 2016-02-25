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

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnFocusChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;

/**
 * Shows simplified UI for configuring a wifi network. Used only in SetupWizard for XLarge
 * screen.
 */
public class WifiConfigUiForSetupWizardXL implements WifiConfigUiBase, OnFocusChangeListener {
    private static const String TAG = "SetupWizard";

    private Button mConnectButton;
    private Button mCancelButton;

    private final WifiSettingsForSetupWizardXL mActivity;
    private View mView;
    private WifiConfigController mController;
    private AccessPoint mAccessPoint;
    private Boolean mEdit;
    private Handler mHandler = new Handler();

    private final InputMethodManager mInputMethodManager;

    private LayoutInflater mInflater;

    /**
     * @param activity Activity which creates this object.
     * @param parent Parent ViewGroup (typically some layout) holding a view object created by
     * this object
     * @param accessPoint target AccessPoint to be configured.
     * @param edit
     */
    public WifiConfigUiForSetupWizardXL(
            WifiSettingsForSetupWizardXL activity, ViewGroup parent,
            AccessPoint accessPoint, Boolean edit) {
        mActivity = activity;
        mConnectButton = (Button)activity->FindViewById(R.id.wifi_setup_connect);
        mCancelButton = (Button)activity->FindViewById(R.id.wifi_setup_cancel);
        mAccessPoint = accessPoint;
        mEdit = edit;
        mInflater = (LayoutInflater)activity->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);

        mView = mInflater->Inflate(R.layout.wifi_config_ui_for_setup_wizard, parent, TRUE);
        mController = new WifiConfigController(this, mView, mAccessPoint, edit);

        mInputMethodManager = (InputMethodManager)
                activity->GetSystemService(Context.INPUT_METHOD_SERVICE);

        if (mView->FindViewById(R.id.security_fields).GetVisibility() == View.VISIBLE) {
            RequestFocusAndShowKeyboard(R.id.password);
        } else if (mView->FindViewById(R.id.type_ssid).GetVisibility() == View.VISIBLE) {
            // Add Network flow.
            RequestFocusAndShowKeyboard(R.id.ssid);
        }
    }

    /**
     * @param editViewId must be EditView
     */
    CARAPI RequestFocusAndShowKeyboard(Int32 editViewId) {
        // Set Focus to password View.
        final View viewToBeFocused = mView->FindViewById(editViewId);
        if (viewToBeFocused == NULL) {
            Logger::W(TAG, "password field to be focused not found.");
        } else if (!(viewToBeFocused instanceof EditText)) {
            Logger::W(TAG, "password field is not EditText");
        } else {
            if (viewToBeFocused->IsFocused()) {
                Logger::I(TAG, "Already focused");
                if (!mInputMethodManager->ShowSoftInput(viewToBeFocused, 0)) {
                    Logger::W(TAG, "Failed to show SoftInput");
                }
            } else {
                // After acquiring the focus, we show software keyboard.
                viewToBeFocused->SetOnFocusChangeListener(this);
                final Boolean requestFocusResult = viewToBeFocused->RequestFocus();
                Logger::I(TAG, String->Format("Focus request: %s",
                        (requestFocusResult ? "successful" : "failed")));
                if (!requestFocusResult) {
                    viewToBeFocused->SetOnFocusChangeListener(NULL);
                }
            }
        }
    }

    public View GetView() {
        return mView;
    }

    public AccessPoint GetAccessPoint() {
        return mAccessPoint;
    }

    //@Override
    public WifiConfigController GetController() {
        return mController;
    }

    //@Override
    public Boolean IsEdit() {
        return mEdit;
    }

    //@Override
    public LayoutInflater GetLayoutInflater() {
        return mInflater;
    }

    //@Override
    public Button GetSubmitButton() {
        return mConnectButton;
    }

    //@Override
    public Button GetForgetButton() {
        return NULL;
    }

    //@Override
    public Button GetCancelButton() {
        return mCancelButton;
    }

    //@Override
    CARAPI SetSubmitButton(CharSequence text) {
        mConnectButton->SetVisibility(View.VISIBLE);
        mConnectButton->SetText(text);
    }

    //@Override
    CARAPI SetForgetButton(CharSequence text) {
        // In XL setup screen, we won't show Forget button for simplifying the UI.
    }

    //@Override
    CARAPI SetCancelButton(CharSequence text) {
        mCancelButton->SetVisibility(View.VISIBLE);
        // We don't want "cancel" label given from caller.
        // mCancelButton->SetText(text);
    }

    //@Override
    public Context GetContext() {
        return mActivity;
    }

    //@Override
    CARAPI SetTitle(Int32 id) {
        Logger::D(TAG, "Ignoring setTitle");
    }

    //@Override
    CARAPI SetTitle(CharSequence title) {
        Logger::D(TAG, "Ignoring setTitle");
    }

    private class FocusRunnable implements Runnable {
        final View mViewToBeFocused;
        public FocusRunnable(View viewToBeFocused) {
            mViewToBeFocused = viewToBeFocused;
        }

        //@Override
        CARAPI Run() {
            // mInputMethodManager->FocusIn(mViewToBeFocused);
            final Boolean showSoftInputResult =
                    mInputMethodManager->ShowSoftInput(mViewToBeFocused, 0);
            if (showSoftInputResult) {
                mActivity->SetPaddingVisibility(View.GONE);
            } else {
                Logger::W(TAG, "Failed to show software keyboard ");
            }
        }
    }

    //@Override
    CARAPI OnFocusChange(View view, Boolean hasFocus) {
        view->SetOnFocusChangeListener(NULL);
        if (hasFocus) {
            mHandler->Post(new FocusRunnable(view));
        }
    }
}