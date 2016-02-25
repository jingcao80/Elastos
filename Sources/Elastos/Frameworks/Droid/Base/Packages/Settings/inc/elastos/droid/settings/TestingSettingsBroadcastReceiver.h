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
