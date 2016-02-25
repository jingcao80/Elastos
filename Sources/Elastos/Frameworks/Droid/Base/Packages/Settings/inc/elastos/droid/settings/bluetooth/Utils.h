

package com.android.settings.bluetooth;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

/**
 * Utils is a helper class that contains constants for various
 * Android resource IDs, debug logging flags, and static methods
 * for creating dialogs.
 */
final class Utils {
    static const Boolean V = FALSE; // verbose logging
    static const Boolean D = TRUE;  // regular logging

    private Utils() {
    }

    public static Int32 GetConnectionStateSummary(Int32 connectionState) {
        switch (connectionState) {
        case BluetoothProfile.STATE_CONNECTED:
            return R::string::bluetooth_connected;
        case BluetoothProfile.STATE_CONNECTING:
            return R::string::bluetooth_connecting;
        case BluetoothProfile.STATE_DISCONNECTED:
            return R::string::bluetooth_disconnected;
        case BluetoothProfile.STATE_DISCONNECTING:
            return R::string::bluetooth_disconnecting;
        default:
            return 0;
        }
    }

    // Create (or recycle existing) and show disconnect dialog.
    static AlertDialog ShowDisconnectDialog(Context context,
            AlertDialog dialog,
            DialogInterface.OnClickListener disconnectListener,
            CharSequence title, CharSequence message) {
        if (dialog == NULL) {
            dialog = new AlertDialog->Builder(context)
                    .SetPositiveButton(android.R::string::ok, disconnectListener)
                    .SetNegativeButton(android.R::string::cancel, NULL)
                    .Create();
        } else {
            if (dialog->IsShowing()) {
                dialog->Dismiss();
            }
            // use disconnectListener for the correct Profile(s)
            CharSequence okText = context->GetText(android.R::string::ok);
            dialog->SetButton(DialogInterface.BUTTON_POSITIVE,
                    okText, disconnectListener);
        }
        dialog->SetTitle(title);
        dialog->SetMessage(message);
        dialog->Show();
        return dialog;
    }

    // TODO: wire this up to show connection errors...
    static void ShowConnectingError(Context context, String name) {
        // if (!mIsConnectingErrorPossible) {
        //     return;
        // }
        // mIsConnectingErrorPossible = FALSE;

        ShowError(context, name, R::string::bluetooth_connecting_error_message);
    }

    static void ShowError(Context context, String name, Int32 messageResId) {
        String message = context->GetString(messageResId, name);
        LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(context);
        Context activity = manager->GetForegroundActivity();
        If(manager->IsForegroundActivity()) {
            new AlertDialog->Builder(activity)
                .SetTitle(R::string::bluetooth_error_title)
                .SetMessage(message)
                .SetPositiveButton(android.R::string::ok, NULL)
                .Show();
        } else {
            Toast->MakeText(context, message, Toast.LENGTH_SHORT).Show();
        }
    }

    /**
     * Update the search Index for a specific class name and resources.
     */
    public static void UpdateSearchIndex(Context context, String className, String title,
            String screenTitle, Int32 iconResId, Boolean enabled) {
        SearchIndexableRaw data = new SearchIndexableRaw(context);
        data.className = className;
        data.title = title;
        data.screenTitle = screenTitle;
        data.iconResId = iconResId;
        data.enabled = enabled;

        Index->GetInstance(context).UpdateFromSearchIndexableData(data);
    }
}
