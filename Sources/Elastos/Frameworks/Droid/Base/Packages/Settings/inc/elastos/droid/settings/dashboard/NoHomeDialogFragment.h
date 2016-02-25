

package com.android.settings.dashboard;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Settings::IR;

public class NoHomeDialogFragment extends DialogFragment {
    public static void Show(Activity parent) {
        final NoHomeDialogFragment dialog = new NoHomeDialogFragment();
        dialog->Show(parent->GetFragmentManager(), NULL);
    }

    //@Override
    public Dialog OnCreateDialog(Bundle savedInstanceState) {
        return new AlertDialog->Builder(GetActivity())
                .SetMessage(R::string::only_one_home_message)
                .SetPositiveButton(android.R::string::ok, NULL)
                .Create();
    }
}
