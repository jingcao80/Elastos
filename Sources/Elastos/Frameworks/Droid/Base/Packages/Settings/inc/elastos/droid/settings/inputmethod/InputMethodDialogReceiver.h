
package com.android.settings.inputmethod;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;

public class InputMethodDialogReceiver extends BroadcastReceiver {
    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        if (Settings.ACTION_SHOW_INPUT_METHOD_PICKER->Equals(intent->GetAction())) {
            ((InputMethodManager) context->GetSystemService(Context.INPUT_METHOD_SERVICE))
                    .ShowInputMethodPicker();
        }
    }
}
