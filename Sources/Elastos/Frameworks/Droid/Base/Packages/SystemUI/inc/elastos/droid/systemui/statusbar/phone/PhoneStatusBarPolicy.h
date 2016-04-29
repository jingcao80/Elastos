
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBARPOLICY_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBARPOLICY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastControllerCallback;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class CPhoneStatusBarPolicyIntentReceiver;

/**
 * This class contains all of the policy about which icons are installed in the status
 * bar at boot time.  It goes through the normal API for icons, even though it probably
 * strictly doesn't need to.
 */
class PhoneStatusBarPolicy: public Object
{
private:
    class CastCallback
        : public Object
        , public ICastControllerCallback
    {
    public:
        CAR_INTERFACE_DECL();

        CastCallback(
            /* [in] */ PhoneStatusBarPolicy* host);

        // @Override
        CARAPI OnCastDevicesChanged();

    private:
        PhoneStatusBarPolicy* mHost;
    };

public:
    PhoneStatusBarPolicy(
        /* [in] */ IContext* context,
        /* [in] */ ICastController* cast);

    CARAPI_(void) SetZenMode(
        /* [in] */ Int32 zen);

private:
    CARAPI_(void) UpdateAlarm();

    CARAPI_(void) UpdateSyncState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateSimState(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateVolumeZen();

    CARAPI_(void) UpdateBluetooth();

    CARAPI_(void) UpdateTTY(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateCast();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Boolean SHOW_SYNC_ICON;

    static const String SLOT_SYNC_ACTIVE;
    static const String SLOT_CAST;
    static const String SLOT_BLUETOOTH;
    static const String SLOT_TTY;
    static const String SLOT_ZEN;
    static const String SLOT_VOLUME;
    static const String SLOT_CDMA_ERI;
    static const String SLOT_ALARM_CLOCK;

    AutoPtr<IContext> mContext;
    AutoPtr<IStatusBarManager> mService;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ICastController> mCast;

    // Assume it's all good unless we hear otherwise.  We don't always seem
    // to get broadcasts that it *is* there.
    IccCardConstantsState mSimState;

    Boolean mZenVisible;
    Boolean mVolumeVisible;

    Int32 mZen;

    Boolean mBluetoothEnabled;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
    AutoPtr<ICastControllerCallback> mCastCallback;

    friend class CPhoneStatusBarPolicyIntentReceiver;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBARPOLICY_H__
