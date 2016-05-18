
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_QSTILEHOST_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_QSTILEHOST_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/settings/CurrentUserTracker.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::SystemUI::Qs::IQSTile;
using Elastos::Droid::SystemUI::Qs::IQSTileHost;
using Elastos::Droid::SystemUI::Qs::IQSTileHostCallback;
using Elastos::Droid::SystemUI::Settings::CurrentUserTracker;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBluetoothController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IFlashlightController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitor;
using Elastos::Droid::SystemUI::StatusBar::Policy::ILocationController;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IRotationLockController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ISecurityController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserSwitcherController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IZenModeController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IHotspotController;
using Elastos::Core::IThrowable;
using Elastos::Core::Object;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/** Platform implementation of the quick settings tile host **/
class QSTileHost
    : public Object
    , public IPhoneQSTileHost
    , public IQSTileHost
{
private:
    class Observer: public ContentObserver
    {
    public:
        TO_STRING_IMPL("QSTileHost::Observer")

        Observer(
            /* [in] */ QSTileHost* host);

        CARAPI Register();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        Boolean mRegistered;
        QSTileHost* mHost;
    };

    class UserTracker: public CurrentUserTracker
    {
    public:
        UserTracker(
            /* [in] */ QSTileHost* host);

        // @Override
        CARAPI_(void) OnUserSwitched(
            /* [in] */ Int32 newUserId);

    private:
        QSTileHost* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    QSTileHost(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneStatusBar* statusBar,
        /* [in] */ IBluetoothController* bluetooth,
        /* [in] */ ILocationController* location,
        /* [in] */ IRotationLockController* rotation,
        /* [in] */ INetworkController* network,
        /* [in] */ IZenModeController* zen,
        /* [in] */ IHotspotController* hotspot,
        /* [in] */ ICastController* cast,
        /* [in] */ IFlashlightController* flashlight,
        /* [in] */ IUserSwitcherController* userSwitcher,
        /* [in] */ IKeyguardMonitor* keyguard,
        /* [in] */ ISecurityController* security);

    // @Override
    CARAPI SetCallback(
        /* [in] */ IQSTileHostCallback* callback);

    // @Override
    CARAPI GetTiles(
        /* [out] */ ICollection** tiles);

    // @Override
    CARAPI StartSettingsActivity(
        /* [in] */ IIntent* intent);

    // @Override
    CARAPI Warn(
        /* [in] */ const String& message);

    // @Override
    CARAPI CollapsePanels();

    // @Override
    CARAPI GetLooper(
        /* [out] */ ILooper** looper);

    // @Override
    CARAPI GetContext(
        /* [out] */ IContext** ctx);

    // @Override
    CARAPI GetBluetoothController(
        /* [out] */ IBluetoothController** bc);

    // @Override
    CARAPI GetLocationController(
        /* [out] */ ILocationController** result);

    // @Override
    CARAPI GetRotationLockController(
        /* [out] */ IRotationLockController** result);

    // @Override
    CARAPI GetNetworkController(
        /* [out] */ INetworkController** result);

    // @Override
    CARAPI GetZenModeController(
        /* [out] */ IZenModeController** result);

    // @Override
    CARAPI GetHotspotController(
        /* [out] */ IHotspotController** result);

    // @Override
    CARAPI GetCastController(
        /* [out] */ ICastController** result);

    // @Override
    CARAPI GetFlashlightController(
        /* [out] */ IFlashlightController** result);

    // @Override
    CARAPI GetKeyguardMonitor(
        /* [out] */ IKeyguardMonitor** result);

    CARAPI GetUserSwitcherController(
        /* [out] */ IUserSwitcherController** result);

    CARAPI GetSecurityController(
        /* [out] */ ISecurityController** result);

private:
    CARAPI_(void) RecreateTiles();

    CARAPI CreateTile(
        /* [in] */ const String& tileSpec,
        /* [out] */ IQSTile** values);

    CARAPI_(AutoPtr<IList>) LoadTileSpecs();  /*<String*/

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String TILES_SETTING;

    AutoPtr<IContext> mContext;
    AutoPtr<IPhoneStatusBar> mStatusBar;
    AutoPtr<ILinkedHashMap> mTiles;  /*<String, QSTile<?>*/
    AutoPtr<Observer> mObserver;
    AutoPtr<IBluetoothController> mBluetooth;
    AutoPtr<ILocationController> mLocation;
    AutoPtr<IRotationLockController> mRotation;
    AutoPtr<INetworkController> mNetwork;
    AutoPtr<IZenModeController> mZen;
    AutoPtr<IHotspotController> mHotspot;
    AutoPtr<ICastController> mCast;
    AutoPtr<ILooper> mLooper;
    AutoPtr<CurrentUserTracker> mUserTracker;
    AutoPtr<IFlashlightController> mFlashlight;
    AutoPtr<IUserSwitcherController> mUserSwitcherController;
    AutoPtr<IKeyguardMonitor> mKeyguard;
    AutoPtr<ISecurityController> mSecurity;

    AutoPtr<IQSTileHostCallback> mCallback;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_QSTILEHOST_H__
