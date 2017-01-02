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
