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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::IButton;

/**
 * Foundation interface glues between Activities and UIs like
 * {@link WifiDialog} or {@link WifiConfigUiForSetupWizardXL}.
 */
public interface WifiConfigUiBase {
    public Context GetContext();
    public WifiConfigController GetController();
    public LayoutInflater GetLayoutInflater();
    public Boolean IsEdit();

    CARAPI SetTitle(Int32 id);
    CARAPI SetTitle(CharSequence title);

    CARAPI SetSubmitButton(CharSequence text);
    CARAPI SetForgetButton(CharSequence text);
    CARAPI SetCancelButton(CharSequence text);
    public Button GetSubmitButton();
    public Button GetForgetButton();
    public Button GetCancelButton();
}