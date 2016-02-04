
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_CDOZESERVICE_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_CDOZESERVICE_H__

#include "_Elastos_Droid_Packages_SystemUI_Doze_CDozeService.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/service/dreams/DreamService.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IUiModeManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ITriggerEvent;
using Elastos::Droid::Hardware::ITriggerEventListener;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Packages::SystemUI::StatusBar::Phone::IDozeParameters;
using Elastos::Droid::Service::Dreams::DreamService;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace Doze {

CarClass(CDozeService)
    , public DreamService
    , public IDozeService
{
private:
    class DozeBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        DozeBroadcastReceiver(
            /* [in] */ CDozeService* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CDozeService* mHost;
    };

    class DozeHostCallback
        : public Object
        , public IDozeHostCallback
    {
    public:
        CAR_INTERFACE_DECL();

        DozeHostCallback(
            /* [in] */ CDozeService* host);

        // @Override
        CARAPI OnNewNotifications();

        // @Override
        CARAPI OnBuzzBeepBlinked();

        // @Override
        CARAPI OnNotificationLight(
            /* [in] */ Boolean on);

        // @Override
        CARAPI OnPowerSaveChanged(
            /* [in] */ Boolean active);

    private:
        CDozeService* mHost;
    };

    class TriggerSensor
        : public Object
        , public ITriggerEventListener
    {
    public:
        CAR_INTERFACE_DECL();

        TriggerSensor(
            /* [in] */ Int32 type,
            /* [in] */ Boolean configured,
            /* [in] */ Boolean debugVibrate,
            /* [in] */ CDozeService* host);

        CARAPI SetListening(
            /* [in] */ Boolean listen);

        CARAPI SetDisabled(
            /* [in] */ Boolean disabled);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        // @Override
        CARAPI OnTrigger(
            /* [in] */ ITriggerEvent* event);

    private:
        CARAPI_(void) UpdateListener();

    private:
        CDozeService* mHost;
        AutoPtr<ISensor> mSensor;
        Boolean mConfigured;
        Boolean mDebugVibrate;

        Boolean mRequested;
        Boolean mRegistered;
        Boolean mDisabled;
    };

    class ProximityCheck
        : public Object
        , public ISensorEventListener
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        ProximityCheck(
            /* [in] */ CDozeService* host);

        virtual CARAPI OnProximityResult(
            /* [in] */ Int32 result) = 0;

        CARAPI Check();

        // @Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

    private:
        CARAPI_(void) FinishWithResult(
            /* [in] */ Int32 result);

    protected:
        static const Int32 RESULT_UNKNOWN;
        static const Int32 RESULT_NEAR;
        static const Int32 RESULT_FAR;
        CDozeService* mHost;

    private:
        static const Int32 TIMEOUT_DELAY_MS;
        String mTag;

        Boolean mRegistered;
        Boolean mFinished;
        Float mMaxRange;
    };

    class ProximityCheckImpl: public ProximityCheck
    {
    private:
        class PulseCallback
            : public Object
            , public IDozeHostPulseCallback
        {
        public:
            CAR_INTERFACE_DECL();

            PulseCallback(
                /* [in] */ ProximityCheckImpl* host);

            // @Override
            CARAPI OnPulseStarted();

            // @Override
            CARAPI OnPulseFinished();

        private:
            ProximityCheckImpl* mHost;
        };

    public:
        ProximityCheckImpl(
            /* [in] */ CDozeService* host,
            /* [in] */ Int64 start);

        // @Override
        CARAPI OnProximityResult(
            /* [in] */ Int32 result);

    private:
        Int64 mStart;
    };

    class DSRunnable : public Runnable
    {
    public:
        DSRunnable(
            /* [in] */ CDozeService* host);

        // @Override
        CARAPI Run();

    private:
        CDozeService* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CDozeService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDreamingStarted();

    // @Override
    CARAPI OnDreamingStopped();

protected:
    // @Override
    CARAPI_(void) DumpOnHandler(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) RequestPulse();

    CARAPI_(void) TurnDisplayOff();

    CARAPI_(void) TurnDisplayOn();

    CARAPI_(void) FinishToSavePower();

    CARAPI_(void) FinishForCarMode();

    CARAPI_(void) ListenForPulseSignals(
        /* [in] */ Boolean listen);

    CARAPI_(void) ListenForBroadcasts(
        /* [in] */ Boolean listen);

    CARAPI_(void) ListenForNotifications(
        /* [in] */ Boolean listen);

    CARAPI_(void) ResetNotificationResets();

    CARAPI_(void) UpdateNotificationPulse();

    CARAPI_(AutoPtr<IPendingIntent>) NotificationPulseIntent(
        /* [in] */ Int64 instance);

    CARAPI_(void) RescheduleNotificationPulse(
        /* [in] */ Boolean predicate);

    static CARAPI_(String) TriggerEventToString(
        /* [in] */ ITriggerEvent* event);

private:
    static const String TAG;
    static Boolean DEBUG;

    static const String ACTION_BASE;
    static const String PULSE_ACTION;
    static const String NOTIFICATION_PULSE_ACTION;
    static const String EXTRA_INSTANCE;

    AutoPtr<DozeBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<IDozeHostCallback> mHostCallback;

    String mTag;
    AutoPtr<IContext> mContext;
    AutoPtr<IDozeParameters> mDozeParameters;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IDozeHost> mHost;
    AutoPtr<ISensorManager> mSensors;
    AutoPtr<TriggerSensor> mSigMotionSensor;
    AutoPtr<TriggerSensor> mPickupSensor;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IUiModeManager> mUiModeManager;
    Boolean mDreaming;
    Boolean mPulsing;
    Boolean mBroadcastReceiverRegistered;
    Boolean mDisplayStateSupported;
    Boolean mNotificationLightOn;
    Boolean mPowerSaveActive;
    Boolean mCarMode;
    Int64 mNotificationPulseTime;
    Int32 mScheduleResetsRemaining;
};

} // namespace Doze
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_CDOZESERVICE_H__
