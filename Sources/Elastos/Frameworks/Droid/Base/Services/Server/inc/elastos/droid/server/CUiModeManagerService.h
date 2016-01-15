
#ifndef __ELASTOS_DROID_SERVER_CUIMODEMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CUIMODEMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CUiModeManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "TwilightService.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Server::TwilightService;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;


namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CUiModeManagerService)
{
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
            /* [in] */ CUiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CUiModeManagerService::ResultReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CUiModeManagerService> mHost;
    };

    class DockModeReceiver
        : public BroadcastReceiver
    {
    public:
        DockModeReceiver(
            /* [in] */ CUiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CUiModeManagerService::DockModeReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CUiModeManagerService> mHost;
    };

    class BatteryReceiver
        : public BroadcastReceiver
    {
    public:
        BatteryReceiver(
            /* [in] */ CUiModeManagerService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CUiModeManagerService::BatteryReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CUiModeManagerService> mHost;
    };

    class _TwilightListener
        : public ElRefBase
        , public TwilightService::ITwilightListener
    {
    public:
        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID* iid);

        _TwilightListener(
            /* [in] */ CUiModeManagerService* host);

        //@Override
        CARAPI OnTwilightStateChanged();

    private:
        AutoPtr<CUiModeManagerService> mHost;
    };


public:
    CUiModeManagerService();

    CARAPI constructor(
        /* [in] */ IContext *context,
        /* [in] */ Handle32 twilight);

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

    CARAPI SystemReady();

protected:
    static CARAPI_(AutoPtr<IIntent>) BuildHomeIntent(
        /* [in] */ const String& category);

private:
    CARAPI IsDoingNightModeLocked(
        /* [out] */ Boolean *isDoingNightModeLocked);

    CARAPI SetCarModeLocked(
        /* [in] */ Boolean enabled);

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

    AutoPtr<IContext> mContext;
    AutoPtr<TwilightService> mTwilightService;
    AutoPtr<IHandler> mHandler;

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

    Boolean mComputedNightMode;
    Int32 mCurUiMode;
    Int32 mSetUiMode;

    Boolean mHoldingConfiguration;
    AutoPtr<IConfiguration> mConfiguration;

    Boolean mSystemReady;

    AutoPtr<INotificationManager> mNotificationManager;

    AutoPtr<IStatusBarManager> mStatusBarManager;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<ResultReceiver> mResultReceiver;

    AutoPtr<DockModeReceiver> mDockModeReceiver;

    AutoPtr<BatteryReceiver> mBatteryReceiver;

    AutoPtr<_TwilightListener> mTwilightListener;

};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CUIMODEMANAGERSERVICE_H__
