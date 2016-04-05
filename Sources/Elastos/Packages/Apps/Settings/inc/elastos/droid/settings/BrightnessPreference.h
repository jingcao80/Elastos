

package com.android.settings;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;

public class BrightnessPreference extends Preference {

    public BrightnessPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    //@Override
    protected void OnClick() {
        GetContext()->StartActivityAsUser(new Intent(IIntent::ACTION_SHOW_BRIGHTNESS_DIALOG),
                UserHandle.CURRENT_OR_SELF);
    }
}
