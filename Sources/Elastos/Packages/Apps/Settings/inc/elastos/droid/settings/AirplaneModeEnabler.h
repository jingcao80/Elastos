
#ifndef __ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__
#define __ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Telephony::IPhoneStateIntentReceiver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::ISwitchPreference;

namespace Elastos {
namespace Droid {
namespace Settings {

class AirplaneModeEnabler
    : public Object
    , public IPreferenceOnPreferenceChangeListener
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AirplaneModeEnabler::MyHandler")

        MyHandler(
            /* [in] */ AirplaneModeEnabler* host);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AirplaneModeEnabler* mHost;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("AirplaneModeEnabler::MyContentObserver")

        MyContentObserver(
            /* [in] */ AirplaneModeEnabler* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AirplaneModeEnabler* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("AirplaneModeEnabler")

    AirplaneModeEnabler(
        /* [in] */ IContext* context,
        /* [in] */ ISwitchPreference* airplaneModeCheckBoxPreference);

    CARAPI Resume();

    CARAPI Pause();

    static CARAPI_(Boolean) IsAirplaneModeOn(
        /* [in] */ IContext* context);

    /**
     * Called when someone clicks on the checkbox preference.
     */
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

    CARAPI SetAirplaneModeInECM(
        /* [in] */ Boolean isECMExit,
        /* [in] */ Boolean isAirplaneModeOn);

private:
    CARAPI_(void) SetAirplaneModeOn(
        /* [in] */ Boolean enabling);

    /**
     * Called when we've received confirmation that the airplane mode was set.
     * TODO: We update the checkbox summary when we get notified
     * that mobile radio is powered up/down. We should not have dependency
     * on one radio alone. We need to do the following:
     * - handle the case of wifi/bluetooth failures
     * - mobile does not send failure notification, fail on timeout.
     */
    CARAPI_(void) OnAirplaneModeChanged();

private:
    AutoPtr<IContext> mContext;

    AutoPtr<IPhoneStateIntentReceiver> mPhoneStateReceiver;

    AutoPtr<ISwitchPreference> mSwitchPref;

    static const Int32 EVENT_SERVICE_STATE_CHANGED = 3;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<MyContentObserver> mAirplaneModeObserver;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_AIRPLANEMODEENABLER_H__
