
#ifndef __ELASTOS_DROID_SYSTEMUI_POWER_POWERNOTIFICATIONWARNINGS_H__
#define __ELASTOS_DROID_SYSTEMUI_POWER_POWERNOTIFICATIONWARNINGS_H__

#include "_SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::Power::IPowerUIWarningsUI;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Phone::ISystemUIDialog;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Power {

class PowerNotificationWarnings
    : public Object
    , public IPowerUIWarningsUI
{
private:
    class Receiver
        : public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ PowerNotificationWarnings* host);

        CARAPI_(void) Init();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PowerNotificationWarnings* mHost;
    };

    class MyOnClickRunnable
        : public Runnable
    {
    public:
        MyOnClickRunnable(
            /* [in] */ PowerNotificationWarnings* host);

        //@Override
        CARAPI Run();

    private:
        PowerNotificationWarnings* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ PowerNotificationWarnings* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        PowerNotificationWarnings* mHost;
    };

    class MyOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnDismissListener(
            /* [in] */ PowerNotificationWarnings* host);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        PowerNotificationWarnings* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PowerNotificationWarnings(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneStatusBar* phoneStatusBar);

    // @Override
    CARAPI Update(
        /* [in] */ Int32 batteryLevel,
        /* [in] */ Int32 bucket,
        /* [in] */ Int64 screenOffTime);

    // @Override
    CARAPI ShowSaverMode(
        /* [in] */ Boolean mode);

    // @Override
    CARAPI IsInvalidChargerWarningShowing(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI UpdateLowBatteryWarning();

    // @Override
    CARAPI DismissLowBatteryWarning();

    // @Override
    CARAPI ShowLowBatteryWarning(
        /* [in] */ Boolean playSound);

    // @Override
    CARAPI DismissInvalidChargerWarning();

    // @Override
    CARAPI ShowInvalidChargerWarning();

private:
    CARAPI_(void) UpdateNotification();

    CARAPI_(void) ShowInvalidChargerNotification();

    CARAPI_(void) ShowWarningNotification();

    CARAPI_(void) ShowSaverNotification();

    CARAPI_(void) AddStopSaverAction(
        /* [in] */ INotificationBuilder* nb);

    CARAPI_(void) DismissSaverNotification();

    CARAPI_(AutoPtr<IPendingIntent>) PendingActivity(
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<IPendingIntent>) PendingBroadcast(
        /* [in] */ const String& action);

    static CARAPI_(AutoPtr<IIntent>) Settings(
        /* [in] */ const String& action);

    CARAPI_(void) DismissLowBatteryNotification();

    CARAPI_(Boolean) HasBatterySettings();

    CARAPI_(Boolean) HasSaverSettings();

    CARAPI_(void) AttachLowBatterySound(
        /* [in] */ INotificationBuilder* b);

    CARAPI_(void) DismissInvalidChargerNotification();

    CARAPI_(void) ShowStartSaverConfirmation();

    CARAPI_(void) SetSaverMode(
        /* [in] */ Boolean mode);

private:
    const static String TAG;
    const static Boolean DEBUG;

    const static String TAG_NOTIFICATION;
    const static Int32 ID_NOTIFICATION = 100;

    const static Int32 SHOWING_NOTHING = 0;
    const static Int32 SHOWING_WARNING = 1;
    const static Int32 SHOWING_SAVER = 2;
    const static Int32 SHOWING_INVALID_CHARGER = 3;
    const static AutoPtr<ArrayOf<String> > SHOWING_STRINGS;

    const static String ACTION_SHOW_BATTERY_SETTINGS;
    const static String ACTION_START_SAVER;
    const static String ACTION_STOP_SAVER;

    const static AutoPtr<IAudioAttributes> AUDIO_ATTRIBUTES;

    AutoPtr<IContext> mContext;
    AutoPtr<INotificationManager> mNoMan;
    AutoPtr<IPowerManager> mPowerMan;
    AutoPtr<IHandler> mHandler;
    AutoPtr<Receiver> mReceiver;
    AutoPtr<IIntent> mOpenBatterySettings;
    AutoPtr<IIntent> mOpenSaverSettings;

    Int32 mBatteryLevel;
    Int32 mBucket;
    Int64 mScreenOffTime;
    Int32 mShowing;

    Int64 mBucketDroppedNegativeTimeMs;

    Boolean mSaver;
    Boolean mWarning;
    Boolean mPlaySound;
    Boolean mInvalidCharger;
    AutoPtr<ISystemUIDialog> mSaverConfirmation;
    AutoPtr<MyOnClickListener> mStartSaverMode;
};

} // namespace Power
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_POWER_POWERNOTIFICATIONWARNINGS_H__