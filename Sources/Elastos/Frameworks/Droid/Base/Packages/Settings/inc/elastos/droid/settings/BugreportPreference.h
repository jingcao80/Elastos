

package com.android.settings;

using Elastos::Droid::App::AlertDialog::IBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

public class BugreportPreference extends DialogPreference {
    public BugreportPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    //@Override
    protected void OnPrepareDialogBuilder(Builder builder) {
        super->OnPrepareDialogBuilder(builder);
        builder->SetPositiveButton(R::string::report, this);
        builder->SetMessage(R::string::bugreport_message);
    }

    //@Override
    protected void ShowDialog(Bundle state) {
        super->ShowDialog(state);
    }

    //@Override
    protected void OnBindDialogView(View view) {
        super->OnBindDialogView(view);
    }

    //@Override
    protected void OnDialogClosed(Boolean positiveResult) {
        super->OnDialogClosed(positiveResult);
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            SystemProperties->Set("ctl.start", "bugreport");
        }
    }
}
