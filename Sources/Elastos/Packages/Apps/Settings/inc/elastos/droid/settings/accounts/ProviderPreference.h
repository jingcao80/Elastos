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

package com.android.settings.accounts;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;

/**
 * ProviderPreference is used to display an image to the left of a provider name.
 * The preference ultimately calls AccountManager->AddAccount() for the account type.
 */
public class ProviderPreference extends Preference {
    private String mAccountType;

    public ProviderPreference(
            Context context, String accountType, Drawable icon, CharSequence providerName) {
        Super(context);
        mAccountType = accountType;
        SetIcon(icon);
        SetPersistent(FALSE);
        SetTitle(providerName);
    }

    public String GetAccountType() {
        return mAccountType;
    }
}
