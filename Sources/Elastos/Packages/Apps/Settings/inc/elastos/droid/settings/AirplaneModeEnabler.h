
#ifndef __ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__
#define __ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__

#include "Elastos.Droid.Preference.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Database::IContentObserver;
// using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::Os::IMessage;
// using Elastos::Droid::Os::ISystemProperties;
// using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
// using Elastos::Droid::Preference::ICheckBoxPreference;
// using Elastos::Droid::Preference::IPreference;
// using Elastos::Droid::Preference::ISwitchPreference;
// using Elastos::Droid::Provider::ISettings;

// using Elastos::Droid::Internal::Telephony::IPhoneStateIntentReceiver;
// using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

namespace Elastos {
namespace Droid {
namespace Settings {

class AirplaneModeEnabler
    : public Object
    , public IPreferenceOnPreferenceChangeListener
{
public:
    CAR_INTERFACE_DECL();
    // private final Context mContext;

    // private PhoneStateIntentReceiver mPhoneStateReceiver;

    // private final SwitchPreference mSwitchPref;

    // private static const Int32 EVENT_SERVICE_STATE_CHANGED = 3;

    // private Handler mHandler = new Handler() {
    //     //@Override
    //     CARAPI HandleMessage(Message msg) {
    //         switch (msg.what) {
    //             case EVENT_SERVICE_STATE_CHANGED:
    //                 OnAirplaneModeChanged();
    //                 break;
    //         }
    //     }
    // };

    // private ContentObserver mAirplaneModeObserver = new ContentObserver(new Handler()) {
    //     //@Override
    //     CARAPI OnChange(Boolean selfChange) {
    //         OnAirplaneModeChanged();
    //     }
    // };

    // public AirplaneModeEnabler(Context context, SwitchPreference airplaneModeCheckBoxPreference) {

    //     mContext = context;
    //     mSwitchPref = airplaneModeCheckBoxPreference;

    //     airplaneModeCheckBoxPreference->SetPersistent(FALSE);

    //     mPhoneStateReceiver = new PhoneStateIntentReceiver(mContext, mHandler);
    //     mPhoneStateReceiver->NotifyServiceState(EVENT_SERVICE_STATE_CHANGED);
    // }

    // CARAPI Resume() {

    //     mSwitchPref->SetChecked(IsAirplaneModeOn(mContext));

    //     mPhoneStateReceiver->RegisterIntent();
    //     mSwitchPref->SetOnPreferenceChangeListener(this);
    //     mContext->GetContentResolver()->RegisterContentObserver(
    //             Settings::Global::>GetUriFor(Settings::Global::AIRPLANE_MODE_ON), TRUE,
    //             mAirplaneModeObserver);
    // }

    // CARAPI Pause() {
    //     mPhoneStateReceiver->UnregisterIntent();
    //     mSwitchPref->SetOnPreferenceChangeListener(NULL);
    //     mContext->GetContentResolver()->UnregisterContentObserver(mAirplaneModeObserver);
    // }

    static CARAPI_(Boolean) IsAirplaneModeOn(
        /* [in] */ IContext* context);

    // private void SetAirplaneModeOn(Boolean enabling) {
    //     // Change the system setting
    //     Settings::Global::>PutInt(mContext->GetContentResolver(), Settings::Global::AIRPLANE_MODE_ON,
    //                             enabling ? 1 : 0);
    //     // Update the UI to reflect system setting
    //     mSwitchPref->SetChecked(enabling);

    //     // Post the intent
    //     Intent intent = new Intent(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    //     intent->PutExtra("state", enabling);
    //     mContext->SendBroadcastAsUser(intent, UserHandle.ALL);
    // }

    /**
     * Called when we've received confirmation that the airplane mode was set.
     * TODO: We update the checkbox summary when we get notified
     * that mobile radio is powered up/down. We should not have dependency
     * on one radio alone. We need to do the following:
     * - handle the case of wifi/bluetooth failures
     * - mobile does not send failure notification, fail on timeout.
     */
    // private void OnAirplaneModeChanged() {
    //     mSwitchPref->SetChecked(IsAirplaneModeOn(mContext));
    // }

    /**
     * Called when someone clicks on the checkbox preference.
     */
    // public Boolean OnPreferenceChange(Preference preference, Object newValue) {
    //     if (Boolean->ParseBoolean(
    //                 SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE))) {
    //         // In ECM mode, do not update database at this point
    //     } else {
    //         SetAirplaneModeOn((Boolean) newValue);
    //     }
    //     return TRUE;
    // }

    // CARAPI SetAirplaneModeInECM(Boolean isECMExit, Boolean isAirplaneModeOn) {
    //     if (isECMExit) {
    //         // update database based on the current checkbox state
    //         SetAirplaneModeOn(isAirplaneModeOn);
    //     } else {
    //         // update summary
    //         OnAirplaneModeChanged();
    //     }
    // }

};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__
