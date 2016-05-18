
#ifndef __ELASTOS_DROID_SYSTEMUI_POWER_POWERUI_H__
#define __ELASTOS_DROID_SYSTEMUI_POWER_POWERUI_H__

#include "_Elastos_Droid_SystemUI_Power_CPowerUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/systemui/SystemUI.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::SystemUI::SystemUI;
using Elastos::Droid::SystemUI::Power::IPowerUIWarningsUI;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Power {

CarClass(CPowerUI)
    , public SystemUI
    , public IPowerUI
{
private:
    class Receiver
        : public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ CPowerUI* host);

        CARAPI Init();

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("CPowerUI.Receiver");
            return NOERROR;
        }

    private:
        CARAPI_(void) UpdateSaverMode();

    public:
        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CPowerUI* mHost;
    };

    class MyObs
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CPowerUI::MyObs")

        MyObs(
            /* [in] */ CPowerUI* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CPowerUI* mHost;
    };

    friend class Receiver;
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPowerUI();

    CARAPI Start();

    CARAPI UpdateBatteryWarningLevels();

private:
    CARAPI SetSaverMode(
        /* [in] */ Boolean mode);

    /**
     * Buckets the battery level.
     *
     * The code in this function is a little weird because I couldn't comprehend
     * the bucket going up when the battery level was going down. --joeo
     *
     * 1 means that the battery is "ok"
     * 0 means that the battery is between "ok" and what we should warn about.
     * less than 0 means that the battery is low
     */
    CARAPI_(Int32) FindBatteryLevelBucket(
        /* [in] */ Int32 level);

public:
    static const String TAG;
    static const Boolean DEBUG;

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<Receiver> mReceiver;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerUIWarningsUI> mWarnings;
    Int32 mBatteryLevel;
    Int32 mBatteryStatus;
    Int32 mPlugType;
    Int32 mInvalidCharger;
    Int32 mLowBatteryAlertCloseLevel;
    AutoPtr<ArrayOf<Int32> > mLowBatteryReminderLevels;
    Int64 mScreenOffTime;
};

} // namespace Power
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_POWER_POWERUI_H__