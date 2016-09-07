#ifndef __ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__
#define __ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CVibratorService.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::IIVibratorService;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CVibratorService)
    , public Object
    , public IIVibratorService
    , public IInputDeviceListener
    , public IBinder
{
public:
    class VibrationRunnable
        : public Runnable
    {
    public:
        VibrationRunnable(
            /* [in] */ CVibratorService* service)
            : mHost(service)
        {}

        CARAPI Run();

    private:
        CVibratorService* mHost;
    };

    class Vibration
        : public Object
        , public IProxyDeathRecipient
    {
        friend class VibrateThread;
        friend class CVibratorService;

    public:
        CAR_INTERFACE_DECL()

        Vibration(
            /* [in] */ IBinder* token,
            /* [in] */ Int64 millis,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 usageHint,
            /* [in] */ const String& opPkg,
            /* [in] */ CVibratorService* owner);

        Vibration(
            /* [in] */ IBinder* token,
            /* [in] */ ArrayOf<Int64>* pattern,
            /* [in] */ Int32 repeat,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 usageHint,
            /* [in] */ const String& opPkg,
            /* [in] */ CVibratorService* owner);

        CARAPI ProxyDied();

        CARAPI_(Boolean) HasLongerTimeout(
            /* [in] */ Int64 millis);

        CARAPI_(Boolean) IsSystemHapticFeedback();

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        AutoPtr<IBinder> mToken;
        Int64 mTimeout;
        Int64 mStartTime;
        AutoPtr<ArrayOf<Int64> > mPattern;
        Int32 mRepeat;
        Int32 mUsageHint;
        Int32 mUid;
        String mOpPkg;
        CVibratorService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CVibratorService::SettingsObserver")

        SettingsObserver(
            /* [in] */ CVibratorService* owner);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        CVibratorService* mHost;
    };

    class LowPowerModeListener
        : public Object
        , public ILowPowerModeListener
    {
    public:
        CAR_INTERFACE_DECL()

        LowPowerModeListener(
            /* [in] */ CVibratorService* mHost)
            : mHost(mHost)
        {}

        // @Override
        CARAPI OnLowPowerModeChanged(
            /* [in] */ Boolean enabled)
        {
            mHost->UpdateInputDeviceVibrators();
            return NOERROR;
        }

    private:
        CVibratorService* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CVibratorService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CVibratorService::MyBroadcastReceiver: ")
    private:
        CVibratorService* mHost;
    };

    class VibrateThread
        : public Thread
    {
        friend class CVibratorService;

    public:
        VibrateThread(
            /* [in] */ Vibration* vib,
            /* [in] */ CVibratorService* owner);

        CARAPI Run();

    private:
        CARAPI_(void) Delay(
            /* [in] */ Int64 duration);

    public:
        AutoPtr<Vibration> mVibration;
        Boolean mDone;
        CVibratorService* mHost;
    };

    class IntentReceiver
        : public BroadcastReceiver
    {
        friend class CVibratorService;

    public:
        TO_STRING_IMPL("CVibratorService::IntentReceiver: ")

        IntentReceiver(
            /* [in] */ CVibratorService* owner)
            : mHost(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CVibratorService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVibratorService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) VibratorExists();

    static CARAPI_(void) VibratorOn(
        /* [in] */ Int64 milliseconds);

    static CARAPI_(void) VibratorOff();

    CARAPI SystemReady();

    CARAPI HasVibrator(
        /* [out] */ Boolean* hasVibrator);

    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ Int64 milliseconds,
        /* [in] */ Int32 usageHint,
        /* [in] */ IBinder* token);

    CARAPI VibratePattern(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ Int32 usageHint,
        /* [in] */ IBinder* token);

    CARAPI CancelVibrate(
        /* [in] */ IBinder* token);

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI VerifyIncomingUid(
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) IsAll0(
        /* [in] */ const ArrayOf<Int64>& pattern);

    // Lock held on mVibrations
    CARAPI_(void) DoCancelVibrateLocked();

    // Lock held on mVibrations
    CARAPI_(void) StartNextVibrationLocked();

    // Lock held on mVibrations
    CARAPI_(void) StartVibrationLocked(
        /* [in] */ Vibration* vib);

    CARAPI_(void) ReportFinishVibrationLocked();

    // Lock held on mVibrations
    CARAPI_(AutoPtr<Vibration>) RemoveVibrationLocked(
        /* [in] */ IBinder* token);

    CARAPI_(void) UnlinkVibration(
        /* [in] */ Vibration* vib);

    CARAPI_(void) UpdateInputDeviceVibrators();

    CARAPI_(Boolean) DoVibratorExists();

    CARAPI_(void) DoVibratorOn(
        /* [in] */ Int64 millis,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 usageHint);

    CARAPI_(void) DoVibratorOff();

private:
    static const String TAG;
    static const Boolean DBG;

    List< AutoPtr<Vibration> > mVibrations;
    Object mVibrationsLock;
    AutoPtr<Vibration> mCurrentVibration;
    AutoPtr<IWorkSource> mTmpWorkSource;
    AutoPtr<IHandler> mH;

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IIAppOpsService> mAppOpsService;
    AutoPtr<IIBatteryStats> mBatteryStatsService;
    AutoPtr<IPowerManagerInternal> mPowerManagerInternal;

    AutoPtr<IInputManager> mIm;

    AutoPtr<VibrateThread> mThread;
    // Object mThreadLock;

    // mInputDeviceVibrators lock should be acquired after mVibrations lock, if both are
    // to be acquired
    List< AutoPtr<IVibrator> > mInputDeviceVibrators;
    Object mInputDeviceVibratorsLock;
    Boolean mVibrateInputDevicesSetting; // guarded by mInputDeviceVibrators
    Boolean mInputDeviceListenerRegistered; // guarded by mInputDeviceVibrators

    Int32 mCurVibUid;
    Boolean mLowPowerMode;
    AutoPtr<SettingsObserver> mSettingObserver;

    AutoPtr<IntentReceiver> mIntentReceiver;

    AutoPtr<IRunnable> mVibrationRunnable;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__
