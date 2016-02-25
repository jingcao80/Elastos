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

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;

class WifiDialog extends AlertDialog implements WifiConfigUiBase {
    static const Int32 BUTTON_SUBMIT = DialogInterface.BUTTON_POSITIVE;
    static const Int32 BUTTON_FORGET = DialogInterface.BUTTON_NEUTRAL;

    private final Boolean mEdit;
    private final DialogInterface.OnClickListener mListener;
    private final AccessPoint mAccessPoint;

    private View mView;
    private WifiConfigController mController;
    private Boolean mHideSubmitButton;

    public WifiDialog(Context context, DialogInterface.OnClickListener listener,
            AccessPoint accessPoint, Boolean edit, Boolean hideSubmitButton) {
        This(context, listener, accessPoint, edit);
        mHideSubmitButton = hideSubmitButton;
    }

    public WifiDialog(Context context, DialogInterface.OnClickListener listener,
            AccessPoint accessPoint, Boolean edit) {
        Super(context);
        mEdit = edit;
        mListener = listener;
        mAccessPoint = accessPoint;
        mHideSubmitButton = FALSE;
    }

    //@Override
    public WifiConfigController GetController() {
        return mController;
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        mView = GetLayoutInflater()->Inflate(R.layout.wifi_dialog, NULL);
        SetView(mView);
        SetInverseBackgroundForced(TRUE);
        mController = new WifiConfigController(this, mView, mAccessPoint, mEdit);
        super->OnCreate(savedInstanceState);

        if (mHideSubmitButton) {
            mController->HideSubmitButton();
        } else {
            /* During creation, the submit button can be unavailable to determine
             * visibility. Right after creation, update button visibility */
            mController->EnableSubmitIfAppropriate();
        }
    }

    //@Override
    public Boolean IsEdit() {
        return mEdit;
    }

    //@Override
    public Button GetSubmitButton() {
        return GetButton(BUTTON_SUBMIT);
    }

    //@Override
    public Button GetForgetButton() {
        return GetButton(BUTTON_FORGET);
    }

    //@Override
    public Button GetCancelButton() {
        return GetButton(BUTTON_NEGATIVE);
    }

    //@Override
    CARAPI SetSubmitButton(CharSequence text) {
        SetButton(BUTTON_SUBMIT, text, mListener);
    }

    //@Override
    CARAPI SetForgetButton(CharSequence text) {
        SetButton(BUTTON_FORGET, text, mListener);
    }

    //@Override
    CARAPI SetCancelButton(CharSequence text) {
        SetButton(BUTTON_NEGATIVE, text, mListener);
    }
}
