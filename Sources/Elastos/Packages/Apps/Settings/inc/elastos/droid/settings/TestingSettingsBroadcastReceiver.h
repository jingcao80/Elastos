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

package com.android.settings;

using Elastos::Droid::Provider::ITelephony;
using static com::Android::Internal::Telephony::TelephonyIntents::ISECRET_CODE_ACTION;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IKeyEvent;


public class TestingSettingsBroadcastReceiver extends BroadcastReceiver {
  
    public TestingSettingsBroadcastReceiver() {
    }
    
    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        if (intent->GetAction()->Equals(SECRET_CODE_ACTION)) {
            Intent i = new Intent(IIntent::ACTION_MAIN);
            i->SetClass(context, TestingSettings.class);
            i->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            context->StartActivity(i);
        }
    }
}
