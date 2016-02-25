

package com.android.settings.bluetooth;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;

using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Settings::IR;

/**
 * Dialog fragment for setting the discoverability timeout.
 */
public class BluetoothVisibilityTimeoutFragment extends DialogFragment
        implements DialogInterface.OnClickListener {

    private final BluetoothDiscoverableEnabler mDiscoverableEnabler;

    public BluetoothVisibilityTimeoutFragment() {
        mDiscoverableEnabler = LocalBluetoothManager->GetInstance(GetActivity())
                .GetDiscoverableEnabler();
    }

    //@Override
    public Dialog OnCreateDialog(Bundle savedInstanceState) {
        return new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::bluetooth_visibility_timeout)
                .SetSingleChoiceItems(R.array.bluetooth_visibility_timeout_entries,
                        mDiscoverableEnabler->GetDiscoverableTimeoutIndex(), this)
                .SetNegativeButton(android.R::string::cancel, NULL)
                .Create();
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        mDiscoverableEnabler->SetDiscoverableTimeout(which);
        Dismiss();
    }
}
