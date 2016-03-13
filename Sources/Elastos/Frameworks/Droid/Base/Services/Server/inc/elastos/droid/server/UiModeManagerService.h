
#ifndef __ELASTOS_DROID_SERVER_UIMODEMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_UIMODEMANAGERSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/server/SystemService.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::App::IIUiModeManager;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Server::Twilight::ITwilightState;
using Elastos::Droid::Server::Twilight::ITwilightListener;
using Elastos::Droid::Server::Twilight::ITwilightManager;

namespace Elastos {
namespace Droid {
namespace Server {

class UiModeManagerService
    : public SystemService
{
public:
    class BinderService
        : public Object
        , public IIUiModeManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* umm);

        //@Override // Binder call
        CARAPI DisableCarMode(
            /* [in] */ Int32 flags);

        //@Override // Binder call
        CARAPI EnableCarMode(
            /* [in] */ Int32 flags);

        //@Override // Binder call
        CARAPI GetCurrentModeType(
            /* [out] */ Int32 *type);

        //@Override // Binder call
        CARAPI SetNightMode(
            /* [in] */ Int32 mode);

        //@Override // Binder call
        CARAPI GetNightMode(
            /* [out] */ Int32 *mode);

        CARAPI ToString(
            /* [out] */ String* result);
    private:
        UiModeManagerService* mHost;
    };

private:
    // The broadcast receiver which receives the result of the ordered broadcast sent when
    // the dock state changes. The original ordered broadcast is sent with an initial result
    // code of RESULT_OK. If any of the registered broadcast receivers changes this value, e.g.,
    // to RESULT_CANCELED, then the intent to start a dock app will not be sent.
    class ResultReceiver
        : public BroadcastReceiver
    {
    public:
        ResultReceiver(
            /* [in] */ UiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("UiModeManagerService::ResultReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<UiModeManagerService> mHost;
    };

    class DockModeReceiver
        : public BroadcastReceiver
    {
    public:
        DockModeReceiver(
            /* [in] */ UiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("UiModeManagerService::DockModeReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<UiModeManagerService> mHost;
    };

    class BatteryReceiver
        : public BroadcastReceiver
    {
    public:
        BatteryReceiver(
            /* [in] */ UiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("UiModeManagerService::BatteryReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<UiModeManagerService> mHost;
    };

    class MyTwilightListener
        : public Object
        , public ITwilightListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyTwilightListener(
            /* [in] */ UiModeManagerService* host);

        //@Override
        CARAPI OnTwilightStateChanged();

    private:
        AutoPtr<UiModeManagerService> mHost;
    };

public:
    UiModeManagerService();

    CARAPI constructor(
        /* [in] */ IContext *context);

    CARAPI OnStart();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

private:
    static CARAPI_(AutoPtr<IIntent>) BuildHomeIntent(
        /* [in] */ const String& category);

    CARAPI IsDoingNightModeLocked(
        /* [out] */ Boolean *isDoingNightModeLocked);

    CARAPI SetCarModeLocked(
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 flags);

    CARAPI UpdateDockState(
        /* [in] */ Int32 newState);

    static CARAPI_(Boolean) IsDeskDockState(
        /* [in] */ Int32 state);

    CARAPI UpdateConfigurationLocked();

    CARAPI SendConfigurationLocked();

    CARAPI UpdateLocked(
        /* [in] */ Int32 enableFlags,
        /* [in] */ Int32 disableFlags);

    CARAPI UpdateAfterBroadcastLocked(
        /* [in] */ const String& action,
        /* [in] */ Int32 enableFlags,
        /* [in] */ Int32 disableFlags);

    CARAPI SendConfigurationAndStartDreamOrDockAppLocked(
        /* [in] */ const String& category);

    CARAPI AdjustStatusBarCarModeLocked();

    CARAPI UpdateTwilight() ;

    CARAPI UpdateComputedNightModeLocked();

private:
    static const String TAG;
    static const Boolean LOG;

    // Enable launching of applications when entering the dock.
    static const Boolean ENABLE_LAUNCH_CAR_DOCK_APP;
    static const Boolean ENABLE_LAUNCH_DESK_DOCK_APP;

    Object mLock;
    Int32 mDockState;

    Int32 mLastBroadcastState;
    Int32 mNightMode;

    Boolean mCarModeEnabled;
    Boolean mCharging;
    Int32 mDefaultUiModeType;
    Boolean mCarModeKeepsScreenOn;
    Boolean mDeskModeKeepsScreenOn;
    Boolean mTelevision;
    Boolean mWatch;

    Boolean mComputedNightMode;
    Int32 mCarModeEnableFlags;
    Int32 mCurUiMode;
    Int32 mSetUiMode;

    Boolean mHoldingConfiguration;
    AutoPtr<IConfiguration> mConfiguration;

    Boolean mSystemReady;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ITwilightManager> mTwilightManager;
    AutoPtr<INotificationManager> mNotificationManager;

    AutoPtr<IStatusBarManager> mStatusBarManager;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<ResultReceiver> mResultReceiver;

    AutoPtr<DockModeReceiver> mDockModeReceiver;

    AutoPtr<BatteryReceiver> mBatteryReceiver;

    AutoPtr<ITwilightListener> mTwilightListener;

    AutoPtr<IIUiModeManager> mService;

};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_UIMODEMANAGERSERVICE_H__
