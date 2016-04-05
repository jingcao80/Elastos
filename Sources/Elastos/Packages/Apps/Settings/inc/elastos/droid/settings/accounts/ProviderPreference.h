

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
