#ifndef __ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__
#define __ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CVibratorService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CVibratorService)
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
        : public ElRefBase
        , public IProxyDeathRecipient
    {
        friend class VibrateThread;
        friend class CVibratorService;

    public:
        Vibration(
            /* [in] */ IBinder* token,
            /* [in] */ Int64 millis,
            /* [in] */ Int32 uid,
            /* [in] */ CVibratorService* owner);

        Vibration(
            /* [in] */ IBinder* token,
            /* [in] */ ArrayOf<Int64>* pattern,
            /* [in] */ Int32 repeat,
            /* [in] */ Int32 uid,
            /* [in] */ CVibratorService* owner);

        CAR_INTERFACE_DECL();

        CARAPI ProxyDied();

        CARAPI_(Boolean) HasLongerTimeout(
            /* [in] */ Int64 millis);

    // private:
    //     Vibration(
    //         /* [in] */ IBinder* token,
    //         /* [in] */ Int64 millis,
    //         /* [in] */ ArrayOf<Int64>* pattern,
    //         /* [in] */ Int32 repeat,
    //         /* [in] */ Int32 uid);

    private:
        AutoPtr<IBinder> mToken;
        Int64 mTimeout;
        Int64 mStartTime;
        AutoPtr<ArrayOf<Int64> > mPattern;
        Int32 mRepeat;
        Int32 mUid;
        CVibratorService* mOwner;
    };

    class MyContentObserver : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CVibratorService* owner);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        CVibratorService* mOwner;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CVibratorService* owner);

    public:
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CVibratorService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CVibratorService* mOwner;
    };

    class VibrateThread
        : public ElRefBase
        , public IRunnable
    {
        friend class CVibratorService;

    public:
        VibrateThread(
            /* [in] */ Vibration* vib,
            /* [in] */ CVibratorService* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

        CARAPI Start();

    private:
        CARAPI_(void) Delay(
            /* [in] */ Int64 duration);

    public:
        AutoPtr<Vibration> mVibration;
        Boolean mDone;
        CVibratorService* mOwner;
        AutoPtr<IThread> mThread;
    };

    class IntentReceiver : public BroadcastReceiver
    {
        friend class CVibratorService;

    public:
        IntentReceiver(
            /* [in] */ CVibratorService* owner)
            : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CVibratorService::IntentReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CVibratorService* mOwner;
    };

public:
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
        /* [in] */ Int64 milliseconds,
        /* [in] */ IBinder* token);

    CARAPI VibratePattern(
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat,
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
    CARAPI_(Boolean) IsAll0(
        /* [in] */ const ArrayOf<Int64>& pattern);

    // Lock held on mVibrations
    CARAPI_(void) DoCancelVibrateLocked();

    // Lock held on mVibrations
    CARAPI_(void) StartNextVibrationLocked();

    // Lock held on mVibrations
    CARAPI_(void) StartVibrationLocked(
        /* [in] */ Vibration* vib);

    // Lock held on mVibrations
    CARAPI_(AutoPtr<Vibration>) RemoveVibrationLocked(
        /* [in] */ IBinder* token);

    CARAPI_(void) UnlinkVibration(
        /* [in] */ Vibration* vib);

    CARAPI_(void) UpdateInputDeviceVibrators();

    CARAPI_(Boolean) DoVibratorExists();

    CARAPI_(void) DoVibratorOn(
        /* [in] */ Int64 millis);

    CARAPI_(void) DoVibratorOff();

private:
    static const String TAG;

    List< AutoPtr<Vibration> > mVibrations;
    Object mVibrationsLock;
    AutoPtr<Vibration> mCurrentVibration;
    AutoPtr<IWorkSource> mTmpWorkSource;
    AutoPtr<IHandler> mH;

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IInputManager> mIm;

    AutoPtr<VibrateThread> mThread;
    // Object mThreadLock;

    // mInputDeviceVibrators lock should be acquired after mVibrations lock, if both are
    // to be acquired
    List< AutoPtr<IVibrator> > mInputDeviceVibrators;
    Object mInputDeviceVibratorsLock;
    Boolean mVibrateInputDevicesSetting; // guarded by mInputDeviceVibrators
    Boolean mInputDeviceListenerRegistered; // guarded by mInputDeviceVibrators

    AutoPtr<IntentReceiver> mIntentReceiver;

    AutoPtr<IRunnable> mVibrationRunnable;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CVIBRATORSERVICE_H__
