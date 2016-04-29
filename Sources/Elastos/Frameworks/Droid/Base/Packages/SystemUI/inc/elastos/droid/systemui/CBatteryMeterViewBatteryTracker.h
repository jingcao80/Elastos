#ifndef __ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__
#define __ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__

#include "_Elastos_Droid_SystemUI_CBatteryMeterViewBatteryTracker.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CBatteryMeterView;

CarClass(CBatteryMeterViewBatteryTracker), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CBatteryMeterViewBatteryTracker();

    CARAPI constructor(
        /* [in] */ IBatteryMeterView* view);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static Int32 UNKNOWN_LEVEL;

    // current battery status
    Int32 mLevel;
    String mPercentStr;
    Int32 mPlugType;
    Boolean mPlugged;
    Int32 mHealth;
    Int32 mStatus;
    String mTechnology;
    Int32 mVoltage;
    Int32 mTemperature;
    Boolean mTestmode;

    CBatteryMeterView* mHost;
    friend class CBatteryMeterView;
};

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__
